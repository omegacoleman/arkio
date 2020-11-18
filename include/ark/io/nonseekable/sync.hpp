#pragma once

#include <ark/bindings.hpp>

#include <ark/buffer.hpp>
#include <ark/io/completion_condition.hpp>
#include <ark/io/iovecs.hpp>
#include <ark/io/nonseekable/nonseekable_fd.hpp>

namespace ark {

/*! \addtogroup io
 *  @{
 */

namespace sync {

/*!
 * \brief read from fd to buffer until eof or completion condition is met.
 *
 * blocks until complete or error.
 */
template <class MutableBufferSequence, class CompletionCondition>
inline result<size_t> read(nonseekable_fd &f, const MutableBufferSequence &b,
                           CompletionCondition cond) {
  static_assert(is_mutable_buffer_sequence_v<MutableBufferSequence>);

  size_t done_sz = 0;

  while (size_t to_transfer_max = cond(b, done_sz)) {
    auto iov = to_iovecs(b, done_sz, to_transfer_max);
    ssize_t syscall_ret = clinux::readv(f.get(), iov.data(), iov.size());
    if (syscall_ret == -1) {
      return clinux::errno_ec();
    } else if (syscall_ret == 0) { // eof
      return done_sz;
    } else {
      done_sz += syscall_ret;
    }
  }

  return done_sz;
}

/*!
 * \brief read from fd to buffer until eof or completion condition is met.
 *
 * blocks until complete or error, same as read(f, b, transfer_all()).
 */
template <class MutableBufferSequence>
inline result<size_t> read(nonseekable_fd &f, const MutableBufferSequence &b) {
  return read(f, b, transfer_all());
}

/*!
 * \brief write to fd from buffer until completion condition is met.
 *
 * blocks until complete or error.
 */
template <class ConstBufferSequence, class CompletionCondition>
inline result<size_t> write(nonseekable_fd &f, const ConstBufferSequence &b,
                            CompletionCondition cond = transfer_all()) {
  static_assert(is_const_buffer_sequence_v<ConstBufferSequence>);

  size_t done_sz = 0;

  while (size_t to_transfer_max = cond(b, done_sz)) {
    auto iov = to_iovecs(b, done_sz, to_transfer_max);
    ssize_t syscall_ret = clinux::writev(f.get(), iov.data(), iov.size());
    if (syscall_ret == -1) {
      return clinux::errno_ec();
    } else {
      done_sz += syscall_ret;
    }
  }

  return done_sz;
}

/*!
 * \brief write to fd from buffer until completion condition is met.
 *
 * blocks until complete or error, same as write(f, b, transfer_all()).
 */
template <class ConstBufferSequence>
inline result<size_t> write(nonseekable_fd &f, const ConstBufferSequence &b) {
  return write(f, b, transfer_all());
}

} // namespace sync

/*! @} */

} // namespace ark
