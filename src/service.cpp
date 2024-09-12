#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <memory>

#include "server_connection.h"

using tcp = boost::asio::ip::tcp;

void start_accept(tcp::acceptor& acceptor)
{
    acceptor.async_accept(
        [&acceptor](const boost::system::error_code error, tcp::socket socket)
        {
            sp_server_connection_c sp_conn( new server_connection_c(std::move(socket)));
            // start_read() will put shared_from_this in io_service's slot so it will be alive
            // even when sp_conn will go out of scope
            sp_conn->start_read(); // it'll die itself when there will be no work for it.
            start_accept(acceptor);
        });
}

bool run_server(int port)
{
    boost::asio::io_service io_service;
    tcp::acceptor acceptor{io_service, tcp::endpoint(tcp::v4(), port)};
    start_accept(acceptor);
    io_service.run();
#ifdef VERBOSE
    std::cout << "run end" << std::endl;
#endif
    return true;
}
