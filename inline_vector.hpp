#ifndef INLINE_VECTOR_HPP
#define INLINE_VECTOR_HPP

#include <span.hpp>
#include <utility.hpp>

#include <memory>

namespace tr::data_structures {

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
    inline_vector() noexcept(std::is_nothrow_default_constructible_v<Allocator>);
    explicit inline_vector(const Allocator& alloc) noexcept;
    inline_vector(const inline_vector& other, const Allocator& alloc);
    inline_vector(inline_vector&& other) noexcept;
    inline_vector(inline_vector&& other, const Allocator& alloc);

public: // Assignment
    inline_vector& operator=(const inline_vector& other);
    inline_vector& operator=(inline_vector&& other);

    template <typename InputIt>
    void assign(InputIt first, InputIt last);

    void assign(std::initializer_list<span<std::remove_const_t<T>>> ilist);

public: // Allocator
    [[nodiscard]] allocator_type get_allocator() const noexcept;

public: // Capacity
    [[nodiscard]] bool empty() const noexcept;

    // This returns the current size of the container in sizeof(T). 
    [[nodiscard]] size_type size() const noexcept;

    // This returns the current capacity of the container in sizeof(T).
    [[nodiscard]] size_type capacity() const noexcept;

    // This returns the number of elements (ranges) in the container.
    [[nodiscard]] size_type num_elements() const noexcept;

    // This returns the maximum size that the container can grow to.
    [[nodiscard]] size_type max_size() const noexcept;

    // Allocate enough memory for `new_cap` Ts. 
    void reserve(size_type new_cap);

public: // Element Access
    [[nodiscard]] reference operator[](size_type index) noexcept;
    [[nodiscard]] const_reference operator[](size_type index) const noexcept;

    [[nodiscard]] reference at(size_type index);
    [[nodiscard]] const_reference at(size_type index) const noexcept;

    [[nodiscard]] reference front() noexcept;
    [[nodiscard]] const_reference front() const noexcept;

    [[nodiscard]] reference back() noexcept;
    [[nodiscard]] const_reference back() const noexcept;

public: // Iterators
    [[nodiscard]] iterator begin() noexcept;
    [[nodiscard]] const_iterator begin() const noexcept;
    [[nodiscard]] const_iterator cbegin() const noexcept;

    [[nodiscard]] iterator end() noexcept;
    [[nodiscard]] const_iterator end() const noexcept;
    [[nodiscard]] const_iterator cend() const noexcept;

    [[nodiscard]] reverse_iterator rbegin() noexcept;
    [[nodiscard]] const_reverse_iterator rbegin() const noexcept;
    [[nodiscard]] const_reverse_iterator crbegin() const noexcept;

    [[nodiscard]] reverse_iterator rend() noexcept;
    [[nodiscard]] const_reverse_iterator rend() const noexcept;
    [[nodiscard]] const_reverse_iterator crend() const noexcept;

public: // Modifiers
    void clear();

    iterator erase_range(const_iterator pos);
    iterator erase_range(const_iterator first, const_iterator last);

    iterator insert_range(const_iterator pos, span<std::remove_const_t<T>> range);
    template <typename InputIt>
    iterator insert_range(const_iterator pos, InputIt first, InputIt last);

    iterator insert_ranges(const_iterator pos, std::initializer_list<std::remove_const_t<T>> ranges);
    template <typename InputIt>
    iterator insert_ranges(const_iterator pos, InputIt first, InputIt last);

    void push_back_range(span<std::remove_const_t<T>> range);
    template <typename InputIt>
    void push_back_range(InputIt first, InputIt last);

    void pop_back_range() noexcept;

private: // Private Types
    template <typename T>
    using ReboundAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<T>;

private: // Helper variables
    static constexpr size_type RESIZE_FACTOR = 2u;

private:
    using Block = std::pair<size_type, size_type>; // Pair of [start_pos, length]
    using BlockManager = std::vector<Block, ReboundAlloc<Block>>;
    BlockManager        d_blockManager;

    T*                  d_buffer;
    size_type           d_size;
    size_type           d_capacity;
};

// =================================================================
// INLINE DEFINITIONS
// =================================================================

// Constructors
template <typename T, typename Allocator>
inline inline_vector<T, Allocator>::inline_vector()
    noexcept(std::is_nothrow_default_constructible_v<Allocator>)
    : inline_vector(Allocator{})
{}

