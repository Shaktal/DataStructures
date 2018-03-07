#include <inline_vector.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace tr;

TEST(InlineVector, default_constructed_is_empty_with_no_capacity)
{
    data_structures::inline_vector<int> vec;

    using namespace ::testing;
    EXPECT_THAT(vec.empty(), Eq(true));
    EXPECT_THAT(vec.size(), Eq(0u));
    EXPECT_THAT(vec.capacity(), Eq(0u));
    EXPECT_THAT(vec.begin(), Eq(vec.end()));
}

TEST(InlineVector, can_correctly_insert_single_range)
{
    constexpr std::array<int, 5u> arr = {1, 2, 3, 4, 5};
    data_structures::inline_vector<int> vec;

    vec.push_back_range(arr);
    
    using namespace ::testing;
    ASSERT_THAT(vec.empty(), Eq(false));
    ASSERT_THAT(vec.size(), Eq(arr.size()));
    ASSERT_THAT(vec.num_ranges(), Eq(1u));
    EXPECT_THAT(vec.capacity(), Ge(arr.size()));
    EXPECT_THAT(vec, ElementsAre(ElementsAre(1, 2, 3, 4, 5)));
}

TEST(InlineVector, can_correctly_insert_multiple_ranges)
{
    constexpr std::array<int, 3u> arr1 = {-1, 0, 1};
    constexpr std::array<int, 5u> arr2 = {10, 20, 30, 40, -50};
    constexpr std::array<int, 1u> arr3 = {10000};
    data_structures::inline_vector<int> vec;

    vec.push_back_range(arr1);
    vec.push_back_range(arr2.cbegin(), arr2.cend());
    vec.push_back_range(arr3);

    using namespace ::testing;
    ASSERT_THAT(vec.empty(), Eq(false));
    ASSERT_THAT(vec.size(), Eq(arr1.size() + arr2.size() + arr3.size()));
    ASSERT_THAT(vec.num_ranges(), Eq(3u));
    EXPECT_THAT(vec.capacity(), Ge(arr1.size() + arr2.size() + arr3.size()));
    EXPECT_THAT(vec, ElementsAre(ElementsAre(-1, 0, 1), ElementsAre(10, 20, 30, 40, -50), ElementsAre(10000)));
}

TEST(InlineVector, can_clear_vector)
{
    constexpr std::array<int, 3u> arr1 = {-1, 0, 1};
    constexpr std::array<const int, 5u> arr2 = {10, 20, 30, 40, 50};
    data_structures::inline_vector<int> vec;

    vec.push_back_range(arr1);
    vec.push_back_range(arr2.cbegin(), arr2.cend());
    vec.clear();

    using namespace ::testing;
    ASSERT_THAT(vec.empty(), Eq(true));
    ASSERT_THAT(vec.size(), Eq(0u));
    ASSERT_THAT(vec.num_ranges(), Eq(0u));
}

TEST(InlineVector, can_copy_populated_vector)
{
    constexpr std::array<int, 3u> arr1 = {-1, 0, 1};
    constexpr std::array<int, 5u> arr2 = {10, 20, 30, 40, -50};
    constexpr std::array<int, 1u> arr3 = {10000};
    data_structures::inline_vector<int> vec;

    vec.push_back_range(arr1);
    vec.push_back_range(arr2);
    vec.push_back_range(arr3);

    data_structures::inline_vector<int> vec2{vec};

    using namespace ::testing;
    EXPECT_THAT(vec2.empty(), Eq(vec.empty()));
    EXPECT_THAT(vec2.size(), Eq(vec.size()));
    EXPECT_THAT(vec2.num_ranges(), Eq(vec.num_ranges()));
    EXPECT_THAT(vec2.capacity(), Le(vec.capacity()));
    EXPECT_THAT(vec2, ElementsAreArray(vec.cbegin(), vec.cend()));
}

TEST(InlineVector, can_copy_empty_vector)
{
    data_structures::inline_vector<int> vec;
    data_structures::inline_vector<int> vec2{vec};

    using namespace ::testing;
    EXPECT_THAT(vec2.empty(), Eq(vec.empty()));
    EXPECT_THAT(vec2.size(), Eq(vec.size()));
    EXPECT_THAT(vec2.num_ranges(), Eq(vec.num_ranges()));
    EXPECT_THAT(vec2.capacity(), Le(vec.capacity()));
    EXPECT_THAT(vec2, ElementsAreArray(vec.cbegin(), vec.cend()));
}

TEST(InlineVector, can_move_populated_vector)
{
    constexpr std::array<int, 3u> arr1 = {-1, 0, 1};
    constexpr std::array<int, 5u> arr2 = {10, 20, 30, 40, -50};
    constexpr std::array<int, 1u> arr3 = {10000};
    data_structures::inline_vector<int> vec;

    vec.push_back_range(arr1);
    vec.push_back_range(arr2);
    vec.push_back_range(arr3);

    data_structures::inline_vector<int> vec2{std::move(vec)};

    using namespace ::testing;
    EXPECT_THAT(vec2.empty(), Eq(false));
    EXPECT_THAT(vec2.size(), Eq(arr1.size() + arr2.size() + arr3.size()));
    EXPECT_THAT(vec2.num_ranges(), Eq(3u));
    EXPECT_THAT(vec2.capacity(), Ge(arr1.size() + arr2.size() + arr3.size()));
    EXPECT_THAT(vec2, ElementsAre(ElementsAre(-1, 0, 1), ElementsAre(10, 20, 30, 40, -50), ElementsAre(10000)));
}

