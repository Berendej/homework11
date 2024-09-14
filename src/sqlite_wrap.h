#pragma once

#include <sqlite3.h>
#include "common_types.h"


// wrapper
class sqlite_c
{
private:
    sqlite3 *m_db;
    bool m_valid;
    std::string m_response;
    table_t m_table;
    int m_rows;

    sqlite_c();
    void transform_table();

public:
    static sqlite_c& get_instance()
    {
        static sqlite_c instance;
        return instance;
    }
    int callback(int argc, char **argv, char **azColName);
    std::string execute(std::string oper);
    virtual ~sqlite_c();
};
