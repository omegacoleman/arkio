#pragma once

#include <iasr/bindings.hpp>

#include <iasr/misc/is_iterable.hpp>

namespace iasr {

// buffer-like

namespace detail {
template <
    typename T,
    typename =
        enable_if_t<is_convertible_v<decltype(declval<T &>().size()), size_t>>,
    typename = enable_if_t<is_pointer_v<decltype(declval<T &>().data())>>,
    typename =
        enable_if_t<is_convertible_v<decltype(*declval<T &>().data()), byte>>,
    typename =
        enable_if_t<sizeof(decltype(*declval<T &>().data())) == sizeof(byte)>>
true_type is_buffer_like_impl(int);

template <typename T> false_type is_buffer_like_impl(...);

} // namespace detail

template <typename T>
using is_buffer_like = decltype(detail::is_buffer_like_impl<T>(0));

template <typename T>
inline constexpr bool is_buffer_like_v = is_buffer_like<T>::value;

// buffer sequence

namespace detail {
template <
    typename T, typename = enable_if_t<is_iterable_v<T>>,
    typename = enable_if_t<is_buffer_like_v<decltype(*begin(declval<T &>()))>>>
true_type is_buffer_seq_impl(int);

template <typename T> false_type is_buffer_seq_impl(...);
} // namespace detail

template <typename T>
using is_buffer_seq = decltype(detail::is_buffer_seq_impl<T>(0));

template <typename T>
inline constexpr bool is_buffer_seq_v = is_buffer_seq<T>::value;
} // namespace iasr
