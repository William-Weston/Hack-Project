/**
 * @file    main.cpp
 * @author  William Weston
 * @brief   Hack Emulator main
 * @version 0.1
 * @date    2024-03-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "Emulator.h"       // for Emulator
#include "SDL_InitError.h"  // for SDL_InitError

#include <cstdlib>          // for EXIT_FAILURE, EXIT_SUCCESS
#include <exception>        // for exception
#include <iostream>         // for operator<<, basic_ostream, char_traits, cerr



auto main( int, char*[] ) -> int
{
    try
    {
        auto emulator = Hack::Emulator( "Hack Emulator", 50, 50, 1340, 720, false );

        emulator.run();
    }

    catch ( SDL_InitError const& error )                                          // SDL_InitError
    {
        std::cerr << "SDL Initialization Failure\n";
        std::cerr << error.what() << '\n';
        return EXIT_FAILURE;
    }

    catch( std::exception const& e )
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    catch( ... )
    {
        std::cerr << "Uknown Exception";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
} 