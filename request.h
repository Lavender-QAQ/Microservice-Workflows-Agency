/*
 * @Author: Lavender
 * @Date: 2022-02-23 20:22:38
 * @LastEditors: Lavender
 * @LastEditTime: 2022-02-24 14:22:22
 * @Description: 发送网络请求的头文件
 * @FilePath: /Microservice-Workflows-Agency/request.h
 */

#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>

#include <iostream>
#include <chrono>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

//
// 报告错误的函数
//
void fail(beast::error_code ec, const char* what);

//
// 客户端与服务端建立连接的类
//
class session : public std::enable_shared_from_this<session> {
public:
    explicit session(asio::io_context& io)
        : _resolver(asio::make_strand(io)), _stream(asio::make_strand(io)) {
    }
    //
    // 启动连接
    //
    void run(const char* host, const char* port, const char* target, int version = 10);
private:
    //
    // 解析域名转换为ip
    //
    void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
    //
    // 连接服务器
    //
    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type);
    //
    // 保存服务器返回的数据到buffer
    //
    void on_write(beast::error_code ec, std::size_t bytes_transferred);
    //
    // 从buffer读取服务器返回的数据
    //
    void on_read(beast::error_code ec, std::size_t bytes_transferred);
    tcp::resolver _resolver;
    beast::tcp_stream _stream;
    beast::flat_buffer _buffer;
    http::request<http::empty_body> _req;
    http::response<http::string_body> _res;
};


// // Get
// std::string GetRequest(char* host, char* path);
// std::string GetRequest(string url);

// // Post
// std::string PostRequest(char* host, char* path, std::string form);

