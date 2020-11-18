#pragma once

#include <ark/bindings.hpp>

#include <ark/buffer/buffer.hpp>
#include <ark/buffer/sequence.hpp>

namespace ark {

/*! \addtogroup buffer
 *  @{
 */

namespace detail {
template <typename T,
          typename = enable_if_t<is_convertible_v<
              decltype(*buffer_sequence_begin(declval<T>())), mutable_buffer>>,
          typename = enable_if_t<is_convertible_v<
              decltype(*buffer_sequence_end(declval<T>())), mutable_buffer>>>
true_type is_mutable_buffer_sequence_impl(int);

template <typename T> false_type is_mutable_buffer_sequence_impl(...);
} // namespace detail

/*!
 * \brief std::true_type if T is a MutableBufferSequence
 *
 * std::false_type otherwise.
 *
 * A MutableBufferSequence denotes a type convertible to \ref
 * ::ark::mutable_buffer, or an iterable of it.
 *
 * \remark [buffer.traits] as defined in N4771, see \ref info_network
 */
template <typename T>
using is_mutable_buffer_sequence =
    decltype(detail::is_mutable_buffer_sequence_impl<T>(0));

/*!
 * \brief helper for is_mutable_buffer_sequence::value
 *
 * \see \ref ::ark::is_mutable_buffer_sequence
 */
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

/*!
 * \brief std::true_type if T is a ConstBufferSequence
 *
 * std::false_type otherwise.
 *
 * A ConstBufferSequence denotes a type convertible to \ref ::ark::const_buffer,
 * or an iterable of it.
 *
 * By theory, a MutableBufferSequence is a ConstBufferSequence, but there's no
 * vice-versa.
 *
 * \remark [buffer.traits] as defined in N4771, see \ref info_network
 */
template <typename T>
using is_const_buffer_sequence =
    decltype(detail::is_const_buffer_sequence_impl<T>(0));

/*!
 * \brief helper for is_const_buffer_sequence::value
 *
 * \see \ref ::ark::is_const_buffer_sequence
 */
template <typename T>
inline constexpr bool is_const_buffer_sequence_v =
    is_const_buffer_sequence<T>::value;

/*! @} */

} // namespace ark
