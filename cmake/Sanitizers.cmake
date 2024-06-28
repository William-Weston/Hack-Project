function( enable_sanitizers 
   target
   ENABLE_SANITIZER_ADDRESS
   ENABLE_SANITIZER_LEAK
   ENABLE_SANITIZER_UNDEFINED
   ENABLE_SANITIZER_THREAD
) 
   if ( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" )

      set( HACK_SANITIZERS "" )

      if ( ${ENABLE_SANITIZER_ADDRESS} )
         list( APPEND HACK_SANITIZERS "address" )
      endif()

      if ( ${ENABLE_SANITIZER_LEAK} )
         list( APPEND HACK_SANITIZERS "leak" )
      endif()

      if ( ${ENABLE_SANITIZER_UNDEFINED} )
         list( APPEND HACK_SANITIZERS "undefined" )
      endif()

      if( ${ENABLE_SANITIZER_THREAD} )
         if( "address" IN_LIST HACK_SANITIZERS OR "leak" IN_LIST HACK_SANITIZERS )
            message( WARNING "Thread sanitizer can NOT be used with address and leak sanitizers. " 
                              "To enable thread sanitizer use cmake-gui to disable address and leak sanitizers "
                              "by setting HACK_ENABLE_SANITIZER_ADDRESS & HACK_ENABLE_SANITIZER_LEAK to OFF"
            )
         else()
            list( APPEND HACK_SANITIZERS "thread" )
         endif()
      endif()

   else()
      message( WARNING "Hack Project does not currently support using sanitizers with ${CMAKE_CXX_COMPILER_ID}" )
      return()
   endif()

   list( JOIN HACK_SANITIZERS "," HACK_ENABLED_SANITIZERS )

   if ( HACK_ENABLED_SANITIZERS AND NOT "${HACK_ENABLED_SANITIZERS}" STREQUAL "" )

      target_compile_options( ${target} INTERFACE -fsanitize=${HACK_ENABLED_SANITIZERS} )
      target_link_options( ${target}    INTERFACE -fsanitize=${HACK_ENABLED_SANITIZERS} )

      list( JOIN HACK_SANITIZERS ", " MESSAGE_SANITIZERS )
      message( STATUS "Enabling Sanitizers: ${MESSAGE_SANITIZERS}" )
   endif()

endfunction()
