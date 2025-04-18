#pragma once

#include <chrono>
#include <cstdint>

template <typename Unit = std::chrono::milliseconds>
class timer {
public:
  timer() : start_time_(std::chrono::steady_clock::now()) {}
  
  /// @brief Resets the timer to the current time.
  /// 
  /// This function resets the timer by setting the start time to the current
  /// time. Subsequent calls to `read()` will measure the elapsed time since
  /// this reset.
  inline auto reset() noexcept -> void {
    start_time_ = std::chrono::steady_clock::now();
  }

  /// @brief Alias to `reset()`
  inline auto start() noexcept -> void {
    reset();
  }

  /// @brief Reads the elapsed time since the timer was started or last reset.
  ///
  /// This function calculates the elapsed time since the timer was started or
  /// last reset, and returns the duration in the specified `Unit`.
  ///
  /// @return The elapsed time in the specified `Unit`.
  inline auto read() const noexcept -> uint64_t {
    const auto end_time = std::chrono::steady_clock::now();
    const auto count = [&](){
        // never return a negative count
        const auto c_ = std::chrono::duration_cast<Unit>(end_time - start_time_).count();
        if (c_ < 0) {
            return std::numeric_limits<decltype(c_)>::max();
        }
        return c_;
    }();
    return static_cast<uint64_t>(count);
  }

private:
  std::chrono::steady_clock::time_point start_time_;
};
