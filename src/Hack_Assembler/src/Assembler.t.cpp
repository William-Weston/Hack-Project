/**
 * @file    Assembler.t.cpp
 * @author  William Weston
 * @brief   Test file for Assembler.h
 * @version 0.1
 * @date    2024-03-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Hack/Assembler.h"

#include "Hack/Utilities/exceptions.hpp"      // parse_error

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <catch2/catch_all.hpp>


TEST_CASE( "Assembler: Basic assemble istream" )
{
   auto const data = std::string
   ( 
      "// Computes R0 = 2 + 3  (R0 refers to RAM[0])\n"
      "\n"
      "@SCREEN // line comment\n"
      "@KBD\n"
      "@SP\n"
      "@LCL\n"
      "@ARG\n"
      "@THIS\n"
      "@THAT\n"
      "@R0\n"
      "@R15\n"
      "@n\n"
      "@m\n"
      "@2\n"
      "D=A\n"
      "@3\n"
      "D=D+A\n"
      "@0\n"
      "M=D\n"
      "A=D+M\n"
      "// comment"
   );

   auto iss = std::istringstream( data );
   
   auto assembler    = Hack::Assembler();
   auto const result = assembler.assemble( iss );

   SECTION( "Size should be 18" )
   {
      REQUIRE( result.size() == 18 );
   }

   SECTION( "expected results" )
   {
      auto const expected = std::vector<std::string>
      {
         "0100000000000000",     // @SCREEN
         "0110000000000000",     // @KBD
         "0000000000000000",     // @SP
         "0000000000000001",     // @LCL 
         "0000000000000010",     // @ARG
         "0000000000000011",     // @THIS 
         "0000000000000100",     // @THAT
         "0000000000000000",     // @R0
         "0000000000001111",     // @R15
         "0000000000010000",     // @n    -  variable: 16
         "0000000000010001",     // @m    -  variable: 17
         "0000000000000010",     // @2
         "1110110000010000",     // D=A
         "0000000000000011",     // @3
         "1110000010010000",     // D=D+A
         "0000000000000000",     // @0
         "1110001100001000",     // M=D
         "1111000010100000",     // A=D+M

      };

      REQUIRE( result == expected );
   }
}


TEST_CASE( "Assembler: assemble( istream& ) - label parsing tests" )
{
   SECTION( "expect zero" )
   {
      auto const data = std::string
      (
         "(label)\n"
         "@label\n"
      );

      auto iss = std::istringstream( data );
      
      auto assembler    = Hack::Assembler();
      auto const result = assembler.assemble( iss );

      SECTION( "Size should be 1" )
      {
         REQUIRE( result.size() == 1 );
      }

      SECTION( "expected results" )
      {
         REQUIRE( result[0] == "0000000000000000" );
      }
   }

   SECTION( "expect zero" )
   {
      auto const data = std::string
      (
         "@0\n"
         "(label.first$if_true0)\n"
         "@label.first$if_true0\n"
      );

      auto iss = std::istringstream( data );
      
      auto assembler    = Hack::Assembler();
      auto const result = assembler.assemble( iss );

      SECTION( "Size should be 2" )
      {
         REQUIRE( result.size() == 2 );
      }

      SECTION( "expected results" )
      {
         REQUIRE( result[1] == "0000000000000001" );
      }
   }
}


TEST_CASE( "Assembler: assemble( istream& ) - variable parsing tests" )
{
   SECTION( "single variable" )
   {
      auto const data = std::string
      (
         "@n\n"
      );

      auto iss = std::istringstream( data );
      
      auto assembler    = Hack::Assembler();
      auto const result = assembler.assemble( iss );

      SECTION( "Size should be 1" )
      {
         REQUIRE( result.size() == 1 );
      }

      SECTION( "expected results" )
      {
         // variables are assigned starting at address 16 and increasing
         REQUIRE( result[0] == "0000000000010000" );
      }
   }

   SECTION( "single variable: multiple references" )
   {
      auto const data = std::string
      (
         "@n\n"
         "@n\n"
      );

      auto iss = std::istringstream( data );
      
      auto assembler    = Hack::Assembler();
      auto const result = assembler.assemble( iss );

      SECTION( "Size should be 2" )
      {
         REQUIRE( result.size() == 2 );
      }

      SECTION( "variables are assigned starting at address 16 and increasing" )
      {
         REQUIRE( result[0] == "0000000000010000" );
      }

      SECTION( "multiple references should return same result" )
      {
         REQUIRE( result[1] == "0000000000010000" );
      }
   }

   SECTION( "multiple variable" )
   {
      auto const data = std::string
      (
         "@n\n"
         "@m\n"
         "@variable_name\n"
      );

      auto iss = std::istringstream( data );
      
      auto assembler    = Hack::Assembler();
      auto const result = assembler.assemble( iss );

      SECTION( "Size should be 3" )
      {
         REQUIRE( result.size() == 3 );
      }

      SECTION( "expected results: variables are assigned starting at address 16 and increasing" )
      {
         REQUIRE( result[0] == "0000000000010000" );
         REQUIRE( result[1] == "0000000000010001" );
         REQUIRE( result[2] == "0000000000010010" );
      }
   }

}


TEST_CASE( "Assembler: assemble( istream& ) - failure cases" )
{
   SECTION( "exceptions" )
   {
      SECTION( "bad instruction" )
      {
         auto const data = std::string
         ( 
            "// Comment \n"
            "\n"
            "@SCREEN // line comment\n"
            "@KBD\n"
            "@SP\n"
            "@LCL\n"
            "@ARG\n"
            "@THIS\n"
            "@THAT\n"
            "The other thing  // ERROR on line no 10\n"
            "@R15\n"
            "@n\n"
            "@m\n"
            "@2\n"
            "D=A\n"
            "@3\n"
            "D=D+A\n"
            "@0\n"
            "M=D\n"
            "A=D+M\n"
            "@n\n"
            "// comment"
         );

         auto iss       = std::istringstream( data );
         auto assembler = Hack::Assembler();

         SECTION( "Unparsable data results in a Hack::Utils::parse_error exception being thrown" )
         {
            REQUIRE_THROWS_AS( assembler.assemble( iss ), Hack::Utils::parse_error );
         }

         SECTION( "parse_error should reflect line of data that generates the exception" )
         {
            try
            {
               auto const result = assembler.assemble( iss );
            }
            catch( Hack::Utils::parse_error const& error )
            {
               REQUIRE( error.data().text == "The other thing  // ERROR on line no 10" );
               REQUIRE( error.data().line_no == 10 );
            }
         }
      }

      SECTION( "no matching braces" )
      {
         auto const data = std::string
         ( 
            "// Comment \n"
            "\n"
            "(LABEL\n"
         );

         auto iss       = std::istringstream( data );
         auto assembler = Hack::Assembler();

         SECTION( "Unparsable data results in a Hack::Utils::parse_error exception being thrown" )
         {
            REQUIRE_THROWS_AS( assembler.assemble( iss ), Hack::Utils::parse_error );
         }

         SECTION( "parse_error should reflect line of data that generates the exception" )
         {
            try
            {
               auto const result = assembler.assemble( iss );
            }
            catch( Hack::Utils::parse_error const& error )
            {
               REQUIRE( error.data().text == "(LABEL" );
               REQUIRE( error.data().line_no == 3 );
            }
         }
      }

      SECTION( "A instuction with text" )
      {
         auto const data = std::string
         ( 
            "// Comment \n"
            "\n"
            "@123ASS\n"
         );

         auto iss       = std::istringstream( data );
         auto assembler = Hack::Assembler();

          SECTION( "Unparsable data results in a Hack::Utils::parse_error exception being thrown" )
         {
            REQUIRE_THROWS_AS( assembler.assemble( iss ), Hack::Utils::parse_error );
         }
      }
   }
}


TEST_CASE( "Assembler: Basic - assemble span" )
{
   auto const data = std::vector<std::string>
   { 
      "@2",
      "D=A",
      "@3",
      "D=D+A",
      "@0",
      "M=D",
      "A=D+M"
   };

   auto assembler    = Hack::Assembler();
   auto const result = assembler.assemble( data );

   SECTION( "Size should be 7" )
   {
      REQUIRE( result.size() == 7 );
   }

   SECTION( "expected results" )
   {
      auto const expected = std::vector<std::string>
      {
         "0000000000000010",     // @2
         "1110110000010000",     // D=A
         "0000000000000011",     // @3
         "1110000010010000",     // D=D+A
         "0000000000000000",     // @0
         "1110001100001000",     // M=D
         "1111000010100000",     // A=D+M
      };

      REQUIRE( result == expected );
   }
}


TEST_CASE( "Assembler: assemble( span<std::string const> ) - failure cases" )
{
   SECTION( "exceptions" )
   {
      SECTION( "bad instruction" )
      {
         auto const data = std::vector<std::string>
         { 
            "@2",
            "D=A",
            "@3",
            "D=D+Anatomy Park",
            "@0",
            "M=D",
            "A=D+M"
         };
         auto assembler = Hack::Assembler();

         SECTION( "Unparsable data results in a Hack::Utils::parse_error exception being thrown" )
         {
            REQUIRE_THROWS_AS( assembler.assemble( data ), Hack::Utils::parse_error );
         }

         SECTION( "parse_error should reflect line of data that generates the exception" )
         {
            try
            {
               auto const result = assembler.assemble( data );
            }
            catch( Hack::Utils::parse_error const& error )
            {
               REQUIRE( error.what() == std::string( "Assembly failed on instruction number 3" ) );
               REQUIRE( error.data().text == "D=D+Anatomy Park" );
               REQUIRE( error.data().line_no == 3);
            }
         }
      }

   }
}


TEST_CASE( "Assembler: Basic: - assemble_expected istream" )
{
   auto const data = std::string
   ( 
      "// Computes R0 = 2 + 3  (R0 refers to RAM[0])\n"
      "\n"
      "@SCREEN // line comment\n"
      "@KBD\n"
      "@SP\n"
      "@LCL\n"
      "@ARG\n"
      "@THIS\n"
      "@THAT\n"
      "@R0\n"
      "@R15\n"
      "@n\n"
      "@m\n"
      "@2\n"
      "D=A\n"
      "@3\n"
      "D=D+A\n"
      "@0\n"
      "M=D\n"
      "A=D+M\n"
      "@n\n"
      "@m\n"
      "// comment"
   );

   auto iss = std::istringstream( data );
   
   auto assembler    = Hack::Assembler();
   auto const result = assembler.assemble_expected( iss );

   SECTION( "expected should have value" )
   {
      REQUIRE( result );
   }

   SECTION( "Size should be 20" )
   {
      REQUIRE( result->size() == 20 );
   }

   SECTION( "expected results" )
   {
      auto const expected = std::vector<std::string>
      {
         "0100000000000000",     // @SCREEN
         "0110000000000000",     // @KBD
         "0000000000000000",     // @SP
         "0000000000000001",     // @LCL 
         "0000000000000010",     // @ARG
         "0000000000000011",     // @THIS 
         "0000000000000100",     // @THAT
         "0000000000000000",     // @R0
         "0000000000001111",     // @R15
         "0000000000010000",     // @n    -  variable: 16
         "0000000000010001",     // @m    -  variable: 17
         "0000000000000010",     // @2
         "1110110000010000",     // D=A
         "0000000000000011",     // @3
         "1110000010010000",     // D=D+A
         "0000000000000000",     // @0
         "1110001100001000",     // M=D
         "1111000010100000",     // A=D+M
         "0000000000010000",     // @n    -  variable: 16
         "0000000000010001"      // @m    -  variable: 17

      };

      REQUIRE( *result == expected );
   }
}


TEST_CASE( "Assembler: assemble_expected( istream ) - expect failure" )
{
   auto const data = std::string
   ( 
      "// Computes R0 = 2 + 3  (R0 refers to RAM[0])\n"
      "\n"
      "@SCREEN // line comment\n"
      "@KBD\n"
      "@SP\n"
      "@LCL\n"
      "@ARG\n"
      "@THIS\n"
      "@THAT\n"
      "The other thing  // ERROR on line no 10\n"
      "@R15\n"
      "@n\n"
      "@m\n"
      "@2\n"
      "D=A\n"
      "@3\n"
      "D=D+A\n"
      "@0\n"
      "M=D\n"
      "A=D+M\n"
      "@n\n"
      "// comment"
   );

   auto iss = std::istringstream( data );
   
   auto assembler    = Hack::Assembler();
   auto const result = assembler.assemble_expected( iss );

   SECTION( "expected should not have value" )
   {
      REQUIRE( !result );
   }

   SECTION( "Error of type Code_Line should contain correct line no" )
   {
      REQUIRE( result.error().line_no == 10 );
   }

   SECTION( "Error of type Code_Line should contain text of line that caused failure" )
   {
      REQUIRE( result.error().text == "The other thing  // ERROR on line no 10" );
   }

}


TEST_CASE( "Assembler:  assemble( string_view )" )
{
   auto assembler = Hack::Assembler();

   // @xxx
   //    •  where 'xxx' is a decimal value in the range (0, 32'767)
   SECTION( "a instructions" )
   {
      SECTION( "@0" )
      {
         auto const result = assembler.assemble( "@0" );

         REQUIRE( result );

         REQUIRE( *result == "0000000000000000" );
      }

      SECTION( "@1" )
      {
         auto const result = assembler.assemble( "@1" );

         REQUIRE( result );

         REQUIRE( *result == "0000000000000001" );
      }

      SECTION( "@23" )
      {
         auto const result = assembler.assemble( "@23" );

         REQUIRE( result );

         REQUIRE( *result == "0000000000010111" );
      }

      // largerst 15-bit number that can be used in an a instruction
      SECTION( "@32767" )
      {
         auto const result = assembler.assemble( "@32767" );

         REQUIRE( result );

         REQUIRE( *result == "0111111111111111" );
      }

      // only 15 bit values can be used in an a instruction
      SECTION( "@32768" )
      {
         auto const result = assembler.assemble( "@32768" );

         REQUIRE( !result );
      }

      // negative numbers can not be used in an a instruction
      SECTION( "@-1" )
      {
         auto const result = assembler.assemble( "@-1" );

         REQUIRE( !result );
      }
   }

   
   // dest=comp;jump
   //    •  'comp' is manditory
   //    •  if 'dest' is empty the '=' is omitted
   //    •  if 'jump' is empty the ';' is omitted
   SECTION( "c instructions" )
   {
      SECTION( "comp tests" )
      {
         SECTION( "0" )
         {
            auto const result = assembler.assemble( "0" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110101010000000" );
         }

         SECTION( "1" )
         {
            auto const result = assembler.assemble( "1" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110111111000000" );
         }

         SECTION( "-1" )
         {
            auto const result = assembler.assemble( "-1" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110111010000000" );
         }

         SECTION( "D" )
         {
            auto const result = assembler.assemble( "D" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110001100000000" );
         }

         SECTION( "A" )
         {
            auto const result = assembler.assemble( "A" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110110000000000" );
         }

         SECTION( "M" )
         {
            auto const result = assembler.assemble( "M" );
            
            REQUIRE( result );
            REQUIRE( *result == "1111110000000000" );
         }

         SECTION( "!D" )
         {
            auto const result = assembler.assemble( "!D" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110001101000000" );
         }

         SECTION( "!A" )
         {
            auto const result = assembler.assemble( "!A" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110110001000000" );
         }

         SECTION( "!M" )
         {
            auto const result = assembler.assemble( "!M" );
            
            REQUIRE( result );
            REQUIRE( *result == "1111110001000000" );
         }

         SECTION( "-D" )
         {
            auto const result = assembler.assemble( "-D" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110001111000000" );
         }

         SECTION( "-A" )
         {
            auto const result = assembler.assemble( "-A" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110110011000000" );
         }

         SECTION( "-M" )
         {
            auto const result = assembler.assemble( "-M" );
            
            REQUIRE( result );
            REQUIRE( *result == "1111110011000000" );
         }

         SECTION( "D+1" )
         {
            auto const result = assembler.assemble( "D+1" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110011111000000" );
         }

         SECTION( "A+1" )
         {
            auto const result = assembler.assemble( "A+1" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110110111000000" );
         }

         SECTION( "M+1" )
         {
            auto const result = assembler.assemble( "M+1" );
            
            REQUIRE( result );
            REQUIRE( *result == "1111110111000000" );
         }

         SECTION( "D-1" )
         {
            auto const result = assembler.assemble( "D-1" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110001110000000" );
         }

         SECTION( "A-1" )
         {
            auto const result = assembler.assemble( "A-1" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110110010000000" );
         }

         SECTION( "M-1" )
         {
            auto const result = assembler.assemble( "M-1" );
            
            REQUIRE( result );
            REQUIRE( *result == "1111110010000000" );
         }

         SECTION( "D+A" )
         {
            auto const result = assembler.assemble( "D+A" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110000010000000" );
         }

         SECTION( "D+M" )
         {
            auto const result = assembler.assemble( "D+M" );
            
            REQUIRE( result );
            REQUIRE( *result == "1111000010000000" );
         }

         SECTION( "D-A" )
         {
            auto const result = assembler.assemble( "D-A" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110010011000000" );
         }

         SECTION( "D-M" )
         {
            auto const result = assembler.assemble( "D-M" );
            
            REQUIRE( result );
            REQUIRE( *result == "1111010011000000" );
         }

         SECTION( "A-D" )
         {
            auto const result = assembler.assemble( "A-D" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110000111000000" );
         }

         SECTION( "M-D" )
         {
            auto const result = assembler.assemble( "M-D" );
            
            REQUIRE( result );
            REQUIRE( *result == "1111000111000000" );
         }

         SECTION( "D&A" )
         {
            auto const result = assembler.assemble( "D&A" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110000000000000" );
         }

         SECTION( "D&M" )
         {
            auto const result = assembler.assemble( "D&M" );
            
            REQUIRE( result );
            REQUIRE( *result == "1111000000000000" );
         }

         SECTION( "D|A" )
         {
            auto const result = assembler.assemble( "D|A" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110010101000000" );
         }

         SECTION( "D|M" )
         {
            auto const result = assembler.assemble( "D|M" );
            
            REQUIRE( result );
            REQUIRE( *result == "1111010101000000" );
         }
      }
   
      SECTION( "dest tests" )
      {
         SECTION( "M" )
         {
            auto const result = assembler.assemble( "M=0" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110101010001000" );
         }

         SECTION( "D" )
         {
            auto const result = assembler.assemble( "D=0" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110101010010000" );
         }

         SECTION( "DM" )
         {
            auto const result = assembler.assemble( "DM=0" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110101010011000" );
         }

         SECTION( "A" )
         {
            auto const result = assembler.assemble( "A=0" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110101010100000" );
         }

         SECTION( "AM" )
         {
            auto const result = assembler.assemble( "AM=0" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110101010101000" );
         }

         SECTION( "AD" )
         {
            auto const result = assembler.assemble( "AD=0" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110101010110000" );
         }

         SECTION( "ADM" )
         {
            auto const result = assembler.assemble( "ADM=0" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110101010111000" );
         }
      }
   
      SECTION( "jump tests" )
      {
         SECTION( "JGT" )
         {
            auto const result = assembler.assemble( "0;JGT" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110101010000001" );
         }

         SECTION( "JEQ" )
         {
            auto const result = assembler.assemble( "0;JEQ" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110101010000010" );
         }

         SECTION( "JGE" )
         {
            auto const result = assembler.assemble( "0;JGE" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110101010000011" );
         }

         SECTION( "JLT" )
         {
            auto const result = assembler.assemble( "0;JLT" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110101010000100" );
         }

         SECTION( "JNE" )
         {
            auto const result = assembler.assemble( "0;JNE" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110101010000101" );
         }

         SECTION( "JLE" )
         {
            auto const result = assembler.assemble( "0;JLE" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110101010000110" );
         }

         SECTION( "JMP" )
         {
            auto const result = assembler.assemble( "0;JMP" );
            
            REQUIRE( result );
            REQUIRE( *result == "1110101010000111" );
         }
      }
   }
}


TEST_CASE( "Assembler: assemble( string_view ) - Expect Failure" )
{
   auto assembler = Hack::Assembler();

   SECTION( "spaces not permitted" )
   {
      REQUIRE( !assembler.assemble( "A = D" ) );
      REQUIRE( !assembler.assemble( "0; JMP" ) );
      REQUIRE( !assembler.assemble( "A = M + D" ) );
      REQUIRE( !assembler.assemble( "@ 1234" ) );
   }

   SECTION( "bad dest" )
   {
      REQUIRE( !assembler.assemble( "0=0" ) );
      REQUIRE( !assembler.assemble( "Z=0;JMP" ) );
      REQUIRE( !assembler.assemble( "-1=0" ) );
   }

   SECTION( "bad comp" )
   {
      SECTION( "invalid register" )
      {
         REQUIRE( !assembler.assemble( "Z" ) );
         REQUIRE( !assembler.assemble( "5" ) );
         REQUIRE( !assembler.assemble( "A-S" ) );
      }
      
      SECTION( "invalid operation" )
      {
         REQUIRE( !assembler.assemble( "A*D" ) );
         REQUIRE( !assembler.assemble( "A/D" ) );
         REQUIRE( !assembler.assemble( "M%D" ) );
      }

      SECTION( "spaces not permitted" )
      {
         REQUIRE( !assembler.assemble( "A + D" ) );
         REQUIRE( !assembler.assemble( "A - D" ) ); 
      }
   }

   SECTION( "bad jmp" )
   {
      SECTION( "Must contain comp section" )
      {
         REQUIRE( !assembler.assemble( ";JMP" ) );
         REQUIRE( !assembler.assemble( "JMP" ) );
      }

      SECTION( "invalid jump instruction" )
      {
         REQUIRE( !assembler.assemble( "0;JMPY" ) );
         REQUIRE( !assembler.assemble( "0;-JMP" ) );
         REQUIRE( !assembler.assemble( "0;0" ) );
         REQUIRE( !assembler.assemble( "0;JMP to the moon" ) );
      }
   }
}