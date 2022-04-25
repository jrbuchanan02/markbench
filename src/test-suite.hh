/**
 * @file test-suite.hh
 * @author Joshua Buchanan (joshuarobertbuchanan@gmail.com)
 * @brief Suite of tests.
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

struct individual_test
{
    std::string              name_id;
    markbench::test_function function;
};

using test_suite = std::vector< individual_test >;

test_suite version_now ( );
test_suite version_000 ( );