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