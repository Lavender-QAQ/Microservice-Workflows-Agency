/*
 * @Author: Lavender
 * @Date: 2022-02-20 21:14:26
 * @LastEditors: Lavender
 * @LastEditTime: 2022-02-24 14:39:10
 * @Description: sidecar的main函数（主要用于命令行参数配置以及启动对应类型的sidecar）
 * @FilePath: /Microservice-Workflows-Agency/main.cpp
 */

#include "cmdline.h"
#include "request.h"

#include <boost/asio.hpp>

#include <iostream>
#include <list>
#include <stdexcept>


using boost::asio::ip::tcp;
using string = std::string;

void setParser(cmdline::parser& cmdParser) {
    cmdParser.add<string>("host", '\0', "host", true);
    cmdParser.add<string>("port", '\0', "port", true);
    cmdParser.add<string>("target", '\0', "target", true);
}
int main(int argc, char* argv[]) {
    try {
        // 使用第三方库cmdline进行命令行参数解析
        cmdline::parser cmdParser;
        setParser(cmdParser);
        cmdParser.parse_check(argc, argv);

        boost::asio::io_service io_service;
        string host = cmdParser.get<string>("host");
        string port = cmdParser.get<string>("port");
        string target = cmdParser.get<string>("target");
        std::make_shared<session>(io_service)->run(host.c_str(), port.c_str(), target.c_str());
        io_service.run();
    } catch (std::invalid_argument& ia) {
        // 捕捉stoi异常
        std::cerr << "stoi转换异常:" << ia.what() << std::endl;

    } catch (std::exception& e) {
        // 捕捉其他异常
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}
