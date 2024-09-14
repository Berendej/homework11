#pragma once

#include <map>
#include <string>
#include <vector>

using str_vector_t = std::vector<std::string>;
using i_str_vector_t = str_vector_t::iterator;
using table_t =  std::map<std::string, str_vector_t>;
using i_table_t = table_t::iterator;
