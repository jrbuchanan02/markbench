/**
 * @file test-functions.hh
 * @author Joshua Buchanan (joshuarobertbuchanan@gmail.com)
 * @brief Declarations for the functions used in the various tests.
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

std::vector< markbench::test_function > get_test_functions ( );

std::vector< std::string > get_test_name_ids ( );