/**
 * @file    Code_Generator.h
 * @author  William Weston
 * @brief   Generates binary code for Hack Computer
 * @version 0.1
 * @date    2024-03-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef HACK_2024_03_21_CODE_GENERATOR_H
#define HACK_2024_03_21_CODE_GENERATOR_H


#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Hack
{

class Code_Generator final
{
public:
   auto dest( std::string_view op_code ) const -> std::optional<std::string>;
   auto comp( std::string_view op_code ) const -> std::optional<std::string>;
   auto jump( std::string_view op_code ) const -> std::optional<std::string>;

private:
   inline static std::unordered_map<std::string_view, std::string> const dest_table_ = 
   {
      { "", "000" },  { "M", "001" },  { "D", "010" },  { "DM", "011" }, 
      { "A", "100" }, { "AM", "101" }, { "AD", "110" }, { "ADM", "111" }
   };

   inline static std::unordered_map<std::string_view, std::string> const comp_table_ = 
   {
      { "0",   "0101010" },
      { "1",   "0111111" },
      { "-1",  "0111010" },
      { "D",   "0001100" },
      { "A",   "0110000" }, { "M",   "1110000" },
      { "!D",  "0001101" },
      { "!A",  "0110001" }, { "!M",  "1110001" },
      { "-D",  "0001111" },
      { "-A",  "0110011" }, { "-M",  "1110011" },
      { "D+1", "0011111" },
      { "A+1", "0110111" }, { "M+1", "1110111" },
      { "D-1", "0001110" },
      { "A-1", "0110010" }, { "M-1", "1110010" },
      { "D+A", "0000010" }, { "D+M", "1000010" }, 
      { "A+D", "0000010" }, { "M+D", "1000010" },
      { "D-A", "0010011" }, { "D-M", "1010011" },
      { "A-D", "0000111" }, { "M-D", "1000111" },
      { "D&A", "0000000" }, { "D&M", "1000000" }, 
      { "A&D", "0000000" }, { "M&D", "1000000" },
      { "D|A", "0010101" }, { "D|M", "1010101" }, 
      { "A|D", "0010101" }, { "M|D", "1010101" }
   };

   inline static std::unordered_map<std::string_view, std::string> const jump_table_ = 
   {
      { "", "000" },    { "JGT", "001" }, { "JEQ", "010" }, { "JGE", "011" },
      { "JLT", "100" }, { "JNE", "101" }, { "JLE", "110" }, { "JMP", "111" }
   };
};

}     // namespace Hack
#endif   // HACK_2024_03_21_CODE_GENERATOR_H