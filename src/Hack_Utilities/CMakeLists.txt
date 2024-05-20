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
)

include( Coverage )
CleanCoverage( Hack_Utilities )
EnableCoverage( Hack_Utilities )


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