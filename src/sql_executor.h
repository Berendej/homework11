#pragma once

#include <condition_variable>

#include "executor.h"

class sql_executor_c : public executor_c
{
    static std::mutex   s_mutex;
    static std::condition_variable s_cond_var;

public:
    sql_executor_c();

    virtual bool open_db(const std::string &file_name);
    virtual void close_db();
    virtual void work();
    virtual void finish();

};
