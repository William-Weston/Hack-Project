
/**
 * @file    DatumDisplay.h
 * @author  William Weston (wjtWeston@protonmail.com)
 * @brief 
 * @version 0.1
 * @date    2024-06-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef INCLUDE_2024_06_28_HACK_DATUMDISPLAY_H
#define INCLUDE_2024_06_28_HACK_DATUMDISPLAY_H

#include "Definitions.h"             // for DataLocation, Format
#include <string>

namespace Hack
{

class DatumDisplay final
{
public:
   explicit DatumDisplay( std::string text );
   explicit DatumDisplay( std::string text, float alignment );
   explicit DatumDisplay( std::string text, std::string label, float alignment = 0.0f );

   // update state of GUI elements for next animation frame
   auto update( Format fmt )   -> void;

   // update the text to be displayed
   auto update_text( std::string const& text )         -> void;

   // highlight/unhighlight texst
   auto highlight()                                    -> void;
   auto unhighlight()                                  -> void;

   // returns location and value of selected item
   [[nodiscard]]
   auto get_data_location()                      const -> DataLocation;

private:
   std::string  text_{};
   std::string  label_{};
   float        alignment_{ 0.0f };
   DataLocation data_location_{};
   Format       current_format_{ Format::SIGNED };
   bool         highlight_{ false };
};

}  // namespace Hack

#endif   // INCLUDE_2024_06_28_HACK_DATUMDISPLAY_H