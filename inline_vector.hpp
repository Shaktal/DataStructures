#ifndef INLINE_VECTOR_HPP
#define INLINE_VECTOR_HPP

#include <span.hpp>
#include <utility.hpp>

#include <algorithm>
#include <memory>
#include <numeric>
#include <vector>

// TODO:
//      - Find a way for this to work with move-only types, perhaps
//        just accepting random ranges instead of specifically `span`s.

namespace tr::data_structures {

template <typename T, typename Allocator = std::allocator<T>>
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
    using reference = const span<T>&;
    using const_reference = const span<T>&;
    using pointer = const span<T>*;
    using const_pointer = const span<T>*;

    using iterator = const span<T>*;
    using const_iterator = const span<T>*; // TODO: Work out how to allow this to prevent access
                                           // to the underlying `T` without violating strict-aliasing
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using size_type = typename std::allocator_traits<Allocator>::size_type;
    using difference_type = typename std::iterator_traits<iterator>::difference_type;

    using allocator_type = Allocator;

public: // Constructors
    inline_vector() noexcept(std::is_nothrow_default_constructible_v<Allocator>);
    explicit inline_vector(const Allocator& alloc) noexcept;
    inline_vector(const inline_vector& other);
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
    [[nodiscard]] size_type num_ranges() const noexcept;

    // This returns the maximum size that the container can grow to.
    [[nodiscard]] size_type max_size() const noexcept;

    // Allocate enough memory for `new_cap` Ts. 
    void reserve(size_type new_cap);

public: // Element Access
    [[nodiscard]] reference operator[](size_type index) noexcept;
    [[nodiscard]] const_reference operator[](size_type index) const noexcept;

    [[nodiscard]] reference at(size_type index);
    [[nodiscard]] const_reference at(size_type index) const;

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

    iterator insert_range(const_iterator pos, span<std::add_const_t<T>> range);
    // template <typename InputIt>
    // iterator insert_range(const_iterator pos, InputIt first, InputIt last);

    // iterator insert_ranges(const_iterator pos, std::initializer_list<std::remove_const_t<T>> ranges);
    // template <typename InputIt>
    // iterator insert_ranges(const_iterator pos, InputIt first, InputIt last);

    void push_back_range(span<std::add_const_t<T>> range);
    template <typename InputIt>
    void push_back_range(InputIt first, InputIt last);

