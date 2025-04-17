#include "core.hpp" // Include the header declaring write_output
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include <cstdint>
#include <numeric> // For std::iota
#include <cstdio>  // For std::remove

// Helper function to read binary file content into a vector
std::vector<int16_t> read_binary_file(const std::filesystem::path& file_path) {
    std::vector<int16_t> data;
    std::ifstream input(file_path, std::ios::binary);
    if (!input.is_open()) {
        // Return empty vector if file can't be opened (test will likely fail)
        return data;
    }

    input.seekg(0, std::ios::end);
    std::streampos size = input.tellg();
    input.seekg(0, std::ios::beg);

    if (size > 0 && size % sizeof(int16_t) == 0) {
        data.resize(static_cast<size_t>(size) / sizeof(int16_t));
        input.read(reinterpret_cast<char*>(data.data()), size);
    }
    input.close();
    return data;
}

// Define a test fixture for managing temporary files if needed,
// but for simple cases, managing within the test is fine.

// Test case for writing valid data
TEST(WriteOutputTest, WriteValidData) {
    const std::filesystem::path test_file = "test_output_valid.raw";
    std::vector<int16_t> expected_data = {10, -20, 300, -4000, INT16_MAX, INT16_MIN, 0};

    // Ensure the file doesn't exist before the test
    std::filesystem::remove(test_file);
    ASSERT_FALSE(std::filesystem::exists(test_file));

    // Call the function under test
    write_output(test_file, tcb::span<int16_t>(expected_data.data(), expected_data.size()));

    // Verify the file was created
    ASSERT_TRUE(std::filesystem::exists(test_file));

    // Verify the file content
    const auto actual_data = read_binary_file(test_file);
    ASSERT_EQ(actual_data.size(), expected_data.size());
    EXPECT_EQ(actual_data, expected_data); // Use EXPECT_EQ to see all differences if they exist

    // Clean up the test file
    std::filesystem::remove(test_file);
}

// Test case for writing empty data
TEST(WriteOutputTest, WriteEmptyData) {
    const std::filesystem::path test_file = "test_output_empty.raw";
    std::vector<int16_t> empty_data;

    // Ensure the file doesn't exist before the test
    std::filesystem::remove(test_file);
    ASSERT_FALSE(std::filesystem::exists(test_file));

    // Call the function under test
    // Note: This should print a warning to stderr/stdout, which we aren't capturing here.
    write_output(test_file, tcb::span<int16_t>(empty_data.data(), empty_data.size()));

    // Verify the file was created
    ASSERT_TRUE(std::filesystem::exists(test_file));

    // Verify the file is empty
    ASSERT_EQ(std::filesystem::file_size(test_file), 0);

    // Verify reading the file results in an empty vector
    const auto actual_data = read_binary_file(test_file);
    EXPECT_TRUE(actual_data.empty());

    // Clean up the test file
    std::filesystem::remove(test_file);
}

// Test case attempting to write to an invalid location (e.g., a directory path)
// Note: This test's success depends on the OS/filesystem disallowing writes to directory paths like files.
TEST(WriteOutputTest, WriteToInvalidPath) {
    // Assuming "." (current directory) is not a valid file path to open for writing
    const std::filesystem::path invalid_path = ".";
    std::vector<int16_t> some_data = {1, 2, 3};

    // We expect the function to fail gracefully (print an error, not crash)
    // We don't expect it to create a file named "." or modify the directory.
    // The primary check is that the test completes without crashing.
    // We also check that no file named "." (if interpreted as a file) was created unexpectedly.
    // Note: Capturing stderr to check for the error message is possible but adds complexity.

    ASSERT_NO_THROW(write_output(invalid_path, tcb::span<int16_t>(some_data.data(), some_data.size())));

    // Check that the path still refers to a directory and wasn't somehow replaced by a file
    EXPECT_TRUE(std::filesystem::is_directory(invalid_path));
}

// Test case for writing a larger amount of data
TEST(WriteOutputTest, WriteLargeData) {
    const std::filesystem::path test_file = "test_output_large.raw";
    std::vector<int16_t> expected_data(10000); // 10k elements
    // Fill with sequential data for easy verification
    std::iota(expected_data.begin(), expected_data.end(), static_cast<int16_t>(-5000));

    // Ensure the file doesn't exist before the test
    std::filesystem::remove(test_file);
    ASSERT_FALSE(std::filesystem::exists(test_file));

    // Call the function under test
    write_output(test_file, tcb::span<int16_t>(expected_data.data(), expected_data.size()));

    // Verify the file was created
    ASSERT_TRUE(std::filesystem::exists(test_file));

    // Verify the file content
    const auto actual_data = read_binary_file(test_file);
    ASSERT_EQ(actual_data.size(), expected_data.size());
    EXPECT_EQ(actual_data, expected_data);

    // Clean up the test file
    std::filesystem::remove(test_file);
}

// You might add more tests, e.g., overwriting an existing file,
// but these cover the main success, edge, and failure modes.
