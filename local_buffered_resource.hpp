#ifndef LOCAL_BUFFERED_RESOURCE
#define LOCAL_BUFFERED_RESOURCE

#include <type_traits>

namespace data_structures {

template <std::size_t Capacity, std::size_t Alignment = alignof(std::max_align_t)>
struct LocalBufferedResource {
public: // Types
    using size_type = std::size_t;

public: // Construction
    LocalBufferedResource();
    LocalBufferedResource(const LocalBufferedAllocator&) = delete;
    
public: // Assignment
    LocalBufferedResource& operator=(const LocalBufferedResource&) = delete;

public: // Allocation
    [[nodiscard]] constexpr void* allocate(size_type numBytes, 
        size_type alignment = alignof(std::max_align_t)) noexcept;

    constexpr void deallocate(void* position, size_type numBytes) noexcept;

private: // Members
    using BufferType = std::aligned_storage_t<Capacity, Alignment>;
    
    BufferType              d_buffer;
    std::bitset<Capacity>   d_byteBook; // Keeps track of which bytes are allocated
}; 

} // close namespace data_structures

#endif // LOCAL_BUFFERED_RESOURCE
