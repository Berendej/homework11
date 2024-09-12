#include <string>
#include <fstream>
#include <iostream> // for debug remove in release
#include "sql_executor.h"

cmd_vector_t sql_executor_c::s_command_queue;
std::mutex   sql_executor_c::s_mutex;
std::condition_variable sql_executor_c::s_cond_var;

sql_executor_c::sql_executor_c() : m_p_stream(nullptr)
{
}

// static
void sql_executor_c::receive_command(const cmd_vector_t &commands)
{
    std::unique_lock lck(s_mutex);
    for( auto cmd : commands )
    {
        s_command_queue.push_back(cmd);
    }
    s_cond_var.notify_one();
}

std::ostream* sql_executor_c::get_stream()
{
    std::ostream*  p_ostream = (std::ostream*)m_p_stream;
    return p_ostream;
}

bool sql_executor_c::open_stream(const std::string &file_name)
{
    bool result = false;
    try
    {
        m_p_stream = new std::ofstream(file_name);
        if ( nullptr != m_p_stream)
        {
            result = true;
        }
    }
    catch(...)
    {

    }
    return result;
}

void sql_executor_c::close_stream()
{
    //m_p_stream->close();
    delete m_p_stream;
    m_p_stream = nullptr;
}

std::string sql_executor_c::file_prefix()
{
    // looks like "f_1_blablabla"
    std::string result = "f_";
    result += std::to_string(m_instance_id);
    result += "_";
    return result;
}

void sql_executor_c::work()
{
    cmd_vector_t cmd_vector;
    while( not m_shutdown )
    {
        std::unique_lock<std::mutex> lck(s_mutex);
        while( not m_shutdown and s_command_queue.empty())
        {
                try
                {
                    s_cond_var.wait(lck);
                }
                catch( std::exception ex)
                {
                    std::cout << ex.what();
                }
        }
        if ( s_command_queue.size() > 0 )
        {
            for ( auto cmd : s_command_queue )
            {
                cmd_vector.push_back(cmd);
            }
            execute(cmd_vector); // not virtual
            cmd_vector.clear();
            s_command_queue.clear();
        }
    }
}

void sql_executor_c::finish()
{
    if (m_started )
    {
        {
            std::unique_lock<std::mutex> lck(s_mutex);
            m_shutdown = true;
            s_cond_var.notify_all();
        }
        m_thread.join();
    }
}
