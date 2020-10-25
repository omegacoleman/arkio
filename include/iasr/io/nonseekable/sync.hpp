#pragma once

#include <iasr/bindings.hpp>
#include <iasr/clinux.hpp>

#include <iasr/buffer/buffer_view.hpp>
#include <iasr/buffer/traits.hpp>
#include <iasr/error/ec_or.hpp>
#include <iasr/io/iovecs.hpp>
#include <iasr/io/nonseekable/nonseekable_fd.hpp>

namespace iasr {
namespace sync {

template <class BufferSeq, enable_if_t<is_buffer_seq_v<BufferSeq>, int> = 0>
inline ec_or<size_t> __read_some_buffer_seq(nonseekable_fd &f,
                                            BufferSeq &b) noexcept {
  auto iov = to_iovec_array_ptr(b);
  ssize_t syscall_ret = clinux::preadv2(f.get(), iov.data(), iov.size(), -1, 0);
  if (syscall_ret == -1) {
    return clinux::errno_ec();
  } else {
    return static_cast<size_t>(syscall_ret);
  }
}

template <class ConstBufferSeq,
          enable_if_t<is_buffer_seq_v<ConstBufferSeq>, int> = 0>
inline ec_or<size_t> __write_some_buffer_seq(nonseekable_fd &f,
                                             ConstBufferSeq &b) noexcept {
  auto iov = to_iovec_array_ptr(b);
  ssize_t syscall_ret =
      clinux::pwritev2(f.get(), iov.data(), iov.size(), -1, 0);
  if (syscall_ret == -1) {
    return clinux::errno_ec();
  } else {
    return static_cast<size_t>(syscall_ret);
  }
}

} // namespace sync
} // namespace iasr
