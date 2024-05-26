
macro( project_setup )

   option( HACK_ENABLE_USER_LINKER "Enable custom linker" ON )

   include( Cache )
   include( IPO )
   enable_ipo()

   # link against the Hack::project_warnings target to enable warnings
   add_library( project_warnings INTERFACE )
   
   include( CompilerWarnings )
   set_project_warnings( project_warnings )

   add_library( project_options INTERFACE )

   include( Linker )
   configure_linker( project_options )


   

endmacro()
