function( CleanCoverage target )

   message( STATUS "Cleaning .gcda files for ${target}" )
   add_custom_command( TARGET ${target} 
      PRE_BUILD 
      COMMAND
         find ${CMAKE_BINARY_DIR} -type f -name '*.gcda' -exec rm {} +
   )
   
endfunction()


function( EnableCoverage target )

   message( STATUS "Enabling Coverage on ${target}" )
   if ( CMAKE_BUILD_TYPE STREQUAL Debug 
        AND 
        ( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" ) 
      )
      
      target_compile_options( ${target} PRIVATE --coverage -fno-inline )
      target_link_options( ${target} PUBLIC --coverage )
   endif()

endfunction()


function( AddCoverage target )

   message( STATUS "Adding Coverage to ${target}" )

   find_program( LCOV_PATH lcov REQUIRED )
   find_program( GENHTML_PATH genhtml REQUIRED )

   add_custom_target( coverage-${target}
      COMMENT "Running coverage for ${target}..."
      COMMAND ${LCOV_PATH} -d . --zerocounters
      COMMAND $<TARGET_FILE:${target}>
      COMMAND ${LCOV_PATH} -d . --capture -o coverage.info 
      COMMAND ${LCOV_PATH} -r coverage.info '/usr/include/*' 
                           -o filtered.info
      COMMAND ${GENHTML_PATH} -o coverage-${target} 
                              filtered.info --legend
      COMMAND rm -rf coverage.info filtered.info
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
   )

endfunction()