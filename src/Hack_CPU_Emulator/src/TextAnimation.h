/**
 * @file     TextAnimation.h
 * @author   William Weston (wjtWeston@protonmail.com)
 * @brief    A text box that moves from a source to a target location
 * @version 0.1
 * @date    2024-06-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef HACK_PROJECT_INCLUDE_2024_06_28_TEXTANIMATION_H
#define HACK_PROJECT_INCLUDE_2024_06_28_TEXTANIMATION_H

#include <imgui.h>           // for ImVec2

#include <string>

namespace Hack
{

class TextAnimation final
{
public:
   TextAnimation( ImVec2 source_topleft, ImVec2 source_bottomright, ImVec2 target, std::string text, float speed = 1.0f );

   auto update( float speed )      -> void;
   auto draw()               const -> void;
   auto is_done()            const -> bool  { return done_; }

private:
   // used to calculate animation speed
   static constexpr float FACTOR = 250.0f;

   std::string text_;
   ImVec2      current_;
   ImVec2      target_;
   ImVec2      velocity_;
   float       width_;
   float       height_;
   float       speed_;
   bool        done_{ false };
};


}  // namespace Hack

#endif   // HACK_PROJECT_INCLUDE_2024_06_28_TEXTANIMATION_H