#pragma once

#include <ark/bindings.hpp>

#include <ark/buffer.hpp>
#include <ark/io/completion_condition.hpp>
#include <ark/io/concepts.hpp>
#include <ark/io/fd.hpp>
#include <ark/io/iovecs.hpp>

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
template <concepts::Fd Fd,
          concepts::MutableBufferSequence MutableBufferSequence,
          concepts::CompletionCondition CompletionCondition>
inline result<size_t> read(Fd &f, const MutableBufferSequence &b,
                           CompletionCondition cond) {
  size_t done_sz = 0;

  while (size_t to_transfer_max = cond(buffer_size(b), done_sz)) {
    auto iov = to_iovecs(b, done_sz, to_transfer_max);
    ssize_t syscall_ret;
    if constexpr (concepts::Seekable<Fd>) {
      syscall_ret =
          clinux::preadv2(f.get(), iov.data(), iov.size(), f.offset(), 0);
    } else {
      syscall_ret = clinux::readv(f.get(), iov.data(), iov.size());
    }
    if (syscall_ret == -1) {
      return clinux::errno_ec();
    } else if (syscall_ret == 0) { // eof
      return done_sz;
    } else {
      if constexpr (concepts::Seekable<Fd>) {
        f.feed(syscall_ret);
      }
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
template <concepts::Fd Fd,
          concepts::MutableBufferSequence MutableBufferSequence>
inline result<size_t> read(Fd &f, const MutableBufferSequence &b) {
  return read(f, b, transfer_all());
}

/*!
 * \brief write to fd from buffer until completion condition is met.
 *
 * blocks until complete or error.
 */
template <concepts::Fd Fd, concepts::ConstBufferSequence ConstBufferSequence,
          concepts::CompletionCondition CompletionCondition>
inline result<size_t> write(Fd &f, const ConstBufferSequence &b,
                            CompletionCondition cond) {
  size_t done_sz = 0;

  while (size_t to_transfer_max = cond(buffer_size(b), done_sz)) {
    auto iov = to_iovecs(b, done_sz, to_transfer_max);
    ssize_t syscall_ret;
    if constexpr (concepts::Seekable<Fd>) {
      syscall_ret =
          clinux::pwritev2(f.get(), iov.data(), iov.size(), f.offset(), 0);
    } else {
      syscall_ret = clinux::writev(f.get(), iov.data(), iov.size());
    }
    if (syscall_ret == -1) {
      return clinux::errno_ec();
    } else {
      if constexpr (concepts::Seekable<Fd>) {
        f.feed(syscall_ret);
      }
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
template <concepts::Fd Fd, concepts::ConstBufferSequence ConstBufferSequence>
inline result<size_t> write(Fd &f, const ConstBufferSequence &b) {
  return write(f, b, transfer_all());
}

} // namespace sync

/*! @} */

} // namespace ark
