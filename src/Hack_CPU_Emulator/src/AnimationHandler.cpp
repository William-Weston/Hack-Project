/**
 * @file    AnimationHandler.cpp
 * @author  William Weston (wjtWeston@protonmail.com)
 * @brief   Source file for AnimationHandler.h
 * @version 0.1
 * @date    2024-06-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "AnimationHandler.h"

#include "animation_t.hpp"

auto 
Hack::AnimationHandler::update( float speed )  -> void
{
   if ( processing_ )         { process();     return; }
   if ( animations_.empty() ) { done_ = false; return; }
  
   auto all_done = true;

   for ( auto idx = 0uz; idx <= current_; ++idx )
   {
      for ( auto& anim : animations_[idx] )
      {
         if ( !anim.is_done() )
         {
            all_done = false;
            anim.update( speed );
         }
      }
   }
   if ( !all_done )  return;
   
   ++current_;
   
   if ( current_ == animations_.size() )
   {
      animations_.clear();
      idx_     = 0;
      current_ = 0;
      done_    = true;
   }
}

auto 
Hack::AnimationHandler::draw() const -> void
{
   if ( animations_.empty()  )  return;

   for ( auto idx = 0uz; idx <= current_; ++idx )
   {
      for ( auto const& anim : animations_[idx] )
      { 
         anim.draw();
      }
   }
}


auto 
Hack::AnimationHandler::next() -> void
{
   animations_.push_back( animationStage_t{} );
   ++idx_;
}

auto 
Hack::AnimationHandler::is_done() const -> bool
{
   return done_;
}

auto 
Hack::AnimationHandler::process() -> void
{
   if ( delay_ > 0 )
   {
      --delay_;
      return;
   }

   delay_      = DELAY;
   processing_ = false;

   operation_( *this );
}