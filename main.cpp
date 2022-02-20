/*
 * @Author: Lavender
 * @Date: 2022-02-20 21:14:26
 * @LastEditors: Lavender
 * @LastEditTime: 2022-02-20 21:39:55
 * @Description: sidecar的main函数
 * @FilePath: /Microservice-Workflows-Sidecar/main.cpp
 */

#include <boost/asio.hpp>

#include <iostream>
#include <list>


using boost::asio::ip::tcp;

class server {
public:
    server(boost::asio::io_service& io_service, const tcp::endpoint& endpoint)
        : _acceptor(io_service, endpoint), _socket(io_service) {

    }
private:
    tcp::acceptor _acceptor;
    tcp::socket _socket;
};

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            std::cerr << "Usage: sidecar <source port> <target port>" << std::endl;
            return 1;
        }
        boost::asio::io_service io_service;
        // 定义源端口和目标端口，完成数据转发
        tcp::endpoint source_endpoint(tcp::v4(), std::atoi(argv[1]));
        tcp::endpoint target_endpoint(tcp::v4(), std::atoi(argv[2]));
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}
