#pragma once

#include <ark/bindings.hpp>

namespace ark {

namespace concepts {

namespace detail {
template <class T, class U> concept SameHelper = std::is_same_v<T, U>;
}

template <class T, class U>
concept same_as = detail::SameHelper<T, U> &&detail::SameHelper<U, T>;

template <class From, class To>
concept convertible_to = std::is_convertible_v<From, To> &&requires(
    std::add_rvalue_reference_t<From> (&f)()) {
  static_cast<To>(f());
};

} // namespace concepts

} // namespace ark
