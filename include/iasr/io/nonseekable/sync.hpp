#pragma once

#include <iasr/bindings.hpp>
#include <iasr/clinux.hpp>

#include <iasr/buffer/sequence.hpp>
#include <iasr/buffer/traits.hpp>
#include <iasr/error/ec_or.hpp>
#include <iasr/io/completion_condition.hpp>
#include <iasr/io/iovecs.hpp>
#include <iasr/io/nonseekable/nonseekable_fd.hpp>

namespace iasr {
namespace sync {

template <class MutableBufferSequence,
          class CompletionCondition = transfer_all_t>
inline ec_or<size_t> read(nonseekable_fd &f, const MutableBufferSequence &b,
                          CompletionCondition cond = transfer_all()) {
  static_assert(is_mutable_buffer_sequence_v<MutableBufferSequence>);

  size_t done_sz = 0;

  while (size_t to_transfer_max = cond(b, done_sz)) {
    auto iov = to_iovecs(b, done_sz, to_transfer_max);
    ssize_t syscall_ret =
        clinux::preadv2(f.get(), iov.data(), iov.size(), -1, 0);
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

template <class ConstBufferSequence, class CompletionCondition = transfer_all_t>
inline ec_or<size_t> write(nonseekable_fd &f, const ConstBufferSequence &b,
                           CompletionCondition cond = transfer_all()) {
  static_assert(is_const_buffer_sequence_v<ConstBufferSequence>);

  size_t done_sz = 0;

  while (size_t to_transfer_max = cond(b, done_sz)) {
    auto iov = to_iovecs(b, done_sz, to_transfer_max);
    ssize_t syscall_ret =
        clinux::pwritev2(f.get(), iov.data(), iov.size(), -1, 0);
    if (syscall_ret == -1) {
      return clinux::errno_ec();
    } else {
      done_sz += syscall_ret;
    }
  }

  return done_sz;
}

} // namespace sync
} // namespace iasr
