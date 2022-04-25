/**
 * @file test.cc
 * @author Joshua Buchanan (joshuarobertbuchanan@gmail.com)
 * @brief Test implementation
 * @version 1
 * @date 2022-04-16
 *
 * @copyright Copyright (C) 2022. Intellectual property of the author(s) listed
 * above.
 *
 */
#include "test.hh"

#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
using counter_type = std::unique_ptr< std::atomic_uintmax_t >;

class test_latch
{
    std::atomic< markbench::thread_count > latch;
public:
    test_latch ( markbench::thread_count const &value ) : latch { value } { }

    void wait ( )
    {
        while ( latch.load ( ) )
        {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for ( 1ms );
        }
    }

    void arrive ( ) { latch.fetch_sub ( 1 ); }
};

markbench::test_counters markbench::test::run ( thread_count const hardware )
{
    test_running.store ( true );
    std::vector< std::jthread > threads;
    std::vector< counter_type > counters;
    std::atomic_bool            test = false;
    test_latch                  arrive { hardware };
    test_latch                  exit { hardware };
    test_counters               result;

    auto run_single = [ & ] ( thread_count id ) {
        arrive.arrive ( );
        while ( !test.load ( ) ) { }
        while ( test.load ( ) )
        {
            test_fn ( );
            counters.at ( id )->fetch_add ( 1 );
        }
        exit.arrive ( );
    };

    auto add_thread = [ & ] ( thread_count id ) {
        threads.push_back ( std::jthread ( std::bind ( run_single, id ) ) );
        counters.push_back ( std::unique_ptr< std::atomic_uintmax_t > (
                new std::atomic_uintmax_t ( 0 ) ) );
    };

    auto start_thread = [ & ] ( std::jthread &j ) { j.detach ( ); };

    auto stop_thread = [ & ] ( std::jthread &j ) {
        while ( j.joinable ( ) ) { j.join ( ); }
    };

    auto copy_result = [ & ] ( counter_type const &c ) {
        result.push_back ( c->load ( ) );
    };
    // std::cout << "here 1\n";
    for ( thread_count i = 0; i < hardware; i++ ) { add_thread ( i ); }
    // std::cout << "here 2\n";
    std::for_each ( threads.begin ( ), threads.end ( ), start_thread );
    // std::cout << "here 3\n";
    arrive.wait ( );
    // std::cout << "here 4\n";
    test.store ( true );
    // std::cout << "here 5\n";
    using namespace std::chrono_literals;
    std::this_thread::sleep_for ( 1s );
    // std::cout << "here 6\n";
    test.store ( false );
    exit.wait ( );
    // std::cout << "here 7\n";
    std::for_each ( threads.begin ( ), threads.end ( ), stop_thread );
    threads.clear ( );
    // std::cout << "here 8\n";
    std::for_each ( counters.begin ( ), counters.end ( ), copy_result );
    // std::cout << "here 9\n";
    test_running.store ( false );
    // std::cout << "here A\n";
    return result;
}