TEST(InlineVector, can_move_empty_vector)
{
    data_structures::inline_vector<int> vec;
    data_structures::inline_vector<int> vec2{std::move(vec)};

    using namespace ::testing;
    EXPECT_THAT(vec2.empty(), Eq(true));
    EXPECT_THAT(vec2.size(), Eq(0u));
    EXPECT_THAT(vec2.num_ranges(), Eq(0u));
    EXPECT_THAT(vec2.capacity(), Eq(0u));
}

TEST(InlineVector, max_size_forwarded_to_allocator)
{
    data_structures::inline_vector<int> vec;
    auto alloc = vec.get_allocator();
    
    using namespace ::testing;
    EXPECT_THAT(vec.max_size(), Eq(std::allocator_traits<decltype(alloc)>::max_size(alloc)));
}

TEST(InlineVector, square_brackets_operator_works)
{
    constexpr std::array<int, 3u> arr1 = {-1, 0, 1};
    constexpr std::array<int, 5u> arr2 = {10, 20, 30, 40, -50};
    constexpr std::array<int, 1u> arr3 = {10000};
    data_structures::inline_vector<int> vec;

    vec.push_back_range(arr1);
    vec.push_back_range(arr2);
    vec.push_back_range(arr3);

    using namespace ::testing;
    ASSERT_THAT(vec.num_ranges(), Eq(3u));
    EXPECT_THAT(vec[0], ElementsAre(-1, 0, 1));
    EXPECT_THAT(vec[1], ElementsAre(10, 20, 30, 40, -50));
    EXPECT_THAT(vec[2], ElementsAre(10000));
}

TEST(InlineVector, at_works_correctly)
{
    constexpr std::array<int, 5u> arr = {1, 2, 3, 4, 5};
    data_structures::inline_vector<int> vec;

    vec.push_back_range(arr);

    using namespace ::testing;
    ASSERT_THAT(vec.num_ranges(), Eq(1u));
    EXPECT_THAT(vec.at(0u), ElementsAre(1, 2, 3, 4, 5));
    EXPECT_THROW(vec.at(1u), std::out_of_range);
}

TEST(InlineVector, front_works_correctly)
{
    constexpr std::array<int, 3u> arr1 = {-1, 0, 1};
    constexpr std::array<int, 5u> arr2 = {10, 20, 30, 40, -50};
    constexpr std::array<int, 1u> arr3 = {10000};
    data_structures::inline_vector<int> vec;

    vec.push_back_range(arr1);
    vec.push_back_range(arr2);
    vec.push_back_range(arr3);

    using namespace ::testing;
    ASSERT_THAT(vec.num_ranges(), Eq(3u));
    EXPECT_THAT(vec.front(), ElementsAre(-1, 0, 1));
}

TEST(InlineVector, back_works_correctly)
{
    constexpr std::array<int, 3u> arr1 = {-1, 0, 1};
    constexpr std::array<int, 5u> arr2 = {10, 20, 30, 40, -50};
    constexpr std::array<int, 1u> arr3 = {10000};
    data_structures::inline_vector<int> vec;

    vec.push_back_range(arr1);
    vec.push_back_range(arr2);
    vec.push_back_range(arr3);

    using namespace ::testing;
    ASSERT_THAT(vec.num_ranges(), Eq(3u));
    EXPECT_THAT(vec.back(), ElementsAre(10000));
}

TEST(InlineVector, erase_first_element)
{
    constexpr std::array<int, 3u> arr1 = {-1, 0, 1};
    constexpr std::array<int, 5u> arr2 = {10, 20, 30, 40, -50};
    constexpr std::array<int, 1u> arr3 = {10000};
    data_structures::inline_vector<int> vec;

    vec.push_back_range(arr1);
    vec.push_back_range(arr2);
    vec.push_back_range(arr3);
    vec.erase_range(vec.begin());

    using namespace ::testing;
    ASSERT_THAT(vec.num_ranges(), Eq(2u));
    EXPECT_THAT(vec, ElementsAre(ElementsAre(10, 20, 30, 40, -50), ElementsAre(10000)));
}

TEST(InlineVector, erase_middle_element)
{
    constexpr std::array<int, 3u> arr1 = {-1, 0, 1};
    constexpr std::array<int, 5u> arr2 = {10, 20, 30, 40, -50};
    constexpr std::array<int, 1u> arr3 = {10000};
    data_structures::inline_vector<int> vec;

    vec.push_back_range(arr1);
    vec.push_back_range(arr2);
    vec.push_back_range(arr3);
    vec.erase_range(vec.begin() + 1u);

    using namespace ::testing;
    ASSERT_THAT(vec.num_ranges(), Eq(2u));
    EXPECT_THAT(vec, ElementsAre(ElementsAre(-1, 0, 1), ElementsAre(10000)));
}

TEST(InlineVector, erase_end_element)
{
    constexpr std::array<int, 3u> arr1 = {-1, 0, 1};
    constexpr std::array<int, 5u> arr2 = {10, 20, 30, 40, -50};
    constexpr std::array<int, 1u> arr3 = {10000};
    data_structures::inline_vector<int> vec;

    vec.push_back_range(arr1);
    vec.push_back_range(arr2);
    vec.push_back_range(arr3);
    vec.erase_range(vec.begin() + 2u);

    using namespace ::testing;
    ASSERT_THAT(vec.num_ranges(), Eq(2u));
    EXPECT_THAT(vec, ElementsAre(ElementsAre(-1, 0, 1), ElementsAre(10, 20, 30, 40, -50)));
}
