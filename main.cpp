/*
 * @Author: Lavender
 * @Date: 2022-02-20 21:14:26
 * @LastEditors: Lavender
 * @LastEditTime: 2022-02-28 12:21:24
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

//
// 设置命令行参数
//
void setParser(cmdline::parser& cmdParser) {
    cmdParser.add<string>("host", '\0', "host", true);
    cmdParser.add<string>("port", '\0', "port", true);
    cmdParser.add<string>("target", '\0', "target", true);
    cmdParser.add<string>("method", '\0', "request method", false, "GET", cmdline::oneof<string>("GET", "POST"));
    cmdParser.add<string>("ifile", '\0', "input filename", false);
    cmdParser.add<string>("ofile", '\0', "output filename", false);
}

inline void setIfstream(std::ifstream& input_stream, std::string&& input_filename) {
    input_stream.open(input_filename, std::ios::in);
}

inline void setOfstream(std::ofstream& output_stream, std::string&& output_filename) {
    output_stream.open(output_filename, std::ios::out | std::ios::trunc);
}

inline int getFilename(cmdline::parser& cmdParser, std::string& input_filename, std::string& output_filename) {
    int state = 0;
    if (cmdParser.exist("ifile")) input_filename = cmdParser.get<string>("ifile"), state += 1;
    if (cmdParser.exist("ofile")) output_filename = cmdParser.get<string>("ofile"), state += 1 << 1;
    return state;
}

int main(int argc, char* argv[]) {
    try {
        // 使用第三方库cmdline进行命令行参数解析
        cmdline::parser cmdParser;
        setParser(cmdParser);
        cmdParser.parse_check(argc, argv);

        std::ifstream input_stream;
        std::ofstream output_stream;
        string input_filename, output_filename;

        boost::asio::io_service io_service;
        string host = cmdParser.get<string>("host");
        string port = cmdParser.get<string>("port");
        string target = cmdParser.get<string>("target");
        string method = cmdParser.get<string>("method");
        int state = getFilename(cmdParser, input_filename, output_filename);
        setIfstream(input_stream, std::move(input_filename));
        setOfstream(output_stream, std::move(output_filename));
        std::shared_ptr<session> client;
        switch (state) {
            case 1:
                {
                    client = std::make_shared<session>(io_service, std::move(input_stream));
                    break;
                }
            case 2:
                {
                    client = std::make_shared<session>(io_service, std::move(output_stream));
                    break;
                }
            case 3:
                {
                    client = std::make_shared<session>(io_service, std::move(input_stream), std::move(output_stream));
                    break;
                }
            default:
                {
                    client = std::make_shared<session>(io_service);
                    break;
                }
        }
        client->run(host.c_str(), port.c_str(), target.c_str(), method.c_str());
        io_service.run();
    } catch (std::invalid_argument& ia) {
        // 捕捉stoi异常
        std::cerr << "stoi转换异常:" << ia.what() << std::endl;

    } catch (std::exception& e) {
        // 捕捉其他异常
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}
