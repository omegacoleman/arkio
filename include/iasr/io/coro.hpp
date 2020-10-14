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
struct read_some_buffer_awaitable : public awaitable_op<size_t> {
  async_context &ctx_;
  fd &f_;
  const buffer_view b_;

  read_some_buffer_awaitable(async_context &ctx, fd &f,
                             const buffer_view b) noexcept
      : ctx_(ctx), f_(f), b_(b) {}
  void invoke(callback<size_t> &&cb) noexcept override {
    async::read_some(ctx_, f_, b_, forward<callback<size_t>>(cb));
  }
};

struct write_some_buffer_awaitable : public awaitable_op<size_t> {
  async_context &ctx_;
  fd &f_;
  const const_buffer_view b_;

  write_some_buffer_awaitable(async_context &ctx, fd &f,
                              const const_buffer_view b) noexcept
      : ctx_(ctx), f_(f), b_(b) {}
  void invoke(callback<size_t> &&cb) noexcept override {
    async::write_some(ctx_, f_, b_, forward<callback<size_t>>(cb));
  }
};

struct read_buffer_awaitable : public awaitable_op<size_t> {
  async_context &ctx_;
  fd &f_;
  const buffer_view b_;

  read_buffer_awaitable(async_context &ctx, fd &f, const buffer_view b) noexcept
      : ctx_(ctx), f_(f), b_(b) {}
  void invoke(callback<size_t> &&cb) noexcept override {
    async::read(ctx_, f_, b_, forward<callback<size_t>>(cb));
  }
};

struct write_buffer_awaitable : public awaitable_op<size_t> {
  async_context &ctx_;
  fd &f_;
  const const_buffer_view b_;

  write_buffer_awaitable(async_context &ctx, fd &f,
                         const const_buffer_view b) noexcept
      : ctx_(ctx), f_(f), b_(b) {}
  void invoke(callback<size_t> &&cb) noexcept override {
    async::write(ctx_, f_, b_, forward<callback<size_t>>(cb));
  }
};

template <class BufferSeq>
struct read_some_bseq_awaitable : public awaitable_op<size_t> {
  async_context &ctx_;
  fd &f_;
  BufferSeq &b_;

  read_some_bseq_awaitable(async_context &ctx, fd &f, BufferSeq &b) noexcept
      : ctx_(ctx), f_(f), b_(b) {}
  void invoke(callback<size_t> &&cb) noexcept override {
    async::read_some<BufferSeq>(ctx_, f_, b_, forward<callback<size_t>>(cb));
  }
};

template <class BufferSeq>
struct read_bseq_awaitable : public awaitable_op<size_t> {
  async_context &ctx_;
  fd &f_;
  BufferSeq &b_;

  read_bseq_awaitable(async_context &ctx, fd &f, BufferSeq &b) noexcept
      : ctx_(ctx), f_(f), b_(b) {}
  void invoke(callback<size_t> &&cb) noexcept override {
    async::read<BufferSeq>(ctx_, f_, b_, forward<callback<size_t>>(cb));
  }
};

template <class ConstBufferSeq>
struct write_some_bseq_awaitable : public awaitable_op<size_t> {
  async_context &ctx_;
  fd &f_;
  ConstBufferSeq &b_;

  write_some_bseq_awaitable(async_context &ctx, fd &f,
                            ConstBufferSeq &b) noexcept
      : ctx_(ctx), f_(f), b_(b) {}
  void invoke(callback<size_t> &&cb) noexcept override {
    async::write_some<ConstBufferSeq>(ctx_, f_, b_,
                                      forward<callback<size_t>>(cb));
  }
};

template <class ConstBufferSeq>
struct write_bseq_awaitable : public awaitable_op<size_t> {
  async_context &ctx_;
  fd &f_;
  ConstBufferSeq &b_;

  write_bseq_awaitable(async_context &ctx, fd &f, ConstBufferSeq &b) noexcept
      : ctx_(ctx), f_(f), b_(b) {}
  void invoke(callback<size_t> &&cb) noexcept override {
    async::write<ConstBufferSeq>(ctx_, f_, b_, forward<callback<size_t>>(cb));
  }
};

inline auto read_some(async_context &ctx, fd &f, const buffer_view b) noexcept {
  return read_some_buffer_awaitable(ctx, f, b);
}

inline auto write_some(async_context &ctx, fd &f,
                       const const_buffer_view b) noexcept {
  return write_some_buffer_awaitable(ctx, f, b);
}

template <class BufferSeq, enable_if_t<is_buffer_seq_v<BufferSeq>, int> = 0>
inline auto read_some(async_context &ctx, fd &f, BufferSeq &b) noexcept {
  return read_some_bseq_awaitable(ctx, f, b);
}

template <class ConstBufferSeq,
          enable_if_t<is_buffer_seq_v<ConstBufferSeq>, int> = 0>
inline auto write_some(async_context &ctx, fd &f, ConstBufferSeq &b) noexcept {
  return write_some_bseq_awaitable(ctx, f, b);
}

inline auto read(async_context &ctx, fd &f, const buffer_view b) noexcept {
  return read_buffer_awaitable(ctx, f, b);
}

inline auto write(async_context &ctx, fd &f,
                  const const_buffer_view b) noexcept {
  return write_buffer_awaitable(ctx, f, b);
}

template <class BufferSeq, enable_if_t<is_buffer_seq_v<BufferSeq>, int> = 0>
inline auto read(async_context &ctx, fd &f, BufferSeq &b) noexcept {
  return read_bseq_awaitable(ctx, f, b);
}

template <class ConstBufferSeq,
          enable_if_t<is_buffer_seq_v<ConstBufferSeq>, int> = 0>
inline auto write(async_context &ctx, fd &f, ConstBufferSeq &b) noexcept {
  return write_bseq_awaitable(ctx, f, b);
}

} // namespace coro
} // namespace iasr
