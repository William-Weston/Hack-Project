

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

      message( WARNING "clang-tidy requested for ${target} but executable not found" )

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
         PROPERTIES 
            CXX_CPPCHECK 
               "${CPPCHECK_OPTIONS}"
      )

      message( STATUS "cppcheck enabled on target: ${target}" )

   else()

      message( WARNING "cppcheck requested for ${target} but executable not found" )

   endif()

endfunction() 


function( AddIWYU target )

   find_program( IWYU_PATH NAMES iwyu include-what-you-use )

   if ( IWYU_PATH )

      set( IWYU_OPTIONS 
         "${IWYU_PATH}"
         "-Xiwyu"
         "any"
         "-Xiwyu"
         "iwyu"
         "-Xiwyu"
         "-I ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}"
      )

      set_target_properties( ${target}
         PROPERTIES 
            CXX_INCLUDE_WHAT_YOU_USE
               ${IWYU_OPTIONS}
      )

      message( STATUS "iwyu enabled on target: ${target}" )

   else()

      message( WARNING "iwyu requested but executable not found" )

   endif()

endfunction()


function( AddLWYU target )

   if( CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" )
   
      set_target_properties( ${target}
            PROPERTIES
               LINK_WHAT_YOU_USE ON
      )

      message( STATUS "link what you use enabled on target: ${target}" )

   else()

      message( WARNING "Your compiler ${CMAKE_CXX_COMPILER_ID} does not support link what you use" )

   endif()

endfunction()

