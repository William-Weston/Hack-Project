/**
 * @file    DatumDisplay.h
 * @author  William Weston (wjtWeston@protonmail.com)
 * @brief   A widget that holds a string for displaying integral data in various formats with optional label and alignment.
 * @version 0.1
 * @date    2024-06-28
 * 
 * @copyright Copyright (c) 2024
 * 
 * Note:  This class can be made more Generic by templating on the update_item function and on the Format enum type
 *        if future use dictates.
 */
#ifndef INCLUDE_2024_06_28_HACK_DATADISPLAY_H
#define INCLUDE_2024_06_28_HACK_DATADISPLAY_H


#include "Definitions.h"                   // for DataLocation, Format, ITEM_WIDTH
#include "DisplayFormats.hpp"              // for update_item, format_asm, format_binary, format_decimal, format_hex, format_none

#include "Utilities.h"                     // for get_ImGuiDataType

#include "Hack/Assembler.h"                // for Assembler
#include "Hack/Disassembler.h"             // for Disassembler

#include "ImGuiSugar/imgui_sugar.hpp"

#include <imgui.h>                          // for ImVec2
#include <imgui_stdlib.h>                   // for ImputText

#include <SDL_log.h>  

#include <string>
#include <string_view>


namespace Hack
{

// TODO: add exceptions for out-of-range requests in select & display methods? or rely on container handle out of range?
/**
 * @brief 
 * 
 * @tparam Container 
 *         requires:
 *            - Container::size_type
 *            - Container::value_type 
 *            - Container::value_type must be an integral type
 *            - Container& container
 *                container.at( Container::size_type ) = Container::value_type
 *                Container::value_type v = container.at( Container::size_type )
 *                Container::size_type  s = container.size();
 *  
 * Scroll bug: 
 *    Occurs when 
 *       • selected_item_ is highlighted and visible on screen 
 *       • the user calls 'select' for item that is already visible on screen
 *       • the user then calls 'track' 
 *    
 *    ie:
 *       data.select( new_item );
 *       data.track();
 *    
 *    A glitch appears where the newly selected item is highlighted for a single frame at its current location
 *    and then on the next frame is moved to the scroll position.
 *    This occurs because calls to SetScrollHereY only effect the next frame.
 * 
 * Fix:   
 *    The fix is to make the 'select' function now conditionally scroll based on whether or not the newly selected item 
 *    is currently visible.
 *    Callers now just call 'select' and it will take care of tracking and scrolling
 * 
 * Remaining Bug:
 *    Occurs when newly selected item is partially visible   
 */

template <typename Container>
class DataDisplay final
{
public:
   using size_type  = Container::size_type;
   using value_type = Container::value_type;

   explicit DataDisplay( Container& data, size_type start, size_type end );

   // update state of GUI elements for next animation frame
   auto update( Format fmt, ImGuiWindowFlags flags )   -> void;

   // update selected item
   auto update_value( value_type value )               -> void;

   // select item to highlight
   auto select( size_type index )                      -> void;

   // highlight selected item
   auto highlight()                                    -> void;

   // unhighlight selected item
   auto unhighlight()                                  -> void;

   // scroll to specified item
   auto display( size_type index )                     -> void;

   // scroll to selected item
   auto track()                                        -> void;

   // is selected item visible
   auto is_selected_visible()                    const -> bool;

   // return selected item
   auto selected()                               const -> size_type;

   // set the position to scroll to, must be in the range (0.0f, 1.0f)
   auto set_scroll( float scroll )                     -> void;

