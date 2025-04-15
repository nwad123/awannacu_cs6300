#include "parallel_cpu.hpp"
#include "core.hpp"
#include <fmt/core.h>
#include <algorithm>

#ifdef _OPENMP 
#include <omp.h>
#endif
