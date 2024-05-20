cmake_minimum_required( VERSION 3.26 )

# =====================================
# Define Targets
# =====================================


add_library( Hack_Disassembler )
add_library( Hack::Disassembler ALIAS Hack_Disassembler )

target_sources( Hack_Disassembler 
   PRIVATE
      src/Disassembler.cpp
)

set_target_properties( Hack_Disassembler 
   PROPERTIES 
      PUBLIC_HEADER include/Hack/Disassembler.h 
)

target_include_directories( Hack_Disassembler
   PUBLIC 
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
      "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
   PRIVATE
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Hack>"
)

target_link_libraries( Hack_Disassembler 
   PRIVATE
      Hack::project_warnings
      Hack::Utilities
)

include( Coverage )
CleanCoverage( Hack_Disassembler )
EnableCoverage( Hack_Disassembler )


# =====================================
# 	TEST SUITE
# =====================================


add_executable( Hack_Disassembler_Tests )

target_sources( Hack_Disassembler_Tests
   PRIVATE
      src/Disassembler.t.cpp
)


target_link_libraries( Hack_Disassembler_Tests 
   PRIVATE 
      Catch2::Catch2 
      Catch2::Catch2WithMain
      Hack::project_warnings
      Hack::Disassembler
)

enable_testing()

if( PROJECT_IS_TOP_LEVEL )
   include( CTest )
endif()

include( Coverage )
AddCoverage( Hack_Disassembler_Tests )

include( Catch )
catch_discover_tests( Hack_Disassembler_Tests )