   // returns location and value of selected item
   [[nodiscard]]
   auto get_data_location()                      const -> DataLocation;
 

private:
   Container&   data_;
   DataLocation data_location_{};
   size_type    start_;                // first element of container to display
   size_type    end_;                  // one past the last element of the container to display
   size_type    selected_item_{ 0 };
   size_type    display_item_{ 0 };
   float        scroll_{ 0.25f };
   bool         highlight_{ true };
   bool         track_selected_{ false };
   bool         track_displayed_{ false };
   bool         update_selected_{false };
   bool         is_selected_visible_{};
};


// ------------------------------------- Implentation ---------------------------------------------



template <typename Container>
DataDisplay<Container>::DataDisplay( Container& data, size_type start, size_type end )
   : data_{ data }, start_{ start }, end_{ end }, selected_item_{ start }, display_item_{ start }
{}


template <typename Container> auto
DataDisplay<Container>::update( Format fmt, ImGuiWindowFlags flags )   -> void
{
   with_Child( "##datadisplay_update", ImGui::GetContentRegionAvail(), ImGuiChildFlags_Border, flags )
   {
      // used to check for visibility
      auto const avail = ImGui::GetContentRegionAvail().y;

      auto clipper = ImGuiListClipper();
      clipper.Begin( static_cast<int>( end_ - start_ ) );

      if ( track_selected_ || update_selected_ )
      {
         clipper.IncludeItemByIndex( static_cast<int>( selected_item_ - start_) );
      }
      if ( track_displayed_ )
      {
         clipper.IncludeItemByIndex( static_cast<int>( display_item_ - start_) );
      }

      while( clipper.Step() )
      {
         auto const scrollY = ImGui::GetScrollY();

         for ( auto idx = clipper.DisplayStart; idx < clipper.DisplayEnd; ++idx )
         {
            auto const real_index = idx + static_cast<int>( start_ );
            auto const selected   = real_index == static_cast<int>( selected_item_ );
            auto const visible    = [&]
            {
               auto const tmp = ImGui::GetCursorPosY() - scrollY;
               return ( tmp > 0.0f && tmp < avail ) ? true : false;
            }();

            auto& value = data_.at( static_cast<DataDisplay::size_type>( real_index ) );

            with_ID( real_index )
               Formats::update_item( value, real_index, selected && highlight_, fmt );
         
            if ( selected )
            {
               is_selected_visible_ = ImGui::IsItemVisible();
              
               data_location_       = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), Hack::EMULATOR::Utils::to_string( fmt, value ) };
               is_selected_visible_ = visible;

               if ( track_selected_ || ( !visible && update_selected_ ) )
               {
                  ImGui::SetScrollHereY( scroll_ );
                  track_selected_ = false;
               }
            }

            if ( track_displayed_ && ( real_index == static_cast<int>( display_item_ ) ) )
            {
               ImGui::SetScrollHereY( scroll_ );
               track_displayed_ = false;
            }
         }
      }
   }
   update_selected_ = false;
}


template <typename Container> auto
DataDisplay<Container>::update_value( value_type value ) -> void
{
   data_.at( selected_item_ ) = value;
}


template <typename Container> auto
DataDisplay<Container>::select( size_type index )  -> void
{
   selected_item_   = index;
   update_selected_ = true;
}


template <typename Container> auto
DataDisplay<Container>::highlight()  -> void
{
   highlight_ = true;
}


template <typename Container> auto
DataDisplay<Container>::unhighlight() -> void
{
   highlight_ = false;
}


template <typename Container> auto
DataDisplay<Container>::display( size_type index ) -> void
{
   display_item_    = index;
   track_displayed_ = true;
}


template <typename Container> auto
DataDisplay<Container>::track()  -> void
{
   track_selected_ = true;
}


template <typename Container> auto
DataDisplay<Container>::is_selected_visible() const -> bool
{
   return is_selected_visible_;
}

template <typename Container> auto
DataDisplay<Container>::selected()  const -> size_type
{
   return selected_item_;
}

template <typename Container> auto
DataDisplay<Container>::set_scroll( float scroll ) -> void
{
   if ( scroll < 0.0f || scroll > 1.0f ) return;

   scroll_ = scroll;  
}


template <typename Container> auto
DataDisplay<Container>::get_data_location()  const -> DataLocation
{
   return data_location_;
}

}  // namesoace Hack


#endif  // INCLUDE_2024_06_28_HACK_DATADISPLAY_H