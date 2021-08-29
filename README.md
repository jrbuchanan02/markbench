# markbench
 Markbench is a benchmarking software designed for x86-64. Markbench is terminal based, and seems capable of reporting near the theoretical speeds of the processor (on my machine, 5.3 GHz). I plan on adding features to markbench as a benchmarking tool. 
## Compilation Instructions:
 1. Clone / download the code on your local machine with g++ installed.
 2. In the directory with `src/` run `g++ ./src/main.c++ -Ofast`. Markbench will be in the output file (either `a.exe` for Windows or `a.out` for Mac and Linux).
## Current features:
 1. Markbench reports the instruction and result throughput of the cpu over the ~5 second sampling period. For certain instructions, such as ADD RAX, RAX, this speed is known to be close enough to the maximum processor speed. 
 2. Markbench uses its own resource file (the .mkbrs file) to store its resources. While limited, this format (delimited by newlines) works well enough for now (though it should probably be made more versatile in the future)
## Current Issues:
 1. Markbench assumes the presence of AVX instructions but does not require the ABI for AVX. In other words, Markbench should mysteriously crash when running the AVX-requiring tests on a machine that does not support AVX instructions.
 2. Markbench's code is not as neat as I would like it to be (as in the code feels cluttered). (The solution is obviously to move the function-implementations into their own .c++ files, but refactoring is boring)
 3. Markbench's implementations currently only support single instruction commands, however, I would prefer to also include some common pieces of code, including, but not limited to mergesort, std-sort, bubble sort, and pidgeonhole-sort.
 4. The CPU Clock Estimate test and the `--clock` commandline switch both assume that the ALU is double-pumped. On (really old) machines where the ALU is **not** double-pumped, `--clock` and the CPU Clock Estimate test will report half the true value.
## Wishlist:
 1. (optionally) reporting benchmarks to some database so that processor speeds may be recorded. 
 2. Adding more tests. At some point I would love to test all (non-system) instructions available on the machine and to test certain code implementations, such as creating and destroying a window or allocating and deallocating memory.
 3. Adding additional languages and locales. Localization is not my C++ strong suit, but it does not seem so hard so far...
 4. While there is already one commandline switch (`--clock`), I would like to add more of these and place them in a resource file.