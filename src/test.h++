/**
 * @file test.h++
 * @author Joshua Buchanan (joshuarobertbuchanan@gmail.com)
 * @brief A test setup
 * @version 1
 * @date 2022-02-15
 *
 * @copyright Copyright (C) 2022. Intellectual property of the author(s) listed above.
 *
 */
#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <thread>

using SteadyClock = std::chrono::steady_clock;
using Milliseconds = std::chrono::milliseconds;
// expected to be of the form
// while (bool &) { actions...; counter+=n;} return counter;
using TestFunction = std::function<std::size_t(bool &)>;

inline void waitFor(unsigned long long int const millis)
{
    auto start = SteadyClock::now();
    while (SteadyClock::now() - start < Milliseconds(millis))
    {
        std::this_thread::sleep_for(Milliseconds(1));
    }
}

inline std::uint64_t getThreadNumber(std::int64_t const from)
{
    if (from < 1)
    {
        return std::thread::hardware_concurrency();
    }
    else
    {
        return from < std::thread::hardware_concurrency() ? from : std::thread::hardware_concurrency();
    }
}

struct TestInformation
{
    // time requested for test
    Milliseconds requestedTime = Milliseconds(0);
    // time it took to complete the test
    std::chrono::nanoseconds empiricalTime = std::chrono::nanoseconds(0);
    // the function that comprises the test
    TestFunction testFunction = [&](bool &)
    { return std::size_t(0); };
    // the amount of threads requested to run on, 0 means hardware concurrency
    std::uint64_t requestedThreads = 0;
    // the amount of threads that actually ran. Will not exceed hardware
    // concurrency.
    std::uint64_t empiricalThreads = 0;
    // sum of the values given by all counters.
    std::size_t total = 0;
    // the values from each individual counter. Length is empirical threads.
    std::size_t *counters = nullptr;

    ~TestInformation()
    {
        if (counters)
        {
            delete[] counters;
            counters = nullptr;
        }
    }
};

void runTest(TestInformation &information)
{
    information.empiricalThreads = getThreadNumber(information.requestedThreads);
    if (information.counters)
    {
        delete[] information.counters;
    }
    information.counters = new std::size_t[information.empiricalThreads];
    for (std::size_t i = 0; i < information.empiricalThreads; i++)
    {
        information.counters[i] = 0;
    }
    std::atomic_size_t startsLine = information.empiricalThreads;
    std::atomic_size_t finishLine = information.empiricalThreads;
    std::atomic_bool onYourMarks = false;
    bool starterPistol = false;
    auto test = [&](std::size_t id)
    {
        startsLine.fetch_sub(1);
        while (!onYourMarks.load())
        {
        }
        information.counters[id] = information.testFunction(starterPistol);
        finishLine.fetch_sub(1);
    };
    std::thread threads[256]; // one beyond the maximum amount of threads currently
                              // possible on a single chip in x86.
    for (std::size_t i = 0; i < information.empiricalThreads; i++)
    {
        std::construct_at(&threads[i], [=, &test]()
                          { test(i); });
        threads[i].detach();
    }
    while (startsLine.load())
    {
    }
    // start time, everyone's ready
    auto start = SteadyClock::now();
    starterPistol = true;
    onYourMarks.store(true);
    waitFor(information.requestedTime.count());
    starterPistol = false;
    while (finishLine.load())
    {
    }
    for (std::size_t i = 0; i < information.empiricalThreads; i++)
    {
        while (threads[i].joinable())
            threads[i].join();
    }
    auto finish = SteadyClock::now();
    information.empiricalTime = (finish - start);
    for (std::size_t i = 0; i < information.empiricalThreads; i++)
    {
        information.total += information.counters[i];
    }
}

#define MARK_DO_FOR_16(...) \
    __VA_ARGS__             \
    __VA_ARGS__             \
    __VA_ARGS__             \
    __VA_ARGS__             \
    __VA_ARGS__             \
    __VA_ARGS__             \
    __VA_ARGS__             \
    __VA_ARGS__             \
    __VA_ARGS__             \
    __VA_ARGS__             \
    __VA_ARGS__             \
    __VA_ARGS__             \
    __VA_ARGS__             \
    __VA_ARGS__             \
    __VA_ARGS__             \
    __VA_ARGS__

#define MARK_ASM_INSTRUCTION_FOR_TIME(INST, name) \
    std::size_t name##ForTime(bool &go)           \
    {                                             \
        std::size_t counter = 0;                  \
        while (go)                                \
        {                                         \
            MARK_DO_FOR_16(__asm__(INST);)        \
            counter += 16;                        \
        }                                         \
        return counter;                           \
    }
    