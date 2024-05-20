macro( enable_ipo )

   include( CheckIPOSupported )
   check_ipo_supported( RESULT result OUTPUT output )
   if( result )
      set( CMAKE_INTERPROCEDURAL_OPTIMIZATION ON )
      message( STATUS "Interprocedural Optimizations Enabled" )
   else()
      message( STATUS "Interprocedural Optimizations are not supported: ${output}" )
   endif()

endmacro()