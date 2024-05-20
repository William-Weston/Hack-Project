/**
 * @file    Symbol_Table.h
 * @author  William Weston
 * @brief   Symbol Table for Hack Assembler
 * @version 0.1
 * @date    2024-03-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef HACK_2024_03_21_SYMBOL_TABLE_H
#define HACK_2024_03_21_SYMBOL_TABLE_H

#include "Hack/Utilities/utilities.hpp"            // string_hash

#include <functional>                              // equal_to
#include <string>
#include <string_view>
#include <unordered_map>


namespace Hack
{

class Symbol_Table final
{
public:
   
   auto add_entry( std::string_view symbol, int address )       -> void;
   auto contains( std::string_view symbol )               const -> bool;
   auto get_address( std::string_view symbol )            const -> int;


private:
   std::unordered_map<std::string, int, Hack::Utils::string_hash, std::equal_to<>> table_ =
   {
      { "R0", 0 }, { "R1", 1 },  { "R2", 2 },   { "R3", 3 },   { "R4", 4 },   { "R5", 5 },   { "R6", 6 },   { "R7", 7 },
      { "R8", 8 }, { "R9", 9 },  { "R10", 10 }, { "R11", 11 }, { "R12", 12 }, { "R13", 13 }, { "R14", 14 }, { "R15", 15 },
      { "SP", 0 }, { "LCL", 1 }, { "ARG", 2 },  { "THIS", 3 }, { "THAT", 4 },
      { "SCREEN", 16'384 },      { "KBD", 24'576 }
   };
};

}  // namespace Hack

#endif   // HACK_2024_03_21_SYMBOL_TABLE_H