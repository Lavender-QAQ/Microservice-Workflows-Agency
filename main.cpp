#include <iostream>
#include <boost/asio.hpp>

int main(int, char**) {
    boost::asio::io_service io;
    std::cout << "Hello, world!\n";
}
