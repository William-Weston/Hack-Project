/**
 * @file    Assembler.cpp
 * @author  William Weeston
 * @brief   Hack Assembler for Emulator Project
 * @version 0.1
 * @date    2024-03-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Assembler.h"

#include "Code_Generator.h"               // for Code_Generator
#include "Code_Line.h"                    // for Code_Line

#include "Hack/Utilities/exceptions.hpp"  // for parse_error
#include "Hack/Utilities/utilities.hpp"   // for is_alpha, to_binary16_string

#include <tl/expected.hpp>                // for expected, unexpected

#include <algorithm>                      // for sort
#include <cassert>                        // for assert
#include <istream>                        // for basic_istream, basic_ios::c...
#include <optional>                       // for optional, nullopt
#include <span>                           // for span
#include <string>                         // for char_traits, allocator, ope...
#include <string_view>                    // for basic_string_view, string_view
#include <tuple>                          // for tuple
#include <utility>                        // for move
#include <vector>                         // for vector


// ------------------------------------------------------------------------------------------------
namespace   // helper function declarations -------------------------------------------------------
{
   auto remove_whitespace( std::string text )               -> std::string;
   auto trim_line_comments( std::string_view text )         -> std::string_view;
   auto parse_c_instruction( std::string_view instruction ) -> std::tuple<std::string, std::string, std::string>;
}


// ------------------------------------------------------------------------------------------------
// -------------------------------------- Assembler API -------------------------------------------

auto 
Hack::Assembler::assemble( std::istream& file ) -> std::vector<std::string>
{
   auto const code = prepare( file );

   auto result = std::vector<std::string>();
   result.reserve( code.size() );

   for ( auto const& [instruction, text, line_no] : code )
   {
      auto binary_opt = assemble( instruction );

      if ( binary_opt )
      {
         result.push_back( std::move( *binary_opt ) );
      }
      else
      {
         auto msg = "Assembly failed on instruction number " + std::to_string( line_no );
         
         throw Hack::Utils::parse_error( std::move( msg ), { text, line_no } );
      }
   }

   return result;
}


auto 
Hack::Assembler::assemble( std::span<std::string const> instructions )  const -> std::vector<std::string>
{
   auto result = std::vector<std::string>();
   result.reserve( instructions.size() );

   // instruction number for error reporting
   auto count = std::span<std::string const>::size_type{ 0 };   

   for ( auto const& instruction : instructions )
   {
      auto binary_opt = assemble( instruction );

      if ( binary_opt )
      {
         result.push_back( std::move( *binary_opt ) );
      }
      else
      {
         auto msg = "Assembly failed on instruction number " + std::to_string( count );

         throw Hack::Utils::parse_error( 
            std::move( msg ),
            { instruction, count }
         );
      }
      ++count;
   }

   return result;
}

// TODO: exception handling 
// assemble from file that may contain labels and variables
auto 
Hack::Assembler::assemble_expected( std::istream& file ) -> tl::expected<std::vector<std::string>, Code_Line>
{
   auto const code = prepare( file );     // TODO: this throws

   auto result = std::vector<std::string>();
   result.reserve( code.size() );

   for ( auto const& line : code )
   {
      auto binary_opt = assemble( line.instruction );

      if ( binary_opt )
      {
         result.push_back( std::move( *binary_opt ) );
      }
      else
      {
         return tl::unexpected<Code_Line>( line );
      }
   }

   return { result };
}


auto 
Hack::Assembler::assemble( std::string_view instruction ) const -> std::optional<std::string>
{
   if ( instruction.empty() )
   {
      return std::nullopt;
   }
   // a instruction
   if ( instruction[0] == '@' )              
   {
      return process_a_instruction( instruction );
   }
   
   // c instruction
   return process_c_instruction( instruction );
}


// ------------------------------------------------------------------------------------------------
// --------------------------------- Assembler Implementation -------------------------------------


auto 
Hack::Assembler::prepare( std::istream& file ) -> std::vector<Code_Line>
{
   first_pass( file );
   file.clear();
   file.seekg( std::ios_base::beg );
   return second_pass( file );
}


/**
 * @brief   First pass that scans the file for labels and adds them to the symbol table
 * 
 * @param   file    data stream containing assembly code
 * @throws Hack::Utils::Parse_Error    if stream contains errors an exception is thrown
 */
auto 
Hack::Assembler::first_pass( std::istream& file ) -> void
{
   auto line                   = std::string();
   auto current_line_no        = 1zu;
   auto current_instruction_no = 0;

   while ( std::getline( file, line ) )                  // cppcheck-suppress[accessMoved]
   {
      // remove whitespace
      auto const result = remove_whitespace( line );     // cppcheck-suppress[accessMoved]

      // don't load comments or empty lines
      if ( result.starts_with( "//" ) || result.empty() )
      {
         ++current_line_no;
         continue;
      }

      // handle lables
      if ( result.starts_with( '(' ) )
      {
         auto const end_bracket = result.find( ')' );
         
         // check for matching brackets
         if ( end_bracket == std::string::npos )
         {
            auto error_msg = "Error on line number " + std::to_string( current_line_no ) + '\n';
            error_msg += "\t>>>  " + line + '\n';
            error_msg += "No Closing Bracket";

            throw Hack::Utils::parse_error(  std::move( error_msg ),
                                             { std::move( line ), current_line_no }
                                          );
         }
         
         // add label to symbol table with address of next instruction ie: current_instruction_no
         auto label = std::string_view( result );
         
         label.remove_suffix( label.size() - end_bracket );
         label.remove_prefix( 1 );
       
         symbol_table_.add_entry( label, current_instruction_no );
      }
      else
      {
         ++current_instruction_no;
      }

      ++current_line_no;
   }
}

