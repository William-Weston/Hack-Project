/**
 * @file    Assembler.h
 * @author  William Weston
 * @brief   Generates binary for the Hack architecture from Hack Assembly Code
 * @version 0.1
 * @date 2024-03-21
 * 
 * @copyright Copyright (c) 2024
 * 
 * @details Hack Assembler for the Hack Emulator project.  
 * 
 *          This assembler differs from the command line version in the following ways:
 *             •  performs two initial passes instead of one
 *                •  the first puts labels in the symbol table
 *                •  the second replace variables with a number instead of entering them in the 
 *                   symbol table as the command line version does
 *             •  returns a vector<string> of the assembly after processing by the first two passes
 *             •  this vector is then used for display and for transformation into binary
 *             •  must be able to convert a single assembly instruction into binary as long as
 *                it contains no labels or variables
 */
#ifndef HACK_2024_03_21_ASSEMBLER_H
#define HACK_2024_03_21_ASSEMBLER_H

#include "Code_Line.h"           // for Code_Line
#include "Symbol_Table.h"        // for Symbol_Table

#include <iosfwd>                // for istream
#include <optional>              // for optional
#include <span>                  // for span
#include <string>                // for string
#include <string_view>           // for string_view
#include <tl/expected.hpp>       // for expected
#include <vector>                // for vector


namespace Hack
{

class Assembler final
{
public:

   // assemble from file that may contain labels and variables
   auto assemble( std::istream& file )                             -> std::vector<std::string>;

   // assemble instructions containing no labels or variables
   auto assemble( std::span<std::string const> instruction ) const -> std::vector<std::string>;

   // assemble_expected from file that may contain labels and variables
   auto assemble_expected( std::istream& file )                    -> tl::expected<std::vector<std::string>, Code_Line>;
   
   // assemble one assembly instruction, not containing labels or variables, to binary
   auto assemble( std::string_view instruction ) const             -> std::optional<std::string>;


private:
   Symbol_Table symbol_table_{};

   static constexpr int instruction_size = 16;
   
   // performs first two passes
   auto prepare( std::istream& file )           -> std::vector<Code_Line>;
   auto first_pass( std::istream& file )        -> void;
   auto second_pass( std::istream& file )       -> std::vector<Code_Line>;

   auto process_a_instruction( std::string_view instruction ) const -> std::optional<std::string>;
   auto process_c_instruction( std::string_view instruction ) const -> std::optional<std::string>;
};



}  // namespace Hack

#endif      // HACK_2024_03_21_ASSEMBLER_H