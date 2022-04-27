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

#if defined( LINUX )
#    include "gtk/gtk.h"
#endif

#include <algorithm>
#include <cstring>
#include <iostream>
#include <map>
#include <mutex>
#include <random>
#include <sstream>
#include <thread>

void null_test ( );
void allocate_deallocate_test ( );
void crypto_test ( );
void forced_cache_miss_test ( );
void window_create_destroy_test ( );
void primes_sieve_test ( );
void salty_test ( );
void isqrt_test ( );
void random_software_matrix_rref_test ( );
void random_hardware_matrix_rref_test ( );
void random_gloves_off_matrix_rref_test ( );

namespace suites
{
    // the original tests.

    /**
     * @brief The null test
     * @details This test literally does nothing. This test exists in part to be
     * a freebie. However, if any test keeps pace with the null test, the
     * compiler immediately becomes suspect. The compiler can (correctly)
     * recognize that we are not using any of our computation results and may
     * optimize out the entire test, which defeats the purpose of a benchmark.
     */
    static individual_test const null_test = {
            "test.null",
            ::null_test,
    };

    /**
     * @brief The heap-thrashing test.
     * @details Quite literally, every program makes heap allocations and
     * heap deallocations. Even if the software did not call malloc, free, new,
     * or delete, the software is on the operating system's heap. Measuring the
     * speed of these heap allocations and deallocations directly rewards an
     * operating system for a high-performance implementation of these
     * functions.
     */
    static individual_test const allocate_deallocate_test = {
            "test.heap_thrash",
            ::allocate_deallocate_test,
    };

    /**
     * @brief The cryptographically secure random number generation test.
     * @details Computers are not naturally random devices. As a result, they
     * struggle to generate random numbers. However, we need random numbers for
     * hashing, encrpytion, decryption, and everything else that keeps your
     * digital information more private. We want to directly reward a device
     * which can generate an acceptably random number in a shorter amount of
     * time since that means more data encrypted and, in theory, more security.
     */
    static individual_test const crypto_test = {
            "test.crypto_safe_random",
            ::crypto_test,
    };

    /**
     * @brief The troll-test (cache miss test)
     * @details The bus speed for x64 processors remains 100 MHz, meaning, that
     * if a processor does not have information in its cache, it may have to
     * wait hundreds of cycles for that one datum. This test, my personal
     * favorite, gives the computer a task where RAM access is so much of a
     * bottleneck that it may not complete in time. This system forces cache
     * misses by allocating enough memory that it cannot fit in the processor
     * cache.
     */
    static individual_test const forced_cache_miss_test = {
            "test.force_cache_miss",
            ::forced_cache_miss_test,
    };

    // tests added for version 001

    /**
     * @brief The window creation and destruction test.
     * @details Literally every GUI-based application creates and destroys
     * windows. So, the faster a machine can do that, in theory the faster
     * these applications can load. This function is optimized O1 since
     * optimizing to O2 did not actually create said windows.
     */
    static individual_test const window_create_destroy_test {
            "test.window_create_destroy",
            ::window_create_destroy_test,
    };

    /**
     * @brief The Primes-Sieve test
     * @details Literally every program uses integer operations. While raw
     * arithmetic processing power does not mean overall performance, this
     * primes sieve originally made by David Plummer @PlummersSoftwareLLC
     * is simple enough and telling enough to be part of the markbench metric.
     */
    static individual_test const primes_sieve_test {
            "test.davidpl_primes_sieve",
            ::primes_sieve_test,
    };

    /**
     * @brief The test powered by my anger towards terribly designed programming
     * languages.
     * @details This test is inspired by my digust for this video's (
     * https://www.youtube.com/watch?v=UVUjnzpQKUo) preferred solution to the
     * following challenge: write a function that takes the greatest common
     * denominator of the minimal and maximal values of a list of integers.
     * The video's preferred solution is from a functional programming language
     * that describes the value in about seven characters. These hieroglyphs do
     * not constitute code. Code should be readable by anyone. That's why we
     * use human readable programming languages and use compilers and do not
     * program directly in machine bytecode. This implementation is a middle
     * finger to unreadable code and I'm quite proud of the readability in it.
     */
    static individual_test const salty_test {
            "test.joshuas_salt",
            ::salty_test,
    };

