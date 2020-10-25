#pragma once

#include <iasr/bindings.hpp>
#include <iasr/clinux.hpp>

#include <iasr/async/async_op.hpp>
#include <iasr/async/context.hpp>
#include <iasr/async/io_uring/async_syscall.hpp>
#include <iasr/buffer/buffer_view.hpp>
#include <iasr/buffer/traits.hpp>
#include <iasr/error/ec_or.hpp>
#include <iasr/io/iovecs.hpp>
#include <iasr/io/seekable/seekable_fd.hpp>

namespace iasr {
namespace async {

struct seekable_read_some_bseq_impl {
  struct locals_t {
    seekable_fd &f_;
    vector<clinux::iovec> iovecs_;

    template <class BufferSeq>
    locals_t(seekable_fd &f, BufferSeq &b) noexcept
        : f_(f), iovecs_(to_iovec_array_ptr(b)) {}
  };
  using ret_t = ec_or<size_t>;
  using op_t = async_op<seekable_read_some_bseq_impl>;

  static void run(op_t &op) noexcept {
    auto ret = async_syscall::readv(
        op.ctx_, op.locals_->f_.get(), op.locals_->iovecs_.data(),
        op.locals_->iovecs_.size(), op.locals_->f_.offset(),
        op.yield_syscall(seekable_read_some_bseq_impl::finish));
    if (!ret)
      finish(op, ret.ec());
  }

  static void finish(op_t &op, ec_or<long> ret) noexcept {
    if (!ret) {
      op.complete(ret.ec());
    } else {
      size_t retv = static_cast<size_t>(ret.get());
      op.locals_->f_.feed(retv);
      op.complete(retv);
    }
  }
};

struct seekable_write_some_bseq_impl {
  struct locals_t {
    seekable_fd &f_;
    vector<clinux::iovec> iovecs_;

    template <class ConstBufferSeq>
    locals_t(seekable_fd &f, ConstBufferSeq &b) noexcept
        : f_(f), iovecs_(to_iovec_array_ptr(b)) {}
  };
  using ret_t = ec_or<size_t>;
  using op_t = async_op<seekable_write_some_bseq_impl>;

  static void run(op_t &op) noexcept {
    auto ret = async_syscall::writev(
        op.ctx_, op.locals_->f_.get(), op.locals_->iovecs_.data(),
        op.locals_->iovecs_.size(), op.locals_->f_.offset(),
        op.yield_syscall(seekable_write_some_bseq_impl::finish));
    if (!ret)
      finish(op, ret.ec());
  }

  static void finish(op_t &op, ec_or<long> ret) noexcept {
    if (!ret) {
      op.complete(ret.ec());
    } else {
      size_t retv = static_cast<size_t>(ret.get());
      op.locals_->f_.feed(retv);
      op.complete(retv);
    }
  }
};

template <class BufferSeq, enable_if_t<is_buffer_seq_v<BufferSeq>, int> = 0>
inline void __read_some_buffer_seq(seekable_fd &f, BufferSeq &b,
                                   callback<ec_or<size_t>> &&cb) noexcept {
  using impl_t = seekable_read_some_bseq_impl;
  async_op<impl_t>(f.context(), forward<callback<ec_or<size_t>>>(cb),
                   make_unique<impl_t::locals_t>(f, b))
      .run();
}

template <class ConstBufferSeq,
          enable_if_t<is_buffer_seq_v<ConstBufferSeq>, int> = 0>
inline void __write_some_buffer_seq(seekable_fd &f, ConstBufferSeq &b,
                                    callback<ec_or<size_t>> &&cb) noexcept {
  using impl_t = seekable_write_some_bseq_impl;
  async_op<impl_t>(f.context(), forward<callback<ec_or<size_t>>>(cb),
                   make_unique<impl_t::locals_t>(f, b))
      .run();
}

} // namespace async
} // namespace iasr
