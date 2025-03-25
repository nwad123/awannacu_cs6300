# Test Suite

## Running the Tests 

With the provided makefile in the top level directory, simply run:

```sh 
make test_lib
```

## Adding a New Test 

1. Create a new `.cpp` file in this directory with the name of your test.
2. Add it to the tests in the [CMakeLists](./CMakeLists.txt) file in this directory
   by using the `add_test()` function. The function is called by declaring the test 
   name, and then the file in which it's defined. 

   For example, if you want to test the `party_animal()` function, you might put 
   together test cases in [party_animal.cpp], and you would add it to CMakeLists 
   as follows:

   ```cmake 
   add_test(party_animal party_animal.cpp)
   ```
3. Add tests into your `.cpp` file. Tests are declared with the following format:
   
   ```cpp
   TEST(/* Testing Section */, /* Test Name */) {
       // ... 
   }
   ```

   To continue our example from before, our test might look like this:

   ```cpp
   TEST(PartyAnimalTest, DoTheyParty) {
       auto animal = party_animal();

       EXPECT_TRUE(animal.does_it_party());
   }
   
   TEST(PartyAnimalTest, OnlyOneAnimal) {
       auto animal = party_animal();

       EXPECT_EQ(animal.count(), 1);
   }
   ```

   There are a lot of equality macros, but the basics are:

   - `EXPECT_TRUE`
   - `EXPECT_FALSE`
   - `EXPECT_EQ`

   Check the [GTest docs](https://google.github.io/googletest/reference/assertions.html) for more information.

