# Scaling Study

Nick Waddoups, Scott Pinkston, Jared Moulton

## Project Description

Given a digital elevation map we compute the number of pixels visible from each pixel in a 100-pixel
radius. This information is then stored in an image format similar to input format, where each pixel
value represents the number of visible pixels that it can see. An example output of this is given
in Figure 1.

![Visibility Map](./visibility_map_header.png)

## Project Build

See the [README](./README.md) on our Github page for instructions on how to build this project.

## Scaling Results

### Shared Memory Serial vs. Parallel Execution

For this we ran `./build/src/parallel_cpu/par_cpu input.raw output.raw 6000 6000 36 n`, where `n` was the
number of specified threads. We used the `lonepeak` CHPC cluster to do these tests, and we used the
following parameters:

- Nodes: 1
- Tasks: 32

| Threads | Time (ms) |
|:----:|:----:|
| 1 | 88044 ms |

![Distributed cpu vs. gpu]()

### Single GPU Execution

![Distributed cpu vs. gpu]()

### Distributed CPU vs. GPU Execution

![Distributed cpu vs. gpu]()