    /**
     * @brief The Naiive Vector Normalization Test
     * @details This test normalizes not-random vectors in an intentionally
     * naiive manner. Meant to test any particularly special, machine-specific
     * instructions *cough* AVX-512 *cough* that calculate the inverse square
     * root and / or reward the compiler on its ability to detect that we don't
     * need the exact value here / its ability to calculate the inverse square
     * root of the value.
     * @warning In single threaded performance, this function performs
     * suspiciously similarly to the null test, the only reason it's not marked
     * -O1 is because in multithreaded performance it significantly falls behind
     * the null test.
     */
    static individual_test const isqrt_naiive_test {
            "test.naiive_isqrt",
            ::isqrt_test,
    };

    /**
     * @brief The Software-Based Floating Point Reduced Row Echelon Form of a
     * Matrix Test.
     * @details This test checks the performance of the machine on the built-in
     * long double type. By default, in GCC, the size of long double is 16
     * bytes. In x64 and other common architectures, this size cannot be done in
     * hardware, so long double is software implemented. This matrix is 256 by
     * 256 and filled with random values from - (2^31) to (2^31 - 1).
     */
    static individual_test const software_matrix_test {
            "test.matrix_rref_triple",
            ::random_software_matrix_rref_test,
    };

    /**
     * @brief The Hardware-based Floating Point Reduced Row Echelon Form of a
     * Matrix Test.
     * @details This test checks the perfomance of the machine on the built-in
     * double type. By default, in GCC, this is a double-precision IEEE-754
     * binary floating point. In most modern computers, double precision
     * floating points are implemented in hardware, however, they are not as
     * fast as single-precision because the SIMD registers used are the same
     * size (that is, we can perform any given instruction at the same speed,
     * but we can operate on say 2 double precision floats or 4 single precision
     * floats, resulting in double precision being slower).
     */
    static individual_test const hardware_matrix_test {
            "test.matrix_rref_double",
            ::random_hardware_matrix_rref_test,
    };

    /**
     * @brief The Single Precision Floating Point Reduced Row Echelon Form of a
     * Matrix Test.
     * @details This test is like the other RREF of a matrix tests, but uses
     * float. Float, given which random values we have chosen, actually loses
     * quite a bit of precision, but, we don't actually care about the value
     * we calculate, just how fast we can get it.
     */
    static individual_test const gloves_off_matrix_test {
            "test.matrix_rref_single",
            ::random_gloves_off_matrix_rref_test,
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
            suites::salty_test,
            suites::isqrt_naiive_test,
            suites::software_matrix_test,
            suites::hardware_matrix_test,
            suites::gloves_off_matrix_test,
    };
}

test_suite version_now ( ) { return version_001 ( ); }

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
 * @note Explicitly no optimize since it seems that gcc is completely
 * eliminating the test itself (it outscored the null test!)
 */
void __attribute__ ( ( optimize ( "O0" ) ) ) allocate_deallocate_test ( )
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

/**
 * @brief Generates the randomness.
 *
 */
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
 * incrementing that 20 in the function ;).
 * @details I have yet to see a score higher than 2 on single-threaded
 * performance and a time of less than 2 seconds on multithreaded performance.
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

/**
 * @brief Creates a window of default parameters then immmediately destroys
 * it.
 * @note Optimized to -O0 because optimizations did not actually create the
 * window. I guess it's pretty obvious that we're creating then immediately
 * destroying the window.
 */
void __attribute__ ( ( optimize ( "O0" ) ) ) window_create_destroy_test ( )
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
#elif defined( LINUX ) || defined( DARWIN )
    static std::mutex one_at_a_time;
    GtkWidget        *window;
    {
        std::scoped_lock lock { one_at_a_time };
        window = gtk_window_new ( GTK_WINDOW_TOPLEVEL );
    }
    // gtk_window_present ( window );
    {
        std::scoped_lock lock { one_at_a_time };
        gtk_widget_destroy ( window );
    }
    window = nullptr;
#endif
}
// credit: David Plummer.

/**
 * @brief Essentially std::vector<bool>.
 *
 */
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

/**
 * @brief Prime sieve.
 *
 */
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

// the amount of primes below a certain power of 10.
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

/**
 * @brief Runs an iteration of the primes sieve.
 *
 */
void primes_sieve_test ( )
{
    prime_sieve sieve ( 1000000L );
    sieve.run_sieve ( );
}

