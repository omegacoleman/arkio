#pragma once

#include <ark/bindings.hpp>
#include <ark/clinux.hpp>

#include <ark/buffer/sequence.hpp>
#include <ark/buffer/traits.hpp>
#include <ark/io/completion_condition.hpp>
#include <ark/io/iovecs.hpp>
#include <ark/io/seekable/seekable_fd.hpp>

namespace ark {
namespace sync {

template <class MutableBufferSequence,
          class CompletionCondition = transfer_all_t>
inline result<size_t> read(seekable_fd &f, const MutableBufferSequence &b,
                           CompletionCondition cond = transfer_all()) {
  static_assert(is_mutable_buffer_sequence_v<MutableBufferSequence>);

  size_t done_sz = 0;

  while (size_t to_transfer_max = cond(b, done_sz)) {
    auto iov = to_iovecs(b, done_sz, to_transfer_max);
    ssize_t syscall_ret =
        clinux::preadv2(f.get(), iov.data(), iov.size(), f.offset(), 0);
    if (syscall_ret == -1) {
      return clinux::errno_ec();
    } else if (syscall_ret == 0) { // eof
      return done_sz;
    } else {
      f.feed(syscall_ret);
      done_sz += syscall_ret;
    }
  }

  return done_sz;
}

template <class ConstBufferSequence, class CompletionCondition = transfer_all_t>
inline result<size_t> write(seekable_fd &f, const ConstBufferSequence &b,
                            CompletionCondition cond = transfer_all()) {
  static_assert(is_const_buffer_sequence_v<ConstBufferSequence>);

  size_t done_sz = 0;

  while (size_t to_transfer_max = cond(b, done_sz)) {
    auto iov = to_iovecs(b, done_sz, to_transfer_max);
    ssize_t syscall_ret =
        clinux::pwritev2(f.get(), iov.data(), iov.size(), f.offset(), 0);
    if (syscall_ret == -1) {
      return clinux::errno_ec();
    } else {
      f.feed(syscall_ret);
      done_sz += syscall_ret;
    }
  }

  return done_sz;
}

} // namespace sync
} // namespace ark
