#ifndef STATIC_VECTOR_HPP
#define STATIC_VECTOR_HPP

#include <utility.hpp>

#include <initializer_list>
#include <type_traits>
#include <utility>

namespace tr::data_structures {

template <typename T, std::size_t N>
class static_vector {
    // A container implementation that has a compile-time
    // maximum capacity like std::array or a C array.
    // But has uninitialized storage unlike std::array or C-arrays,
    // does not require that `T` is default-constructible.

public: // Types
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const iterator>;

public: // Construction
    constexpr static_vector() noexcept;
    template <std::size_t M, typename = std::enable_if_t<(M <= N)>>
    static_vector(const static_vector<T, M>& other) 
        noexcept(std::is_nothrow_copy_constructible_v<T>);
    template <std::size_t M, typename = std::enable_if_t<(M <= N)>>
    static_vector(static_vector<T, M>&& other)
        noexcept(std::is_nothrow_move_constructible_v<T>);
    ~static_vector();

public: // Assignment
    template <std::size_t M, typename = std::enable_if_t<(M <= N)>>
    static_vector& operator=(static_vector<T, M>& other) 
        noexcept(std::is_nothrow_copy_constructible_v<T> &&
            std::is_nothrow_copy_assignable_v<T>);

    template <std::size_t M, typename = std::enable_if_t<(M <= N)>>
    static_vector& operator=(static_vector<T, M>&& other)
        noexcept(std::is_nothrow_move_constructible_v<T> &&
            std::is_nothrow_move_assignable_v<T>);  

    void assign(size_type count, const T& value)
        noexcept(std::is_nothrow_copy_constructible_v<T> &&
            std::is_nothrow_copy_assignable_v<T>);
    
    template <typename InputIt, typename = at_least_input_iterator_v<InputIt>
    void assign(InputIt first, InputIt last)
        noexcept(std::is_nothrow_constructible_v<T, 
            typename std::iterator_traits<InputIt>::value_type> && 
            std::is_nothrow_assignable_v<T, typename std::iterator_traits<InputIt>::value_type);

    void assign(std::initializer_list<T> ilist)
        noexcept(std::is_nothrow_move_constructible_v<T> &&
            std::is_nothrow_move_assignable_v<T>)

public: // Capacity
    [[nodiscard]] constexpr bool empty() const noexcept;
    [[nodiscard]] constexpr size_type size() const noexcept;
    [[nodiscard]] constexpr size_type max_size() const noexcept;

public: // Accessors
    [[nodiscard]] reference operator[](size_type index) noexcept;
    [[nodiscard]] const_reference operator[](size_type index) const noexcept;

private: // Members
    size_type                                       d_size;
    std::aligned_storage_t<sizeof(T), alignof(T)>   d_storage[N];
};

// =================================================================
// INLINE DEFINITIONS
// =================================================================


} // close namespace tr::data_structures

#endif // STATIC_VECTOR_HPP
