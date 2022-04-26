/**
 * @file test-runner.cc
 * @author Joshua Buchanan (joshuarobertbuchanan@gmail.com)
 * @brief The functions for the test runner
 * @version 1
 * @date 2022-04-26
 *
 * @copyright Copyright (C) 2022. Intellectual property of the author(s) listed
 * above.
 *
 */

#include "test-runner.hh"

#include <chrono>
#include <functional>
#include <iostream>
#include <vector>

#include "messages.hh"
#include "test-suite.hh"
#include "test-utils.hh"

void test_runner::run_test ( )
{
    // now, if the test becomes significantly long, we want to account for the
    // system heating up. So, we will shuffle around the test.
    std::shuffle ( suite.begin ( ), suite.end ( ), randomness );

    for ( auto x : suite ) { run_tests ( x ); }

    std::cout << generator->list_rhedstone_count ( one_thread_total,
                                                   all_thread_total );
}

void test_runner::run_tests ( individual_test t )
{
    run_test_pass ( false, t );
    run_test_pass ( true, t );
}

void test_runner::run_test_pass ( bool count, individual_test t )
{
    auto [ id, fn ]             = t;
    auto                    now = current_time ( );
    auto                    end = current_time ( );
    accumulated_score      *as  = count ? &all_thread_total : &one_thread_total;
    markbench::thread_count threads = count ? all_thread : one_thread;
    markbench::test        *runner  = new markbench::test ( fn );

    std::cout << generator->test_message ( id, threads );
    now          = current_time ( );
    auto results = runner->run ( threads );
    end          = current_time ( );
    accumulate ( *as, results, end - now );
    std::cout << generator->list_results ( results );
    delete runner;
    as = nullptr;
}