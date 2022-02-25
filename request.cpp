/*
 * @Author: Lavender
 * @Date: 2022-02-23 20:26:38
 * @LastEditors: Lavender
 * @LastEditTime: 2022-02-25 18:46:35
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

void session::run(const char* host, const char* port, const char* target, const char* verb) {
    int version = 11;
    _req.version(version);
    // _req.method(http::verb::get);
    _req.target(target);
    _req.set(http::field::host, "192.168.31.57");
    _req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    if (!strcmp(verb, "GET")) {
        _req.method(http::verb::get);
    } else if (!strcmp(verb, "POST")) {
        if (_state == nfile || _state == ofile) return fail(boost::system::errc::make_error_code(boost::system::errc::function_not_supported), "run");
        std::stringstream buffer;
        buffer << _input_stream.rdbuf();
        std::string json(buffer.str());
        _req.method(http::verb::post);
        _req.set(http::field::content_type, "application/json");
        _req.body() = json;
        // 不调这个就不行，踩坑擦
        _req.prepare_payload();
    }

    // std::cout << _req << std::endl;

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
    std::cout << _res.body() << std::endl;
    if (_state == ofile || _state == iofile) {
        _output_stream << _res.body();
    }
    _stream.socket().shutdown(tcp::socket::shutdown_both, ec);
    if (ec && ec != beast::errc::not_connected)
        return fail(ec, "shutdown");
}