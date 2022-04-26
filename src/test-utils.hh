/**
 * @file test-utils.hh
 * @author Joshua Buchanan (joshuarobertbuchanan@gmail.com)
 * @brief Generic utilities.
 * @version 1
 * @date 2022-04-26
 *
 * @copyright Copyright (C) 2022. Intellectual property of the author(s) listed
 * above.
 *
 */
#pragma once

#include <random>

/**
 * @brief Version of std::default_random_engine that automatically gives itself
 * a seed using std::random_device
 *
 */
class rng
{
    std::random_device         device;
    std::default_random_engine engine { device ( ) };
public:
    using result_type = typename std::default_random_engine::result_type;

    result_type min ( ) { return engine.min ( ); }
    result_type max ( ) { return engine.max ( ); }
    auto        operator( ) ( ) noexcept { return engine ( ); }
};