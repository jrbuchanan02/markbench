/**
 * @file messages.hh
 * @author Joshua Buchanan (joshuarobertbuchanan@gmail.com)
 * @brief Message generator. These make the resources more abstract and will
 * eventually allow for loading the text-resources from a file. Which will allow
 * for easy localization.
 * @version 1
 * @date 2022-04-25
 *
 * @copyright Copyright (C) 2022. Intellectual property of the author(s) listed
 * above.
 *
 */
#pragma once

#include "test.hh"
#include <string>
#include <vector>

class message_generator
{
public:
    virtual std::string
            test_message ( std::string const             &id,
                           markbench::thread_count const &count ) = 0;
    virtual std::string
            list_results ( markbench::test_counters const &results ) = 0;
    virtual std::string list_rhedstone_count (
            std::vector< long double > const &single,
            std::vector< long double > const &multi ) = 0;
};

// different locales
message_generator *en_us_locale();