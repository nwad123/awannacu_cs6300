#include "core.hpp"
#include "fmt/base.h"
#include "serial.hpp"
#include <algorithm>
#include <cstdlib>

[[nodiscard]]
static inline auto bad_usage(const tcb::span<char*> args) -> int;

auto main(int argc, char** argv) -> int
{
    const tcb::span<char*> args = tcb::span(argv, static_cast<size_t>(argc));

    if (args.size() < 3) { return bad_usage(args); }

    solve(args[1], args[2]);

    return EXIT_SUCCESS;
}

auto bad_usage(const tcb::span<char*> args) -> int 
{
    fmt::println("Usage: {} <input-file> <output-file>", args[0]);
    return EXIT_FAILURE;
}
