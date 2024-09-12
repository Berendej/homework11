#pragma once

#include <thread>
#include <mutex>
#include <memory>

#include "cmd.h"

class executor_c
{
    static int s_instance_id;
    static std::mutex s_instatinate_mutex;

protected:
    bool m_shutdown;
    bool m_started;
    int m_instance_id;
    std::thread m_thread;
public:
    executor_c();
    virtual bool open(const std::string &file_name) = 0;
    virtual void close() = 0;
    virtual void work() = 0;
    virtual void finish() = 0;
    int get_handle()
    {
        return m_instance_id;
    }
    void execute(std::string command); // not virtual
    int start();
    bool is_working() 
    {
        return m_started;
    }
};
