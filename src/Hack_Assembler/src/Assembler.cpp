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

#include "Code_Generator.h"
#include "Hack/Utilities/exceptions.hpp"           // parse_error
#include "Hack/Utilities/utilities.hpp"            // to_uint_t

#include <algorithm>    // sort
#include <cassert>      // assert
#include <charconv>     // from_chars
#include <cstdint>      // uint16_t
#include <cctype>
#include <istream>
#include <optional>
#include <stdexcept>    // runtime_error
#include <string>
#include <tl/expected.hpp>
#include <utility>      // move

// ------------------------------------------------------------------------------------------------
namespace   // helper function declarations -------------------------------------------------------
{
   auto is_alpha( char c ) -> bool;
   auto remove_whitespace( std::string text )               -> std::string;
   auto trim_line_comments( std::string_view text )         -> std::string_view;
   auto to_binary_16_string( std::string_view )             -> std::optional<std::string>;
   auto digit_to_char( int digit, int base )                -> char;
   auto to_binary16( int number )                           -> std::optional<std::string>;
   auto int_to_binary_string( int number )                  -> std::string;
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
Hack::Assembler::assemble( std::span<std::string const> instructions )  -> std::vector<std::string>
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

   return tl::expected<std::vector<std::string>, Code_Line>( result );
}


auto 
Hack::Assembler::assemble( std::string_view instruction ) const -> std::optional<std::string>
{
   if ( instruction.size() == 0 )   return std::nullopt;

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
      if ( result.starts_with( "//" ) || result.size() == 0 )
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
   auto code            = std::vector<Code_Line>();
   auto line            = std::string();
   auto current_line_no = 1;
   auto variable_no     = 16;

   while ( std::getline( file, line ) )                  // cppcheck-suppress[accessMoved]
   {
      // remove whitespace
      auto result = remove_whitespace( line );           // cppcheck-suppress[accessMoved]

      // don't load comments, empty lines or labels
      if ( !( result.starts_with( "//" ) || result.size() == 0 || result.starts_with( '(' ) ) )
      {
         // trim line comments
         auto trimmed = trim_line_comments( result );

         if ( trimmed.starts_with( '@' ) && trimmed.size() > 1 && is_alpha( trimmed.at( 1 ) ) )    // variable, label or predefined symbol
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
   instruction.remove_prefix( 1 );        // remove '@'

   return to_binary_16_string( instruction );
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

   if ( binary_instruction.size() != 16 )
   {
      return std::nullopt;
   }

   return binary_instruction;
}


//-------------------------------------------------------------------------------------------------
namespace   // helper function definitions --------------------------------------------------------
{

auto
is_alpha( char ch ) -> bool
{
   return std::isalpha( static_cast<unsigned char>( ch ) );
}

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


auto
int_to_binary_string( int number ) -> std::string
{
   static constexpr auto base = 2;

   auto stack = std::vector<char>();

   while ( number >= base )
   {
      int  const  remainder = number % base;
      char const digit      = digit_to_char( remainder, base );

      stack.push_back( digit );
      number = ( number - remainder ) / base;
   }

   stack.push_back( digit_to_char( number, base ) );

   return std::string( stack.rbegin(), stack.rend() );
}


auto
digit_to_char( int const digit, int const base ) -> char
{
   assert( base > 0 && "Error: base must be greater than zero" );
   assert( digit < base && "Error: digit must be less than base" );

   if ( base <= 10 || digit < 10 )
   {
        // ASCII code 48 is '0'
      return static_cast< char >( digit + 48 );
   }

     // numbers starting at 10 are represented with letters starting at A which is ASCII code 65
   return static_cast< char >( 55 + digit );
}


auto 
to_binary16( int const number )  -> std::optional<std::string>
{
   auto const tmp     = int_to_binary_string( number );
   auto const length  = tmp.size();

   if ( length > 16 )             // overflow check
      return std::nullopt;
      
   auto const padding = 16 - length;

   return std::optional<std::string>( std::string( padding, '0' ) + tmp );
}

auto 
to_binary_16_string( std::string_view const number )  -> std::optional<std::string>
{
   // convert to uint16_t
   auto const result = Hack::Utils::to_uint16_t( number );

   if ( !result || ( *result > 32'767 ) )
   {
      return std::nullopt;
   }

   // convert to binary string
   return to_binary16( *result );
}

// instruction must not be empty
auto
parse_c_instruction( std::string_view instruction ) -> std::tuple<std::string, std::string, std::string>
{
   assert( instruction.size() != 0 );

   auto current_pos = 0ul;

   // parse dest
   auto const equal_pos = instruction.find( '=' );
   auto const has_equal = equal_pos != std::string::npos;
   auto const dest_end  = has_equal ? equal_pos : 0;
   auto const dest      = instruction.substr( current_pos, dest_end );

   current_pos = dest_end;

   if ( has_equal )
      ++current_pos; // advance past equal sign

   // parse comp
   auto const semicolon_pos = instruction.find( ';', current_pos );
   auto const has_semicolon = semicolon_pos != std::string::npos;
   auto const comp_end      = has_semicolon ? semicolon_pos : instruction.size();
   auto const comp_size     = comp_end - current_pos;
   auto const comp          = instruction.substr( current_pos, comp_size );

   current_pos = comp_end;

   if ( has_semicolon )
      ++current_pos;

   auto const jump_size = instruction.size() - current_pos;
   auto const jump      = instruction.substr( current_pos, jump_size );

   return { std::string( dest ),  std::string( comp ),  std::string( jump ) };
}

}  // namespace