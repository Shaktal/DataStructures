#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <utility>
#include <type_traits>

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

} // close namespace tr::data_structures

#endif // UTILITY_HPP