#include <iostream>

#include "sqlite_wrap.h"

#define DB_NAME "db.sqlite"

int callback_aux(void* sqlite_ptr, int argc, char **argv, char **azColName)
{
    sqlite_c *p_sqlite = (sqlite_c*)sqlite_ptr;
    if ( NULL == p_sqlite )
    {
        std::cout << "wrong ptr in sqlite callback";
        return -1;
    }
    return p_sqlite->callback(argc, argv, azColName);
}

int sqlite_c::callback(int argc, char **argv, char **azColName)
{
    std::cout << "callback" << std::endl;
    int i;
    std::string field;
    std::string val;
    str_vector_t empty_row;
    bool ok;
    for(i=0; i<argc; i++)
    {
        ok = true;
        field = azColName[i];
        i_table_t it{ m_table.find(field) };
        if ( m_table.end() == it )
        {
            auto p = m_table.insert(std::make_pair(field, empty_row));
            if ( p.second )
            {
                it = p.first;
            }
            else
            {
                std::cout << "not ok\n";
                ok = false;
            }
        }
        if ( ok )
        {
            val = argv[i];
            it->second.push_back(val);
            m_rows = it->second.size();
        }
    }
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

void sqlite_c::transform_table()
{
    std::cout << "t t size " << m_rows << std::endl;
    str_vector_t v;
    v.resize(m_rows + 1);
    i_table_t it = m_table.begin();
    while ( it != m_table.end() )
    {
        int row_ix = 0;
        v[row_ix].append(it->first);
        v[row_ix++].append(" ");
        i_str_vector_t ist = it->second.begin();
        for( auto s : it->second)
        {
            std::cout << "value " << s << std::endl;
            v[row_ix].append(s);
            v[row_ix++].append(" ");
        }
        it++;
    }
    for( auto s : v )
    {
        m_response.append(s);
        m_response.append("\n");
    }
}

std::string sqlite_c::execute(std::string oper)
{
    char *err_msg = 0;
    if (!m_valid) 
    {
        return "ERR\n";
    }
    m_response = "OK\n";
    m_table.clear();
    m_rows = 0;
    int rc = sqlite3_exec(m_db, oper.c_str(), callback_aux, this, &err_msg);
    if( rc != SQLITE_OK )
    {
          m_response = "ERR ";
          m_response += std::string(err_msg);
          sqlite3_free(err_msg);
          return m_response;
    }
    if ( m_table.size() > 0 )
    {
        transform_table();
    }
    return m_response;
}

sqlite_c::~sqlite_c()
{
    sqlite3_close(m_db);
}
