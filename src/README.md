# Solvers Library

This is where the bulk of our library is going to go. Each solver is relatively 
independant of each other, you just need to implement the `solve` method on 
each instance in order to get it to work correctly.

@jared, when you get to the GPU impl we'll have to link cuda using cmake somehow.
I don't know how to do that, but tag me when you get to that point and I'll add 
it into the build process.

## The `solve` Method 

```cpp
solve(data_type data, index2 point) -> output_type;
```

The solve method is what will actually do all the heavy lifting for this project.
Here's how you can approach implementing it:

### Tackling the inputs

The two inputs to the solve method are `data_type` and `index2`. Behind the scenes,
`data_type` is a `mat_2d_i16`, which in turn is a `Kokkos::mdspan`. `Kokkos::mdspan` is 
a non-owning multidimensional span around the input data and is used to access 
the height at each location in the map. `index2` is an (x, y) coordinate where you 
want to start the search from.

1. `data` - multidimensional spans are awesome. They turn a linear storage of data 
   (typically a `vector`) into a multidimensional array you can access willy-nilly.
   The documentation for `std::mdspan` on the internet works for `Kokkos::mdspan`, 
   as it's a standards conforming implementation.

   Here's some basics to get you started:

   - Getting the width and height of the input data:

     ```cpp
     auto width = data.extents(0);
     auto height = data.extents(1);
     ```
   - Accessing data with (x,y) coordinates:
     
     ```cpp
     Kokkos::mdspan data = /* ... */;
     auto z = data(x, y);
     ```

   - Changing data with (x, y) coordinates:

     ```cpp 
     data(x, y) = z;
     ```
   
   - Getting the underlying pointer to the original data (say if you need to use 
     `memcpy` or something.

     ```cpp 
     auto ptr = data.data_handle();
     ```
    
   That should give a good handle on how to start with `mdspan`.

2. `point` - this is just where you start at. Access it with `point.x`, `point.y`.

### Looking at the Output 

The output is an owning vector of `Bool`. We use a custom `vector<Bool>` rather than 
`vector<bool>` because working with `vector<bool>` was actually so sad.

In the serial implementation, there is an example of how to assign the output by wrapping 
it inside of the `solve` function with an `mdspan`. That is done to make it super easy to 
access the (x, y) coordinates of the output, as shown in the example.

```cpp
// allocate some space for the output 
output_type output_vec(width * height, false);

// make a little wrapper for the output with a span 
auto output = Kokkos::mdspan(output_vec.data(), data.extents());

//....

if (/* is visible from starting point */)
    output(x,y) = true;
else 
    output(x,y) = false;
```

The owning vector object is returned from the function, because that way the compiler knows 
that the memory for the vector should not be freed until later.

## Testing

There is a test directory set up, with some basic test harnesses. I figure we'll 
first just test a flat plane, a sloped plane, and then move forward from there.

If we want to do automated testing, we'll have to spend some more time on that.
