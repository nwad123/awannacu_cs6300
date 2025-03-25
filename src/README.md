# Solvers Library

This is where the bulk of our library is going to go. Each solver is relatively 
independant of each other, you just need to implement the `solve` method on 
each instance in order to get it to work correctly.

@jared, when you get to the GPU impl we'll have to link cuda using cmake somehow.
I don't know how to do that, but tag me when you get to that point and I'll add 
it into the build process.

## The `solve` Method 

```cpp
solve(input_data data, index3 point) -> output_type;
```

The solve method is what will actually do all the heavy lifting for this project.
Currently you have two choices for the type of `input_data`:

1. `mat2d_i16` (Default) - This is a 2d non-owning matrix of int16_t values. You 
   can query the size of the matrix like so:

   ```cpp
   auto width = data.extents(0);
   auto height = data.extents(1);
   ```

2. `std::span<int16_t>` - This is just a non-owning pointer with an associated type 
   and size. You will have to write your own 2-dimensional indexing code for this 
   bad boi to get the heights at a x,y coordinate. 

## Testing

There is a test directory set up, with some basic test harnesses. I figure we'll 
first just test a flat plane, a sloped plane, and then move forward from there.
