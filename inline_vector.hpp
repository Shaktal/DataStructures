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

public: // Constructors
    inline_vector() noexcept(Allocator());
    explicit inline_vector(const Allocator& alloc) 
        noexcept(std::is_nothrow_copy_constructible_v<Allocator>);
    inline_vector(const inline_vector& other);
    inline_vector(inline_vector&& other);

public: // Allocator
    allocator_type get_allocator() 
        const noexcept(std::is_nothrow_copy_constructible_v<allocator_type>);

public: // Capacity
    // This returns the current size of the container in bytes. 
    size_type size() const noexcept;

    // This returns the current capacity of the container in bytes.
    size_type capacity() const noexcept;

    // This returns the number of elements (ranges) in the container.
    size_type num_elements() const noexcept;

    // This returns the maximum size that the container can grow to.
    size_type max_size() const noexcept;

private: // Private Types
    template <typename T>
    using ReboundAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<T>;

private:
    using BlockManager = std::vector<size_type, ReboundAlloc<size_type>>;
    BlockManager        d_blockManager;

    T*                  d_buffer;
    T*                  d_bufferEnd;
    size_type           d_capacity;
};

// =================================================================
// INLINE DEFINITIONS
// =================================================================

// Capacity
template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::size_type inline_vector<T, Allocator>::max_size()
    const noexcept
{
    return std::allocator_traits<Allocator>::max_size(*this);
}

} // close namespace data_structures

#endif // INLINE_VECTOR_HPP