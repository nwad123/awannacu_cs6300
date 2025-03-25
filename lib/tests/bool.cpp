#include "gtest/gtest.h"
#include "bool.hpp"

// DISCLAIMER: tests written by AI

TEST(BoolTest, Constructor) {
    Bool b1;
    ASSERT_FALSE(b1()); // Default constructor should initialize to false

    Bool b2(true);
    ASSERT_TRUE(b2());

    Bool b3(false);
    ASSERT_FALSE(b3());
}

TEST(BoolTest, OperatorCall) {
    Bool b1(true);
    ASSERT_TRUE(b1());

    Bool b2(false);
    ASSERT_FALSE(b2());
}

TEST(BoolTest, OperatorNot) {
    Bool b1(true);
    ASSERT_FALSE(!b1());

    Bool b2(false);
    ASSERT_TRUE(!b2());
}

TEST(BoolTest, OperatorAnd) {
    Bool t(true);
    Bool f(false);

    ASSERT_FALSE(f && f);
    ASSERT_FALSE(f && t);
    ASSERT_FALSE(t && f);
    ASSERT_TRUE(t && t);

    ASSERT_FALSE(false && f);
    ASSERT_FALSE(false && t);
    ASSERT_FALSE(true && f);
    ASSERT_TRUE(true && t);

    ASSERT_FALSE(f && false);
    ASSERT_FALSE(f && true);
    ASSERT_FALSE(t && false);
    ASSERT_TRUE(t && true);
}

TEST(BoolTest, OperatorOr) {
    Bool t(true);
    Bool f(false);

    ASSERT_FALSE(f || f);
    ASSERT_TRUE(f || t);
    ASSERT_TRUE(t || f);
    ASSERT_TRUE(t || t);

    ASSERT_FALSE(false || f);
    ASSERT_TRUE(false || t);
    ASSERT_TRUE(true || f);
    ASSERT_TRUE(true || t);

    ASSERT_FALSE(f || false);
    ASSERT_TRUE(f || true);
    ASSERT_TRUE(t || false);
    ASSERT_TRUE(t || true);
}

TEST(BoolTest, OperatorXor) {
    Bool t(true);
    Bool f(false);

    ASSERT_FALSE(f ^ f);
    ASSERT_TRUE(f ^ t);
    ASSERT_TRUE(t ^ f);
    ASSERT_FALSE(t ^ t);

    ASSERT_FALSE(false ^ f);
    ASSERT_TRUE(false ^ t);
    ASSERT_TRUE(true ^ f);
    ASSERT_FALSE(true ^ t);

    ASSERT_FALSE(f ^ false);
    ASSERT_TRUE(f ^ true);
    ASSERT_TRUE(t ^ false);
    ASSERT_FALSE(t ^ true);
}
