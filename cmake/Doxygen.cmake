find_package( Doxygen REQUIRED )

include( FetchContent )

FetchContent_Declare( doxygen-awesome-css
  GIT_REPOSITORY
    https://github.com/jothepro/doxygen-awesome-css.git
  GIT_TAG
    v2.3.3
)

FetchContent_MakeAvailable( doxygen-awesome-css )


# Generate Project-Wide Docs
function( Enable_Doxygen )
   
   set( NAME                           "doxygen-${target}" )
   set( DOXYGEN_EXCLUDE_PATTERNS       "${PROJECT_SOURCE_DIR}/external" "${PROJECT_SOURCE_DIR}/build" )
   set( DOXYGEN_GENERATE_HTML          YES )
   set( DOXYGEN_GENERATE_TREEVIEW      YES )
   set( DOXYGEN_HAVE_DOT               YES )
   set( DOXYGEN_DOT_IMAGE_FORMAT       svg )
   set( DOXYGEN_DOT_TRANSPARENT        YES )
   set( DOXYGEN_CALLER_GRAPH           YES ) 
   set( DOXYGEN_CALL_GRAPH             YES )
   set( DOXYGEN_EXTRACT_ALL            YES )
   set( DOXYGEN_HTML_EXTRA_STYLESHEET  ${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome.css )

   doxygen_add_docs( doxygen-docs 
      ALL
      ${PROJECT_SOURCE_DIR}
      COMMENT "Generating documentation - entry file: ${CMAKE_CURRENT_BINARY_DIR}/html/index.html"
   )

   message( STATUS "Adding `doxygen-docs` target that builds project documentation." )
   
endfunction()
