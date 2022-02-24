/*
 * @Author: Lavender
 * @Date: 2022-02-23 20:26:38
 * @LastEditors: Lavender
 * @LastEditTime: 2022-02-24 14:36:02
 * @Description: 发送网络请求的类
 * @FilePath: /Microservice-Workflows-Agency/request.cpp
 */

#include "request.h"

 //
 // 报告错误的函数
 //
void fail(beast::error_code ec, const char* what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

void session::run(const char* host, const char* port, const char* target, int version) {
    _req.version(version);
    _req.method(http::verb::get);
    _req.target(target);
    _req.set(http::field::host, host);
    _req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // 解析域名
    _resolver.async_resolve(
        host,
        port,
        beast::bind_front_handler(
            &session::on_resolve,
            shared_from_this()));
}

void session::on_resolve(beast::error_code ec, tcp::resolver::results_type results) {
    if (ec) return fail(ec, "resolve");
    // 尝试连接时间，如果这段时间内没连上就放弃
    _stream.expires_after(std::chrono::seconds(30));
    // 结果存储在results中，调用on_connect进行异步连接
    _stream.async_connect(
        results,
        beast::bind_front_handler(
            &session::on_connect,
            shared_from_this()));
}

void session::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type) {
    if (ec) return fail(ec, "connect");
    // 尝试连接时间，如果这段时间内没连上就放弃
    _stream.expires_after(std::chrono::seconds(30));
    // 向服务器发送请求
    http::async_write(_stream, _req,
        beast::bind_front_handler(&session::on_write,
            shared_from_this()));
}

void session::on_write(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    if (ec) return fail(ec, "write");
    http::async_read(_stream, _buffer, _res,
        beast::bind_front_handler(
            &session::on_read,
            shared_from_this()));
}

void session::on_read(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    if (ec) return fail(ec, "read");
    std::cout << _res << std::endl;
    _stream.socket().shutdown(tcp::socket::shutdown_both, ec);
    if (ec && ec != beast::errc::not_connected)
        return fail(ec, "shutdown");
}