/**
 * @file    Disassembler.t.cpp
 * @author  William Weston
 * @brief   Test file for Disassembler.h
 * @version 0.1
 * @date    2024-03-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Hack/Disassembler.h"

#include <catch2/catch_all.hpp>


TEST_CASE( "Disassembler: disassemble( string_view )" )
{
   using namespace Hack;

   SECTION( "a instructions" )
   {   
      SECTION( "0000000000000000" )
      {
         auto const result = Disassembler::disassemble( "0000000000000000" );

         REQUIRE( result );

         REQUIRE( *result == "@0" );
      }

      SECTION( "0000000000000001" )
      {
         auto const result = Disassembler::disassemble( "0000000000000001" );

         REQUIRE( result );

         REQUIRE( *result == "@1" );
      }

      SECTION( "0000000000010111" )
      {
         auto const result = Disassembler::disassemble( "0000000000010111" );

         REQUIRE( result );

         REQUIRE( *result == "@23" );
      }

      SECTION( "0000000000101010" )
      {
         auto const result = Disassembler::disassemble( "0000000000101010" );

         REQUIRE( result );

         REQUIRE( *result == "@42" );
      }

      SECTION( "0111111111111111" )
      {
         auto const result = Disassembler::disassemble( "0111111111111111" );

         REQUIRE( result );

         REQUIRE( *result == "@32767" );
      }
   }

   SECTION( "c instructions" )
   {
      SECTION( "1110101010000000 : 0" )
      {
         auto const result = Disassembler::disassemble( "1110101010000000" );

         REQUIRE( result );

         REQUIRE( *result == "0" );
      }

      SECTION( "1110101010111000 : ADM=0" )
      {
         auto const result = Disassembler::disassemble( "1110101010111000" );

         REQUIRE( result );

         REQUIRE( *result == "ADM=0" );
      }

      SECTION( "1110110111111000 : ADM=A+1" )
      {
         auto const result = Disassembler::disassemble( "1110110111111000" );

         REQUIRE( result );

         REQUIRE( *result == "ADM=A+1" );
      }

      SECTION( "1111110111111000 : ADM=M+1" )
      {
         auto const result = Disassembler::disassemble( "1111110111111000" );

         REQUIRE( result );

         REQUIRE( *result == "ADM=M+1" );
      }

      SECTION( "1110101010000111 : 0;JMP" )
      {
         auto const result = Disassembler::disassemble( "1110101010000111" );

         REQUIRE( result );

         REQUIRE( *result == "0;JMP" );
      }
   }

   SECTION( "invalid instructions" )
   {
      SECTION( "a instruction less than 16 bits" )
      {
         auto const result = Disassembler::disassemble( "0" );

         REQUIRE( !result );
      }

      SECTION( "c instruction less than 16 bits" )
      {
         auto const result = Disassembler::disassemble( "1" );

         REQUIRE( !result );
      }

      SECTION( "a instruction has more than 16 bits" )
      {
         auto const result = Disassembler::disassemble( "01111111111111111" );

         REQUIRE( !result );
      }

      SECTION( "c instruction has more than 16 bits" )
      {
         auto const result = Disassembler::disassemble( "11111111111111111" );

         REQUIRE( !result );
      }

      SECTION( "invalid c instruction with unsupported comp 000011" )
      {
         auto const result = Disassembler::disassemble( "11110000111111111" );

         REQUIRE( !result );
      }
   }

   SECTION( "instruction contains invalid characters" )
   {
      REQUIRE( !Disassembler::disassemble( "1111000a111111111" ) );
      REQUIRE( !Disassembler::disassemble( "a1111000111111111" ) );
      REQUIRE( !Disassembler::disassemble( "1111000111111111a" ) );
      REQUIRE( !Disassembler::disassemble( "91111000111111111" ) );
      REQUIRE( !Disassembler::disassemble( "11110001111111117" ) );
      REQUIRE( !Disassembler::disassemble( " 1111000111111111" ) );
      REQUIRE( !Disassembler::disassemble( "1111000111111111 " ) );
      REQUIRE( !Disassembler::disassemble( ".1111000111111111" ) );
      REQUIRE( !Disassembler::disassemble( "\n1111000111111111" ) );
      REQUIRE( !Disassembler::disassemble( "1111000\t111111111" ) );
      REQUIRE( !Disassembler::disassemble( "1111000 111111111" ) );
      REQUIRE( !Disassembler::disassemble( "1111000111111111%" ) );
      REQUIRE( !Disassembler::disassemble( "1111000111$111111" ) );
     
   }
}


// TODO
TEST_CASE( "Disassembler: computation" )
{
   
}

// TODO
TEST_CASE( "Disassembler: destination" )
{
   
}