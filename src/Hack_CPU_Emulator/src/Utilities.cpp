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

#include "Hack/Assembler.h"              // for Assembler
#include "Hack/Utilities/utilities.hpp"  // for binary_to_uint16

#include "ImGuiSugar/imgui_sugar.hpp"    // for with_

#include <fstream>                       // for basic_ifstream, basic_istream
#include <optional>                      // for optional
#include <utility>                       // for unreachable




/**
 * @brief Open a hack binary text file from the given path
 * 
 * @param path the path to open
 * @return std::vector<std::uint16_t> 
 * @throws Hack::EMULATOR::Utils::Parse_Error
 */
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
        throw Hack::Utils::parse_error( "Error parsing Hack binary file", Hack::Utils::ParseErrorData{ line, line_no } );
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



auto 
Hack::GUI::Utils::error_popup( std::string_view description, std::string_view msg ) -> bool
{
   auto done       = false;
   auto const width = std::max( ImGui::CalcTextSize( description.data() ).x, ImGui::CalcTextSize( msg.data() ).x ) + 15.0f;
   ImGui::SetNextWindowSize( ImVec2{ width, 0.0 } );

   with_StyleVar( ImGuiStyleVar_PopupRounding, 10.0 )
   with_StyleVar( ImGuiStyleVar_WindowTitleAlign, ImVec2{ 0.5, 0.5 } )
   {
      auto popup_open = true;
      with_PopupModal( "Error", &popup_open )
      {
         CentreTextUnformatted( description.data() );
         ImGui::Spacing();
         ImGui::TextUnformatted( msg.data() );

         ImGui::Spacing();
         ImGui::Spacing();
         
         if ( CentreButton( " Done " ) )
         {
            ImGui::CloseCurrentPopup();
            done = true;
         }
      }
   }
   return done;
}

auto
Hack::GUI::Utils::CentreTextUnformatted( std::string_view text, float alignment ) -> void
{
   auto const size   = ImGui::CalcTextSize( text.data() ).x;
   auto const avail  = ImGui::GetContentRegionAvail().x;
   auto const offset = ( avail - size ) * alignment;

   if ( offset > 0.0 )
   {
      ImGui::SetCursorPosX( ImGui::GetCursorPosX() + offset );
   }

   ImGui::TextUnformatted( text.data() );
}

auto
Hack::GUI::Utils::CentreButton( std::string_view text, float alignment ) -> bool
{
   auto const size   = ImGui::CalcTextSize( text.data() ).x + ImGui::GetStyle().FramePadding.x * 2.0f;
   auto const avail  = ImGui::GetContentRegionAvail().x;
   auto const offset = ( avail - size ) * alignment;

   if ( offset > 0.0 )
   {
      ImGui::SetCursorPosX( ImGui::GetCursorPosX() + offset );
   }

   return ImGui::Button( text.data() );
}

