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
#include <cstring>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <thread>

void null_test ( );
void allocate_deallocate_test ( );
void crypto_test ( );
void forced_cache_miss_test ( );
void window_create_destroy_test ( );
void primes_sieve_test ( );

namespace suites
{
    // the original tests.

    // this test literally does nothing. It sort-of documents how quickly the
    // system can perform the test just as the test itself. There is
    // not-insignificant overhead in markbench -- and that's ok since markbench
    // is a program and uses the standard library where possible to ensure that
    // optimizing for markbench optimized for the most amount of programs
    // possible.
    static individual_test const null_test = {
            "test.null",
            ::null_test,
    };

    // this test measures the speed at which a (large) heap allocation can be
    // made and immediately freed. Heap-management directly affects the
    // performace of everything on the system perhaps more than most other
    // common functions.
    static individual_test const allocate_deallocate_test = {
            "test.heap_thrash",
            ::allocate_deallocate_test,
    };

    // this test specifically measures the speed at which this CPU can generate
    // cryptographically secure random numbers. These are important for
    // cyptography, which is something all computers need to do relatively
    // quicky.
    static individual_test const crypto_test = {
            "test.crypto_safe_random",
            ::crypto_test,
    };

    // your computer's CPU is perfectly capable of performing the operations in
    // this function within a second. The RAM is not. That's what this test
    // measures.
    static individual_test const forced_cache_miss_test = {
            "test.force_cache_miss",
            ::forced_cache_miss_test,
    };

    // tests added for version 001

    // window creation / destruction, which affects the speed of all
    // applications.
    static individual_test const window_create_destroy_test {
            "test.window_create_destroy",
            ::window_create_destroy_test,
    };

