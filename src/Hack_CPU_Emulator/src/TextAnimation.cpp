/**
 * @file    TextAnimation.cpp
 * @author  William Weston (wjtWeston@protonmail.com)
 * @brief   Source file for TextAnimation.h
 * @version 0.1
 * @date    2024-06-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "TextAnimation.h"


#include <imgui.h>

#include <cmath>             // fabs


Hack::TextAnimation::TextAnimation( ImVec2 source_topleft, ImVec2 source_bottomright, ImVec2 target, std::string text, float speed )
   :  text_{ text },
      current_{ source_topleft }, 
      target_{ target }, 
      velocity_{ ImVec2{ ( target.x - source_topleft.x ) / FACTOR, ( target.y - source_topleft.y ) / FACTOR } },
      width_{ source_bottomright.x - source_topleft.x }, 
      height_{ source_bottomright.y - source_topleft.y },
      speed_{ speed + 1.0f }
{
}

auto 
Hack::TextAnimation::update( float speed )  -> void
{
   if ( done_ ) { return; }

   speed_ = speed + 1.0f;
   
   if ( std::fabs( target_.x - current_.x ) <= 1.0f && std::fabs( target_.y - current_.y ) <= 1.0f )
   {
      current_ = target_;
      done_    = true;
   }
   else
   {
      velocity_ = ImVec2{ ( target_.x - current_.x ) / ( FACTOR / speed_ ), 
                          ( target_.y - current_.y ) / ( FACTOR / speed_ )  };

      if ( velocity_.x < 1.0f )          velocity_.x *= 1.1f;
      if ( velocity_.y < 1.0f )          velocity_.y *= 1.1f;
     
      current_.x += velocity_.x;
      current_.y += velocity_.y;
   }
}

auto 
Hack::TextAnimation::draw()    const -> void
{
   auto draw_list    = ImGui::GetForegroundDrawList();
   auto const& style = ImGui::GetStyle();

   draw_list->AddRectFilled( current_, ImVec2{ current_.x + width_, current_.y + height_ }, ImColor{ 255, 255, 0, 245 }, style.FrameRounding );
   draw_list->AddText( ImVec2{ current_.x + style.FramePadding.x, current_.y + style.FramePadding.y }, 
                       ImColor{ 0, 0, 0 }, 
                       text_.data() 
                     );
}