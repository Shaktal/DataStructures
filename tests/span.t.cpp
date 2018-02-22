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

TEST(Span, span_has_correct_size_and_length)
{
    constexpr int arr1[4] = {1, 2, 3, 4}, arr2[1] = {1};
    data_structures::span<const int> s1(arr1);
    data_structures::span<const int> s2(std::begin(arr2), 1u);

    using namespace ::testing;
    EXPECT_THAT(s1.size(), Eq(sizeof(arr1) / sizeof(int)));
    EXPECT_THAT(s1.length(), Eq(s1.size()));
    
    EXPECT_THAT(s2.size(), Eq(sizeof(arr2) / sizeof(int)));
    EXPECT_THAT(s2.length(), Eq(s2.size()));
}

TEST(Span, front_and_back_work_correctly)
{
    constexpr double arr[3] = {-1.5, 0.0, 1.5};
    data_structures::span<const double> sp(arr);

    using namespace ::testing;
    EXPECT_THAT(sp.front(), DoubleEq(-1.5));
    EXPECT_THAT(sp.back(), DoubleEq(1.5));
}

TEST(Span, at_throws_an_exception_iff_out_of_range)
{
    constexpr float arr[4] = {-6.f, 0.5f, 1.2f, 9.6f};
    data_structures::span<const float> sp(arr);

    using namespace ::testing;
    EXPECT_THAT(sp.at(3u), FloatEq(9.6f));
    EXPECT_THROW(sp.at(6u), std::out_of_range);
    EXPECT_NO_THROW((sp.at(0u), sp.at(1u), sp.at(2u)));
}
