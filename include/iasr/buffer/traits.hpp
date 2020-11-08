#pragma once

#include <iasr/bindings.hpp>

#include <iasr/buffer/buffer.hpp>
#include <iasr/buffer/sequence.hpp>

namespace iasr {

// N4771 [buffer.traits]

namespace detail {
template <typename T,
          typename = enable_if_t<is_convertible_v<
              decltype(*buffer_sequence_begin(declval<T>())), mutable_buffer>>,
          typename = enable_if_t<is_convertible_v<
              decltype(*buffer_sequence_end(declval<T>())), mutable_buffer>>>
true_type is_mutable_buffer_sequence_impl(int);

template <typename T> false_type is_mutable_buffer_sequence_impl(...);
} // namespace detail

template <typename T>
using is_mutable_buffer_sequence =
    decltype(detail::is_mutable_buffer_sequence_impl<T>(0));

template <typename T>
inline constexpr bool is_mutable_buffer_sequence_v =
    is_mutable_buffer_sequence<T>::value;

namespace detail {
template <typename T,
          typename = enable_if_t<is_convertible_v<
              decltype(*buffer_sequence_begin(declval<T>())), const_buffer>>,
          typename = enable_if_t<is_convertible_v<
              decltype(*buffer_sequence_end(declval<T>())), const_buffer>>>
true_type is_const_buffer_sequence_impl(int);

template <typename T> false_type is_const_buffer_sequence_impl(...);
} // namespace detail

template <typename T>
using is_const_buffer_sequence =
    decltype(detail::is_const_buffer_sequence_impl<T>(0));

template <typename T>
inline constexpr bool is_const_buffer_sequence_v =
    is_const_buffer_sequence<T>::value;

} // namespace iasr
