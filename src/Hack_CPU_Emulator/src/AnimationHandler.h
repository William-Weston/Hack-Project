/**
 * @file    AnimationHandler.h
 * @author  William Weston (wjtWeston@protonmail.com)
 * @brief   Handle program animations
 * @version 0.1
 * @date   2024-06-28
 * 
 * @copyright Copyright (c) 2024
 * 
 *    Animations are grouped in stages.  
 *    Each stage is animated as a group.
 *    The next group begins when the last animation in the previous group completes
 * 
 *    Accepted animation types must implement the interface define by animation_t
 */
#ifndef HACK_PROJECT_INCLUDE_2024_06_28_ANIMATIONHANDLER_H
#define HACK_PROJECT_INCLUDE_2024_06_28_ANIMATIONHANDLER_H

#include "animation_t.hpp"

#include <functional>
#include <type_traits>      // is_invocable
#include <utility>          // move
#include <vector>

namespace Hack
{

class AnimationHandler;

template <typename F>
concept AnimationOp = requires( F )
{
   requires std::is_invocable_v<F, AnimationHandler&> && 
            !std::is_invocable_v<F, AnimationHandler>;
};


class AnimationHandler final
{
public:
   AnimationHandler()  = default;
   ~AnimationHandler() = default;

   // Operation must have the signatrure: void (AnimationHandler& )
   template <AnimationOp Operation>
   auto handle( Operation op )               -> void;

   auto update( float speed )                -> void;
   auto draw()                         const -> void;
   
   // add an animation to the current animation stage
   auto add( auto animation )                -> void;

   // begin a new animation stage
   auto next()                               -> void;

   // returns true when all animations have completed
   auto is_done()                      const -> bool;

   
   AnimationHandler( AnimationHandler const& )                      = delete;
   AnimationHandler( AnimationHandler&&      )                      = delete;
   auto operator=  ( AnimationHandler const& ) -> AnimationHandler& = delete;
   auto operator=  ( AnimationHandler&&      ) -> AnimationHandler& = delete;

private:
   using operation_t      = std::function<void ( AnimationHandler& )>;
   using animationStage_t = std::vector<animation_t>;
   using index_t          = std::vector<animationStage_t>::size_type;

   // operation must be delayed for times when the highlighted source and/or destination item
   // is out of view and must be scrolled to during the next animation frame
   static constexpr int          DELAY{ 3 };

   std::vector<animationStage_t> animations_{};
   operation_t                   operation_ {};
   int                           delay_     { DELAY };
   index_t                       idx_       { 0 };
   index_t                       current_   { 0 };
   bool                          done_      { false };
   bool                          processing_{ false };
   
   auto process() -> void;
};


template <AnimationOp Operation> auto
AnimationHandler::handle( Operation op ) -> void
{
   operation_  = std::move( op );
   processing_ = true;
}


auto 
AnimationHandler::add( auto animation )       -> void
{
   if ( animations_.empty() )  { animations_.push_back( animationStage_t{} ); }
   
   animations_[idx_].emplace_back( std::move( animation ) );
}

}  // namespace Hack

#endif   // HACK_PROJECT_INCLUDE_2024_06_28_ANIMATIONHANDLER_H