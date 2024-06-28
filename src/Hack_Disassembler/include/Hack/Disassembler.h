/**
 * @file    Disassembler.h
 * @author  William Weston
 * @brief   Translates Hack binary into Hack assembly code
 * @version 0.1
 * @date    2024-03-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef HACK_DISASSEMBLER_2024_03_29_DISASSEMBLER_H
#define HACK_DISASSEMBLER_2024_03_29_DISASSEMBLER_H

#include <cstdint>  
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Hack
{

class Disassembler final 
{
public:
   static auto disassemble( std::string_view binary )   -> std::optional<std::string>;
   static auto disassemble( std::uint16_t instruction ) -> std::optional<std::string>;

   // return the computation performed by the ALU given an instruction
   static auto computation( std::string_view binary )   -> std::optional<std::string>;
   static auto computation( std::uint16_t instruction ) -> std::optional<std::string>;

private:
   static constexpr auto INSTRUCTION_SIZE = 16zu;

   inline static std::unordered_map<std::string_view, std::string> const dest_table_ = 
   {
      { "000", ""  }, { "001", "M"  }, { "010", "D"  }, { "011", "DM"  }, 
      { "100", "A" }, { "101", "AM" }, { "110", "AD" }, { "111", "ADM" }
   };

   inline static std::unordered_map<std::string_view, std::string> const comp_table_ = 
   {
      { "0101010", "0"   },
      { "0111111", "1"   },
      { "0111010", "-1"  },
      { "0001100", "D"   },
      { "0110000", "A"   },   { "1110000", "M"   },
      { "0001101", "!D"  },
      { "0110001", "!A"  },   { "1110001", "!M"  },
      { "0001111", "-D"  },
      { "0110011", "-A"  },   { "1110011", "-M"  },
      { "0011111", "D+1" },
      { "0110111", "A+1" },   { "1110111", "M+1" },
      { "0001110", "D-1" },
      { "0110010", "A-1" },   { "1110010", "M-1" },
      { "0000010", "A+D" },   { "1000010", "M+D" },
      { "0010011", "D-A" },   { "1010011", "D-M" },
      { "0000111", "A-D" },   { "1000111", "M-D" },
      { "0000000", "A&D" },   { "1000000", "M&D" },
      { "0010101", "A|D" },   { "1010101", "M|D" }
   };

   inline static std::unordered_map<std::string_view, std::string> const jump_table_ = 
   {
      { "000", ""    },   { "001", "JGT" },   { "010", "JEQ" },   { "011", "JGE" },
      { "100", "JLT" },   { "101", "JNE" },   { "110", "JLE" },   { "111", "JMP" }
   };

   static auto a_instruction( std::string_view binary ) -> std::optional<std::string>;
   static auto c_instruction( std::string_view binary ) -> std::optional<std::string>;


};

}   // namespace Hack

#endif      //  HACK_DISASSEMBLER_2024_03_29_DISASSEMBLER_H