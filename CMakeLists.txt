# =====================================
# Preamble
# =====================================

cmake_minimum_required( VERSION 3.29 )

project( Hack_Computer_Project 
		VERSION 			0.0.1.0
		DESCRIPTION		"Hack Computer Project"
		HOMEPAGE_URL 	"www.MyProj.com"
		LANGUAGES		CXX
)

list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

include( PreventInSourceBuilds )

 
# =====================================
# Project Wide Setup
# =====================================

set( CMAKE_CXX_STANDARD 23 )
set( CMAKE_CXX_STANDARD_REQUIRED YES )
set( CMAKE_CXX_EXTENSIONS NO )

set( CMAKE_THREAD_PREFER_PTHREAD TRUE )
set( THREADS_PREFER_PTHREAD_FLAG TRUE )

# Generate compile_commands.json, enhance error reporting & compiler messages, and set default build
include( StandardProjectSettings )

# link to Hack::BuildInfo to add build info to target, then add #include "Hack/BuildInfo.h"
include( BuildInfo )
add_library( Hack::BuildInfo ALIAS BuildInfo )

include( ProjectOptions )
project_options()

add_library( Hack::project_warnings ALIAS project_warnings )
add_library( Hack::project_options ALIAS project_options )

include( Doxygen )
Enable_Doxygen()

if( PROJECT_IS_TOP_LEVEL )
   include( CTest )
endif()

enable_testing()

# =====================================
# Externally Provided Content
# =====================================

include( dependencies.cmake )
add_subdirectory( external )


# =====================================
# Main Targets Built By This Project
# =====================================

add_subdirectory( src )

# Not typically needed if there is a parent project
if( PROJECT_IS_TOP_LEVEL )
	add_subdirectory( tests )
#	add_subdirectory( packaging )
endif() 

