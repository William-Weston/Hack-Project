macro( configure_linker target )

   include( CheckCXXCompilerFlag )

   set( USER_LINKER_OPTION "mold" CACHE STRING "Linker to be used" )
   set( USER_LINKER_OPTION_VALUES "mold" "gold" "bfd" "lld" )
   set_property( CACHE USER_LINKER_OPTION PROPERTY STRINGS ${USER_LINKER_OPTION_VALUES} )

   list(
      FIND
      USER_LINKER_OPTION_VALUES
      ${USER_LINKER_OPTION}
      USER_LINKER_OPTION_INDEX
   )

   if( ${USER_LINKER_OPTION_INDEX} EQUAL -1 )
      message( STATUS
         "Using custom linker: '${USER_LINKER_OPTION}', explicitly supported entries are ${USER_LINKER_OPTION_VALUES}" 
      )
   endif()

   if( NOT HACK_ENABLE_USER_LINKER )
      return()
   endif()

   set( LINKER_FLAG "-fuse-ld=${USER_LINKER_OPTION}" )

   check_cxx_compiler_flag( ${LINKER_FLAG} CXX_SUPPORTS_USER_LINKER )
   

   if( CXX_SUPPORTS_USER_LINKER )

      target_compile_options( ${target} INTERFACE ${LINKER_FLAG} )
      target_link_options( ${target} INTERFACE ${LINKER_FLAG} )

      message( STATUS "Using custom linker: ${USER_LINKER_OPTION}" )
      
   endif()

endmacro()
