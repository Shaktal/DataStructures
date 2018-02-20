#ifndef LOCAL_BUFFERED_ALLOCATOR_HPP
#define LOCAL_BUFFERED_ALLOCATOR_HPP

#include <utility>

namespace data_structures {

template <typename T, typename Buffer>
struct LocalBufferedAllocator {
public: // Types
    using value_type = T;
    using size_type = typename Buffer::size_type;

public: // Construction
    constexpr LocalBufferedAllocator(Buffer* buffer) noexcept;
    constexpr LocalBufferedAllocator(const LocalBufferedAllocator& other) noexcept;

public: // Assignment
    constexpr LocalBufferedAllocator& operator=(const LocalBufferedAllocator& other) noexcept;

public: // Allocation
    [[nodiscard]] constexpr T* allocate(size_type n) 
        noexcept(std::declval<Buffer>().allocate(n, alignof(T)));
    
    constexpr void deallocate(T* p, size_type n) 
        noexcept(std::declval<Buffer>().deallocate(p, n));

public: // Capacity
    [[nodiscard]] constexpr size_type max_size() const noexcept;

public: // Modifiers
    friend void swap(LocalBufferedAllocator& lhs, LocalBufferedAllocator& rhs) noexcept;

private: // Members
    Buffer* d_buffer;
};

// =================================================================
// INLINE DEFINITIONS
// =================================================================
// Construction
template <typename T, typename Buffer>
inline constexpr LocalBufferedAllocator<T, Buffer>::LocalBufferedAllocator(Buffer* buffer) noexcept
    : d_buffer(buffer)
{}

template <typename T, typename Buffer>
inline constexpr LocalBufferedAllocator<T, Buffer>::LocalBufferedAllocator(
    const LocalBufferedAllocator& other) noexcept
    : d_buffer(other.d_buffer)
{}

// Assignment
template <typename T, typename Buffer>
inline constexpr LocalBufferedAllocator<T, Buffer>& LocalBufferedAllocator::operator=(
    const LocalBufferedAllocator& other) noexcept
{
    d_buffer = other.d_buffer;
}

// Allocation
template <typename T, typename Buffer>
inline constexpr T* LocalBufferedAllocator<T, Buffer>::allocate(size_type n) 
    noexcept(std::declval<Buffer>().allocate(n, alignof(T)))
{
    return reinterpret_cast<T*>(d_buffer->allocate(sizeof(T) * n, alignof(T)));
}

template <typename T, typename Buffer>
inline constexpr void LocalBufferedAllocator<T, Buffer>::deallocate(T* p, size_type n)
    noexcept(std::declval<Buffer>().deallocate(p, n))
{
    d_buffer->deallocate(p, sizeof(T) * n);
}

// Capacity
template <typename T, typename Buffer>
inline constexpr typename LocalBufferedAllocator<T, Buffer>::size_type 
    LocalBufferedAllocator<T, Buffer>::max_size() const noexcept
{
    return d_buffer->max_size();
}

// Modifiers
template <typename T, typename Buffer>
inline void swap(LocalBufferedAllocator<T, Buffer>& rhs, LocalBufferedAllocator<T, Buffer>& rhs) 
    noexcept
{
    using std::swap;
    swap(lhs.d_buffer, rhs.d_buffer);
}

} // close namespace data_structures

#endif // LOCAL_BUFFERED_ALLOCATOR_HPP
