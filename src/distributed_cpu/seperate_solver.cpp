//this file will use the awannacu algorithm to check visibility of the points.
// it will run seperately using MPI to distribute the load in cpu memory.
//This will run by selecting a 
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <array>
#include <span>
#include "solver/concepts.hpp"
#include <string_view>

#include <mpi.h>

auto check_visibility(const data_type data, index3 p1, index3 p2) -> bool
{
    const auto delta = (p2 - p1);
    const auto delta_abs = abs(delta);

    auto set_dir = [](const auto l, const auto r) {
        if (l > r) return 1;
        else if (l == r) return 0;
        else return -1;
    };

    const auto xs = set_dir(p2.x, p1.x);
    const auto ys = set_dir(p2.y, p1.y);
    const auto zs = set_dir(p2.z, p1.z);

    const auto max = std::max({delta_abs.x, delta_abs.y, delta_abs.z});

    bool valid{true};

    if (max == delta_abs.x) {
        auto p_1 = 2 * delta_abs.y - delta_abs.x;
        auto p_2 = 2 * delta_abs.z - delta_abs.x;
        while (p1.x != p2.x) {
            p1.x += xs;
            if (p_1 >= 0) {
                p1.y += ys;
                p_1 -= 2 * delta_abs.x;
            }
            if (p_2 >= 0) {
                p1.z += zs;
                p_2 -= 2 * delta_abs.x;
            }
            p_1 += 2 * delta_abs.y;
            p_2 += 2 * delta_abs.z;
            
            if (p1.z  < data(p1.x, p1.y)) { valid = false; }
        }
    }
    else if (max == delta_abs.y) {
        auto p_1 = 2 * delta_abs.x - delta_abs.y;
        auto p_2 = 2 * delta_abs.z - delta_abs.y;

        while (p1.y != p2.y) {
            p1.y += ys;
            if (p_1 >= 0) {
                p1.x += xs;
                p_1 -= 2 * delta_abs.y;
            }
            if (p_2 >= 0) {
                p1.z += zs;
                p_2 -= 2 * delta_abs.y;
            }
            p_1 += 2 * delta_abs.x;
            p_2 += 2 * delta_abs.z;
            
            if (p1.z  < data(p1.x, p1.y)) { valid = false; }
        }
    }
    else {
        auto p_1 = 2 * delta_abs.y - delta_abs.z;
        auto p_2 = 2 * delta_abs.x - delta_abs.z;
    
        while (p1.z != p2.z) {
            p1.z += zs;
            if (p_1 >= 0) {
                p1.y += ys;
                p_1 -= 2 * delta_abs.z;
            }
            if (p_2 >= 0) {
                p1.x += xs;
                p_2 -= 2 * delta_abs.z;
            }
            p_1 += 2 * delta_abs.y;
            p_2 += 2 * delta_abs.x;
            
            if (p1.z  < data(p1.x, p1.y)) { valid = false; }
        }
    }

    return valid;
}

int main(int argc, char** argv) {
    // Initialize MPI
    MPI_Init(&argc, &argv);



    // Finalize MPI
    MPI_Finalize();
    
    return 0;
}