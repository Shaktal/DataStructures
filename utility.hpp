#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <utility>
#include <iterator>
#include <memory>
#include <new>
#include <type_traits>

// Include other utility-related classes/functions
// that should be forwarded when utility is included.
#include "detection.hpp"

namespace tr::data_structures {

// Check to see if an `Iterator` is at one of
// {ForwardIterator, BidirectionalIterator,
// RandomAccessIterator}.
template <typename Iterator, typename = void>
struct at_least_forward_iterator : std::false_type {};

template <typename Iterator>
struct at_least_forward_iterator<Iterator, std::enable_if_t<
        std::is_same_v<
            typename std::iterator_traits<Iterator>::iterator_category,
            std::forward_iterator_tag
        > || 
        std::is_same_v<
            typename std::iterator_traits<Iterator>::iterator_category,
            std::bidirectional_iterator_tag
        > ||
        std::is_same_v<
            typename std::iterator_traits<Iterator>::iterator_category,
            std::random_access_iterator_tag
        >
    >> : std::true_type {};

template <typename Iterator>
using at_least_forward_iterator_t = typename at_least_forward_iterator<Iterator>::type;

template <typename Iterator>
inline constexpr bool at_least_forward_iterator_v = at_least_forward_iterator<Iterator>::value;

// Check to see if an `Iterator` is one of {InputIterator.
// ForwardIterator, BidirectionalIterator, RandomAccessIterator}
template <typename Iterator, typename = void>
struct at_least_input_iterator : std::false_type {};

template <typename Iterator>
struct at_least_input_iterator<Iterator, std::enable_if_t<
        at_least_forward_iterator_v<Iterator> || std::is_same_v<
            typename std::iterator_traits<Iterator>::iterator_category,
            std::input_iterator_tag
        >
    >> : std::true_type {};

template <typename Iterator>
using at_least_input_iterator_t = typename at_least_input_iterator<Iterator>::type;

template <typename Iterator>
inline constexpr bool at_least_input_iterator_v = at_least_input_iterator<Iterator>::value;

// Version of uninitialized_copy which takes into account the allocator
// `construct` function:
template <typename InputIt, typename FwdIt, typename Allocator>
inline void safe_uninitialized_copy(InputIt first, InputIt last, 
    FwdIt output, Allocator& alloc)
{
    if constexpr (std::is_same_v<Allocator, 
        std::allocator<typename std::allocator_traits<Allocator>::value_type>>)
    {
        std::uninitialized_copy(first, last, output);
    }
    else
    {
        FwdIt tmp{output};
        try {
            while (first != last)
            {
                std::allocator_traits<Allocator>::construct(alloc, std::addressof(*tmp), *first);
                ++first, ++tmp;
            }
        }
        catch (...) {
            while (output != tmp)
            {
                std::allocator_traits<Allocator>::destroy(alloc, std::addressof(*output));
                ++output;
            }
        }
    }
}

// Utility to make a `move_iterator` if the type is a non-trivially copyable type
// with a `noexcept` move constructor, else return the provided iterator.
template <typename FwdIt>
constexpr auto make_move_iterator_if_noexcept(FwdIt it) noexcept
    -> std::conditional_t<!std::is_trivially_copyable_v<typename std::iterator_traits<FwdIt>::value_type>
        && std::is_nothrow_move_constructible_v<typename std::iterator_traits<FwdIt>::value_type>,
        std::move_iterator<FwdIt>, FwdIt>
{
    using ResultType = std::conditional_t<!std::is_trivially_copyable_v<typename std::iterator_traits<FwdIt>::value_type>
        && std::is_nothrow_move_constructible_v<typename std::iterator_traits<FwdIt>::value_type>,
        std::move_iterator<FwdIt>, FwdIt>;

    return ResultType{it};
}

// Utility for constructing an equivalent to `back_inserter` for `push_back_range`
template <typename Container>
struct back_range_insert_iterator {
    using value_type = void;
    using reference = void;
    using pointer = void;
    using difference_type = void;
    using iterator_category = std::output_iterator_tag;
    using container_type = Container;

    back_range_insert_iterator& operator*() noexcept { return *this; }
    back_range_insert_iterator& operator++() noexcept { return *this; }
    back_range_insert_iterator& operator++(int) noexcept { return *this; }

    template <typename Range>
    back_range_insert_iterator& operator=(Range&& range) {
        d_container.push_back_range(std::forward<Range>(range).begin(),
            std::forward<Range>(range).end());
        return *this;
    }

    Container& d_container;
};

template <typename Container>
constexpr auto back_range_inserter(Container& container) noexcept 
    -> back_range_insert_iterator<Container> 
{
    return { container };
}

} // close namespace tr::data_structures

#endif // UTILITY_HPP