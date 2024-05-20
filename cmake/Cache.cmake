# enable ccache if avaiable
find_program( CCACHE ccache )

if( CCACHE )
   message( STATUS "Ccache Found: Enabling..." )
   set( CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE} )
   message( STATUS "Ccache Enabled" )
else ()
  message( STATUS "Ccache not found" )
endif ()
