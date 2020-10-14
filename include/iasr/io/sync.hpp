#pragma once

#include <iasr/bindings.hpp>
#include <iasr/clinux.hpp>

#include <iasr/buffer/buffer_view.hpp>
#include <iasr/buffer/traits.hpp>
#include <iasr/error/ec_or.hpp>
#include <iasr/io/iovecs.hpp>

namespace iasr {
namespace sync {

template <class BufferSeq, enable_if_t<is_buffer_seq_v<BufferSeq>, int> = 0>
inline ec_or<size_t> read_some(fd &f, BufferSeq &b) noexcept {
  auto iov = to_iovec_array_ptr(b);
  ssize_t syscall_ret;
  syscall_ret =
      clinux::preadv2(f.get(), iov.data(), iov.size(), f.sync_offset(), 0);
  if (syscall_ret == -1) {
    auto ret = clinux::errno_ec();
    f.on_read(ret);
    return ret;
  } else {
    f.on_read(static_cast<size_t>(syscall_ret));
    return static_cast<size_t>(syscall_ret);
  }
}

template <class ConstBufferSeq,
          enable_if_t<is_buffer_seq_v<ConstBufferSeq>, int> = 0>
inline ec_or<size_t> write_some(fd &f, ConstBufferSeq &b) noexcept {
  auto iov = to_iovec_array_ptr(b);
  ssize_t syscall_ret;
  syscall_ret =
      clinux::pwritev2(f.get(), iov.data(), iov.size(), f.sync_offset(), 0);
  if (syscall_ret == -1) {
    auto ret = clinux::errno_ec();
    f.on_write(ret);
    return ret;
  } else {
    f.on_write(static_cast<size_t>(syscall_ret));
    return static_cast<size_t>(syscall_ret);
  }
}

inline ec_or<size_t> read_some(fd &f, const buffer_view b) noexcept {
  array<buffer_view, 1> b_wrap{b};
  return read_some(f, b_wrap);
}

inline ec_or<size_t> write_some(fd &f, const const_buffer_view b) noexcept {
  array<const_buffer_view, 1> b_wrap{b};
  return write_some(f, b_wrap);
}

inline ec_or<size_t> read(fd &f, const buffer_view b) noexcept {
  size_t done = 0;
  while (b.size() > done) {
    buffer_view bv_remain{b.data() + done, b.data() + b.size()};
    auto ret = read_some(f, bv_remain);
    IASR_PASS_EC_ON(ret);
    size_t ret_int = ret.get();
    if (ret_int == 0) {
      break;
    }
    done += ret_int;
  }
  return done;
}

inline ec_or<size_t> write(fd &f, const const_buffer_view b) noexcept {
  size_t done = 0;
  while (b.size() > done) {
    const_buffer_view bv_remain{b.data() + done, b.data() + b.size()};
    auto ret = write_some(f, bv_remain);
    IASR_PASS_EC_ON(ret);
    done += ret.get();
  }
  return done;
}

template <class BufferSeq, enable_if_t<is_buffer_seq_v<BufferSeq>, int> = 0>
inline ec_or<size_t> read(fd &f, BufferSeq &b) noexcept {
  size_t done = 0;
  for (auto &it : b) {
    auto ret = read(f, it);
    IASR_PASS_EC_ON(ret);
    size_t ret_int = ret.get();
    if (ret_int < it.size()) {
      break;
    }
    done += ret_int;
  }
  return done;
}

template <class ConstBufferSeq,
          enable_if_t<is_buffer_seq_v<ConstBufferSeq>, int> = 0>
inline ec_or<size_t> write(fd &f, ConstBufferSeq &b) noexcept {
  size_t done = 0;
  for (auto &it : b) {
    auto ret = write(f, it);
    IASR_PASS_EC_ON(ret);
    done += ret.get();
  }
  return done;
}
} // namespace sync
} // namespace iasr
