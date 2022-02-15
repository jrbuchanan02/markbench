/// benchmarking software that measures computing performance.
/// start simple: single thread single instruction (inc)
#include "resources.h++"
#include "test.h++"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
//#include"main2.h++"

std::size_t addForTime ( bool &go )
{
    std::size_t counter = 0;
    while ( go )
    {
        MARK_DO_FOR_16 ( __asm__( "add %rax, %rax" ); );
        counter += 16;
    }
    return counter;
}
MARK_ASM_INSTRUCTION_FOR_TIME ( "sub %rax, %rax", subtract )

int main ( int const argc, char const *const *const argv )
{
    std::cout << "Beginning add instruction test that will run for 5 "
                 "seconds...\n";
    TestInformation information { Milliseconds ( 5000 ),
                                  std::chrono::nanoseconds ( 0 ),
                                  addForTime,
                                  1 };
    runTest ( information );
    std::cout << "Test complete. Outputting results:\n";
    std::cout << "Requested time: " << information.requestedTime.count ( )
              << "ms\n";
    std::cout << "Actual time   : " << information.empiricalTime.count ( )
              << "ns\n";
    std::cout << "Requested Threads: " << information.requestedThreads << "\n";
    std::cout << "Actual Threads   : " << information.empiricalThreads << "\n";
    std::cout << "Total Instruction Count: " << information.total << "\n";
    std::cin.get ( );
    information.testFunction = std::function ( subtractForTime );
    runTest ( information );
    std::cout << "Test complete. Outputting results:\n";
    std::cout << "Requested time: " << information.requestedTime.count ( )
              << "ms\n";
    std::cout << "Actual time   : " << information.empiricalTime.count ( )
              << "ns\n";
    std::cout << "Requested Threads: " << information.requestedThreads << "\n";
    std::cout << "Actual Threads   : " << information.empiricalThreads << "\n";
    std::cout << "Total Instruction Count: " << information.total << "\n";
    std::cin.get ( );
}