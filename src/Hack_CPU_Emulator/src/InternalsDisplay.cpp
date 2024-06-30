/**
 * @file    InternalsDisplay.cpp
 * @author  William Weston (wjtWeston@protonmail.com)
 * @brief   Source file for InternalsDisplay.h
 * @version 0.1
 * @date    2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "InternalsDisplay.h"

#include "DisplayFormats.hpp"
#include "Utilities.h"

Hack::InternalsDisplay::InternalsDisplay( Computer& computer )
   : computer_{ computer }
{}

auto 
Hack::InternalsDisplay::update( Format fmt ) -> void
{
   // Ensure enough space to draw table columns, else imgui will crash
   if ( ImGui::GetContentRegionAvail().x > 50.0f )
      do_update( fmt );
}

auto 
Hack::InternalsDisplay::pc_location() const -> DataLocation
{
   return pc_location_;
}


auto 
Hack::InternalsDisplay::a_location() const -> DataLocation
{
   return a_location_;
}


auto 
Hack::InternalsDisplay::d_location() const -> DataLocation
{
   return d_location_;
}


auto 
Hack::InternalsDisplay::m_location() const -> DataLocation
{
   return m_location_;
}

auto
Hack::InternalsDisplay::do_update( Format fmt ) -> void
{
   ImGui::Columns( 5 );

   auto const offset = ( ImGui::GetContentRegionAvail().x - ITEM_WIDTH ) * 0.5f; // must be after call to Columns

   GUI::Utils::CentreTextUnformatted( "--- Program Counter ---" );

   auto& pc = computer_.pc();
   ImGui::SetCursorPosX( ImGui::GetCursorPosX() + offset );
   with_ID( "pc" )
   Formats::update_item( pc, fmt );

   // TODO:  update string based on format
   pc_location_ = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), std::to_string( pc ) };

   ImGui::NextColumn();

   GUI::Utils::CentreTextUnformatted( "--- A Register ---" );

   auto& a_register = computer_.A_Register();
   ImGui::SetCursorPosX( ImGui::GetCursorPosX() + offset );
   with_ID( "a_register" )
   Formats::update_item( a_register, fmt );

   // TODO:  update string based on format
   a_location_ = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), std::to_string( pc ) };

   ImGui::NextColumn();

   GUI::Utils::CentreTextUnformatted( "--- D Register ---" );

   auto& d_register = computer_.D_Register();
   ImGui::SetCursorPosX( ImGui::GetCursorPosX() + offset );
   with_ID( "d_register" )
   Formats::update_item( d_register, fmt );

   // TODO:  update string based on format
   d_location_ = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), std::to_string( pc ) };

   ImGui::NextColumn();

   GUI::Utils::CentreTextUnformatted( "--- M Register ---" );

   auto const label    = std::string( "RAM[" ) + std::to_string( a_register ) + std::string( "]:" );
   auto const offset_m = ( ImGui::GetContentRegionAvail().x - ITEM_WIDTH - ImGui::CalcTextSize( label.data() ).x - ImGui::GetStyle().ItemSpacing.x ) * 0.5f;
   
   ImGui::SetCursorPosX( ImGui::GetCursorPosX() + offset_m );
   ImGui::AlignTextToFramePadding();
   ImGui::TextUnformatted( label.data() );
   ImGui::SameLine();

   if ( a_register < Computer::RAM_SIZE )
   {
      auto& m_register = computer_.M_Register();
      with_ID( "m_register" )
      Formats::update_item( m_register, fmt );
   }
   else
   {
      ImGui::SetNextItemWidth( ITEM_WIDTH );
      with_ID( "m_register" )
      ImGui::TextUnformatted( "N/A" );
   }
   
   // TODO:  update string based on format
   m_location_ = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), std::to_string( pc ) };

   ImGui::NextColumn();

   GUI::Utils::CentreTextUnformatted( "--- Keyboard ---" );
   ImGui::Spacing();
   auto const keyboard = std::to_string( computer_.keyboard() );
   auto const kb_offset = ( ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize( keyboard.data() ).x ) * 0.5f;
   ImGui::SetCursorPosX( ImGui::GetCursorPosX() + kb_offset );
   ImGui::TextUnformatted( keyboard.data() );
}