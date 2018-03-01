#ifndef INLINE_VECTOR_ITERATOR_HPP
#define INLINE_VECTOR_ITERATOR_HPP

#include <iterator>

namespace tr::data_structures {
namespace detail {

template <typename T>
class inline_vector_iterator {
public: // Types
    using iterator_category = std::random_access_iterator_tag;
    using value_type = span<T>;
    using reference = const span<T>&;
    using pointer = const span<T>*;
    using difference_type = std::ptrdiff_t;

public: // Constructors
    inline_vector_iterator() noexcept;
    inline_vector_iterator(span<T>* blocks, std::size_t index) noexcept;
    inline_vector_iterator(const inline_vector_iterator& other) noexcept;
    ~inline_vector_iterator() = default;
    
public: // Assignment
    inline_vector_iterator& operator=(const inline_vector_iterator& other) noexcept;

public: // Comparison
    friend bool operator==(const inline_vector_iterator& lhs, 
        const inline_vector_iterator& rhs) noexcept;
    friend bool operator!=(const inline_vector_iterator& lhs,
        const inline_vector_iterator& rhs) noexcept;

    friend bool operator<(const inline_vector_iterator& lhs,
        const inline_vector_iterator& rhs) noexcept;
    friend bool operator<=(const inline_vector_iterator& lhs,
        const inline_vector_iterator& rhs) noexcept;
    
    friend bool operator>(const inline_vector_iterator& lhs,
        const inline_vector_iterator& rhs) noexcept;
    friend bool operator>=(const inline_vector_iterator& lhs,
        const inline_vector_iterator& rhs) noexcept;

public: // Element access
    reference operator*() const noexcept;
    pointer operator->() const noexcept;

public: // Arithmetic
    inline_vector_iterator& operator++() noexcept;
    inline_vector_iterator operator++(int) noexcept;

    inline_vector_iterator& operator--() noexcept;
    inline_vector_iterator operator--(int) noexcept;

    inline_vector_iterator operator+(std::size_t offset) const noexcept;
    inline_vector_iterator& operator+=(std::size_t offset) noexcept;

    inline_vector_iterator operator-(std::size_t offset) const noexcept;
    inline_vector_iterator& operator-=(std::size_t offset) noexcept;

