#ifndef SPAN_HPP
#define SPAN_HPP

#include <iterator>
#include <stdexcept>

namespace data_structures {

template <typename T>
class span {
public: // Types
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;

    using pointer = T*;
    using const_pointer = const T*;

    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using size_type = std::size_t;
    using difference_type = typename std::iterator_traits<iterator>::difference_type;

public: // Constructors
    constexpr span() noexcept;
    constexpr span(T* ptrBegin, std::size_t length) noexcept;
    constexpr span(const span& other) noexcept;
    ~span() = default;

public: // Assignment
    constexpr span& operator=(const span& other) noexcept;

public: // Accessors
    constexpr reference operator[](size_type index) noexcept;
    constexpr const_reference operator[](size_type index) const noexcept;

    constexpr reference at(size_type index);
    constexpr const_reference at(size_type index); const;

    constexpr reference front() noexcept;
    constexpr const_reference front() const noexcept;

    constexpr reference back() noexcept;
    constexpr const_reference back() const noexcept;

public: // Iterators
    constexpr iterator begin() noexcept;
    constexpr const_iterator begin() const noexcept;
    constexpr const_iterator cbegin() const noexcept;

    constexpr iterator end() noexcept;
    constexpr const_iterator end() const noexcept;
    constexpr const_iterator cend() const noexcept;

    constexpr reverse_iterator rbegin() noexcept;
    constexpr const_reverse_iterator rbegin() const noexcept;
    constexpr const_reverse_iterator crbegin() const noexcept;

    constexpr reverse_iterator rend() noexcept;
    constexpr const_reverse_iterator rend() const noexcept;
    constexpr const_reverse_iterator crend() const noexcept;

public: // Capacity
    constexpr size_type size() const noexcept;
    constexpr size_type length() const noexcept;

private: // Members
    T*          d_begin;
    T*          d_end;
};

// Constructors
template <typename T>
inline constexpr span<T>::span() noexcept
    : d_begin(nullptr)
    , d_end(nullptr)
{}

template <typename T>
inline constexpr span<T>::span(T* ptrBegin, std::size_t length) noexcept
    : d_begin(ptrBegin)
    , d_end(ptrBegin + length)
{}

template <typename T>
inline constexpr span<T>::span(const span& other) noexcept
    : d_begin(other.d_begin)
    , d_end(other.d_end)
{}

// Assignment
template <typename T>
inline constexpr span<T>& span<T>::operator=(const span& other) noexcept
{
    d_begin = other.d_begin;
    d_end = other.d_end;
}

// Accessors
template <typename T>
inline constexpr typename span<T>::reference span<T>::operator[](size_type index) noexcept
{
    assert(index < this->size());
    return d_begin[index];
}

template <typename T>
inline constexpr typename span<T>::const_reference span<T>::operator[](size_type index) const noexcept
{
    assert(index < this->size());
    return d_begin[index];
}

template <typename T>
inline constexpr typename span<T>::reference span<T>::at(size_type index) noexcept
{
    if (index >= this->size()) { throw std::out_of_range("Index not in range"); }
    return d_begin[index];
}

template <typename T>
inline constexpr typename span<T>::const_reference span<T>::at(size_type index) const noexcept
{
    if (index >= this->size()) { throw std::out_of_range("Index not in range"); }
    return d_begin[index];
}

template <typename T>
inline constexpr typename span<T>::reference span<T>::front() noexcept
{
    assert(!this->empty());
    return *this->d_begin;
}

template <typename T>
inline constexpr typename span<T>::const_reference span<T>::front() const noexcept
{
    assert(!this->empty());
    return *this->d_begin;
}

template <typename T>
inline constexpr typename span<T>::reference span<T>::back() noexcept
{
    assert(!this->empty());
    return *(this->d_end - 1u);
}

template <typename T>
inline constexpr typename span<T>::const_reference span<T>::back() const noexcept
{
    assert(!this->empty());
    return *(this->d_end - 1u);
}

} // close namespace data_structures

#endif // SPAN_HPP
