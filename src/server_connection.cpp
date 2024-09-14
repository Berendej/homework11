#include <iostream>
#include <utility>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <vector>

#include "server_connection.h"
#include "sqlite_wrap.h"

#define VERBOSE

int server_connection_c::s_instance_id = 0;
std::mutex server_connection_c::s_instatinate_mutex;

server_connection_c::server_connection_c(boost::asio::ip::tcp::socket sock) : 
    m_socket{std::move(sock)}
{
    std::unique_lock lck(s_instatinate_mutex);
    m_instance_id = ++s_instance_id;
    std::cout << "sqlite is ok :" << sqlite_c::get_instance().is_ok() << std::endl;
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
    std::string response = process_cmd(length);
    response += '\r'; // hack
    m_buffer.consume(length);

    boost::asio::async_write(m_socket, 
                             boost::asio::buffer(response), 
                             boost::bind( &server_connection_c::handle_write, 
                                shared_from_this(),
                                boost::asio::placeholders::error));

    start_read();
}

void server_connection_c::handle_write(const boost::system::error_code& error)
{
#ifdef VERBOSE
    std::cout << "handle_write" << std::endl;
#endif
    start_read();
}

std::vector<std::string> split(const std::string& str, 
                               char d)
{
    std::vector<std::string> r;
    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    while (stop != std::string::npos)
    {
        r.push_back(str.substr(start, stop - start));
        start = stop + 1;
        stop = str.find_first_of(d, start);
    }
    r.push_back(str.substr(start));
    return r;
}

std::string server_connection_c::gen_insert(const std::vector<std::string> &v)
{
    // INSERT table id name
    if ( v.size() < 4 )
    {
        return "";
    }
    std::string sql{"INSERT into "};
    sql += v[1];
    sql += " values ( ";
    sql += v[2];
    sql += ", \"";
    sql += v[3];
    sql += "\" );";
    std::cout << "sql = " << sql << std::endl;
    return sql;
}

std::string server_connection_c::gen_trancate(const std::vector<std::string> &v)
{
// TRUNCATE table
    if ( v.size() < 2 )
    {
        return "";
    }
    std::string sql{"delete from "};
    sql += v[1];
    sql += " ;";
    std::cout << "sql = " << sql << std::endl;
    return sql;
}

std::string server_connection_c::gen_select(const std::vector<std::string> &v)
{
// TRUNCATE table
    if ( v.size() < 2 )
    {
        return "";
    }
    std::string sql{"select *  from "};
    sql += v[1];
    sql += " ;";
    std::cout << "sql = " << sql << std::endl;
    return sql;
}

std::string server_connection_c::gen_intersec(const std::vector<std::string> &v)
{
    std::string sql{" select A.id as id, A.name as A, B.name as B from A inner join B on A.id = B.id;"
                };
    return sql;
}

std::string server_connection_c::gen_dif(const std::vector<std::string> &v)
{
    return ""; // stub
}

std::string server_connection_c::preprocess_cmd(const std::string &s)
{
    std::vector<std::string> v = split(s, ' ');
    if ( v.size() < 1 )
    {
        return "";
    }
    // raise first word to upper case
    std::transform(v[0].begin(), v[0].end(), v[0].begin(), ::toupper);
/*
INSERT table id name
TRUNCATE table
INTERSECTION
SYMMETRIC_DIFFERENCE
*/    
    if ( 0 == v[0].compare("INSERT") )
    {
        return gen_insert(v);
    }
    if ( 0 == v[0].compare("TRUNCATE"))
    {
        return gen_trancate(v);
    }
    if ( 0 == v[0].compare("INTERSECTION"))
    {
        return gen_intersec(v);
    }
    if ( 0 == v[0].compare("SYMMETRIC_DIFFERENCE"))
    {
        return gen_dif(v);
    }
    if ( 0 == v[0].compare("SELECT"))
    {
        return gen_select(v);
    }
    std::cout << "wrong request " << std::endl;
    return "";
}

std::string server_connection_c::process_cmd(int length)
{
    std::string s( (std::istreambuf_iterator<char>(&m_buffer)), 
                    std::istreambuf_iterator<char>() );
#ifdef VERBOSE
    std::cout << "got command " << s << std::endl;
#endif
    // cut away trailing \n
    if (s.length() > 0 and '\n' == s[s.length()-1] )
    {
        s.pop_back();
    }

    std::string sql_text = preprocess_cmd(s);
    if ( 0 == sql_text.length() )
    {
        return "ERR wrong request";
    }
    return sqlite_c::get_instance().execute(sql_text);
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
