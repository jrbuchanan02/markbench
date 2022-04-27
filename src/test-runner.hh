/**
 * @file test-runner.hh
 * @author Joshua Buchanan (joshuarobertbuchanan@gmail.com)
 * @brief Runs the tests.
 * @version 1
 * @date 2022-04-26
 *
 * @copyright Copyright (C) 2022. Intellectual property of the author(s) listed
 * above.
 *
 */

#pragma once

#include <chrono>
#include <iostream>
#include <vector>

#include "messages.hh"
#include "test-suite.hh"
#include "test-utils.hh"

inline markbench::thread_count hardware_threads ( )
{
    return std::thread::hardware_concurrency ( );
}

using accumulated_score = std::vector< long double >;

class test_runner
{
    using duration = std::chrono::steady_clock::duration;
    message_generator                          *generator;
    test_suite                                  suite;
    accumulated_score                           one_thread_total;
    accumulated_score                           all_thread_total;
    rng                                         randomness;
    inline static markbench::thread_count const one_thread = 1;
    inline static markbench::thread_count const all_thread =
            hardware_threads ( );

    static inline void accumulate ( accumulated_score              &a,
                                    markbench::test_counters const &c,
                                    duration const                 &d )
    {
        long double grand_total = 0;
        for ( std::size_t i = 0; i < c.size ( ); i++ )
        {
            long double value = c.at ( i );
            value /= d.count ( );
            grand_total += value;
            a.at ( i ) += value;
        }

        a.at ( c.size ( ) ) += grand_total;
    }

    static inline void fill_score_accumulator ( accumulated_score      &a,
                                                markbench::thread_count c )
    {
        for ( std::size_t i = 0; i <= c; i++ ) { a.push_back ( 0 ); }
    }

    static inline auto current_time ( )
    {
        return std::chrono::steady_clock::now ( );
    }

    // if count -> all threads.
    void run_test_pass ( bool count, individual_test t );

    void run_tests ( individual_test t );
public:
    test_runner ( message_generator *const &g, test_suite const &s )
    {
        generator = g;
        suite     = s;
        fill_score_accumulator ( one_thread_total, one_thread );
        fill_score_accumulator ( all_thread_total, all_thread );
    }

    ~test_runner ( )
    {
        delete generator;
        // everything else default
    }

    void run_test ( );
};
