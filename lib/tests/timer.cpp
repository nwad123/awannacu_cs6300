#include "core.hpp" // Include the header for the timer class
#include <gtest/gtest.h>
#include <thread>   // For std::this_thread::sleep_for
#include <chrono>   // For duration types and literals (e.g., ms, us)
#include <cstdint>  // For uint64_t

// Tests written by AI, reviewed by humans.

// Use chrono literals for convenience (e.g., 100ms)
using namespace std::chrono_literals;

// Test fixture is not strictly necessary for these simple tests,
// but could be used if more complex setup/teardown were needed.

// Test basic timing functionality with default milliseconds
TEST(TimerTest, MeasuresElapsedTimeMilliseconds) {
    timer<> t; // Default template argument is std::chrono::milliseconds

    // Wait for a known duration
    const auto wait_duration = 100ms;
    std::this_thread::sleep_for(wait_duration);

    // Read the elapsed time
    uint64_t elapsed_ms = t.read();

    // Check if the elapsed time is roughly correct.
    // Allow for some scheduling inaccuracies and overhead.
    // Expect elapsed time to be >= wait duration.
    EXPECT_GE(elapsed_ms, static_cast<uint64_t>(wait_duration.count()));
    // Expect elapsed time to be reasonably close (e.g., less than double)
    EXPECT_LT(elapsed_ms, static_cast<uint64_t>((wait_duration * 2).count()));

    // Wait some more
    const auto additional_wait = 50ms;
    std::this_thread::sleep_for(additional_wait);

    // Read again, should be cumulative
    uint64_t total_elapsed_ms = t.read();
    const auto total_wait_duration = wait_duration + additional_wait;

    EXPECT_GE(total_elapsed_ms, static_cast<uint64_t>(total_wait_duration.count()));
    EXPECT_LT(total_elapsed_ms, static_cast<uint64_t>((total_wait_duration * 2).count()));
    EXPECT_GT(total_elapsed_ms, elapsed_ms); // Time should have increased
}

// Test the reset() functionality
TEST(TimerTest, ResetStartsMeasurementOver) {
    timer<> t;

    // Wait, then reset
    std::this_thread::sleep_for(50ms);
    t.reset();

    // Wait again for a specific duration after reset
    const auto wait_after_reset = 120ms;
    std::this_thread::sleep_for(wait_after_reset);

    // Read the elapsed time - should only measure since the reset
    uint64_t elapsed_ms_after_reset = t.read();

    // Check if the time measured is approximately the duration *after* reset
    EXPECT_GE(elapsed_ms_after_reset, static_cast<uint64_t>(wait_after_reset.count()));
    EXPECT_LT(elapsed_ms_after_reset, static_cast<uint64_t>((wait_after_reset * 2).count()));
    // Crucially, it should be much less than the total time (50 + 120 = 170ms)
    EXPECT_LT(elapsed_ms_after_reset, 170); // Check it's not measuring from the start
}

// Test the start() alias for reset()
TEST(TimerTest, StartAliasResetsMeasurement) {
    timer<> t;

    // Wait, then use start()
    std::this_thread::sleep_for(60ms);
    t.start(); // Alias for reset()

    // Wait again
    const auto wait_after_start = 80ms;
    std::this_thread::sleep_for(wait_after_start);

    // Read the elapsed time - should only measure since start() was called
    uint64_t elapsed_ms_after_start = t.read();

    // Check if the time measured is approximately the duration *after* start()
    EXPECT_GE(elapsed_ms_after_start, static_cast<uint64_t>(wait_after_start.count()));
    EXPECT_LT(elapsed_ms_after_start, static_cast<uint64_t>((wait_after_start * 2).count()));
}


// Test using a different time unit (microseconds)
TEST(TimerTest, MeasuresElapsedTimeMicroseconds) {
    // Explicitly specify microseconds as the unit
    timer<std::chrono::microseconds> t_us;

    // Wait for a duration (e.g., 50 milliseconds = 50,000 microseconds)
    const auto wait_duration = 50ms;
    std::this_thread::sleep_for(wait_duration);

    // Read the elapsed time in microseconds
    uint64_t elapsed_us = t_us.read();

    // Convert expected wait duration to microseconds for comparison
    const uint64_t expected_us = std::chrono::duration_cast<std::chrono::microseconds>(wait_duration).count();

    // Check if the elapsed time is roughly correct in microseconds
    EXPECT_GE(elapsed_us, expected_us);
    EXPECT_LT(elapsed_us, expected_us * 2); // Allow reasonable overhead/inaccuracy
}

// Test reading the timer immediately after creation (should be near zero)
TEST(TimerTest, ReadImmediatelyIsNearZero) {
    timer<> t_ms;
    timer<std::chrono::microseconds> t_us;
    timer<std::chrono::nanoseconds> t_ns;

    uint64_t elapsed_ms = t_ms.read();
    uint64_t elapsed_us = t_us.read();
    uint64_t elapsed_ns = t_ns.read();

    // Expect the elapsed time to be very small.
    // Allow a small margin for the execution time of read() itself.
    EXPECT_LE(elapsed_ms, 5);   // Less than or equal to 5 ms
    EXPECT_LE(elapsed_us, 5000); // Less than or equal to 5000 us (5 ms)
    EXPECT_LE(elapsed_ns, 5000000); // Less than or equal to 5,000,000 ns (5 ms)
}

