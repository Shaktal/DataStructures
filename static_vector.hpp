#ifndef STATIC_VECTOR_HPP
#define STATIC_VECTOR_HPP

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
     
};

// =================================================================
// INLINE DEFINITIONS
// =================================================================


} // close namespace tr::data_structures

#endif // STATIC_VECTOR_HPP