    // integer operations -- the most raw form of computational power. The
    // amount of iterations through the prime sieve is not a good benchmark on
    // its own, however.
    static individual_test const primes_sieve_test {
            "test.davidpl_primes_sieve",
            ::primes_sieve_test,
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

test_suite version_001 ( )
{
    return test_suite {
            suites::null_test,
            suites::allocate_deallocate_test,
            suites::crypto_test,
            suites::forced_cache_miss_test,
            suites::window_create_destroy_test,
            suites::primes_sieve_test,
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
    point *million_points = new point [ std::mega::num ];
    delete [] million_points;
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
    std::ifstream random = std::ifstream ( random_file );
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
        for ( std::size_t i = 0; i < count; i++ ) { random >> start [ i ]; }
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

#if defined( WINDOWS )
LRESULT CALLBACK WINAPI window_proc ( HWND   window,
                                      UINT   msg,
                                      WPARAM w_param,
                                      LPARAM l_param )
{
    return DefWindowProc ( window, msg, w_param, l_param );
}
#endif

// creates a window and destroys it immediately after. This system should use
// the lowest level operations possible. Rest in Peace, Desktop Window Manager.
void window_create_destroy_test ( )
{
#if defined( WINDOWS )
    // window classes are thread-global, so we need to add the thread-id to the
    // name of the window class to ensure that we don't frick ourselves up with
    // multithreading shenanegans.
    std::wstringstream _class_name;
    std::wstringstream _window_name;
    _class_name << L"CLASS " << std::this_thread::get_id ( );
    _window_name << L"Window " << std::this_thread::get_id ( );

    std::wstring class_name  = _class_name.str ( );
    std::wstring window_name = _window_name.str ( );

    HINSTANCE  instance = GetModuleHandle ( nullptr );
    WNDCLASSEX window_class;
    window_class.cbSize        = sizeof ( window_class );
    window_class.lpszClassName = class_name.c_str ( );
    window_class.style         = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc   = &window_proc;
    window_class.hbrBackground = ( HBRUSH ) GetStockObject ( DC_BRUSH );
    window_class.lpszMenuName  = L"MENU";
    window_class.hCursor       = LoadCursor ( NULL, IDC_ARROW );
    window_class.hIcon         = NULL;
    window_class.hIconSm       = NULL;
    window_class.hInstance     = instance;

    RegisterClassEx ( &window_class );
    HWND window;
    window = CreateWindowEx ( WS_EX_OVERLAPPEDWINDOW,
                              class_name.c_str ( ),
                              window_name.c_str ( ),
                              WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT,
                              CW_USEDEFAULT,
                              CW_USEDEFAULT,
                              CW_USEDEFAULT,
                              NULL,
                              NULL,
                              instance,
                              nullptr );
    ShowWindow ( window, SHOW_OPENWINDOW );
    DestroyWindow ( window );
    UnregisterClass ( window_class.lpszClassName, window_class.hInstance );
#elif defined( LINUX )
#    warning "This test is not implemented for linux yet."
#elif defined( DARWIN )
#    warning "This test is not implemented for Apple OSes yet."
#endif
}
// credit: David Plummer.

class bit_array
{
    std::uint32_t *array;
    std::size_t    size;

    inline static constexpr std::size_t array_size ( std::size_t const size )
    {
        return ( size >> 5 ) + ( ( size & 31 ) > 0 );
    }

    inline static constexpr std::size_t index ( std::size_t const n )
    {
        return ( n >> 5 );
    }

    inline static constexpr std::uint32_t get_subindex ( std::size_t const   n,
                                                         std::uint32_t const d )
    {
        // I don't know why it's anding with 31 up there and modulo
        // 32 down here, but that's what's in the Primes repository.
        return d & std::uint32_t ( ( std::uint32_t ( 0x01 ) << ( n % 32 ) ) );
    }

    inline void set_false_subindex ( std::size_t n, std::uint32_t &d )
    {
        d &= ~std::uint32_t ( std::uint32_t ( 0x01 )
                              << ( n % ( 8 * sizeof ( std::uint32_t ) ) ) );
    }
public:
    explicit bit_array ( std::size_t size ) : size ( size )
    {
        array = new std::uint32_t [ array_size ( size ) ];
        std::memset ( array, 0xFF, ( size >> 3 ) + ( ( size & 7 ) > 0 ) );
    }

    ~bit_array ( ) { delete [] array; }

    bool get ( std::size_t n ) const
    {
        return get_subindex ( n, array [ index ( n ) ] );
    }

    static constexpr std::uint32_t rol ( std::uint32_t x, std::uint32_t n )
    {
        return ( x << n ) | ( x >> ( 32 - n ) );
    }

    void set_flags_false ( std::size_t n, std::size_t skip )
    {
        auto rolling_mask = ~std::uint32_t ( 1 << n % 32 );
        auto roll_bits    = skip % 32;
        while ( n < size )
        {
            array [ index ( n ) ] &= rolling_mask;
            n += skip;
            rolling_mask = rol ( rolling_mask, roll_bits );
        }
    }
};

class prime_sieve
{
private:
    long                                                sieve_size = 0;
    bit_array                                           bits;
    static const std::map< long long const, int const > results;

    bool validate_results ( )
    {
        return results.contains ( sieve_size )
            && results.at ( sieve_size ) == count_primes ( );
    }
public:
    prime_sieve ( long n ) : bits ( n ), sieve_size ( n ) { }
    ~prime_sieve ( ) { }

    void run_sieve ( )
    {
        int factor = 3;
        int q      = ( int ) std::sqrt ( sieve_size );
        while ( factor <= q )
        {
            for ( int num = factor; num < sieve_size; num += 2 )
            {
                if ( bits.get ( num ) )
                {
                    factor = num;
                    break;
                }
            }
            bits.set_flags_false ( factor * factor, factor << 1 );
            factor += 2;
        }
    }

    int count_primes ( )
    {
        int count = ( sieve_size >= 2 );
        for ( int i = 3; i < sieve_size; i += 2 )
        {
            if ( bits.get ( i ) )
            {
                count++;
            }
        }
        return count;
    }
};

std::map< long long const, int const > const prime_sieve::results = {
        { 10LL, 4 },
        { 100LL, 25 },
        { 1000LL, 168 },
        { 10000LL, 1229 },
        { 100000LL, 9592 },
        { 1000000LL, 78498 },
        { 10000000LL, 664579 },
        { 100000000LL, 5761455 },
        { 1000000000LL, 50845734 },
        { 10000000000LL, 455052511 },
};

void primes_sieve_test ( ) { prime_sieve sieve ( 1000000L ); }