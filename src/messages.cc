/**
 * @file messages.cc
 * @author Joshua Buchanan (joshuarobertbuchanan@gmail.com)
 * @brief Defines the various message generator classes.
 * @version 1
 * @date 2022-04-25
 *
 * @copyright Copyright (C) 2022. Intellectual property of the author(s) listed
 * above.
 *
 */

#include "messages.hh"

class en_us_messages : public virtual message_generator
{
public:
    std::string
            test_message ( std::string const             &id,
                           markbench::thread_count const &count ) override final
    {
        std::string result;
        result += "Running ";
        if ( id == "test.null" )
        {
            result += "null test";
        } else if ( id == "test.heap_thrash" )
        {
            result += "heap thrashing test";
        } else if ( id == "test.crypto_safe_random" )
        {
            result += "cryptographically random number generation test";
        } else if ( id == "test.force_cache_miss" )
        {
            result += "cache miss test";
        } else if ( id == "test.window_create_destroy" )
        {
            result += "window creation / destruction test";
        } else if ( id == "test.davidpl_primes_sieve" )
        {
            result += "prime sieve test";
        } else if ( id == "test.joshuas_salt" )
        {
            result += "GCD of minimal and maximal element test";
        } else if ( id == "test.naiive_isqrt" )
        {
            // a bit long.
            result +=
                    "Naiive implementation of inverse square root / vector "
                    "normalization test";
        } else if ( id == "test.matrix_rref_triple" )
        {
            result += "find the rref of a 256x256 matrix of "
                    + std::to_string ( sizeof ( long double ) * 8 )
                    + "-bit floating points test";
        } else if ( id == "test.matrix_rref_double" )
        {
            result +=
                    "find the rref of a 256x256 matrix of double-precision "
                    "floating points test";
        } else if ( id == "test.matrix_rref_single" )
        {
            result +=
                    "find the rref of a matix of single-precision floating "
                    "points test";
        } else
        {
            result += "!" + id + "! test";
        }
        result += " on " + std::to_string ( count ) + " threads.\n";

        return result;
    }

    std::string list_results (
            markbench::test_counters const &results ) override final
    {
        if ( results.size ( ) == 1 )
        {
            return std::string ( "This computer scored a " )
                 + std::to_string ( results.front ( ) ) + "\n";
        } else
        {
            std::string result = "The threads on this computer scored:\n";
            for ( auto const &r : results )
            {
                result += "\t- " + std::to_string ( r ) + "\n";
            }
            return result;
        }
    }

    std::string list_rhedstone_count (
            std::vector< long double > const &single,
            std::vector< long double > const &multi ) override final
    {
        std::string result = "";

        result += "This computer's single-threaded performance is "
                + std::to_string ( single.front ( ) ) + " rhedstones\n";
        result += "This computer's multi-threaded performance is "
                + std::to_string ( multi.back ( ) ) + " rhedstones\n";
        return result;
    }
};

message_generator *en_us_locale ( ) { return new en_us_messages ( ); }