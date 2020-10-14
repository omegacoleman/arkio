#pragma once

#include <iasr/bindings.hpp>

namespace iasr {

namespace detail {

using std::begin;
using std::end;

template <typename T>
auto is_iterable_impl(int)
    -> decltype(begin(declval<T &>()) != end(declval<T &>()), void(),
                ++declval<decltype(begin(declval<T &>())) &>(),
                void(*begin(declval<T &>())), true_type{});

template <typename T> false_type is_iterable_impl(...);

} // namespace detail

template <typename T>
using is_iterable = decltype(detail::is_iterable_impl<T>(0));

template <typename T>
inline constexpr bool is_iterable_v = is_iterable<T>::value;

} // namespace iasr
