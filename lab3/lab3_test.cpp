#include "unique_ptr.h"

#include <gtest/gtest.h>
#include <utility>

TEST(UniquePtrTest, moveConstructor) {
    auto p1 = MakeUnique<int>(1);
    const auto p2 = std::move(p1);
    EXPECT_EQ(p1, nullptr);
    EXPECT_NE(p2, nullptr);
    EXPECT_EQ(*p2, 1);
}

TEST(UniquePtrTest, moveAssign) {
    auto p1 = MakeUnique<int>(1);
    UniquePtr<int> p2 {};
    p2 = std::move(p1);
    EXPECT_EQ(p1, nullptr);
    EXPECT_NE(p2, nullptr);
    EXPECT_EQ(*p2, 1);
}

TEST(UniquePtrTest, selfAssign) {
    auto p1 = MakeUnique<int>(1);
    p1 = std::move(p1);
    EXPECT_NE(p1, nullptr);
    EXPECT_EQ(*p1, 1);
}

TEST(UniquePtrTest, nullAssign) {
    auto p1 = MakeUnique<int>(1);
    p1 = nullptr;
    EXPECT_EQ(p1, nullptr);
}
