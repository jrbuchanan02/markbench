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

