#include "span.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(Span, can_construct_from_array)
{
    constexpr int arr[3] = {-1, 0, 1};
    data_structures::span<const int> sp(arr);

    using namespace ::testing;
    EXPECT_THAT(sp, ElementsAre(-1, 0, 1));
}

TEST(Span, copied_span_compares_equal)
{
    constexpr int arr[3] = {1, 2, 3};
    data_structures::span<const int> s1(std::begin(arr), 3u);
    data_structures::span<const int> s2(s1);

    using namespace ::testing;
    EXPECT_THAT(s1, Eq(s2));
}

TEST(Span, span_has_correct_elements)
{
    constexpr int arr[3] = {1, 2, 3};
    data_structures::span<const int> s1(std::begin(arr), 3u);

    using namespace ::testing;
    EXPECT_THAT(s1, ElementsAre(1, 2, 3));
}

TEST(Span, span_same_length_different_elements_compares_nonequal)
{
    constexpr int arr1[3] = {1, 2, 3}, arr2[3] = {4, 5, 6};
    data_structures::span<const int> s1(std::begin(arr1), 3u);
    data_structures::span<const int> s2(std::begin(arr2), 3u);

    using namespace ::testing;
    EXPECT_THAT(s1, Ne(s2));
}

TEST(Span, span_different_length_same_initial_elements_compares_nonequal)
{
    constexpr int arr1[3] = {1, 2, 3}, arr2[4] = {1, 2, 3, 4};
    data_structures::span<const int> s1(std::begin(arr1), 3u);
    data_structures::span<const int> s2(std::begin(arr2), 4u);

    using namespace ::testing;
    EXPECT_THAT(s1, Ne(s2));
}
