/**
 * @file Utilities.cpp
 * @author William Weston (wjtWeston@protonmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Utilities.h"
#include "Hack/Assembler.h"            // Assembler

#include <fstream>                     // ifstream

auto 
Hack::EMULATOR::Utils::open_hack_file( std::string const& path ) -> std::vector<std::uint16_t>
{
   auto input = std::ifstream( path );

   if ( !( input && input.is_open() ) )
   {
      throw file_error( "Could not open file", FileError{ std::string( path ) } );
   }

   auto line    = std::string();
   auto data    = std::vector<std::uint16_t>();
   auto line_no = 1zu;                                      // don't confuse users with 0 line numbers

   while ( std::getline( input, line ) )
   {
      auto const binary_optional = Hack::Utils::binary_to_uint16( line );

      if ( !binary_optional )
      {
        throw Hack::Utils::parse_error( "Error parsing Hack binary file", Hack::Utils::ParseErrorData{ std::move( line ), line_no } );
      }

      data.push_back( *binary_optional );
      ++line_no;
   }

   return data;
}

auto
Hack::EMULATOR::Utils::open_asm_file( std::string const& path )  -> std::vector<std::uint16_t>
{
   auto input = std::ifstream( path );

   if ( !( input && input.is_open() ) )
   {
      throw file_error( "Could not open file", FileError{ std::string( path ) } );
   }

   auto asmblr = Hack::Assembler();

   auto const results = asmblr.assemble( input );

   auto data = std::vector<std::uint16_t>();

   for ( auto const& line : results )
   {
      auto const binary_optional = Hack::Utils::binary_to_uint16( line );

      // the assembler should always produce correct binary strings so this should never occur
      if ( !binary_optional )
      {
         throw file_error( "Could not open file", FileError{ std::string( path ) } );
      }

      data.push_back( *binary_optional );
   }
   
   return data;
}