    void pop_back_range() noexcept;

private: // Private Types
    template <typename U>
    using ReboundAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<U>;

private: // Helper variables
    static constexpr size_type RESIZE_FACTOR = 2u;

private:
    using Block = span<T>;
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
    : Allocator(alloc)
    , d_blockManager(alloc)
    , d_buffer(nullptr)
    , d_size(0u)
    , d_capacity(0u)
{}

template <typename T, typename Allocator>
inline inline_vector<T, Allocator>::inline_vector(const inline_vector& other)
    : Allocator(std::allocator_traits<allocator_type>::select_on_container_copy_construction(
        other.get_allocator()))
    , d_blockManager(*this)
    , d_buffer(nullptr)
    , d_size(0u)
    , d_capacity(0u)
{
    this->reserve(other.d_size);
    std::for_each(other.cbegin(), other.cend(), [this](auto block) {
        push_back_range(block);
    });
}

template <typename T, typename Allocator>
inline inline_vector<T, Allocator>::inline_vector(const inline_vector& other, 
    const Allocator& alloc)
    : Allocator(alloc)
    , d_blockManager(alloc)
    , d_buffer(nullptr)
    , d_size(0u)
    , d_capacity(0u)
{
    this->reserve(other.d_size);
    std::for_each(other.cbegin(), other.cend(), [this](auto block) {
        push_back_range(block);
    });
}

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
inline typename inline_vector<T, Allocator>::size_type inline_vector<T, Allocator>::capacity()
    const noexcept
{
    return this->d_capacity;
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::size_type inline_vector<T, Allocator>::num_ranges()
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

template <typename T, typename Allocator>
inline void inline_vector<T, Allocator>::reserve(size_type new_cap) 
{
    if (new_cap <= this->d_capacity) return;

    this->d_blockManager.reserve(this->d_blockManager.capacity() 
        + (new_cap - this->d_capacity));

    T* newBuff = std::allocator_traits<Allocator>::allocate(*this,
        new_cap);

    try {
        safe_uninitialized_copy(make_move_iterator_if_noexcept(this->d_buffer),
            make_move_iterator_if_noexcept(this->d_buffer + this->d_size),
            newBuff, static_cast<Allocator&>(*this));
        std::swap(d_buffer, newBuff);
        std::for_each(newBuff, newBuff + this->d_size, [this](T& obj) noexcept {
            std::allocator_traits<Allocator>::destroy(*this, std::addressof(obj));
        });
        std::for_each(this->d_blockManager.begin(), this->d_blockManager.end(),
            [this, newBuff](Block& block) noexcept {
                block = Block{this->d_buffer + (block.data() - newBuff),
                              block.length()};
            }
        );
        std::allocator_traits<Allocator>::deallocate(*this, newBuff, this->d_capacity);
        this->d_capacity = new_cap;
    }
    catch (...)
    {
        std::allocator_traits<Allocator>::deallocate(*this, newBuff, new_cap);
        throw;
    }
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
    return this->d_blockManager[index];
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::const_reference
    inline_vector<T, Allocator>::operator[](size_type index) const noexcept
{
    assert(index < this->d_blockManager.size());
    return this->d_blockManager[index];
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
template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::iterator inline_vector<T, Allocator>::begin()
    noexcept
{
    return this->d_blockManager.data();
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::const_iterator inline_vector<T, Allocator>::begin()
    const noexcept
{
    return this->cbegin();
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::const_iterator inline_vector<T, Allocator>::cbegin()
    const noexcept
{
    return this->d_blockManager.data();
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::iterator inline_vector<T, Allocator>::end()
    noexcept
{
    return const_cast<const span<T>*>(this->d_blockManager.data()) 
        + this->d_blockManager.size();
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::const_iterator inline_vector<T, Allocator>::end()
    const noexcept
{
    return this->cend();
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::const_iterator inline_vector<T, Allocator>::cend()
    const noexcept
{
    return this->d_blockManager.data()
        + this->d_blockManager.size();
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::reverse_iterator inline_vector<T, Allocator>::rbegin()
    noexcept
{
    return reverse_iterator{this->end()};
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::const_reverse_iterator 
    inline_vector<T, Allocator>::rbegin() const noexcept
{
    return this->crbegin();
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::const_reverse_iterator 
    inline_vector<T, Allocator>::crbegin() const noexcept
{
    return reverse_iterator{this->cend()};
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::reverse_iterator inline_vector<T, Allocator>::rend()
    noexcept
{
    return reverse_iterator{this->begin()};
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::const_reverse_iterator 
    inline_vector<T, Allocator>::rend() const noexcept
{
    return this->crend();
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::const_reverse_iterator 
    inline_vector<T, Allocator>::crend() const noexcept
{
    return reverse_iterator{this->cbegin()};
}

// Modifiers
template <typename T, typename Allocator>
inline void inline_vector<T, Allocator>::clear() 
{
    this->d_blockManager.clear();
    std::for_each(this->d_buffer, this->d_buffer + this->d_size,
        [this](T& obj) noexcept { 
            std::allocator_traits<Allocator>::destroy(*this, std::addressof(obj)); 
        });
    this->d_size = 0u;
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::iterator inline_vector<T, Allocator>::erase_range(
    const_iterator pos)
{
    Block block{const_cast<T*>(pos->data()), pos->length()};
    std::for_each(block.begin(), block.end(), [this](T& obj) noexcept {
        std::allocator_traits<Allocator>::destroy(*this, std::addressof(obj));
    });
    std::copy(make_move_iterator_if_noexcept(block.data() + block.length()),
        make_move_iterator_if_noexcept(this->d_buffer + this->size()),
        block.data());
    
    std::ptrdiff_t offset = const_cast<Block*>(pos) - this->d_blockManager.data();
    std::for_each(this->d_blockManager.begin() + offset, this->d_blockManager.end(),
        [pos](Block& block) noexcept {
            block = Block{block.data() - pos->length(), block.length()};
        }
    );

    if constexpr (std::is_convertible_v<decltype(this->d_blockManager.begin()), iterator>)
    {
        return this->d_blockManager.erase(this->d_blockManager.begin() + offset);
    }
    else
    {
        auto it = this->d_blockManager.erase(this->d_blockManager.begin() + offset);
        offset = std::distance(this->d_blockManager.begin(), it);
        return this->d_blockManager.data() + offset;
    }
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::iterator inline_vector<T, Allocator>::erase_range(
    const_iterator first, const_iterator last)
{
    if (first == last) return first;
    auto effectiveLast = last - 1u;

    Block block{const_cast<T*>(first->data()), (effectiveLast->data() - first->data()) + effectiveLast->length()};
    std::for_each(block.begin(), block.end(), [this](T& obj) noexcept {
        std::allocator_traits<Allocator>::destroy(*this, std::addressof(obj));
    });

    std::copy(make_move_iterator_if_noexcept(block.data() + block.length()),
        make_move_iterator_if_noexcept(this->d_buffer + this->size()),
        block.data());

    std::ptrdiff_t offset = std::distance(this->d_blockManager.data(), const_cast<Block*>(first));
    std::for_each(this->d_blockManager.begin() + offset, this->d_blockManager.end(),
        [block](Block& existingBlock) noexcept {
            existingBlock = Block{existingBlock.data() - block.length(), 
                existingBlock.length()};
        }
    );

    if constexpr (std::is_convertible_v<decltype(this->d_blockManager.begin()), iterator>)
    {
        return this->d_blockManager.erase(
            this->d_blockManager.begin() + offset,
            this->d_blockManager.begin() + offset 
                + std::distance(first, last)
        );
    }
    else
    {
        auto it = this->d_blockManager.erase(
            this->d_blockManager.begin() + offset,
            this->d_blockManager.begin() + offset 
                + std::distance(first, last)
        );
        offset = std::distance(this->d_blockManager.begin(), it);
        return this->d_blockManager.data() + offset;
    }
}

template <typename T, typename Allocator>
inline typename inline_vector<T, Allocator>::iterator inline_vector<T, Allocator>::insert_range(
    const_iterator pos, span<std::add_const_t<T>> range)
{
    auto dist = std::distance(this->d_blockManager.data(), 
        const_cast<Block*>(pos));
    if ((this->d_capacity - this->d_size) < range.length())
    {
        reserve(std::max(this->d_capacity * RESIZE_FACTOR + 1u,
            this->d_capacity + range.length()));
    }

    auto it = this->d_blockManager.begin() + dist;
    it = this->d_blockManager.emplace(it);

    std::ptrdiff_t offset = std::accumulate(this->d_blockManager.begin(), it, 0,
        [](std::ptrdiff_t curr, Block& block) { return curr + block.length(); });
    *it = Block{this->d_buffer + offset, range.length()};

    std::ptrdiff_t numToConstruct = std::min(range.length(), this->d_size - offset);

    try {
        std::uninitialized_copy(
            make_move_iterator_if_noexcept(this->d_buffer + this->d_size - numToConstruct),
            make_move_iterator_if_noexcept(this->d_buffer + this->d_size),
            this->d_buffer + this->d_size
        );
        std::copy_backward(
            make_move_iterator_if_noexcept(this->d_buffer + offset),
            make_move_iterator_if_noexcept(this->d_buffer + this->d_size - numToConstruct),
            this->d_buffer + this->d_size
        );
        std::copy(range.begin(), range.end(), this->d_buffer + offset);
    }
    catch (...)
    {
        this->d_blockManager.erase(it);
        std::copy(
            make_move_iterator_if_noexcept(this->d_buffer + offset + range.length()),
            make_move_iterator_if_noexcept(this->d_buffer + this->d_size + numToConstruct),
            this->d_buffer + offset
        );
        throw;
    }

    std::for_each(it + 1u, this->d_blockManager.end(),
        [len = range.length()](Block& block) noexcept {
            block = Block{block.data() + len, block.length()};
        }
    );

    this->d_size += range.length();
    if constexpr (std::is_convertible_v<decltype(it), iterator>)
    {
        return it;
    }
    else
    {
        return this->d_blockManager.data() + 
            std::distance(this->d_blockManager.begin(), it);
    }
}

template <typename T, typename Allocator>
inline void inline_vector<T, Allocator>::push_back_range(span<std::add_const_t<T>> range)
{
    if ((this->d_capacity - this->d_size) < range.length())
    {
        reserve(std::max(this->d_capacity * RESIZE_FACTOR + 1u, 
            this->d_capacity + range.length()));
    }

    this->d_blockManager.emplace_back(this->d_buffer + this->d_size, range.length());

    try {
        safe_uninitialized_copy(range.begin(), range.end(), this->d_buffer + this->d_size,
            static_cast<Allocator&>(*this));
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
    if constexpr (at_least_forward_iterator_v<InputIt>)
    {
        size_type length = std::distance(first, last);

        if ((this->d_capacity - this->d_size) < length)
        {
            reserve(std::max(this->d_capacity * RESIZE_FACTOR + 1u, this->d_capacity + length));
        }

        this->d_blockManager.emplace_back(this->d_buffer + this->d_size, length);

        try {
            std::uninitialized_copy(first, last, this->d_buffer + this->d_size);
        }
        catch (...)
        {
            this->d_blockManager.pop_back();
            throw;
        }

        this->d_size += length;
    }
    else
    {
        size_type count = 0u;
        try {
            while (first != last)
            {
                if (this->d_size == this->d_capacity)
                {
                    reserve(this->d_capacity * RESIZE_FACTOR + 1u);
                }

                std::allocator_traits<Allocator>::construct(*this, this->d_buffer + this->d_size,
                    *first);
                ++this->d_size;

                ++first, ++count;
            }
        }
        catch (...)
        {
            std::for_each(
                this->d_buffer + this->d_size - count,
                this->d_buffer + this->d_size, 
                [this](T& obj) noexcept {
                    std::allocator_traits<Allocator>::destroy(*this, std::addressof(obj));
                }
            );

            throw;
        }

        this->d_blockManager.emplace_back(this->d_size - count, count);
    }
}

template <typename T, typename Allocator>
inline void inline_vector<T, Allocator>::pop_back_range() noexcept
{
    assert(!this->empty());
    auto [start_pos, length] = this->d_blockManager.back();

    std::for_each(this->d_buffer + start_pos, this->d_buffer + start_pos + length,
        [this](T& obj) noexcept { 
            std::allocator_traits<Allocator>::destroy(*this, std::addressof(obj)); 
        }
    );

    this->d_blockManager.pop_back();
    this->d_size = start_pos;
}

} // close namespace tr::data_structures

#endif // INLINE_VECTOR_HPP
