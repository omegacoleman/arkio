#pragma once

#include <ark/async.hpp>
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

namespace async {

/*! \cond HIDDEN_CLASSES */

// apply the right constraints for BufferType depending on IoOperation
template <concepts::internal::IoOperation IoOperation, class BufferType>
struct io_operation_buffer_type_helper {};

template <concepts::MutableBufferSequence MutableBufferSequence>
struct io_operation_buffer_type_helper<io_operation::read,
                                       MutableBufferSequence> {
  using buffer_ref_type = const MutableBufferSequence &;
};

template <concepts::ConstBufferSequence ConstBufferSequence>
struct io_operation_buffer_type_helper<io_operation::write,
                                       ConstBufferSequence> {
  using buffer_ref_type = const ConstBufferSequence &;
};

template <concepts::internal::IoOperation IoOperation, concepts::Fd Fd,
          class BufferType, concepts::CompletionCondition CompletionCondition>
struct async_io_impl {
  using buffer_ref_type =
      typename io_operation_buffer_type_helper<IoOperation,
                                               BufferType>::buffer_ref_type;

  struct locals_t {
    Fd &f_;
    buffer_ref_type b_;
    CompletionCondition cond_;
    size_t done_sz_;
    vector<clinux::iovec> iov_;

    locals_t(Fd &f, buffer_ref_type b, CompletionCondition cond) noexcept
        : f_(f), b_(b), cond_(cond), done_sz_(0) {
      auto sz = buffer_sequence_end(b) - buffer_sequence_begin(b);
      iov_.reserve(sz);
    }
  };
  using ret_t = result<size_t>;
  using op_t =
      async_op<async_io_impl<IoOperation, Fd, BufferType, CompletionCondition>>;

  static void run(op_t &op) noexcept {
    size_t to_transfer_max =
        op.locals_->cond_(buffer_size(op.locals_->b_), op.locals_->done_sz_);
    if (!to_transfer_max)
      return op.complete(op.locals_->done_sz_);
    op.locals_->iov_.clear();
    transform_to_iovecs(op.locals_->b_, op.locals_->done_sz_, to_transfer_max,
                        back_inserter(op.locals_->iov_));

    auto &ctx = op.ctx_;
    auto f_get = op.locals_->f_.get();
    auto iov_d = op.locals_->iov_.data();
    auto iov_s = op.locals_->iov_.size();
    clinux::off_t off = 0;
    if constexpr (concepts::Seekable<Fd>) {
      off = op.locals_->f_.offset();
    }
    if constexpr (is_same_v<IoOperation, io_operation::read>) {
      auto ret = async_syscall::readv(ctx, f_get, iov_d, iov_s, off,
                                      op.yield_syscall(go_on));
      if (!ret)
        op.complete(ret.error());
    } else if constexpr (is_same_v<IoOperation, io_operation::write>) {
      auto ret = async_syscall::writev(ctx, f_get, iov_d, iov_s, off,
                                       op.yield_syscall(go_on));
      if (!ret)
        op.complete(ret.error());
    }
  }

  static void go_on(op_t &op, result<long> ret) noexcept {
    if (!ret) {
      return op.complete(ret.error());
    }
    size_t ret_sz = static_cast<size_t>(ret.value());
    if constexpr (is_same_v<IoOperation, io_operation::read>) {
      if (ret_sz == 0) { // eof
        return op.complete(op.locals_->done_sz_);
      }
    }
    op.locals_->done_sz_ += ret_sz;
    if constexpr (concepts::Seekable<Fd>) {
      op.locals_->f_.feed(ret_sz);
    }
    run(op);
  }
};

/*! \endcond */

/*!
 * \brief read from fd to buffer until eof or completion condition is met.
 *
 * returns instantly, cb is invoked on completion or error.
 */
template <concepts::Fd Fd,
          concepts::MutableBufferSequence MutableBufferSequence,
          concepts::CompletionCondition CompletionCondition>
inline void read(Fd &f, const MutableBufferSequence &b,
                 CompletionCondition cond,
                 callback<result<size_t>> &&cb) noexcept {
  using impl_t = async_io_impl<io_operation::read, Fd, MutableBufferSequence,
                               CompletionCondition>;
  async_op<impl_t>(f.context(), forward<callback<result<size_t>>>(cb),
                   make_unique<typename impl_t::locals_t>(f, b, cond))
      .run();
}

/*!
 * \brief read from fd to buffer until eof or completion condition is met.
 *
 * returns instantly, cb is invoked on completion or error, same as read(f, b,
 * transfer_all(), cb).
 */
template <concepts::Fd Fd,
          concepts::MutableBufferSequence MutableBufferSequence>
inline void read(Fd &f, const MutableBufferSequence &b,
                 callback<result<size_t>> &&cb) noexcept {
  read(f, b, transfer_all(), forward<callback<result<size_t>>>(cb));
}

/*!
 * \brief write to fd from buffer until completion condition is met.
 *
 * returns instantly, cb is invoked on completion or error.
 */
template <concepts::Fd Fd, concepts::ConstBufferSequence ConstBufferSequence,
          concepts::CompletionCondition CompletionCondition>
inline void write(Fd &f, const ConstBufferSequence &b, CompletionCondition cond,
                  callback<result<size_t>> &&cb) noexcept {
  using impl_t = async_io_impl<io_operation::write, Fd, ConstBufferSequence,
                               CompletionCondition>;
  async_op<impl_t>(f.context(), forward<callback<result<size_t>>>(cb),
                   make_unique<typename impl_t::locals_t>(f, b, cond))
      .run();
}

/*!
 * \brief write to fd from buffer until completion condition is met.
 *
 * returns instantly, cb is invoked on completion or error, same as write(f, b,
 * transfer_all(), cb).
 */
template <concepts::Fd Fd, concepts::ConstBufferSequence ConstBufferSequence>
inline void write(Fd &f, const ConstBufferSequence &b,
                  callback<result<size_t>> &&cb) noexcept {
  write(f, b, transfer_all(), forward<callback<result<size_t>>>(cb));
}

} // namespace async

/*! @} */

} // namespace ark
