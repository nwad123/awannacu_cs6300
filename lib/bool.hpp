#pragma once

// C++ literally has the worst std::vector implementation, because we can't 
// pass it between devices because it doesn't technically have an address 
// because it's really a bit-packed field.
//
// Due to this, I wrote a bool class that literally does the same this as 
// the built in `bool` type, BUT you can instantiate a _real_ vector of 
// `Bool`.
//
// With optimizations I think this should be same speed as normal bool.

// DISCLAIMER: All operators (!, ||, etc.) were written by AI. I did edit them 
// because it made bad choices though.

struct Bool {
    bool data_{false};
    constexpr Bool() = default;
    constexpr Bool(const bool b) : data_(b) {}
    bool operator()() const { return data_; }
    operator bool() const { return data_; }

    // Boolean NOT
    constexpr Bool operator!() const { return Bool(!data_); }

    // Boolean AND
    constexpr Bool operator&&(const Bool other) const { return Bool(data_ && other.data_); }

    // Boolean OR
    constexpr Bool operator||(const Bool other) const { return Bool(data_ || other.data_); }

    // Boolean XOR
    constexpr Bool operator^(const Bool other) const { return Bool(data_ ^ other.data_); }
};

// Overload operators for direct use with bool
constexpr Bool operator&&(const bool lhs, const Bool rhs) { return Bool(lhs && rhs.data_); }
constexpr Bool operator&&(const Bool lhs, const bool rhs) { return Bool(lhs.data_ && rhs); }

constexpr Bool operator||(const bool lhs, const Bool rhs) { return Bool(lhs || rhs.data_); }
constexpr Bool operator||(const Bool lhs, const bool rhs) { return Bool(lhs.data_ || rhs); }

constexpr Bool operator^(const bool lhs, const Bool rhs) { return Bool(lhs ^ rhs.data_); }
constexpr Bool operator^(const Bool lhs, const bool rhs) { return Bool(lhs.data_ ^ rhs); }

