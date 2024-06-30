/**
 * @file    InternalsDisplay.h
 * @author  William Weston (wjtWeston@protonmail.com)
 * @brief   Display the internals of the Hack CPU and allow access to locations of text boxes for animation
 * @version 0.1
 * @date    2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef HACK_PROJECT_INCLUDE_2024_06_29_INTERNALSDISPLAY_H
#define HACK_PROJECT_INCLUDE_2024_06_29_INTERNALSDISPLAY_H

#include "Definitions.h"                     // DataLocation, Format

#include "Hack/Computer.h" 



namespace Hack
{

class InternalsDisplay final
{
public:
   explicit InternalsDisplay( Computer& computer );

   // update state of GUI elements for next animation frame
   auto update( Format fmt )       -> void;

   auto pc_location()        const -> DataLocation;
   auto a_location()         const -> DataLocation;
   auto d_location()         const -> DataLocation;
   auto m_location()         const -> DataLocation;


private:
   Computer&    computer_;
   DataLocation pc_location_{};
   DataLocation a_location_ {};
   DataLocation d_location_ {};
   DataLocation m_location_ {};

   auto do_update( Format fmt ) -> void;
};

}  // namespace Hack


#endif   // HACK_PROJECT_INCLUDE_2024_06_29_INTERNALSDISPLAY_H