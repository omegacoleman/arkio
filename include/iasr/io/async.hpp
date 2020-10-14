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

namespace iasr {
namespace async {

struct read_some_bseq_impl {
  struct locals_t {
    fd &f_;
    vector<clinux::iovec> iovecs_;

    template <class BufferSeq>
    locals_t(fd &f, BufferSeq &b) noexcept
        : f_(f), iovecs_(to_iovec_array_ptr(b)) {}
  };
  using ret_t = size_t;
  using op_t = async_op<read_some_bseq_impl>;

  static void run(op_t &op) noexcept {
    auto ret = async_syscall::readv(
        op.ctx_, op.locals_->f_.get(), op.locals_->iovecs_.data(),
        op.locals_->iovecs_.size(), op.locals_->f_.async_offset(),
        op.yield_syscall(read_some_bseq_impl::finish));
    if (!ret)
      finish(op, ret.ec());
  }

  static void finish(op_t &op, ec_or<long> ret) noexcept {
    if (!ret) {
      op.locals_->f_.on_read(ret.ec());
      op.complete(ret.ec());
    } else {
      size_t retv = static_cast<size_t>(ret.get());
      op.locals_->f_.on_read(retv);
      op.complete(static_cast<ret_t>(retv));
    }
  }
};

struct write_some_bseq_impl {
  struct locals_t {
    fd &f_;
    vector<clinux::iovec> iovecs_;

    template <class ConstBufferSeq>
    locals_t(fd &f, ConstBufferSeq &b) noexcept
        : f_(f), iovecs_(to_iovec_array_ptr(b)) {}
  };
  using ret_t = size_t;
  using op_t = async_op<write_some_bseq_impl>;

  static void run(op_t &op) noexcept {
    auto ret = async_syscall::writev(
        op.ctx_, op.locals_->f_.get(), op.locals_->iovecs_.data(),
        op.locals_->iovecs_.size(), op.locals_->f_.async_offset(),
        op.yield_syscall(write_some_bseq_impl::finish));
    if (!ret)
      finish(op, ret.ec());
  }

  static void finish(op_t &op, ec_or<long> ret) noexcept {
    if (!ret) {
      op.locals_->f_.on_write(ret.ec());
      op.complete(ret.ec());
    } else {
      size_t retv = static_cast<size_t>(ret.get());
      op.locals_->f_.on_write(retv);
      op.complete(static_cast<ret_t>(retv));
    }
  }
};

template <class BufferSeq, enable_if_t<is_buffer_seq_v<BufferSeq>, int> = 0>
inline void read_some(async_context &ctx, fd &f, BufferSeq &b,
                      callback<size_t> &&cb) noexcept {
  using impl_t = read_some_bseq_impl;
  async_op<impl_t>(ctx, forward<callback<size_t>>(cb),
                   make_unique<impl_t::locals_t>(f, b))
      .run();
}

template <class ConstBufferSeq,
          enable_if_t<is_buffer_seq_v<ConstBufferSeq>, int> = 0>
inline void write_some(async_context &ctx, fd &f, ConstBufferSeq &b,
                       callback<size_t> &&cb) noexcept {
  using impl_t = write_some_bseq_impl;
  async_op<impl_t>(ctx, forward<callback<size_t>>(cb),
                   make_unique<impl_t::locals_t>(f, b))
      .run();
}

inline void read_some(async_context &ctx, fd &f, const buffer_view b,
                      callback<size_t> &&cb) noexcept {
  array<buffer_view, 1> b_wrap{b};
  read_some(ctx, f, b_wrap, forward<callback<size_t>>(cb));
}

inline void write_some(async_context &ctx, fd &f, const const_buffer_view b,
                       callback<size_t> &&cb) noexcept {
  array<const_buffer_view, 1> b_wrap{b};
  write_some(ctx, f, b_wrap, forward<callback<size_t>>(cb));
}

struct read_buffer_impl {
  struct locals_t {
    fd &f_;
    buffer_view remain_;
    size_t done_;

    locals_t(fd &f, const buffer_view b) noexcept
        : f_(f), remain_(b), done_(0) {}
  };
  using ret_t = size_t;
  using op_t = async_op<read_buffer_impl>;

  static void run(op_t &op) noexcept { do_read(op); }

  static void do_read(op_t &op) noexcept {
    read_some(op.ctx_, op.locals_->f_, op.locals_->remain_,
              op.yield<size_t>(continue_read));
  }

  static void continue_read(op_t &op, ec_or<size_t> ret) noexcept {
    if (!ret) {
      op.complete(ret.ec());
      return;
    }
    size_t sz = static_cast<size_t>(ret.get());
    if (sz == 0) {
      op.complete(static_cast<ret_t>(op.locals_->done_));
      return;
    }
    op.locals_->done_ += sz;
    if (op.locals_->remain_.size() <= sz) {
      op.complete(static_cast<ret_t>(op.locals_->done_));
      return;
    }
    op.locals_->remain_ = op.locals_->remain_.subview(sz);
    do_read(op);
  }
};

struct write_buffer_impl {
  struct locals_t {
    fd &f_;
    const_buffer_view remain_;
    size_t done_;