/**
 * @brief Finds the greatest common denominator between two integers.
 *
 * @param lhs the left hand side
 * @param rhs the right hand side
 * @return std::integral the greatest common denominator.
 */
std::integral auto gcd ( std::integral auto lhs, std::integral auto rhs )
{
    if ( lhs < 0 )
    {
        lhs = -lhs;
    }
    if ( rhs < 0 )
    {
        rhs = -rhs;
    }

    if ( lhs < rhs )
    {
        std::swap ( lhs, rhs );
    }

    while ( rhs != 0 )
    {
        std::integral auto temp = lhs;
        lhs                     = rhs;
        rhs                     = temp % rhs;
    }
    return rhs;
}

/**
 * @brief Finds the greatest common denominator between the minimal and
 * maximal elements in an array.
 * @note Generates a random array of vectors where the array occupies almost
 * all of the level 1 cache expected on the machine. The reason that we choose
 * 7 pages (124 KiB) instead of 8 pages (128 KiB) is because we need to account
 * for the stack, which may be in a separate area of cache, but since all
 * "units" in the cache are 4096 bytes long, we need to make sure that we have
 * as few cache misses as possible. In other words, we want any performance off
 * of "theoretical" to truly be the CPU's fault.
 *
 */
void salty_test ( )
{
    // the default random number generator
    using rng    = std::default_random_engine;
    // the type that the rng gives us
    using number = decltype ( rng { }( ) );
    using list   = std::vector< number >;

    static constexpr std::integral auto size_of_page = 0x1000;
    static constexpr std::integral auto pages        = 7 * size_of_page;
    static constexpr std::integral auto count = pages / sizeof ( number );

    list numbers;
    rng  random_numbers;

    for ( std::integral auto i = 0; i < count; i++ )
    {
        numbers.push_back ( random_numbers ( ) );
    }

    number min = numbers [ 0 ];
    number max = numbers [ 0 ];

    auto find_extremes = [ & ] ( number n ) {
        if ( n < min )
        {
            min = n;
        }
        if ( n > max )
        {
            max = n;
        }
    };

    std::for_each ( numbers.begin ( ), numbers.end ( ), find_extremes );
    gcd ( min, max );
}

/**
 * @brief Normalizes a vector to what could be a point in 3D space or a
 * direction in 3D space. Normalization is critical for performing the
 * cross-product as the magnitude of the cross product is related to the
 * magnitudes of the two input vectors. Meaning that if we want to calculate the
 * direction of a reflection and we do not use normalized vectors, we may get an
 * erroneous value.
 *
 */
void isqrt_test ( )
{
    // our vector.
    static float vector [ 3 ] = { 0.1, 0.1, 0.1 };
    // the changes in our values. Note that they have no common factors and that
    // one has a different sign.
    static float deltas [ 3 ] = { -0.1, 0.3, 0.5 };

    // calculate the inverse square root.
    float hypot = vector [ 0 ] * vector [ 0 ] + vector [ 1 ] * vector [ 1 ]
                + vector [ 2 ] * vector [ 2 ];
    float normal [ 3 ] = {
            vector [ 0 ] / std::sqrt ( hypot ),
            vector [ 1 ] / std::sqrt ( hypot ),
            vector [ 2 ] / std::sqrt ( hypot ),
    };

    // at the end, change the vector.
    for ( std::integral auto i = 0; i < 3; i++ )
    {
        vector [ i ] += deltas [ i ];
    }
}

