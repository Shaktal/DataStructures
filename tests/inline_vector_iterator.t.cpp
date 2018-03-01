#include <inline_vector_iterator.hpp>

#include <span.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using tr::data_structures::detail::inline_vector_iterator;

namespace {
std::array<tr::data_structures::span<const int>, 3u> constructSpanArray() noexcept
{
    static constexpr int arr1[] = {1, 2, 3, 4, 5};
    static constexpr int arr2[] = {1, 3, 5, 7};
    static constexpr int arr3[] = {-10, -20, -30};

    return {arr1, arr2, arr3};
}
} // close anonymous namespace

TEST(InlineVectorIterator, default_constructed_iterator_compares_equal)
{
    inline_vector_iterator<int> it1{};
    inline_vector_iterator<int> it2{};

    using namespace ::testing;
    EXPECT_THAT(it1, Eq(it2));
}

TEST(InlineVectorIterator, dereference_works_correctly)
{
    constexpr int arr[] = {1, 2, 3};
    std::array<tr::data_structures::span<const int>, 1u> spanArray{arr};

    inline_vector_iterator<const int> it{spanArray.data(), 0u};

    using namespace ::testing;
    EXPECT_THAT(*it, ElementsAre(1, 2, 3));
}

TEST(InlineVectorIterator, increment_works_correctly)
{
    auto spanArray = constructSpanArray();

    inline_vector_iterator<const int> it{spanArray.data(), 0u};

    using namespace ::testing;
    EXPECT_THAT(*it++, ElementsAre(1, 2, 3, 4, 5));
    EXPECT_THAT(*it, ElementsAre(1, 3, 5, 7));
    ++it;
    EXPECT_THAT(*it, ElementsAre(-10, -20, -30));
}

TEST(InlineVectorIterator, decrement_works_correctly)
{
    auto spanArray = constructSpanArray();
    inline_vector_iterator<const int> it{spanArray.data(), spanArray.size() - 1u};

    using namespace ::testing;
    EXPECT_THAT(*it--, ElementsAre(-10, -20, -30));
    EXPECT_THAT(*it, ElementsAre(1, 3, 5, 7));
    --it;
    EXPECT_THAT(*it, ElementsAre(1, 2, 3, 4, 5));
}
