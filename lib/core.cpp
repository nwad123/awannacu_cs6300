#include "core.hpp"
#include <fmt/core.h>

auto shared() -> void
{
    fmt::println("I'm a shared library function!");
}