// copied from my own library-in-progress, ML, but written knowing that it's
// going to run in a C++ 20 environment.
template < std::floating_point F > class matrix
{
    using column = std::vector< F >;
    std::vector< column > rows;
public:
    // no relation to the racing title originally released for the SNES, lol.
    static constexpr F zero = F { 0 };
    // no relation to the racing (league?).
    static constexpr F one  = F { 1 };

    matrix ( ) = default;
    matrix ( std::integral auto const &rows, std::integral auto const &cols )
    {
        for ( std::integral auto i = 0; i < rows; i++ )
        {
            column col { };
            for ( std::integral auto j = 0; j < cols; j++ )
            {
                col.push_back ( F { 0 } );
            }
            this->rows.push_back ( col );
        }
    }

    std::integral auto const row_count ( ) const noexcept
    {
        return rows.size ( );
    }

    std::integral auto const col_count ( ) const noexcept
    {
        return rows.at ( 0 ).size ( );
    }

    std::ranges::range auto &operator[] ( std::integral auto const &index )
    {
        return rows.at ( index );
    }

    std::ranges::range auto const &
            operator[] ( std::integral auto const &index ) const
    {
        return rows.at ( index );
    }

    std::random_access_iterator auto begin ( ) noexcept
    {
        return rows.begin ( );
    }

    std::random_access_iterator auto begin ( ) const noexcept
    {
        return rows.begin ( );
    }

    std::random_access_iterator auto end ( ) noexcept { return rows.end ( ); }

    std::random_access_iterator auto end ( ) const noexcept
    {
        return rows.end ( );
    }

    std::random_access_iterator auto cbegin ( ) const noexcept
    {
        return rows.cbegin ( );
    }

    std::random_access_iterator auto cend ( ) const noexcept
    {
        return rows.cend ( );
    }

    std::random_access_iterator auto rbegin ( ) noexcept
    {
        return rows.rbegin ( );
    }

    std::random_access_iterator auto rend ( ) noexcept { return rows.rend ( ); }

    std::random_access_iterator auto crbegin ( ) const noexcept
    {
        return rows.crbegin ( );
    }

    std::random_access_iterator auto crend ( ) const noexcept
    {
        return rows.crend ( );
    }

    static inline matrix identity ( std::integral auto const &size )
    {
        matrix output { size, size };
        for ( std::integral auto i = 0; i < size; i++ )
        {
            output [ i ][ i ] = one;
        }
        return output;
    }

    template < std::floating_point G,
               std::floating_point H = decltype ( F { 0 } + G { 0 } ) >
    matrix< H > operator+ ( matrix< G > const &that ) const
    {
        if ( row_count ( ) != that.row_count ( ) )
        {
            throw std::out_of_range ( "Row size mismatch" );
        }
        if ( col_count ( ) != that.col_count ( ) )
        {
            throw std::out_of_range ( "Col size mismatch" );
        }

        matrix< H > output { row_count ( ), col_count ( ) };

        // I really wish I could have made these two loops from a structured
        // binding declaration, unfortunately, there's no way to do that
        // without either discarding the result or discarding the qualifiers
        // on *this and that.
        for ( std::integral auto i = 0; i < row_count ( ); i++ )
        {
            for ( std::integral auto j = 0; j < col_count ( ); j++ )
            {
                output [ i ][ j ] = rows [ i ][ j ] + that [ i ][ j ];
            }
        }

        return output;
    }

    template < std::floating_point G,
               std::floating_point H = decltype ( F { 0 } + G { 0 } ) >
    matrix< H > operator- ( matrix< G > const &that ) const
    {
        if ( row_count ( ) != that.row_count ( ) )
        {
            throw std::out_of_range ( "Row size mismatch" );
        }
        if ( col_count ( ) != that.col_count ( ) )
        {
            throw std::out_of_range ( "Col size mismatch" );
        }

        matrix< H > output { row_count ( ), col_count ( ) };

        // I really wish I could have made these two loops from a structured
        // binding declaration, unfortunately, there's no way to do that
        // without either discarding the result or discarding the qualifiers
        // on *this and that.
        for ( std::integral auto i = 0; i < row_count ( ); i++ )
        {
            for ( std::integral auto j = 0; j < col_count ( ); j++ )
            {
                output [ i ][ j ] = rows [ i ][ j ] - that [ i ][ j ];
            }
        }

        return output;
    }

    template < std::floating_point G,
               std::floating_point H = decltype ( F { 0 } + G { 0 } ) >
    matrix< H > operator* ( matrix< G > const &that ) const
    {
        if ( col_count ( ) != that.row_count ( ) )
        {
            throw std::out_of_range ( "Matrix size mismatch" );
        }

        matrix< H > output { row_count ( ), that.col_count ( ) };

        for ( std::integral auto i = 0; i < row_count ( ); i++ )
        {
            for ( std::integral auto j = 0; j < that.col_count ( ); j++ )
            {
                H accumulated = H { 0 };
                for ( std::integral auto k = 0; k < col_count ( ); k++ )
                {
                    accumulated += rows [ i ][ k ] * that [ k ][ j ];
                }
                output [ i ][ j ] = accumulated;
            }
        }
        return output;
    }

    template < std::floating_point G,
               std::floating_point H = decltype ( F { 0 } + G { 0 } ) >
    std::vector< H > operator* ( std::vector< G > const &that ) const
    {
        if ( that.size ( ) != col_count ( ) )
        {
            throw std::out_of_range ( "Vector / Matrix size mismatch" );
        }
        std::vector< H > output;
        for ( auto const &row : *this )
        {
            H accumulated = H { 0 };
            for ( std::integral auto i = 0; i < col_count ( ); i++ )
            {
                accumulated += row [ i ] * that [ i ];
            }
            output.push_back ( accumulated );
        }
        return output;
    }

    template < std::floating_point G,
               std::floating_point H = decltype ( F { 0 } + G { 0 } ) >
    matrix< H > operator* ( G const &scalar ) const
    {
        matrix< H > result { row_count ( ), col_count ( ) };
        for ( std::integral auto i = 0; i < row_count ( ); i++ )
        {
            for ( std::integral auto j = 0; j < col_count ( ); j++ )
            {
                result [ i ][ j ] = rows [ i ][ j ] * scalar;
            }
        }
        return result;
    }

    template < std::floating_point G,
               std::floating_point H = decltype ( F { 0 } + G { 0 } ) >
    matrix< H > operator/ ( G const &scalar ) const
    {
        return *this * ( G { 1 } / scalar );
    }

    template < std::floating_point G,
               std::floating_point H = decltype ( F { 0 } + G { 0 } ) >
    matrix< H > augment ( matrix< G > const &that ) const
    {
        if ( row_count ( ) != that.row_count ( ) )
        {
            throw std::out_of_range ( "Row size mismatch" );
        }

        matrix< H > result { row_count ( ),
                             col_count ( ) + that.col_count ( ) };
        // row major order, so we go by column-major order to copy it properly.
        // so, we handle all of the columns in this and then all of the
        // columns in that.

        for ( std::integral auto i = 0; auto &row : result )
        {
            // columns from this
            std::integral auto j = 0;

            auto copy = [ & ] ( std::floating_point auto mem ) {
                result [ i ][ j++ ] = mem;
            };
            std::for_each ( rows [ i ].begin ( ), rows [ i ].end ( ), copy );
            std::for_each ( that [ i ].begin ( ), that [ i ].end ( ), copy );
            i++;
        }
        return result;
    }

    matrix< F > echelon ( ) const
    {
        matrix< F > result = *this;

        // function that simply eliminates negative 0.
        auto eliminate_neg_zero = [ & ] ( F &x ) {
            if ( x == -zero )
            {
                x = zero;
            }
        };

        // if less than two rows or there are no columns, then we are
        // already considered to be in echelon form since there is no reducing
        // that we can do.
        if ( result.row_count ( ) < 2 || result.col_count ( ) == 0 )
        {
            return result;
        }

        std::integral auto smallest_dimension =
                std::min ( row_count ( ), col_count ( ) );
        // the "forward" phase of transferring to rref form. Here we identify
        // the pivot positions and move them to the proper place.
        for ( std::integral auto i = 0; i < smallest_dimension; i++ )
        {
            if ( result [ i ][ i ] == 0 )
            {
                continue;
            }
            // make the diagonal value 1.
            F factor = result [ i ][ i ];
            for ( auto &x : result [ i ] )
            {
                x /= factor;
                eliminate_neg_zero ( x );
            }
            // eliminate the other elements in the column.
            for ( std::integral auto r = 0; auto &row : result )
            {
                if ( r == i )
                {
                    r++;
                    continue;
                } else
                {
                    F ratio = result [ r ][ i ];
                    for ( std::integral auto j = 0; j < col_count ( ); j++ )
                    {
                        result [ r ][ j ] -= ratio * result [ i ][ j ];
                        eliminate_neg_zero ( result [ r ][ j ] );
                    }
                    r++;
                }
            }

            auto is_row_zero = [] ( column v ) -> bool {
                bool any_nonzero = true;
                std::for_each ( v.begin ( ), v.end ( ), [ & ] ( F f ) {
                    any_nonzero &= ( f == zero );
                } );
                return any_nonzero;
            };

            // simple sorting to move the rows of zeros to the bottom
            bool swapped;
            do {
                swapped = false;
                for ( std::integral auto r = 1; r < row_count ( ); r++ )
                {
                    if ( is_row_zero ( result [ r - 1 ] )
                         && !is_row_zero ( result [ r ] ) )
                    {
                        std::swap ( result [ r - 1 ], result [ r ] );
                        swapped = true;
                    }
                }
            } while ( swapped );
        }
        // that was the forward phase. Now, all the rows have a pivot element
        // or are all zeros. The next step is to make sure that each pivot
        // column only has the pivot element and that all pivots are 1.0
        for ( std::integral auto r = 0; r < row_count ( ); r++ )
        {
            for ( std::integral auto c = 0; c < col_count ( ); c++ )
            {
                if ( result [ r ][ c ] != zero && result [ r ][ c ] != one )
                {
                    F pivot_element = result [ r ][ c ];
                    for ( auto &elem : result [ r ] ) { elem /= pivot_element; }
                    for ( std::integral auto i = 0; i < row_count ( ); i++ )
                    {
                        if ( i == r )
                        {
                            continue;
                        } else
                        {
                            F ratio = result [ i ][ c ];

                            std::integral auto j = 0;
                            for ( ; j < col_count ( ); j++ )
                            {
                                result [ i ][ j ] -= ratio * result [ r ][ j ];
                            }
                        }
                    }
                    break;
                }
            }
        }
        return result;
    }
};

