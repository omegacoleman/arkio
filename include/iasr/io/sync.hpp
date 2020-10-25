#pragma once

#include <iasr/bindings.hpp>
#include <iasr/clinux.hpp>

#include <iasr/buffer/buffer_view.hpp>
#include <iasr/buffer/traits.hpp>
#include <iasr/error/ec_or.hpp>
#include <iasr/io/iovecs.hpp>
#include <iasr/io/nonseekable/sync.hpp>
#include <iasr/io/seekable/sync.hpp>

namespace iasr {
namespace sync {

template <class Fd, class BufferSeq,
          enable_if_t<is_buffer_seq_v<BufferSeq>, int> = 0>
inline ec_or<size_t> read_some(Fd &f, BufferSeq &b) noexcept {
  return __read_some_buffer_seq(f, b);
}

template <class Fd, class ConstBufferSeq,
          enable_if_t<is_buffer_seq_v<ConstBufferSeq>, int> = 0>
inline ec_or<size_t> write_some(Fd &f, ConstBufferSeq &b) noexcept {
  return __write_some_buffer_seq(f, b);
}

template <class Fd>
inline ec_or<size_t> read_some(Fd &f, const buffer_view b) noexcept {
  array<buffer_view, 1> b_wrap{b};
  return read_some(f, b_wrap);
}

template <class Fd>
inline ec_or<size_t> write_some(Fd &f, const const_buffer_view b) noexcept {
  array<const_buffer_view, 1> b_wrap{b};
  return write_some(f, b_wrap);
}

template <class Fd>
inline ec_or<size_t> read(Fd &f, const buffer_view b) noexcept {
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

template <class Fd>
inline ec_or<size_t> write(Fd &f, const const_buffer_view b) noexcept {
  size_t done = 0;
  while (b.size() > done) {
    const_buffer_view bv_remain{b.data() + done, b.data() + b.size()};
    auto ret = write_some(f, bv_remain);
    IASR_PASS_EC_ON(ret);
    done += ret.get();
  }
  return done;
}

template <class Fd, class BufferSeq,
          enable_if_t<is_buffer_seq_v<BufferSeq>, int> = 0>
inline ec_or<size_t> read(Fd &f, BufferSeq &b) noexcept {
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

template <class Fd, class ConstBufferSeq,
          enable_if_t<is_buffer_seq_v<ConstBufferSeq>, int> = 0>
inline ec_or<size_t> write(Fd &f, ConstBufferSeq &b) noexcept {
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
