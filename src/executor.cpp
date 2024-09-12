#include <chrono>
#include <ostream>
#include <thread>
#include <iostream>

#include "executor.h"

int executor_c::s_instance_id = 0;
std::mutex executor_c::s_instatinate_mutex;

executor_c::executor_c() : m_shutdown(false), 
                           m_started(false)
{
    std::unique_lock lck(s_instatinate_mutex);
    m_instance_id = ++s_instance_id;
}

int executor_c::start()
{
    m_thread = std::move( std::thread(&executor_c::work, this ) );
    m_started = true;
    return true;
}