    friend difference_type operator-(const inline_vector_iterator& lhs,
        const inline_vector_iterator& rhs) noexcept;

private:
    span<T>*        d_blocks;
    std::size_t     d_currIndex;
};

// =================================================================
// INLINE DEFINITIONS
// =================================================================

// Constructors
template <typename T>
inline inline_vector_iterator<T>::inline_vector_iterator() noexcept
    : d_blocks(nullptr)
    , d_currIndex(0u)
{}

template <typename T>
inline inline_vector_iterator<T>::inline_vector_iterator(span<T>* blocks, std::size_t index)
    noexcept
    : d_blocks(blocks)
    , d_currIndex(index)
{}

template <typename T>
inline inline_vector_iterator<T>::inline_vector_iterator(const inline_vector_iterator& other)
    noexcept
    : d_blocks(other.d_blocks)
    , d_currIndex(other.d_currIndex)
{}

// Assignment
template <typename T>
inline inline_vector_iterator<T>& inline_vector_iterator<T>::operator=(const inline_vector_iterator& other)
    noexcept
{
    this->d_blocks = other.d_blocks;
    this->d_currIndex = other.d_currIndex;
    return *this;
}

// Comparison
template <typename T>
inline bool operator==(const inline_vector_iterator<T>& lhs, 
    const inline_vector_iterator<T>& rhs) noexcept
{
    return (this->d_blocks == other.d_blocks) 
        && (this->d_currIndex == other.d_currIndex);
}

template <typename T>
inline bool operator!=(const inline_vector_iterator<T>& lhs,
    const inline_vector_iterator<T>& rhs) noexcept
{
    return !(lhs == rhs);
}

template <typename T>
inline bool operator<(const inline_vector_iterator<T>& lhs,
    const inline_vector_iterator<T>& rhs) noexcept
{
    assert(lhs.d_blocks == rhs.d_blocks);
    return lhs.d_currIndex < rhs.d_currIndex;
}

template <typename T>
inline bool operator<=(const inline_vector_iterator<T>& lhs,
    const inline_vector_iterator<T>& rhs) noexcept
{
    assert(lhs.d_blocks == rhs.d_blocks);
    return lhs.d_currIndex <= rhs.d_currIndex;
}

template <typename T>
inline bool operator>(const inline_vector_iterator<T>& lhs,
    const inline_vector_iterator<T>& rhs) noexcept
{
    assert(lhs.d_blocks == rhs.d_blocks);
    return lhs.d_currIndex > rhs.d_currIndex;
}

template <typename T>
inline bool operator>=(const inline_vector_iterator<T>& lhs,
    const inline_vector_iterator<T>& rhs) noexcept
{
    assert(lhs.d_blocks == rhs.d_blocks);
    return lhs.d_currIndex >= rhs.d_currIndex;
}

// Element Access
template <typename T>
inline typename inline_vector_iterator<T>::reference inline_vector_iterator<T>::operator*() 
    noexcept
{
    assert(this->d_blocks);
    return this->d_blocks[this->d_currIndex];
}

template <typename T>
inline typename inline_vector_iterator<T>::pointer inline_vector_iterator<T>::operator->()
{
    assert(this->d_blocks);
    return std::addressof(this->d_blocks[this->d_currIndex]);
}

// Arithmetic
template <typename T>
inline inline_vector_iterator<T>& inline_vector_iterator<T>::operator++() noexcept
{
    ++this->d_currIndex;
    return *this;
}

template <typename T>
inline inline_vector_iterator<T> inline_vector_iterator<T>::operator++(int) noexcept
{
    inline_vector_iterator tmp{this->d_blocks, this->d_currIndex};
    this->operator++();
    return tmp;
}

template <typename T>
inline inline_vector_iterator<T>& inline_vector_iterator<T>::operator--() noexcept
{
    assert(this->d_currIndex);
    --this->d_currIndex;
    return *this;
}

template <typename T>
inline inline_vector_iterator<T> inline_vector_iterator<T>::operator--(int) noexcept
{
    assert(this->d_currIndex);
    inline_vector_iterator tmp{this->d_blocks, this->d_currIndex};
    this->operator--();
    return tmp;
}

template <typename T>
inline inline_vector_iterator<T> inline_vector_iterator<T>::operator+(std::size_t index) 
    const noexcept
{
    return inline_vector_iterator{this->d_blocks, this->d_currIndex + index};
}

template <typename T>
inline inline_vector_iterator<T>& inline_vector_iterator<T>::operator+=(std::size_t index)
    noexcept
{
    this->d_currIndex += index;
    return *this;
}

template <typename T>
inline inline_vector_iterator<T> inline_vector_iterator<T>::operator-(std::size_t index) 
    const noexcept
{
    assert(this->d_currIndex >= index);
    return inline_vector_iterator{this->d_blocks, this->d_currIndex - index};
}

template <typename T>
inline inline_vector_iterator<T>& inline_vector_iterator<T>::operator-=(std::size_t index)
    noexcept
{
    assert(this->d_currIndex >= index);
    this->d_currIndex -= index;
    return *this;
}

template <typename T>
inline typename inline_vector_iterator<T>::difference_type operator-(
    const inline_vector_iterator<T>& lhs, const inline_vector_iterator<T>& rhs) noexcept
{
    using DiffType = typename inline_vector_iterator<T>::difference_type;
    assert(lhs.d_blocks == rhs.d_blocks);
    return static_cast<DiffType>(lhs.d_currIndex) - 
        static_cast<DiffType>(rhs.d_currIndex);
}

} // close namespace detail
} // close namespace tr::data_structures

#endif // INLINE_VECTOR_ITERATOR_HPP