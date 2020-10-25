#pragma once

#include <iasr/bindings.hpp>
#include <iasr/clinux.hpp>

#include <iasr/async/async_op.hpp>
#include <iasr/async/context.hpp>
#include <iasr/coro_bindings.hpp>
#include <iasr/coroutine/awaitable_op.hpp>
#include <iasr/error/ec_or.hpp>
#include <iasr/io/async.hpp>

namespace iasr {
namespace coro {

template <class Fd>
struct read_some_buffer_awaitable : public awaitable_op<ec_or<size_t>> {
  Fd &f_;
  const buffer_view b_;

  read_some_buffer_awaitable(Fd &f, const buffer_view b) noexcept
      : f_(f), b_(b) {}
  void invoke(callback<ec_or<size_t>> &&cb) noexcept override {
    async::read_some(f_, b_, forward<callback<ec_or<size_t>>>(cb));
  }
};

template <class Fd>
struct write_some_buffer_awaitable : public awaitable_op<ec_or<size_t>> {
  Fd &f_;
  const const_buffer_view b_;

  write_some_buffer_awaitable(Fd &f, const const_buffer_view b) noexcept
      : f_(f), b_(b) {}
  void invoke(callback<ec_or<size_t>> &&cb) noexcept override {
    async::write_some(f_, b_, forward<callback<ec_or<size_t>>>(cb));
  }
};

template <class Fd>
struct read_buffer_awaitable : public awaitable_op<ec_or<size_t>> {
  Fd &f_;
  const buffer_view b_;

  read_buffer_awaitable(Fd &f, const buffer_view b) noexcept : f_(f), b_(b) {}
  void invoke(callback<ec_or<size_t>> &&cb) noexcept override {
    async::read(f_, b_, forward<callback<ec_or<size_t>>>(cb));
  }
};

template <class Fd>
struct write_buffer_awaitable : public awaitable_op<ec_or<size_t>> {
  Fd &f_;
  const const_buffer_view b_;

  write_buffer_awaitable(Fd &f, const const_buffer_view b) noexcept
      : f_(f), b_(b) {}
  void invoke(callback<ec_or<size_t>> &&cb) noexcept override {
    async::write(f_, b_, forward<callback<ec_or<size_t>>>(cb));
  }
};

template <class Fd, class BufferSeq>
struct read_some_bseq_awaitable : public awaitable_op<ec_or<size_t>> {
  Fd &f_;
  BufferSeq &b_;

  read_some_bseq_awaitable(Fd &f, BufferSeq &b) noexcept : f_(f), b_(b) {}
  void invoke(callback<ec_or<size_t>> &&cb) noexcept override {
    async::read_some<Fd, BufferSeq>(f_, b_,
                                    forward<callback<ec_or<size_t>>>(cb));
  }
};

template <class Fd, class BufferSeq>
struct read_bseq_awaitable : public awaitable_op<ec_or<size_t>> {
  Fd &f_;
  BufferSeq &b_;

  read_bseq_awaitable(Fd &f, BufferSeq &b) noexcept : f_(f), b_(b) {}
  void invoke(callback<ec_or<size_t>> &&cb) noexcept override {
    async::read<Fd, BufferSeq>(f_, b_, forward<callback<ec_or<size_t>>>(cb));
  }
};

template <class Fd, class ConstBufferSeq>
struct write_some_bseq_awaitable : public awaitable_op<ec_or<size_t>> {
  Fd &f_;
  ConstBufferSeq &b_;

  write_some_bseq_awaitable(Fd &f, ConstBufferSeq &b) noexcept : f_(f), b_(b) {}
  void invoke(callback<ec_or<size_t>> &&cb) noexcept override {
    async::write_some<Fd, ConstBufferSeq>(f_, b_,
                                          forward<callback<ec_or<size_t>>>(cb));
  }
};

template <class Fd, class ConstBufferSeq>
struct write_bseq_awaitable : public awaitable_op<ec_or<size_t>> {
  Fd &f_;
  ConstBufferSeq &b_;

  write_bseq_awaitable(Fd &f, ConstBufferSeq &b) noexcept : f_(f), b_(b) {}
  void invoke(callback<ec_or<size_t>> &&cb) noexcept override {
    async::write<Fd, ConstBufferSeq>(f_, b_,
                                     forward<callback<ec_or<size_t>>>(cb));
  }
};

template <class Fd> inline auto read_some(Fd &f, const buffer_view b) noexcept {
  return read_some_buffer_awaitable(f, b);
}

template <class Fd>
inline auto write_some(Fd &f, const const_buffer_view b) noexcept {
  return write_some_buffer_awaitable(f, b);
}

template <class Fd, class BufferSeq,
          enable_if_t<is_buffer_seq_v<BufferSeq>, int> = 0>
inline auto read_some(Fd &f, BufferSeq &b) noexcept {
  return read_some_bseq_awaitable(f, b);
}

template <class Fd, class ConstBufferSeq,
          enable_if_t<is_buffer_seq_v<ConstBufferSeq>, int> = 0>
inline auto write_some(Fd &f, ConstBufferSeq &b) noexcept {
  return write_some_bseq_awaitable(f, b);
}

template <class Fd> inline auto read(Fd &f, const buffer_view b) noexcept {
  return read_buffer_awaitable(f, b);
}

template <class Fd>
inline auto write(Fd &f, const const_buffer_view b) noexcept {
  return write_buffer_awaitable(f, b);
}

template <class Fd, class BufferSeq,
          enable_if_t<is_buffer_seq_v<BufferSeq>, int> = 0>
inline auto read(Fd &f, BufferSeq &b) noexcept {
  return read_bseq_awaitable(f, b);
}

template <class Fd, class ConstBufferSeq,
          enable_if_t<is_buffer_seq_v<ConstBufferSeq>, int> = 0>
inline auto write(Fd &f, ConstBufferSeq &b) noexcept {
  return write_bseq_awaitable(f, b);
}

} // namespace coro
} // namespace iasr
