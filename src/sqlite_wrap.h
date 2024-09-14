#pragma once

#include <sqlite3.h>
#include "common_types.h"

using table_t =  std::map<std::string, str_vector_t>;
using i_table_t = table_t::iterator;

using fild_order_t =  std::map<int, std::string>;
using i_fild_order_t = fild_order_t::iterator;


// wrapper
class sqlite_c
{
private:
    sqlite3 *m_db;
    bool m_valid;
    std::string m_response;
    table_t m_table;
    fild_order_t m_field_order;
    int m_current_field_num;
    int m_rows;

    sqlite_c();
    bool check_tables();
    void transform_table();

public:
    static sqlite_c& get_instance()
    {
        static sqlite_c instance;
        return instance;
    }
    int callback(int argc, char **argv, char **azColName);
    std::string execute(std::string oper);
    bool is_ok();
    virtual ~sqlite_c();
};