template <typename T, typename Allocator>
inline inline_vector<T, Allocator>::inline_vector(const Allocator& alloc) noexcept
    : d_blockManager(alloc)
    , d_buffer(nullptr)
    , d_size(0u)
    , d_capacity(0u)
{}

template <typename T, typename Allocator>
inline inline_vector<T, Allocator>::inline_vector(inline_vector&& other) noexcept
    : Allocator(other)
    , d_blockManager(std::move(other.d_blockManager), *this)
    , d_buffer(other.d_buffer)
    , d_size(other.d_size)
    , d_capacity(other.d_capacity)
{
    other.d_buffer = nullptr;
}

// Allocator
template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::allocator_type 
    inline_vector<T, Allocator>::get_allocator() const noexcept
{
    return *this;
}

// Capacity
template <typename T, typename Allocator>
inline bool inline_vector<T, Allocator>::empty() const noexcept
{
    return (this->d_size == 0u);
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::size_type inline_vector<T, Allocator>::size()
    const noexcept
{
    return this->d_size;
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::size_type inline_vector<T, Allocator>::num_elements()
    const noexcept
{
    return this->d_blockManager.size();
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::size_type inline_vector<T, Allocator>::max_size()
    const noexcept
{
    return std::allocator_traits<Allocator>::max_size(*this);
}

// Element Access
template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::reference 
    inline_vector<T, Allocator>::at(size_type index)
{
    if (index >= this->d_blockManager.size()) { throw std::out_of_range{"Index out of range"}; }
    return this->operator[](index);
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::const_reference
    inline_vector<T, Allocator>::at(size_type index) const
{
    if (index >= this->d_blockManager.size()) { throw std::out_of_range{"Index out of range"}; }
    return this->operator[](index);
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::reference
    inline_vector<T, Allocator>::operator[](size_type index) noexcept
{
    assert(index < this->d_blockManager.size());
    auto [start_pos, length] = this->d_blockManager[index];
    return {this->d_buffer + start_pos, length};
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::const_reference
    inline_vector<T, Allocator>::operator[](size_type index) const noexcept
{
    assert(index < this->d_blockManager.size());
    auto [start_pos, length] = this->d_blockManager[index];
    return {this->d_buffer + start_pos, length};
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::reference
    inline_vector<T, Allocator>::front() noexcept
{
    assert(!this->empty());
    return this->operator[](0u);
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::const_reference
    inline_vector<T, Allocator>::front() const noexcept
{
    assert(!this->empty());
    return this->operator[](0u);
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::reference
    inline_vector<T, Allocator>::back() noexcept
{
    assert(!this->empty());
    return this->operator[](this->d_blockManager.size() - 1u);
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::const_reference
    inline_vector<T, Allocator>::back() const noexcept
{
    assert(!this->empty());
    return this->operator[](this->d_blockManager.size() - 1u);
}

// Iterators
// TODO: Implement once data_structures::inline_vector_iterator is
//       implemented.

// Modifiers
template <typename T, typename Allocator>
inline void inline_vector<T, Allocator>::clear() 
{
    this->d_blockManager.clear();
    std::for_each(this->d_buffer, this->d_buffer + this->d_size,
        [](T& obj) { obj.~T(); });
    this->d_size = 0u;
}

template <typename T, typename Allocator>
inline void inline_vector<T, Allocator>::push_back_range(span<std::remove_const_t<T>> range)
{
    if (this->d_capacity - this->d_size < range.length())
    {
        reserve(this->d_capacity * RESIZE_FACTOR);
    }

    this->d_blockManager.emplace_back(this->d_size, range.length());

    try {
        std::uninitialized_copy(range.begin(), range.end(), this->d_buffer + this->d_size);
    }
    catch (...)
    {
        this->d_blockManager.pop_back();
        throw;
    }

    this->d_size += range.length();
}

template <typename T, typename Allocator>
template <typename InputIt>
inline void inline_vector<T, Allocator>::push_back_range(InputIt first, InputIt last)
{
    if constexpr (utility::at_least_forward_iterator_v<InputIt>)
    {

    }
    else
    {

    }
}

} // close namespace tr::data_structures

#endif // INLINE_VECTOR_HPP