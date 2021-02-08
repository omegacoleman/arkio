#pragma once

#include <ark/bindings.hpp>

#include <ark/coroutine.hpp>
#include <ark/io/async.hpp>
#include <ark/io/completion_condition.hpp>
#include <ark/io/concepts.hpp>

namespace ark {

/*! \addtogroup io
 *  @{
 */

/*!
 * \brief contains apis that returns an Awaitable
 */
namespace coro {

/*! \cond HIDDEN_CLASSES */

template <concepts::Fd Fd,
          concepts::MutableBufferSequence MutableBufferSequence,
          concepts::CompletionCondition CompletionCondition>
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

template <concepts::Fd Fd, concepts::ConstBufferSequence ConstBufferSequence,
          concepts::CompletionCondition CompletionCondition>
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

/*! \endcond */

/*!
 * \brief returns an Awaitable which read from fd to buffer until eof or
 * completion condition is met.
 *
 * returns an Awaitable which yields an result<size_t> when co_awaited.
 */
template <concepts::Fd Fd,
          concepts::MutableBufferSequence MutableBufferSequence,
          concepts::CompletionCondition CompletionCondition>
inline auto read(Fd &f, const MutableBufferSequence &b,
                 CompletionCondition cond) noexcept {
  return read_awaitable(f, b, cond);
}

/*!
 * \brief returns an Awaitable which read from fd to buffer until eof or
 * completion condition is met.
 *
 * same as read(f, b, transfer_all())
 */
template <concepts::Fd Fd,
          concepts::MutableBufferSequence MutableBufferSequence>
inline auto read(Fd &f, const MutableBufferSequence &b) noexcept {
  return read_awaitable(f, b, transfer_all());
}

/*!
 * \brief returns an Awaitable which write to fd from buffer until completion
 * condition is met.
 *
 * returns an Awaitable which yields an result<size_t> when co_awaited.
 */
template <concepts::Fd Fd, concepts::ConstBufferSequence ConstBufferSequence,
          concepts::CompletionCondition CompletionCondition>
inline auto write(Fd &f, const ConstBufferSequence &b,
                  CompletionCondition cond) noexcept {
  return write_awaitable(f, b, cond);
}

/*!
 * \brief returns an Awaitable which write to fd from buffer until completion
 * condition is met.
 *
 * same as write(f, b, transfer_all())
 */
template <concepts::Fd Fd, concepts::ConstBufferSequence ConstBufferSequence>
inline auto write(Fd &f, const ConstBufferSequence &b) noexcept {
  return write_awaitable(f, b, transfer_all());
}

} // namespace coro

/*! @} */

} // namespace ark