/**
 * @brief Calculates the rref form of a random matrix of size 256 by 256 where
 * each element is between the minimum and maximum signed 32-bit integer values.
 * @note Since long-double is often implemented in software, this test goes a
 * bit beyond flops.
 */
void random_software_matrix_rref_test ( )
{
    using rng    = std::uniform_real_distribution< long double >;
    using matrix = matrix< long double >;
    // random with unpredictable seed.
    std::default_random_engine engine { std::random_device { }( ) };

    rng random { ( std::int32_t ) 0x80000000, ( std::int32_t ) 0x7FFFffff };

    matrix m { 0x100, 0x100 };
    for ( auto &row : m )
    {
        for ( auto &mem : row ) { mem = random ( engine ); }
    }

    m.echelon ( );
}

/**
 * @brief Calculates the rref form of a random matrix of size 256 by 256 where
 * each element is between the minimum and maximum signed 32-bit integer values.
 * @note This test is almost pure flops on a modern machine, but it's not
 * unreasonable for an older machine to need to implement double in software.
 */
void random_hardware_matrix_rref_test ( )
{
    using rng    = std::uniform_real_distribution< double >;
    using matrix = matrix< double >;
    // random with unpredictable seed.
    std::default_random_engine engine { std::random_device { }( ) };

    rng random { ( std::int32_t ) 0x80000000, ( std::int32_t ) 0x7FFFffff };

    matrix m { 0x100, 0x100 };
    for ( auto &row : m )
    {
        for ( auto &mem : row ) { mem = random ( engine ); }
    }

    m.echelon ( );
}

