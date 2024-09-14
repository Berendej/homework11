#pragma once

#include <memory>

#include "common_types.h"

class server_connection_c : public std::enable_shared_from_this<server_connection_c>
{
    static int s_instance_id;
    static std::mutex s_instatinate_mutex;

    int m_instance_id;

    boost::asio::ip::tcp::socket m_socket;
    boost::asio::streambuf m_buffer;
    std::string process_cmd(int length);
    void handle_read(const boost::system::error_code error,
                     const std::size_t length);
    void handle_write(const boost::system::error_code& error);

    std::string gen_insert(const std::vector<std::string> &v);
    std::string gen_trancate(const std::vector<std::string> &v);
    std::string gen_intersec(const std::vector<std::string> &v);
    std::string gen_dif(const std::vector<std::string> &v);
    std::string gen_select(const std::vector<std::string> &v);
    std::string preprocess_cmd(const std::string &s);
public:
    server_connection_c(boost::asio::ip::tcp::socket sock);
    virtual ~server_connection_c();
    void start_read();
};
using sp_server_connection_c = std::shared_ptr<server_connection_c>;
