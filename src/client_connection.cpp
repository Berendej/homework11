#include <iostream>
#include <chrono>
#include <boost/bind/bind.hpp>

#include "client_connection.h"

//#define VERBOSE

client_connection_c::client_connection_c(boost::asio::io_service& service, short port) :
            m_socket(service), 
            m_port(port)
{
#ifdef VERBOSE
    std::cout << "client_connection_c" << std::endl;
#endif
    // right away register itself as a shared pointer
    // in service's slot. client_connection will be alive 
    // untill there will be something to do for it in service
}

void client_connection_c::connect()
{
    boost::asio::ip::tcp::endpoint endpoint(
                    boost::asio::ip::address::from_string("127.0.0.1"), 
                    m_port);
    m_socket.async_connect(endpoint, 
                            boost::bind( &client_connection_c::handle_connect, 
                                shared_from_this(),
                                boost::asio::placeholders::error));
}

bool client_connection_c::read_input()
{
#ifdef VERBOSE
    std::cout << "read_input" << std::endl;
#endif
    m_line.clear();
    std::getline( std::cin, m_line );
    if ( 0 == m_line.length() )
    {
        return false;
    }
#ifdef VERBOSE
    std::cout << "m_line \"" << m_line  << "\"" << std::endl;
#endif
    m_line += "\n";
    return true;
    //getline(digits, MAX_INPUT_LEN);
    //int cnt = cin.gcount();
}

bool client_connection_c::transit_command()
{
    if ( not read_input() ) // <- synchronous operation. io_service hangs here
                       // todo make it async
    {
#ifdef VERBOSE
        std::cout << "read_input ret false " << std::endl;
#endif
        return false;
    }
    boost::asio::async_write(m_socket, 
                             boost::asio::buffer(m_line), 
                             boost::bind( &client_connection_c::handle_write, 
                                shared_from_this(),
                                boost::asio::placeholders::error));
    return true;
}


void client_connection_c::handle_connect(const boost::system::error_code& error)
{
#ifdef VERBOSE
    std::cout << "handle_connect" << std::endl;
#endif
    if ( error )
    {
        // and that's the end of like client_connection's life
        std::cout << "connect failed" << std::endl;
        return;
    }
    transit_command();
}

void client_connection_c::handle_write(const boost::system::error_code& error)
{
#ifdef VERBOSE
    std::cout << "handle_write" << std::endl;
#endif
    if ( error )
    {
        std::cout << "handle_write failed" << std::endl;
        return;
    }

    boost::asio::async_read_until( m_socket, m_buffer, '\r',
        boost::bind(&client_connection_c::handle_read, 
                     shared_from_this(),
                     boost::asio::placeholders::error,
                     boost::asio::placeholders::bytes_transferred));

}

client_connection_c::~client_connection_c()
{
    std::cout << "~client_connection()" << std::endl;
}

void client_connection_c::process_response(int length)
{
    std::string s( (std::istreambuf_iterator<char>(&m_buffer)), 
                    std::istreambuf_iterator<char>() );
    std::cout << "response: " << s << std::endl;
}

void client_connection_c::handle_read(const boost::system::error_code error,
                                     const std::size_t length) 
{
    if (error or 0 == length ) 
    {
#ifdef VERBOSE
        std::cout << "handle read error " << error << " << len " << length << std::endl;
#endif
        // zero length also means connection is closed
        // connection is closed, we do not continue 
        // async calling so that smart pointer will do its work
        // and connection_c will be deleted
        return;
    }
#ifdef VERBOSE
    std::cout << "handle read length " << length <<  std::endl;
#endif
    process_response(length);
    m_buffer.consume(length);
    transit_command();
}
