#ifndef SPAN_HPP
#define SPAN_HPP

#include <array>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <type_traits>

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
    template <std::size_t N>
    constexpr span(T (&arr)[N]) noexcept;
    template <std::size_t N>
    constexpr span(std::array<std::remove_const_t<T>, N>& arr) noexcept;
    template <std::size_t N>
    constexpr span(const std::array<std::remove_const_t<T>, N>& arr) noexcept;
    constexpr span(const span& other) noexcept;
    ~span() = default;

public: // Assignment
    constexpr span& operator=(const span& other) noexcept;

public: // Accessors
    constexpr reference operator[](size_type index) noexcept;
    constexpr const_reference operator[](size_type index) const noexcept;

    constexpr reference at(size_type index);
    constexpr const_reference at(size_type index) const;

    constexpr reference front() noexcept;
    constexpr const_reference front() const noexcept;

    constexpr reference back() noexcept;
    constexpr const_reference back() const noexcept;

    constexpr pointer data() noexcept;
    constexpr const_pointer data() const noexcept;

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

public: // Conversion
    constexpr span<const std::byte> as_bytes() const noexcept;

    template <typename U = T>
    constexpr std::enable_if_t<!std::is_const_v<U>, span<std::byte>> as_writable_bytes() noexcept;

private: // Members
    T*          d_begin;
    T*          d_end;
};

// Comparison
template <typename T>
inline constexpr bool operator==(const span<T>& lhs, const span<T>& rhs) noexcept
{
    return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename T>
inline constexpr bool operator!=(const span<T>& lhs, const span<T>& rhs) noexcept
{
    return !(lhs == rhs);
}

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
template <std::size_t N>
inline constexpr span<T>::span(T (&arr)[N]) noexcept
    : d_begin(std::begin(arr))
    , d_end(d_begin + N)
{}

template <typename T>
template <std::size_t N>
inline constexpr span<T>::span(std::array<std::remove_const_t<T>, N>& arr) noexcept
    : d_begin(std::begin(arr))
    , d_end(std::end(arr))
{}

template <typename T>
template <std::size_t N>
inline constexpr span<T>::span(const std::array<std::remove_const_t<T>, N>& arr) noexcept
    : d_begin(std::begin(arr))
    , d_end(std::end(arr))
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
inline constexpr typename span<T>::reference span<T>::at(size_type index)
{
    if (index >= this->size()) { throw std::out_of_range("Index not in range"); }
    return d_begin[index];
}

template <typename T>
inline constexpr typename span<T>::const_reference span<T>::at(size_type index) const
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

template <typename T>
inline constexpr typename span<T>::pointer span<T>::data() noexcept
{
    return this->d_begin;
}

template <typename T>
inline constexpr typename span<T>::const_pointer span<T>::data() const noexcept
{
    return this->d_begin;
}

// Iterators
template <typename T>
inline constexpr typename span<T>::iterator span<T>::begin() noexcept
{
    return this->d_begin;
}

template <typename T>
inline constexpr typename span<T>::const_iterator span<T>::begin() const noexcept
{
    return this->cbegin();
}

template <typename T>
inline constexpr typename span<T>::const_iterator span<T>::cbegin() const noexcept
{
    return this->d_begin;
}

template <typename T>
inline constexpr typename span<T>::iterator span<T>::end() noexcept
{
    return this->d_end;
}

template <typename T>
inline constexpr typename span<T>::const_iterator span<T>::end() const noexcept
{
    return this->cend();
}

template <typename T>
inline constexpr typename span<T>::const_iterator span<T>::cend() const noexcept
{
    return this->d_end;
}

template <typename T>
inline constexpr typename span<T>::reverse_iterator span<T>::rbegin() noexcept
{
    return reverse_iterator{ this->d_end };
}

template <typename T>
inline constexpr typename span<T>::const_reverse_iterator span<T>::rbegin() const noexcept
{
    return this->crbegin();
}

template <typename T>
inline constexpr typename span<T>::const_reverse_iterator span<T>::crbegin() const noexcept
{
    return const_reverse_iterator{ this->d_end };
}

template <typename T>
inline constexpr typename span<T>::reverse_iterator span<T>::rend() noexcept
{
    return reverse_iterator{ this->d_begin };
}

template <typename T>
inline constexpr typename span<T>::const_reverse_iterator span<T>::rend() const noexcept
{
    return this->crend();
}

template <typename T>
inline constexpr typename span<T>::const_reverse_iterator span<T>::crend() const noexcept
{
    return const_reverse_iterator{ this->d_begin };
}

// Capacity
template <typename T>
inline constexpr typename span<T>::size_type span<T>::size() const noexcept
{
    return (this->d_end - this->d_begin);
}

template <typename T>
inline constexpr typename span<T>::size_type span<T>::length() const noexcept
{
    return this->size();
}

// Conversion
template <typename T>
inline constexpr span<const std::byte> span<T>::as_bytes() const noexcept
{
    return { reinterpret_cast<const std::byte*>(this->d_begin), sizeof(T) * this->size() };
}

template <typename T>
template <typename U>
inline constexpr std::enable_if_t<!std::is_const_v<U>, span<std::byte>> span<T>::as_writable_bytes()
    noexcept
{
    return { reinterpret_cast<std::byte*>(this->d_begin), sizeof(T) * this->size() };
}

} // close namespace data_structures

#endif // SPAN_HPP
