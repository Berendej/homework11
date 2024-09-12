#pragma once

#include <string>
#include <memory>
#include <boost/asio.hpp>

class client_connection_c : public std::enable_shared_from_this<client_connection_c>
{
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::steady_timer m_timer;
    short m_port;
    std::string m_line;
    void handle_write(const boost::system::error_code& error);
    void handle_connect(const boost::system::error_code& error);
    void handle_timer(const boost::system::error_code& error);
    bool read_input();
    bool transit_command();
public:
    client_connection_c(boost::asio::io_service& service, short port);
    virtual ~client_connection_c();
    void connect();
};
using sp_client_connection_c = std::shared_ptr<client_connection_c>;
