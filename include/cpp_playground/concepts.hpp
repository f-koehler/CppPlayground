#ifndef CPPPLAYGROUND_CONCEPTS_HPP
#define CPPPLAYGROUND_CONCEPTS_HPP

#include <concepts>

namespace CppPlayground {

template <typename AllocatorT>
concept IsAllocator =
    requires(AllocatorT allocator, std::size_t n) {
      {
        *allocator.allocate(n)
      } -> std::same_as<typename AllocatorT::value_type &>;
      { allocator.deallocate(allocator.allocate(n), n) };
    } && std::copy_constructible<AllocatorT> &&
    std::equality_comparable<AllocatorT>;

} // namespace CppPlayground

#endif
