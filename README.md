# markbench

Markbench is a performance benchmarking tool designed to improve on other
popular and successful benchmarks. Namely, the whetstone, dhrystone, and
GeekBench benchmarks. Markbench records a score in rhedstones, a pun on
whestone / dhrystone and a reference to MineCraft.

## What is there to improve on those other benchmarks

Markbench addresses these issues with the three benchmarks it seeks to improve
upon.

1. Whetstones strictly measure floating point operations. While FlOPS are
critical to high performance computing, most operations on a modern computer
use integers.

2. Dhrystones, due to the age of the benchmark, have become optimized for.
Moreover, many operations in the dhrystone benchmark focus more on string
copying and comparison, which less reflects the needs of modern
software (especially since these strings are aligned on machine-natural
boundries nad of known length).

3. GeekBench is a relative scale, meaning that GeekBench 4 and GeekBench 5
scores do not allow a comparison between themselves.

## What exactly is a rhedstone

A rhedstone measures one iteration through any of the tests per nanosecond. That
is, if your machine performs 500,000,000 iterations of test 1 in one second and
250,000,000 iterations of test 2 in one second, and 250,000,000 iterations of
test 3 in one second, then it is a (500,000,000 + 250,000,000 + 250,000,000) /
1,000,000,000 = 1 rhedstone machine. Each machine has two rhedstone scores,
one score measures single-threaded performance and the other score measures
performance with all threads at once.

Since this system means that adding tests breaks the comparison, Markbench
will eventually allow for testing any test version.

## Test Wishlist

Here is a wishlist for the various tests I would like to add but I am not sure
how exactly to implement:

1. A version of the window-creation and destruction test that works on Linux
and Mac OS, and, preferably, one that uses identical code on all operating
systems.

2. A test which runs Java Bytecode. Not necessarily Java code, but the bytecode
used by the JRE. Many programs use this bytecode and this metric can be useful.

3. A test which runs the Common Intermediate Language / Common Language Interface
bytecode. Many programs (at least on Windows) use this bytecode and this
metrix can be useful.

4. Tests which rely on computational geometry. That is, tests of whether a line
in 3D space passes through a triangle in 3D space. Almost all 3D engines heavily
utilize any and all forms of computational geometry and these calculations
constitute a not-insignificant part of the modern computing world.

5. Tests which benchmark AES encryption / decrpytion speeds on more generic
data. That is, randomly generated data.

## Changes Wishlist

Here is a wishlist of the various improvements on the benchmarking and scoring
system that I would like to see.

- Rewarding scores which are more similar throuhgout the test suite.
- Accounting for some tests expecting a lower score than others. We expect the
null test to score the highest and the cache-miss to score the lowest (as of
the time of writing), but, both count the same in the final score. Taking less
than around five seconds on the cache miss test for single threads and less than
around ten seconds on the multithreaded cache miss test should provide a massive
boost in score.
