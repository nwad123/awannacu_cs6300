#include "core.hpp"
#include "fmt/base.h"
#include "serial.hpp"
#include <algorithm>
#include <cstdlib>
#include <chrono>

[[nodiscard]]
static inline auto bad_usage(const tcb::span<char*> args) -> int;

auto main(int argc, char** argv) -> int
{
    const tcb::span<char*> args = tcb::span(argv, static_cast<size_t>(argc));

    if (args.size() < 3) { return bad_usage(args); }

    const auto [width, height] = [&]() -> std::pair<size_t, size_t>{
        if (args.size() == 5) {
            return {std::atoi(args[3]), std::atoi(args[4])};
        } else {
            return {6000, 6000};
        }
    }();

    // time the algorithm
    timer time;
    time.reset();
    
    solve(args[1], args[2], width, height);

    // display the elapsed time
    fmt::println("Elapsed time: {} ms", time.read());

    return EXIT_SUCCESS;
}

auto bad_usage(const tcb::span<char*> args) -> int 
{
    fmt::println("Usage: {} <input-file> <output-file> [<width> <height>]", args[0]);
    return EXIT_FAILURE;
}
