/**
 * @file    animation_t.hpp
 * @author  William Weston (wjtWeston@protonmail.com)
 * @brief   Type erased animation
 * @version 0.1
 * @date    2024-06-28
 * 
 * @copyright Copyright (c) 2024
 * 
 * 
 * A type T must support the following interface:
 * 
 *    member functions:
 * 
 *       auto update( float )       -> void
 *       auto draw()          const -> void
 *       auto is_done()       const -> bool
 * 
 */
#ifndef HACK_PROJECT_INCLUDE_2024_06_28_ANIMATION_T_HPP
#define HACK_PROJECT_INCLUDE_2024_06_28_ANIMATION_T_HPP

#include <memory>                   // for unique_ptr

namespace Hack
{

class animation_t final
{
public:
   template <typename T>
   explicit animation_t( T t )
      : pimpl_{ std::make_unique<model<T>>( std::move( t ) ) } 
   {}

   auto update( float speed ) -> void
   {
      pimpl_->update( speed );
   }

   auto is_done() const -> bool
   {
      return pimpl_->is_done();
   }

   auto draw()  const -> void
   {
      pimpl_->draw();
   }

   
private:
   struct concept_t
   {
      virtual ~concept_t() = default;

      virtual auto update( float speed )      -> void = 0;
      virtual auto draw()               const -> void = 0;
      virtual auto is_done()            const -> bool = 0;
   };

   template <typename T>
   struct model final : concept_t
   {
      explicit model( T t )
         : animation_{ std::move( t ) }
      {}

      virtual auto update( float speed )      -> void override
      {
         animation_.update( speed );
      }

      virtual auto is_done()            const -> bool override
      {
         return animation_.is_done();
      }

      virtual auto draw()               const -> void override
      {
         animation_->draw();
      }

      T animation_;
   };

   std::unique_ptr<concept_t> pimpl_{};
};

}  // namespace Hack

#endif   // HACK_PROJECT_INCLUDE_2024_06_28_ANIMATION_T_HPP