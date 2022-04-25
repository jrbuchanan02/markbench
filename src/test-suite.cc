/**
 * @file test-suite.cc
 * @author Joshua Buchanan (joshuarobertbuchanan@gmail.com)
 * @brief Implements the test suite.
 * @version 1
 * @date 2022-04-25
 *
 * @copyright Copyright (C) 2022. Intellectual property of the author(s) listed
 * above.
 *
 */

#include "test-suite.hh"

#if defined( LINUX ) || defined( DARWIN )
#    include <fstream>
#else
#    ifndef UNICODE
#        define UNICODE 1
#    endif
#    include "windows.h"
#endif

#include <algorithm>
#include <iostream>
#include <random>

void null_test ( );
void allocate_deallocate_test ( );
void crypto_test ( );
void forced_cache_miss_test ( );

namespace suites
{
    static individual_test const null_test = {
            "test.null",
            ::null_test,
    };

    static individual_test const allocate_deallocate_test = {
            "test.heap_thrash",
            ::allocate_deallocate_test,
    };

    static individual_test const crypto_test = {
            "test.crypto_safe_random",
            ::crypto_test,
    };

    static individual_test const forced_cache_miss_test = {
            "test.force_cache_miss",
            ::forced_cache_miss_test,
    };
} // namespace suites

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// tests /////////////////////////////////////
test_suite version_000 ( )
{
    return test_suite {
            suites::null_test,
            suites::allocate_deallocate_test,
            suites::crypto_test,
            suites::forced_cache_miss_test,
    };
}

test_suite version_now ( ) { return version_000 ( ); }

/**
 * @brief This test intentionally does nothing. It is meant to generate a
 * point of reference for how fast this computer could theoretically go if
 * it were perfect. That is, this test measures the overhead of the testing
 * suite itself.
 */
void null_test ( ) { return; }

/**
 * @brief This test intentionally thrashes the heap, allocating a large array
 * then immediately deallocating this array as fast as possible. While few
 * programs use the heap in this manner, optimizing for this test means
 * optimizing malloc and free which, in theory, optimizes all programs.
 *
 */
void allocate_deallocate_test ( )
{
    struct point
    {
        float x;
        float y;
        float z;
        float confidence;
        float radius;
    };
    try
    {
        point *million_points = new point [ std::mega::num ];
        delete [] million_points;
    } catch ( ... )
    {
        std::cout << "A HA!\n";
        throw;
    }
}
/**
 * @brief RAII system to get the preferred cryptographically secure random
 * number generator.
 * @note Since markbench is built on mingw and the linker, by default, cannot
 * find Bcrypt.dll, we have to not only explicitly link with it in the command
 * line but dynamically load Bcrypt.dll when this class is constructed and
 * explicitly free Bcrypt.dll when this clas is destructed.
 * @note I apologize for all of the preprocessing macros. This function is
 * fully compile-time polymorphic and for three different operating systems.
 *
 */
class rand_stream
{
#if defined( LINUX )
    static constexpr char const *const random_file = "/dev/random";
#elif defined( DARWIN )
    static constexpr char const *const random_file = "/device/random";
#endif

#if defined( LINUX ) || defined( DARWIN )
    std::ifstream random_device = std::ifstream ( random_file );
#else

    HMODULE bcrypt = NULL;
#endif
public:
    rand_stream ( )
    {
#if !defined( LINUX ) && !defined( DARWIN )
        bcrypt = LoadLibrary ( L"Bcrypt.dll" );
#endif
    }

    virtual ~rand_stream ( )
    {
#if !defined( LINUX ) && !defined( DARWIN )
        FreeLibrary ( bcrypt );
#endif
    }

    void fill_random_bytes ( std::uint8_t *const &start,
                             std::size_t const   &count )
    {
#if defined( LINUX ) || defined( DARWIN )
        for ( std::size_t i = 0; i < count; i++ )
        {
            random_file >> start [ i ];
        }
#else
        BCryptGenRandom ( NULL, start, count, BCRYPT_USE_SYSTEM_PREFERRED_RNG );
#endif
    }
};

void crypto_test ( )
{
    using aes_key = std::uint8_t [ 256 / 8 ];
    static rand_stream stream;
    aes_key            key;
    stream.fill_random_bytes ( key, sizeof ( key ) );
}

/**
 * @brief By sorting a very large array, we force the cache to miss. We can
 * expect around 1 MiB per core on the system. So, each **thread** will use 10
 * MiB times the size of the number used. By ensuring that the structure cannot
 * fit in processor cache, we are ensuring that the system must access RAM at
 * some point in the test. Optimizing for this test means increasing cache
 * size to make this test fit in cache, which optimizes for all code. And,
 * this test can easily double the amount of cache required to cheat it by
 * incrementing that 20 in the function ;)
 */
void forced_cache_miss_test ( )
{
    static std::default_random_engine engine { };
    using int_type = decltype ( engine ( ) );

    static constexpr std::size_t count = 10 * ( 1 << 20 );

    std::vector< int_type > numbers;
    for ( std::size_t i = 0; i < count; i++ )
    {
        numbers.push_back ( engine ( ) );
    }

    std::sort ( numbers.begin ( ), numbers.end ( ) );
}