/**
 * @brief Calculates the rref form of a random matrix of size 256 by 256 where
 * each element is between the minimum and maximum signed 32-bit integer values.
 * @note Notice that the range of integer is beyond the range of numbers where
 * float has a maximum error < 1. This is intended as I wanted to keep the
 * same range of values, even though these values are not as precise. An
 * alternative would be to use the minimum and maximum values for a 23-bit
 * integer.
 * @note "gloves off" since countless machines implement a single precision
 * floating point and floats have the most highly optimized and parallelized
 * vector-instructions. These functions should take well to optimizations, and
 * I am excited to see the performance values :).
 */
void random_gloves_off_matrix_rref_test ( )
{
    using rng    = std::uniform_real_distribution< float >;
    using matrix = matrix< float >;
    // random with unpredictable seed.
    std::default_random_engine engine { std::random_device { }( ) };

    // I know these are narrowing conversions. Part of the loss of precision
    // with float is that we cannot store all the 32-bit integer values with
    // integer precision.
    rng random { ( float ) ( std::int32_t ) 0x80000000,
                 ( float ) ( std::int32_t ) 0x7FFFffff };

    matrix m { 0x100, 0x100 };
    for ( auto &row : m )
    {
        for ( auto &mem : row ) { mem = random ( engine ); }
    }

    m.echelon ( );
}