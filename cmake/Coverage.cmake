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
   if ( CMAKE_BUILD_TYPE STREQUAL Debug AND 
        ( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" ) 
      )
      
      target_compile_options( ${target} PRIVATE --coverage -fno-inline -fkeep-inline-functions -fkeep-static-consts )
      target_link_options( ${target} PUBLIC --coverage )
   endif()

endfunction()


function( AddCoverage target )

   message( STATUS "Adding Coverage to ${target}" )

   find_program( LCOV_PATH lcov REQUIRED )
   find_program( GENHTML_PATH genhtml REQUIRED )

   add_custom_target( coverage-${target}
      COMMENT "Running coverage for ${target}... Entry file ${CMAKE_BINARY_DIR}/coverage-${target}/index.html"
      COMMAND ${LCOV_PATH} -d . --zerocounters 
      COMMAND $<TARGET_FILE:${target}>
      COMMAND ${LCOV_PATH} -d . --capture -o coverage-${target}.info 
      COMMAND ${LCOV_PATH} -r coverage-${target}.info '/usr/include/*' --ignore-errors unused --ignore-errors gcov
                           -o filtered-${target}.info
      COMMAND ${GENHTML_PATH} -o coverage-${target} 
                              filtered-${target}.info --legend
      COMMAND rm -rf coverage-${target}.info filtered-${target}.info
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
   )

endfunction()
