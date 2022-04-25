/**
 * @file test.hh
 * @author Joshua Buchanan (joshuarobertbuchanan@gmail.com)
 * @brief Test function calling.
 * @version 1
 * @date 2022-04-16
 *
 * @copyright Copyright (C) 2022. Intellectual property of the author(s) listed
 * above.
 *
 */
#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <thread>
#include <vector>

namespace markbench
{
    /**
     * @brief A function that a test will attempt to call as many times per
     * second as possible
     */
    using test_function = std::function< void ( ) >;

    using thread_count = decltype ( std::thread::hardware_concurrency ( ) );

    using test_counters = std::vector< std::uintmax_t >;

    class test
    {
        test_function    test_fn      = [] ( ) {};
        std::atomic_bool test_running = false;
    public:
        test ( test_function const &function ) : test_fn { function } { }

        ~test ( )
        {
            while ( test_running.load ( ) ) { }
        }

        test_counters run ( thread_count const hardware = thread_count { 1 } );
    };
} // namespace markbench