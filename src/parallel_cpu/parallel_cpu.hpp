#pragma once

#include "core.hpp"
#include <vector>
#include <cstdint>

auto calculateVisibility(const std::vector<int16_t>& height_map, 
                         size_t width, size_t height, 
                         int radius = 100, int angle = 12) -> std::vector<unsigned int>;