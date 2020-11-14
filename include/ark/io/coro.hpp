#pragma once

#include <ark/bindings.hpp>

#include <ark/coroutine.hpp>
#include <ark/io/async.hpp>
#include <ark/io/completion_condition.hpp>

namespace ark {
namespace coro {

template <class Fd, class MutableBufferSequence, class CompletionCondition>
struct read_awaitable : public awaitable_op<result<size_t>> {
  Fd &f_;
  const MutableBufferSequence &b_;
  CompletionCondition cond_;

  read_awaitable(Fd &f, const MutableBufferSequence &b,
                 CompletionCondition cond) noexcept
      : f_(f), b_(b), cond_(cond) {}
  void invoke(callback<result<size_t>> &&cb) noexcept override {
    async::read(f_, b_, cond_, forward<callback<result<size_t>>>(cb));
  }
};

template <class Fd, class ConstBufferSequence, class CompletionCondition>
struct write_awaitable : public awaitable_op<result<size_t>> {
  Fd &f_;
  const ConstBufferSequence &b_;
  CompletionCondition cond_;

  write_awaitable(Fd &f, const ConstBufferSequence &b,
                  CompletionCondition cond) noexcept
      : f_(f), b_(b), cond_(cond) {}
  void invoke(callback<result<size_t>> &&cb) noexcept override {
    async::write(f_, b_, cond_, forward<callback<result<size_t>>>(cb));
  }
};

template <class Fd, class MutableBufferSequence, class CompletionCondition>
inline auto read(Fd &f, const MutableBufferSequence &b,
                 CompletionCondition cond) noexcept {
  return read_awaitable(f, b, cond);
}

template <class Fd, class ConstBufferSequence, class CompletionCondition>
inline auto write(Fd &f, const ConstBufferSequence &b,
                  CompletionCondition cond) noexcept {
  return write_awaitable(f, b, cond);
}

template <class Fd, class MutableBufferSequence>
inline auto read(Fd &f, const MutableBufferSequence &b) noexcept {
  return read_awaitable(f, b, transfer_all());
}

template <class Fd, class ConstBufferSequence>
inline auto write(Fd &f, const ConstBufferSequence &b) noexcept {
  return write_awaitable(f, b, transfer_all());
}

} // namespace coro
} // namespace ark
