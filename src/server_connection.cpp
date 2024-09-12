#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "server_connection.h"

#define VERBOSE

int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for(i=0; i<argc; i++)
    {
        std::cout << "%s = %s\n" << azColName[i] << (argv[i] ? argv[i] : "NULL");
    }
    std::cout << std::endl;
    return 0;
}

sqlite_c::sqlite_c()
{
    int rc = sqlite3_open(DB_NAME, &m_db);
    if( rc )
    {
#ifdef VERBOSE
        std::cout << "Can't open database: %s\n" << sqlite3_errmsg(m_db);
#endif
        sqlite3_close(m_db);
        m_valid = false;
        return;
    }
    m_valid = true;
}

bool sqlite_c::execute(std::string oper)
{
    char *err_msg = 0;
    if (!m_valid) 
    {
        return false;
    }
    int rc = sqlite3_exec(m_db, oper.c_str(), callback, 0, &err_msg);
    if( rc != SQLITE_OK )
    {
          std::cout <<  "SQL error: " <<  err_msg;
          sqlite3_free(err_msg);
          return false;
    }
    return true;
}

sqlite_c::~sqlite_c()
{
    sqlite3_close(m_db);
}

int server_connection_c::s_instance_id = 0;
std::mutex server_connection_c::s_instatinate_mutex;

server_connection_c::server_connection_c(boost::asio::ip::tcp::socket sock) : 
    m_socket{std::move(sock)}
{
    std::unique_lock lck(s_instatinate_mutex);
    m_instance_id = ++s_instance_id;
#ifdef VERBOSE
    std::cout << "server_connection_c " << m_instance_id << std::endl;
#endif
}

void server_connection_c::start_read()
{
#ifdef VERBOSE
    std::cout << "start read " << m_socket.is_open() << std::endl;
#endif
    boost::asio::async_read_until( m_socket, m_buffer, '\n',
        boost::bind(&server_connection_c::handle_read, 
                     shared_from_this(),
                     boost::asio::placeholders::error,
                     boost::asio::placeholders::bytes_transferred));
#ifdef VERBOSE
    std::cout << "start read p2" << std::endl;
#endif
}

void server_connection_c::handle_read(const boost::system::error_code error,
                  const std::size_t length) 
{
#ifdef VERBOSE
    std::cout << "handle read " << m_socket.is_open() << std::endl;
#endif
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
    process_cmd(length);
    m_buffer.consume(length);
    start_read();
}

void server_connection_c::process_cmd(int length)
{
    std::string s( (std::istreambuf_iterator<char>(&m_buffer)), 
                    std::istreambuf_iterator<char>() );
#ifdef VERBOSE
    std::cout << "got command " << s << std::endl;
#endif
    execute(s);
}

server_connection_c::~server_connection_c()
{
#ifdef THIS_IS_WRONG
    /* Если данные закончились внутри динамического блока, весь динамический блок игнорируется.*/
    if ( m_queue.size() > 0 )
    {
        dump_private_queue();
    }
#endif
#ifdef VERBOSE
    std::cout << "~server_connection_c " << m_socket.is_open() << std::endl;
#endif
}
