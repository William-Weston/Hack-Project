cmake_minimum_required( VERSION 3.29 )

# =====================================
# Define Targets
# =====================================

add_library( Hack_Utilities )
add_library( Hack::Utilities ALIAS Hack_Utilities )

target_sources( Hack_Utilities 
   PRIVATE
      include/Hack/Utilities/utilities.hpp
      include/Hack/Utilities/exceptions.hpp
      src/utilities.cpp
)

set( Hack_Utilities_Public_Headers
   "include/Hack/Utilities/utilities.hpp"
   "include/Hack/Utilities/exceptions.hpp"
)

set_target_properties( Hack_Utilities 
   PROPERTIES 
      PUBLIC_HEADER "${Hack_Utilities_Public_Headers}"
)

target_include_directories( Hack_Utilities 
   PUBLIC 
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
      "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
   PRIVATE
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Hack/Utilities>"
)

target_link_libraries( Hack_Utilities
   PRIVATE 
      Hack::project_warnings
      Hack::project_options
)

include( Coverage )
CleanCoverage( Hack_Utilities )
EnableCoverage( Hack_Utilities )


# =====================================
# 	OPTIONS
# =====================================

option( HACK_UTILITIES_ENABLE_CLANGTIDY "Enable clang-tidy"        OFF )
option( HACK_UTILITIES_ENABLE_CPPCHECK  "Enable cppcheck"          OFF )
option( HACK_UTILITIES_ENABLE_IWYU      "Enable iwyu"              ON  )
option( HACK_UTILITIES_ENABLE_LWYU      "Enable link whay you use" ON  )

include( StaticAnalyzers )

add_static_analyzers( Hack_Utilities 
   HACK_UTILITIES_ENABLE_CLANGTIDY
   HACK_UTILITIES_ENABLE_CPPCHECK
   HACK_UTILITIES_ENABLE_IWYU
   HACK_UTILITIES_ENABLE_LWYU
)



# =====================================
# 	TEST SUITE
# =====================================

add_executable( Hack_Utilities_Tests )

target_sources( Hack_Utilities_Tests 
   PRIVATE
      src/utilities.t.cpp
      src/exceptions.t.cpp
)


target_link_libraries( Hack_Utilities_Tests 
   PRIVATE 
      Catch2::Catch2 
      Catch2::Catch2WithMain
      Hack::project_warnings
      Hack::project_options
      Hack::Utilities
)

include( Coverage )
AddCoverage( Hack_Utilities_Tests )

enable_testing()

if( PROJECT_IS_TOP_LEVEL )
   include( CTest )
endif()

include( Catch )
catch_discover_tests( Hack_Utilities_Tests )