#ifndef INLINE_VECTOR_HPP
#define INLINE_VECTOR_HPP

#include <span.hpp>

namespace data_structures {

template <typename T, typename Allocator>
class inline_vector : private Allocator
{
    // This class is a vector of sequences that
    // are stored inline. 
    // 
    // The closest parallel in the STL would be a
    // std::vector<std::vector<T>>. The reason for
    // writing this container is to help improve
    // cache friendliness.
public: // Types
    using value_type = span<T>;
    using reference = span<T>;
    using const_reference = span<const T>;

    using iterator = inline_vector_iterator<T>;
    using const_iterator = inline_vector_iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const iterator>;

    using size_type = typename std::allocator_traits<Allocator>::size_type;
    using difference_type = typename std::iterator_traits<iterator>::difference_type;

    using allocator_type = Allocator;
};

} // close namespace data_structures

#endif // INLINE_VECTOR_HPP