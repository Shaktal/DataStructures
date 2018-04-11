#ifndef FLAT_SEQUENCE_SET_HPP
#define FLAT_SEQUENCE_SET_HPP

#include "inline_vector.hpp"
#include "utility.hpp"

#include <algorithm>
#include <type_traits>

namespace tr::data_structures {

template <typename Type, 
          typename Compare = std::less<span<Type>>,
          typename Container = inline_vector<Type>>
class flat_sequence_set : private Container, private Compare {
    // This is a container adaptor that takes sequences of 
    // `T` and stores them in `Container` in sorted order
    // according to `Compare`.

public: // Types
    using value_type = typename Container::value_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    using pointer = typename Container::pointer;
    using const_pointer = typename Container::const_pointer;

    using iterator = typename Container::iterator;
    using const_iterator = typename Container::const_iterator;
    using reverse_iterator = typename Container::reverse_iterator;
    using const_reverse_iterator = typename Container::const_reverse_iterator;

    using allocator_type = typename Container::allocator_type;
    using size_type = typename Container::size_type;

public: // Constructors
    flat_sequence_set(const Compare& compare = {}, const Container& cont = {});

    flat_sequence_set(const flat_sequence_set& other);
    flat_sequence_set(flat_sequence_set&& other) noexcept;
    template <typename Alloc, typename = std::enable_if_t<std::uses_allocator_v<Container, Alloc>>>
    explicit flat_sequence_set(const Alloc& alloc);
    template <typename Alloc, typename = std::enable_if_t<std::uses_allocator_v<Container, Alloc>>>
    flat_sequence_set(const Compare& compare, const Alloc& alloc);
    template <typename Alloc, typename = std::enable_if_t<std::uses_allocator_v<Container, Alloc>>>
    flat_sequence_set(const Compare& compare, const Container& cont, const Alloc& alloc);
    template <typename Alloc, typename = std::enable_if_t<std::uses_allocator_v<Container, Alloc>>>
    flat_sequence_set(const flat_sequence_set& other, const Alloc& alloc);
    template <typename Alloc, typename = std::enable_if_t<std::uses_allocator_v<Container, Alloc>>>
    flat_sequence_set(flat_sequence_set&& other, const Alloc& alloc);


public: // Accessors
    reference operator[](size_type index) noexcept;
    using Container::operator[];
    using Container::at;
    using Container::front;
    using Container::back;

public: // Underlying accessors
    Container& get_container() & noexcept;
    const Container& get_container() const & noexcept;
    Container&& get_container() && noexcept;
    
    Compare& get_compare() & noexcept;
    const Compare& get_compare() const & noexcept;
    Compare&& get_compare() && noexcept;

public: // Capacity
    using Container::capacity;
    using Container::size;
    using Container::num_ranges;
    using Container::empty;

public: // Iteration
    using Container::begin;
    using Container::cbegin;

    using Container::end;
    using Container::cend;
    
    using Container::rbegin;
    using Container::crbegin;

    using Container::rend;
    using Container::crend;

public: // Modifiers:
    using Container::clear;
    using Container::erase_range;

    void insert_range(span<std::add_const_t<Type>> range);

private: // Helpers
    void construction_helper(const Container& cont);
};

// =================================================================
// INLINE DEFINITIONS
// =================================================================
// Constructors
template <typename Type, typename Compare, typename Container>
inline flat_sequence_set<Type, Compare, Container>::flat_sequence_set(const Compare& compare, 
    const Container& cont)
    : Container(), Compare(compare)
{
    construction_helper(cont);
}

template <typename Type, typename Compare, typename Container>
inline flat_sequence_set<Type, Compare, Container>::flat_sequence_set(const flat_sequence_set&) 
    = default;

template <typename Type, typename Compare, typename Container>
inline flat_sequence_set<Type, Compare, Container>::flat_sequence_set(flat_sequence_set&&)
    noexcept = default;

template <typename Type, typename Compare, typename Container>
template <typename Alloc, typename>
inline flat_sequence_set<Type, Compare, Container>::flat_sequence_set(const Alloc& alloc)
    : Container(alloc), Compare()
{}

template <typename Type, typename Compare, typename Container>
template <typename Alloc, typename>
inline flat_sequence_set<Type, Compare, Container>::flat_sequence_set(const Compare& compare, 
    const Alloc& alloc)
    : Container(alloc), Compare(compare)
{}

template <typename Type, typename Compare, typename Container>
template <typename Alloc, typename>
inline flat_sequence_set<Type, Compare, Container>::flat_sequence_set(const Compare& compare, 
    const Container& cont, const Alloc& alloc)
    : Container(alloc), Compare(compare)
{
    construction_helper(cont);   
}

template <typename Type, typename Compare, typename Container>
template <typename Alloc, typename>
inline flat_sequence_set<Type, Compare, Container>::flat_sequence_set(
    const flat_sequence_set& other, const Alloc& alloc)
    : Container(static_cast<const Container&>(other), alloc)
    , Compare(static_cast<const Compare&>(other))
{}

template <typename Type, typename Compare, typename Container>
template <typename Alloc, typename>
inline flat_sequence_set<Type, Compare, Container>::flat_sequence_set(
    flat_sequence_set&& other, const Alloc& alloc)
    : Container(static_cast<Container&&>(other), alloc)
    , Compare(static_cast<Compare&&>(other))
{}

// Underlying accessors
template <typename Type, typename Compare, typename Container>
Container& flat_sequence_set<Type, Compare, Container>::get_container() & noexcept
{
    return *this;
}

template <typename Type, typename Compare, typename Container>
const Container& flat_sequence_set<Type, Compare, Container>::get_container() const & noexcept
{
    return *this;
}

template <typename Type, typename Compare, typename Container>
Container&& flat_sequence_set<Type, Compare, Container>::get_container() && noexcept
{
    return static_cast<Container&&>(*this);
}

template <typename Type, typename Compare, typename Container>
Compare& flat_sequence_set<Type, Compare, Container>::get_compare() & noexcept
{
    return *this;
}

template <typename Type, typename Compare, typename Container>
const Compare& flat_sequence_set<Type, Compare, Container>::get_compare() const & noexcept
{
    return *this;
}

template <typename Type, typename Compare, typename Container>
Compare&& flat_sequence_set<Type, Compare, Container>::get_compare() && noexcept
{
    return static_cast<Compare&&>(*this);
}

// Modifiers
template <typename Type, typename Compare, typename Container>
void flat_sequence_set<Type, Compare, Container>::insert_range(span<std::add_const_t<Type>> range)
{
    // We can construct a span directly from the range:
    auto it = std::lower_bound(this->begin(), this->end(), range,
        static_cast<Compare&>(*this));
    Container::insert_range(it, range);
}

// Helpers
template <typename Type, typename Compare, typename Container>
void flat_sequence_set<Type, Compare, Container>::construction_helper(const Container& cont)
{
    // TODO: See if there is an easier way to resolve this hack?
    std::vector<typename Container::value_type> spans;
    spans.resize(cont.size());
    Container::reserve(cont.size());
    std::partial_sort_copy(cont.begin(), cont.end(),
        spans.begin(), spans.end(), static_cast<Compare&>(*this));
    std::copy(spans.begin(), spans.end(), 
        back_range_inserter(static_cast<Container&>(*this)));
}

} // close namespace tr::data_structures

#endif // FLAT_SEQUENCE_SET_HPP