/**
 * @brief   Second pass through input stream removes comments, whitespace, labels, 
 *          adds variables to the symbol table with an increasing number starting at 16
 *          and replaces predefined symbols with their corresponding values from the symbol table.
 *          The parsed data is then returned to the caller.
 * 
 * @param file 
 * @return std::vector<Code_Line> 
 */
auto 
Hack::Assembler::second_pass( std::istream& file ) -> std::vector<Code_Line>
{
   static constexpr auto variable_start_address = 16;

   auto code            = std::vector<Code_Line>();
   auto line            = std::string();
   auto current_line_no = 1;
   auto variable_no     = variable_start_address;

   while ( std::getline( file, line ) )                  // cppcheck-suppress[accessMoved]
   {
      // remove whitespace
      auto result = remove_whitespace( line );           // cppcheck-suppress[accessMoved]

      // don't load comments, empty lines or labels
      if ( !( result.starts_with( "//" ) || result.empty() || result.starts_with( '(' ) ) )
      {
         // trim line comments
         auto trimmed = trim_line_comments( result );

         if ( trimmed.starts_with( '@' ) && trimmed.size() > 1 && Hack::Utils::is_alpha( trimmed.at( 1 ) ) )    // variable, label or predefined symbol
         {
            // check symbol table
            trimmed.remove_prefix( 1 );

            if ( symbol_table_.contains( trimmed ) )
            {
               code.emplace_back(  "@" + std::to_string( symbol_table_.get_address( trimmed ) ), std::move( line ), current_line_no );
            }
            else  // unseen variable
            {
               symbol_table_.add_entry( trimmed, variable_no );
               code.emplace_back( "@" + std::to_string( variable_no++ ), std::move( line ), current_line_no ); 
            }      
         }
         else
         {
            code.emplace_back( std::string( trimmed ), std::move( line ), current_line_no );
         }
      }

      ++current_line_no;
   }

   return code;
}


auto 
Hack::Assembler::process_a_instruction( std::string_view instruction ) const -> std::optional<std::string>
{
   instruction.remove_prefix( 1 );              // remove '@'

   auto result = Hack::Utils::to_binary16_string( instruction );

   if ( !result || result->at( 0 ) == '1' )     // a instruction can NOT begin with a '1'
   {
      return std::nullopt;
   }

   return std::optional<std::string>{ std::move( result ) };
}


auto 
Hack::Assembler::process_c_instruction( std::string_view instruction ) const -> std::optional<std::string>
{
   static constexpr auto c_op_code = "111";

   auto [dest, comp, jump] = parse_c_instruction( instruction );
   
   std::sort( dest.begin(), dest.end() );

   auto const code_gen = Hack::Code_Generator();

   auto const dest_code = code_gen.dest( dest );
   auto const comp_code = code_gen.comp( comp );
   auto const jump_code = code_gen.jump( jump );

   if ( !dest_code || !comp_code || !jump_code )
   {
     return std::nullopt;
   }

   auto binary_instruction = c_op_code + *comp_code + *dest_code + *jump_code;

   if ( binary_instruction.size() != instruction_size )
   {
      return std::nullopt;
   }

   return std::optional<std::string>{ std::move( binary_instruction ) };
}


//-------------------------------------------------------------------------------------------------
namespace   // helper function definitions --------------------------------------------------------
{


auto 
remove_whitespace( std::string text )  -> std::string
{
   std::erase_if( text, [] ( char c ) 
                        { return c == ' ' || c ==  '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v'; }
   );

   return text;
}

auto 
trim_line_comments( std::string_view text )         -> std::string_view
{
   if ( auto pos = text.find( "//" ); pos != std::string_view::npos )
   {
      text.remove_suffix( text.size() - pos );
      return text;
   }

   return text;
}


// instruction must not be empty
auto
parse_c_instruction( std::string_view instruction ) -> std::tuple<std::string, std::string, std::string>
{
   assert( ! instruction.empty() );

   auto current_pos = 0ul;

   // parse dest
   auto const equal_pos = instruction.find( '=' );
   auto const has_equal = equal_pos != std::string::npos;
   auto const dest_end  = has_equal ? equal_pos : 0;
   auto const dest      = instruction.substr( current_pos, dest_end );

   current_pos = dest_end;

   if ( has_equal )
   {
      ++current_pos; // advance past equal sign
   }

   // parse comp
   auto const semicolon_pos = instruction.find( ';', current_pos );
   auto const has_semicolon = semicolon_pos != std::string::npos;
   auto const comp_end      = has_semicolon ? semicolon_pos : instruction.size();
   auto const comp_size     = comp_end - current_pos;
   auto const comp          = instruction.substr( current_pos, comp_size );

   current_pos = comp_end;

   if ( has_semicolon )
   {
      ++current_pos;
   }
   
   auto const jump_size = instruction.size() - current_pos;
   auto const jump      = instruction.substr( current_pos, jump_size );

   return { std::string( dest ),  std::string( comp ),  std::string( jump ) };
}

}  // namespace