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
#include "test-suite.hh"
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

class test_runner
{
    using duration = std::chrono::steady_clock::duration;
    message_generator                          *generator;
    test_suite                                  suite;
    std::vector< long double >                  one_thread_total;
    std::vector< long double >                  all_thread_total;
    inline static markbench::thread_count const one_thread = 1;
    inline static markbench::thread_count const all_thread =
            hardware_threads ( );

    void accumulate_one_thread ( markbench::test_counters const &res,
                                 duration const                 &d )
    {
        one_thread_total.at ( 0 ) +=
                ( ( long double ) res.at ( 0 ) ) / d.count ( );
    }

    void accumulate_all_thread ( markbench::test_counters const &res,
                                 duration const                 &d )
    {
        long double grand_total = 0;
        for ( std::size_t i = 0; i < res.size ( ); i++ )
        {
            all_thread_total.at ( i ) +=
                    ( ( long double ) res.at ( i ) ) / d.count ( );
            grand_total += ( ( long double ) res.at ( i ) ) / d.count ( );
        }

        all_thread_total.at ( all_thread ) += grand_total;
    }

    auto current_time ( ) { return std::chrono::steady_clock::now ( ); }
public:
    test_runner ( message_generator *const &g, test_suite const &s )
    {
        generator = g;
        suite     = s;
        one_thread_total.push_back ( 0.0L );
        all_thread_total.push_back ( 0.0L );
        for ( markbench::thread_count i = 0; i < all_thread; i++ )
        {
            all_thread_total.push_back ( 0.0L );
        }
    }

    ~test_runner ( )
    {
        delete generator;
        // everything else default
    }

    void run_test ( );
};

void test_runner::run_test ( )
{
    using namespace markbench;
    // generate an instance of the default random engine and give it a random
    // seed
    std::random_device         r;
    std::default_random_engine engine { r ( ) };
    // now, if the test becomes significantly long, we want to account for the
    // system heating up. So, we will shuffle around the test.
    std::shuffle ( suite.begin ( ), suite.end ( ), engine );

    for ( auto &x : suite )
    {
        auto [ id, fn ] = x;
        auto now        = current_time ( );
        auto end        = current_time ( );

        test         *runner = nullptr;
        test_counters results;
        std::cout << generator->test_message ( id, one_thread );
        runner  = new test ( fn );
        now     = current_time ( );
        results = runner->run ( one_thread );
        end     = current_time ( );
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

int main ( int const argc, char const *const *const argv )
{
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
