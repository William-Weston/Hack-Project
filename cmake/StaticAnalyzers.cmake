

function( AddClangTidy target )

   find_program( CLANGTIDY_PATH clang-tidy )

   if ( CLANGTIDY_PATH )

      if( NOT CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" )

         get_target_property( TARGET_PCH ${target} INTERFACE_PRECOMPILE_HEADERS )

         if( "${TARGET_PCH}" STREQUAL "TARGET_PCH-NOTFOUND" )
            get_target_property( TARGET_PCH ${target} PRECOMPILE_HEADERS )
         endif()

         if( NOT ( "${TARGET_PCH}" STREQUAL "TARGET_PCH-NOTFOUND" ) )
            message( WARNING
               "clang-tidy cannot be enabled with non-clang compiler and PCH, clang-tidy fails to handle gcc's PCH file"
            )
            return()
         endif()

      endif()

      # construct the clang-tidy command line
      set( CLANG_TIDY_OPTIONS 
         ${CLANGTIDY_PATH}
         -extra-arg=-Wno-unknown-warning-option
         -extra-arg=-Wno-ignored-optimization-argument
         -extra-arg=-Wno-unused-command-line-argument
         -p
      )

      # set standard
      if( NOT "${CMAKE_CXX_STANDARD}" STREQUAL "" )
         set( CLANG_TIDY_OPTIONS ${CLANG_TIDY_OPTIONS} -extra-arg=-std=c++${CMAKE_CXX_STANDARD} )
      endif()

      set_target_properties( ${target}
         PROPERTIES 
            CXX_CLANG_TIDY
               "${CLANG_TIDY_OPTIONS}"
      )
      
      message( STATUS "clang-tidy enabled on target: ${target}" )

   else()

      message( WARNING "clang-tidy requested but executable not found" )

   endif()

endfunction()


function( AddCppCheck target )
   
   find_program( CPPCHECK_PATH cppcheck )

   if ( CPPCHECK_PATH )

      set( CPPCHECK_OPTIONS 
         ${CPPCHECK_PATH}
         --enable=performance,warning,portability
         --inline-suppr
         # We cannot act on a bug/missing feature of cppcheck
         --suppress=cppcheckError
         --suppress=internalAstError
         # if a file does not have an internalAstError, we get an unmatchedSuppression error
         --suppress=unmatchedSuppression
         # noisy and incorrect sometimes
         --suppress=passedByValue
         # ignores code that cppcheck thinks is invalid C++
         --suppress=syntaxError
         --suppress=preprocessorErrorDirective
         --inconclusive
      )

      set_target_properties( ${target}
         PROPERTIES CXX_CPPCHECK 
            "${CPPCHECK_OPTIONS}"
      )

      message( STATUS "cppcheck enabled on target: ${target}" )
   else()

      message( WARNING "cppcheck requested but executable not found" )

   endif()

endfunction()
