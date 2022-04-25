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
#include "test-functions.hh"
#include "test.hh"

#include <algorithm>
#include <iostream>
#include <random>
#include <string>

using test_id_pair = std::tuple< markbench::test_function, std::string >;

markbench::thread_count hardware_threads ( )
{
    return std::thread::hardware_concurrency ( );
}

int main ( int const, char const *const *const )
{
    using namespace markbench;
    using duration                        = std::chrono::steady_clock::duration;
    message_generator          *generator = en_us_locale ( );
    std::vector< test_id_pair > test_info;
    static thread_count const   one_thread = 1;
    static thread_count const   all_thread = hardware_threads ( );
    auto const                 &fns        = get_test_functions ( );
    auto const                 &ids        = get_test_name_ids ( );
    std::vector< long double >  one_thread_total;
    std::vector< long double >  all_thread_total;
    std::size_t                 cnt = fns.size ( );

    for ( std::size_t i = 0; i < cnt; i++ )
    {
        test_info.push_back ( test_id_pair ( fns.at ( i ), ids.at ( i ) ) );
    }

    one_thread_total.push_back ( 0.0L );
    for ( thread_count i = 0; i < all_thread + 1; i++ )
    {
        all_thread_total.push_back ( 0.0L );
    }

    auto accumulate_one_thread = [ & ] ( test_counters const &res,
                                         duration const      &d ) {
        one_thread_total.at ( 0 ) +=
                ( ( long double ) res.at ( 0 ) ) / d.count ( );
    };

    auto accumulate_all_thread = [ & ] ( test_counters const &res,
                                         duration const      &d ) {
        long double grand_total = 0;
        for ( std::size_t i = 0; i < res.size ( ); i++ )
        {
            all_thread_total.at ( i ) +=
                    ( ( long double ) res.at ( i ) ) / d.count ( );
            grand_total += ( ( long double ) res.at ( i ) ) / d.count ( );
        }

        all_thread_total.at ( all_thread ) += grand_total;
    };

    auto current_time = [] ( ) { return std::chrono::steady_clock::now ( ); };

    // now, if the test becomes significantly long, we want to account for the
    // system heating up. So, we will shuffle around the test.
    std::shuffle ( test_info.begin ( ),
                   test_info.end ( ),
                   std::default_random_engine { } );

    for ( auto &x : test_info )
    {
        auto [ fn, id ] = x;
        auto now        = current_time ( );
        auto end        = current_time ( );

        std::cout << generator->test_message ( id, one_thread );
        test *runner = new test ( fn );
        now          = current_time ( );
        auto results = runner->run ( one_thread );
        end          = current_time ( );
        accumulate_one_thread ( results, end - now );
        std::cout << generator->list_results ( results );

        std::cout << generator->test_message ( id, all_thread );
        now     = current_time ( );
        results = runner->run ( all_thread );
        end     = current_time ( );
        accumulate_all_thread ( results, end - now );
        std::cout << generator->list_results ( results );
        delete runner;
    }

    std::cout << generator->list_rhedstone_count ( one_thread_total,
                                                   all_thread_total );
}
