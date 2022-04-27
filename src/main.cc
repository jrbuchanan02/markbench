/**
 * @file main.cc
 * @author Joshua Buchanan (joshuarobertbuchanan@gmail.com)
 * @brief Entry point for markbench.
 * @version 1
 * @date 2022-04-16
 *
 * @copyright Copyright (C) 2022. Intellectual property of the author(s) listed
 * above.
 *
 */

#include "messages.hh"
#include "test-runner.hh"
#include "test-suite.hh"
#include "test-utils.hh"
#include "test.hh"

#include <algorithm>
#include <iostream>
#include <random>
#include <string>

#if defined( LINUX )
#    include "gtk/gtk.h"
#endif

using test_id_pair = std::tuple< markbench::test_function, std::string >;

int main ( int argc, char **argv )
{
#if defined( LINUX )
    gtk_init ( &argc, &argv );
#endif
    using namespace markbench;

    auto test_to_run = version_now;

    if ( argc >= 2 )
    {
        if ( std::string ( argv [ 1 ] ) == "now" )
        {
            std::cout << "Set to run " << argv [ 1 ] << "\n";
            test_to_run = version_now;
        }
        if ( std::string ( argv [ 1 ] ) == "000" )
        {
            std::cout << "Set to run " << argv [ 1 ] << "\n";
            test_to_run = version_000;
        }

        if ( std::string ( argv [ 1 ] ) == "001" )
        {
            std::cout << "Set to run " << argv [ 1 ] << "\n";
            test_to_run = version_001;
        }
    }

    test_runner ( en_us_locale ( ), test_to_run ( ) ).run_test ( );
}
