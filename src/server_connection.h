#pragma once

#include <list>
#include <string>
#include <memory>
#include <sqlite3.h>

#define DB_NAME "db.sqlite"

// wrapper
class sqlite_c
{
private:
    sqlite3 *m_db;
    bool m_valid;
public:
    sqlite_c();
    bool execute(std::string oper);
    virtual ~sqlite_c();
};

class server_connection_c : public sqlite_c, 
                            public std::enable_shared_from_this<server_connection_c>
{
    static int s_instance_id;
    static std::mutex s_instatinate_mutex;

    int m_instance_id;

    boost::asio::ip::tcp::socket m_socket;
    boost::asio::streambuf m_buffer;
    void process_cmd(int length);
    void handle_read(const boost::system::error_code error,
                     const std::size_t length);
public:
    server_connection_c(boost::asio::ip::tcp::socket sock);
    virtual ~server_connection_c();
    void start_read();
};
using sp_server_connection_c = std::shared_ptr<server_connection_c>;
