#pragma once

#include <iasr/bindings.hpp>
#include <iasr/clinux.hpp>

#include <iasr/async/async_op.hpp>
#include <iasr/async/context.hpp>
#include <iasr/async/io_uring/async_syscall.hpp>
#include <iasr/buffer/sequence.hpp>
#include <iasr/buffer/traits.hpp>
#include <iasr/error/ec_or.hpp>
#include <iasr/io/completion_condition.hpp>
#include <iasr/io/iovecs.hpp>
#include <iasr/io/nonseekable/nonseekable_fd.hpp>

namespace iasr {
namespace async {

template <class CompletionCondition>
struct nonseekable_read_buffer_sequence_impl {
  struct locals_t {
    nonseekable_fd &f_;
    vector<mutable_buffer> b_;
    CompletionCondition cond_;
    size_t done_sz_;
    vector<clinux::iovec> iov_;

    template <class MutableBufferSequence>
    locals_t(nonseekable_fd &f, const MutableBufferSequence &b,
             CompletionCondition cond) noexcept
        : f_(f), cond_(cond), done_sz_(0) {
      auto sz = buffer_sequence_end(b) - buffer_sequence_begin(b);
      iov_.reserve(sz);
      b_.reserve(sz);
      auto it_begin = buffer_sequence_begin(b);
      for (auto it = buffer_sequence_begin(b); it != buffer_sequence_end(b);
           ++it) {
        b_.emplace_back(mutable_buffer(*it));
      }
    }
  };
  using ret_t = ec_or<size_t>;
  using op_t = async_op<nonseekable_read_buffer_sequence_impl>;

  static void run(op_t &op) noexcept {
    size_t to_transfer_max =
        op.locals_->cond_(op.locals_->b_, op.locals_->done_sz_);
    if (!to_transfer_max)
      return op.complete(op.locals_->done_sz_);
    op.locals_->iov_.clear();
    transform_to_iovecs(op.locals_->b_, op.locals_->done_sz_, to_transfer_max,
                        back_inserter(op.locals_->iov_));

    auto &ctx = op.ctx_;
    auto f_get = op.locals_->f_.get();
    auto iov_d = op.locals_->iov_.data();
    auto iov_s = op.locals_->iov_.size();
    auto ret = async_syscall::readv(ctx, f_get, iov_d, iov_s, 0,
                                    op.yield_syscall(go_on));
    if (!ret)
      op.complete(ret.ec());
  }

  static void go_on(op_t &op, ec_or<long> ret) noexcept {
    if (!ret) {
      return op.complete(ret.ec());
    }
    size_t ret_sz = static_cast<size_t>(ret.get());
    if (ret_sz == 0) { // eof
      return op.complete(op.locals_->done_sz_);
    }
    op.locals_->done_sz_ += ret_sz;
    run(op);
  }
};

template <class CompletionCondition>
struct nonseekable_write_buffer_sequence_impl {
  struct locals_t {
    nonseekable_fd &f_;
    vector<const_buffer> b_;
    CompletionCondition cond_;
    size_t done_sz_;
    vector<clinux::iovec> iov_;

    template <class ConstBufferSequence>
    locals_t(nonseekable_fd &f, const ConstBufferSequence &b,
             CompletionCondition cond) noexcept
        : f_(f), cond_(cond), done_sz_(0), iov_() {
      auto sz = buffer_sequence_end(b) - buffer_sequence_begin(b);
      iov_.reserve(sz);
      b_.reserve(sz);
      auto it_begin = buffer_sequence_begin(b);
      for (auto it = buffer_sequence_begin(b); it != buffer_sequence_end(b);
           ++it) {
        b_.emplace_back(const_buffer(*it));
      }
    }
  };
  using ret_t = ec_or<size_t>;
  using op_t = async_op<nonseekable_write_buffer_sequence_impl>;

  static void run(op_t &op) noexcept {
    size_t to_transfer_max =
        op.locals_->cond_(op.locals_->b_, op.locals_->done_sz_);
    if (!to_transfer_max)
      return op.complete(op.locals_->done_sz_);
    op.locals_->iov_.clear();
    transform_to_iovecs(op.locals_->b_, op.locals_->done_sz_, to_transfer_max,
                        back_inserter(op.locals_->iov_));

    auto &ctx = op.ctx_;
    auto f_get = op.locals_->f_.get();
    auto iov_d = op.locals_->iov_.data();
    auto iov_s = op.locals_->iov_.size();
    auto ret = async_syscall::writev(ctx, f_get, iov_d, iov_s, 0,
                                     op.yield_syscall(go_on));
    if (!ret)
      op.complete(ret.ec());
  }

  static void go_on(op_t &op, ec_or<long> ret) noexcept {
    if (!ret) {
      return op.complete(ret.ec());
    }
    size_t ret_sz = static_cast<size_t>(ret.get());
    op.locals_->done_sz_ += ret_sz;
    run(op);
  }
};

template <class MutableBufferSequence, class CompletionCondition>
inline void read(nonseekable_fd &f, const MutableBufferSequence &b,
                 CompletionCondition cond,
                 callback<ec_or<size_t>> &&cb) noexcept {
  static_assert(is_mutable_buffer_sequence_v<MutableBufferSequence>);
  using impl_t = nonseekable_read_buffer_sequence_impl<CompletionCondition>;
  async_op<impl_t>(f.context(), forward<callback<ec_or<size_t>>>(cb),
                   make_unique<typename impl_t::locals_t>(f, b, cond))
      .run();
}

template <class ConstBufferSequence, class CompletionCondition>
inline void write(nonseekable_fd &f, const ConstBufferSequence &b,
                  CompletionCondition cond,
                  callback<ec_or<size_t>> &&cb) noexcept {
  static_assert(is_const_buffer_sequence_v<ConstBufferSequence>);
  using impl_t = nonseekable_write_buffer_sequence_impl<CompletionCondition>;
  async_op<impl_t>(f.context(), forward<callback<ec_or<size_t>>>(cb),
                   make_unique<typename impl_t::locals_t>(f, b, cond))
      .run();
}

template <class MutableBufferSequence>
inline void read(nonseekable_fd &f, const MutableBufferSequence &b,
                 callback<ec_or<size_t>> &&cb) noexcept {
  read(f, b, transfer_all(), forward<callback<ec_or<size_t>>>(cb));
}

template <class ConstBufferSequence>
inline void write(nonseekable_fd &f, const ConstBufferSequence &b,
                  callback<ec_or<size_t>> &&cb) noexcept {
  write(f, b, transfer_all(), forward<callback<ec_or<size_t>>>(cb));
}

} // namespace async
} // namespace iasr
