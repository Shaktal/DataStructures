#ifndef LOCAL_BUFFERED_RESOURCE
#define LOCAL_BUFFERED_RESOURCE

#include <bitset>
#include <cassert>
#include <new>
#include <type_traits>

namespace tr::data_structures {

template <std::size_t Capacity, std::size_t Alignment = alignof(std::max_align_t)>
struct LocalBufferedResource {
public: // Types
    using size_type = std::size_t;

public: // Construction
    LocalBufferedResource() = default;
    LocalBufferedResource(const LocalBufferedAllocator&) = delete;
    
public: // Assignment
    LocalBufferedResource& operator=(const LocalBufferedResource&) = delete;

public: // Allocation
    [[nodiscard]] constexpr void* allocate(size_type numBytes, 
        size_type alignment = alignof(std::max_align_t));

    constexpr void deallocate(void* position, size_type numBytes);

private: // Members
    using BufferType = std::aligned_storage_t<Capacity, Alignment>;
    
    BufferType              d_buffer;
    std::bitset<Capacity>   d_byteBook; // Keeps track of which bytes are allocated
}; 

// =================================================================
// INLINE DEFINITIONS
// =================================================================
// Allocation
template <std::size_t Capacity, std::size_t Alignment>
inline constexpr void* LocalBufferedResource<Capacity, Alignment>::allocate(
    size_type numBytes, size_type alignment)
{
    if(numBytes <= (Capacity - d_byteBook.count()))
    {
        throw std::bad_alloc("Unable to allocate more memory. "
        "Buffer does not have enough contiguous space");
    }

    // Find the first set of numBytes contiguous unset bits
    // in the bitset
    for (std::size_t index = 0u; index < Capacity; ++index)
    {
        std::size_t numContiguous;
        for (numContiguous = 0u; numContiguous < (Capacity - numBytes); ++numContiguous)
        {
            if (const_cast<const std::bitset<Capacity>>(d_byteBook)[i])
            {
                break;
            }
        }

        if (numContiguous == numBytes - 1u)
        {
            for (numContiguous = 0u; numContiguous < numBytes; ++numContiguous)
            {
                d_byteBook.set(index + numContiguous);
            }

            return reinterpret_cast<void*>(
                reinterpret_cast<std::byte*>(std::addressof(d_buffer))
                + index);
        }
    }

    throw std::bad_alloc("Unable to allocate more memory. "
        "Buffer does not have enough contiguous space");
}

} // close namespace tr::data_structures

#endif // LOCAL_BUFFERED_RESOURCE