    locals_t(fd &f, const const_buffer_view b) noexcept
        : f_(f), remain_(b), done_(0) {}
  };
  using ret_t = size_t;
  using op_t = async_op<write_buffer_impl>;

  static void run(op_t &op) noexcept { do_write(op); }

  static void do_write(op_t &op) noexcept {
    write_some(op.ctx_, op.locals_->f_, op.locals_->remain_,
               op.yield<size_t>(continue_write));
  }

  static void continue_write(op_t &op, ec_or<size_t> ret) noexcept {
    if (!ret) {
      op.complete(ret.ec());
      return;
    }
    size_t sz = static_cast<size_t>(ret.get());
    op.locals_->done_ += sz;
    if (op.locals_->remain_.size() <= sz) {
      op.complete(static_cast<ret_t>(op.locals_->done_));
      return;
    }
    op.locals_->remain_ = op.locals_->remain_.subview(sz);
    do_write(op);
  }
};

inline void read(async_context &ctx, fd &f, const buffer_view b,
                 callback<size_t> &&cb) noexcept {
  using impl_t = read_buffer_impl;
  async_op<impl_t>(ctx, forward<callback<size_t>>(cb),
                   make_unique<impl_t::locals_t>(f, b))
      .run();
}

inline void write(async_context &ctx, fd &f, const const_buffer_view b,
                  callback<size_t> &&cb) noexcept {
  using impl_t = write_buffer_impl;
  async_op<impl_t>(ctx, forward<callback<size_t>>(cb),
                   make_unique<impl_t::locals_t>(f, b))
      .run();
}

template <class BufferSeq> struct read_bseq_impl {
  struct locals_t {
    fd &f_;

    BufferSeq &bseq_;
    size_t done_;

    decltype(begin(declval<BufferSeq &>())) it_;

    locals_t(fd &f, BufferSeq &bseq) noexcept
        : f_(f), bseq_(bseq), done_(0), it_(begin(bseq_)) {}
  };
  using ret_t = size_t;
  using op_t = async_op<read_bseq_impl<BufferSeq>>;

  static void run(op_t &op) noexcept { do_read(op); }

  static void do_read(op_t &op) noexcept {
    if (op.locals_->it_ == end(op.locals_->bseq_)) {
      op.complete(static_cast<ret_t>(op.locals_->done_));
      return;
    }
    read(op.ctx_, op.locals_->f_, *op.locals_->it_,
         op.template yield<size_t>(continue_read));
  }

  static void continue_read(op_t &op, ec_or<size_t> ret) noexcept {
    if (!ret) {
      op.complete(ret.ec());
      return;
    }
    size_t sz = ret.get();
    op.locals_->done_ += sz;
    if (sz < op.locals_->it_->size()) {
      op.complete(static_cast<ret_t>(op.locals_->done_));
      return;
    }
    op.locals_->it_++;
    do_read(op);
  }
};

template <class ConstBufferSeq> struct write_bseq_impl {
  struct locals_t {
    fd &f_;

    ConstBufferSeq &bseq_;
    size_t done_;

    decltype(begin(declval<ConstBufferSeq &>())) it_;

    locals_t(fd &f, ConstBufferSeq &bseq) noexcept
        : f_(f), bseq_(bseq), done_(0), it_(begin(bseq_)) {}
  };
  using ret_t = size_t;
  using op_t = async_op<write_bseq_impl<ConstBufferSeq>>;

  static void run(op_t &op) noexcept { do_write(op); }

  static void do_write(op_t &op) noexcept {
    if (op.locals_->it_ == end(op.locals_->bseq_)) {
      op.complete(static_cast<ret_t>(op.locals_->done_));
      return;
    }
    write(op.ctx_, op.locals_->f_, *op.locals_->it_,
          op.template yield<size_t>(continue_write));
  }

  static void continue_write(op_t &op, ec_or<size_t> ret) noexcept {
    if (!ret) {
      op.complete(ret.ec());
      return;
    }
    size_t sz = ret.get();
    op.locals_->done_ += sz;
    op.locals_->it_++;
    do_write(op);
  }
};

template <class BufferSeq, enable_if_t<is_buffer_seq_v<BufferSeq>, int> = 0>
inline void read(async_context &ctx, fd &f, BufferSeq &b,
                 callback<size_t> &&cb) noexcept {
  using impl_t = read_bseq_impl<BufferSeq>;
  async_op<impl_t>(ctx, forward<callback<size_t>>(cb),
                   make_unique<typename impl_t::locals_t>(f, b))
      .run();
}

template <class ConstBufferSeq,
          enable_if_t<is_buffer_seq_v<ConstBufferSeq>, int> = 0>
inline void write(async_context &ctx, fd &f, ConstBufferSeq &b,
                  callback<size_t> &&cb) noexcept {
  using impl_t = write_bseq_impl<ConstBufferSeq>;
  async_op<impl_t>(ctx, forward<callback<size_t>>(cb),
                   make_unique<typename impl_t::locals_t>(f, b))
      .run();
}

} // namespace async
} // namespace iasr
