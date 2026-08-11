#include <gtest/gtest.h>

#include "platform/math/Vec2.h"

TEST(Vec2Test, DefaultConstructorIsZero)
{
    constexpr Vec2 v;
    EXPECT_EQ(v.x, 0.0f);
    EXPECT_EQ(v.y, 0.0f);
}

TEST(Vec2Test, ConstructorSetsComponents)
{
    constexpr Vec2 v(1.5f, -2.5f);
    EXPECT_EQ(v.x, 1.5f);
    EXPECT_EQ(v.y, -2.5f);
}

TEST(Vec2Test, EqualityComparesBothComponents)
{
    EXPECT_TRUE(Vec2(1.0f, 2.0f) == Vec2(1.0f, 2.0f));
    EXPECT_FALSE(Vec2(1.0f, 2.0f) == Vec2(1.0f, 2.1f));
    EXPECT_FALSE(Vec2(1.0f, 2.0f) == Vec2(1.1f, 2.0f));
}

TEST(Vec2Test, InequalityIsNegationOfEquality)
{
    EXPECT_FALSE(Vec2(1.0f, 2.0f) != Vec2(1.0f, 2.0f));
    EXPECT_TRUE(Vec2(1.0f, 2.0f) != Vec2(2.0f, 1.0f));
}
