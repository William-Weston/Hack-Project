/**
 * @file    Symbol_Table.cpp
 * @author  Wiliam Weston
 * @brief   Symbol Table for Hack Assembler
 * @version 0.1
 * @date    2024-03-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Symbol_Table.h"

#include <string>             // for operator==, string
#include <string_view>        // for operator==, string_view
#include <unordered_map>      // for unordered_map, _Node_const_iterator
#include <utility>            // for pair

auto 
Hack::Symbol_Table::add_entry( std::string_view symbol, int address ) -> void
{  
   table_[ std::string( symbol )] = address;
}

auto 
Hack::Symbol_Table::contains( std::string_view symbol )  const -> bool
{
   return table_.contains( symbol );
}


auto 
Hack::Symbol_Table::get_address( std::string_view symbol ) const -> int
{
   return table_.find( symbol )->second;        // TODO: this is dangerous if caller didn't first check if the requested symbol exists
}