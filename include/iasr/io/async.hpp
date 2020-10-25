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
#include <iasr/io/nonseekable/async.hpp>
#include <iasr/io/seekable/async.hpp>

namespace iasr {
namespace async {

template <class Fd, class BufferSeq,
          enable_if_t<is_buffer_seq_v<BufferSeq>, int> = 0>
inline void read_some(Fd &f, BufferSeq &b,
                      callback<ec_or<size_t>> &&cb) noexcept {
  __read_some_buffer_seq(f, b, forward<callback<ec_or<size_t>>>(cb));
}

template <class Fd, class ConstBufferSeq,
          enable_if_t<is_buffer_seq_v<ConstBufferSeq>, int> = 0>
inline void write_some(Fd &f, ConstBufferSeq &b,
                       callback<ec_or<size_t>> &&cb) noexcept {
  __write_some_buffer_seq(f, b, forward<callback<ec_or<size_t>>>(cb));
}

template <class Fd>
inline void read_some(Fd &f, const buffer_view b,
                      callback<ec_or<size_t>> &&cb) noexcept {
  array<buffer_view, 1> b_wrap{b};
  read_some(f, b_wrap, forward<callback<ec_or<size_t>>>(cb));
}

template <class Fd>
inline void write_some(Fd &f, const const_buffer_view b,
                       callback<ec_or<size_t>> &&cb) noexcept {
  array<const_buffer_view, 1> b_wrap{b};
  write_some(f, b_wrap, forward<callback<ec_or<size_t>>>(cb));
}

template <class Fd> struct read_buffer_impl {
  struct locals_t {
    Fd &f_;
    buffer_view remain_;
    size_t done_;

    locals_t(Fd &f, const buffer_view b) noexcept
        : f_(f), remain_(b), done_(0) {}
  };
  using ret_t = ec_or<size_t>;
  using op_t = async_op<read_buffer_impl>;

  static void run(op_t &op) noexcept { do_read(op); }

  static void do_read(op_t &op) noexcept {
    read_some(op.locals_->f_, op.locals_->remain_,
              op.template yield<ec_or<size_t>>(continue_read));
  }

  static void continue_read(op_t &op, ec_or<size_t> ret) noexcept {
    if (!ret) {
      op.complete(ret.ec());
      return;
    }
    size_t sz = static_cast<size_t>(ret.get());
    if (sz == 0) {
      op.complete(op.locals_->done_);
      return;
    }
    op.locals_->done_ += sz;
    if (op.locals_->remain_.size() <= sz) {
      op.complete(op.locals_->done_);
      return;
    }
    op.locals_->remain_ = op.locals_->remain_.subview(sz);
    do_read(op);
  }
};

template <class Fd> struct write_buffer_impl {
  struct locals_t {
    Fd &f_;
    const_buffer_view remain_;
    size_t done_;

    locals_t(Fd &f, const const_buffer_view b) noexcept
        : f_(f), remain_(b), done_(0) {}
  };
  using ret_t = ec_or<size_t>;
  using op_t = async_op<write_buffer_impl>;

  static void run(op_t &op) noexcept { do_write(op); }

  static void do_write(op_t &op) noexcept {
    write_some(op.locals_->f_, op.locals_->remain_,
               op.template yield<ec_or<size_t>>(continue_write));
  }

  static void continue_write(op_t &op, ec_or<size_t> ret) noexcept {
    if (!ret) {
      op.complete(ret.ec());
      return;
    }
    size_t sz = static_cast<size_t>(ret.get());
    op.locals_->done_ += sz;
    if (op.locals_->remain_.size() <= sz) {
      op.complete(op.locals_->done_);
      return;
    }
    op.locals_->remain_ = op.locals_->remain_.subview(sz);
    do_write(op);
  }
};

template <class Fd>
inline void read(Fd &f, const buffer_view b,
                 callback<ec_or<size_t>> &&cb) noexcept {
  using impl_t = read_buffer_impl<Fd>;
  async_op<impl_t>(f.context(), forward<callback<ec_or<size_t>>>(cb),
                   make_unique<typename impl_t::locals_t>(f, b))
      .run();
}

template <class Fd>
inline void write(Fd &f, const const_buffer_view b,
                  callback<ec_or<size_t>> &&cb) noexcept {
  using impl_t = write_buffer_impl<Fd>;
  async_op<impl_t>(f.context(), forward<callback<ec_or<size_t>>>(cb),
                   make_unique<typename impl_t::locals_t>(f, b))
      .run();
}

template <class Fd, class BufferSeq> struct read_bseq_impl {
  struct locals_t {
    Fd &f_;

    BufferSeq &bseq_;
    size_t done_;

    decltype(begin(declval<BufferSeq &>())) it_;

    locals_t(Fd &f, BufferSeq &bseq) noexcept
        : f_(f), bseq_(bseq), done_(0), it_(begin(bseq_)) {}
  };
  using ret_t = ec_or<size_t>;
  using op_t = async_op<read_bseq_impl<Fd, BufferSeq>>;

  static void run(op_t &op) noexcept { do_read(op); }

  static void do_read(op_t &op) noexcept {
    if (op.locals_->it_ == end(op.locals_->bseq_)) {
      op.complete(static_cast<ret_t>(op.locals_->done_));
      return;
    }
    read(op.locals_->f_, *op.locals_->it_,
         op.template yield<ec_or<size_t>>(continue_read));
  }

  static void continue_read(op_t &op, ec_or<size_t> ret) noexcept {
    if (!ret) {
      op.complete(ret.ec());
      return;
    }
    size_t sz = ret.get();
    op.locals_->done_ += sz;
    if (sz < op.locals_->it_->size()) {
      op.complete(op.locals_->done_);
      return;
    }
    op.locals_->it_++;
    do_read(op);
  }
};

template <class Fd, class ConstBufferSeq> struct write_bseq_impl {
  struct locals_t {
    Fd &f_;

    ConstBufferSeq &bseq_;
    size_t done_;

    decltype(begin(declval<ConstBufferSeq &>())) it_;

    locals_t(Fd &f, ConstBufferSeq &bseq) noexcept
        : f_(f), bseq_(bseq), done_(0), it_(begin(bseq_)) {}
  };
  using ret_t = ec_or<size_t>;
  using op_t = async_op<write_bseq_impl<Fd, ConstBufferSeq>>;

  static void run(op_t &op) noexcept { do_write(op); }

  static void do_write(op_t &op) noexcept {
    if (op.locals_->it_ == end(op.locals_->bseq_)) {
      op.complete(static_cast<ret_t>(op.locals_->done_));
      return;
    }
    write(op.locals_->f_, *op.locals_->it_,
          op.template yield<ec_or<size_t>>(continue_write));
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

template <class Fd, class BufferSeq,
          enable_if_t<is_buffer_seq_v<BufferSeq>, int> = 0>
inline void read(Fd &f, BufferSeq &b, callback<ec_or<size_t>> &&cb) noexcept {
  using impl_t = read_bseq_impl<Fd, BufferSeq>;
  async_op<impl_t>(f.context(), forward<callback<ec_or<size_t>>>(cb),
                   make_unique<typename impl_t::locals_t>(f, b))
      .run();
}

template <class Fd, class ConstBufferSeq,
          enable_if_t<is_buffer_seq_v<ConstBufferSeq>, int> = 0>
inline void write(Fd &f, ConstBufferSeq &b,
                  callback<ec_or<size_t>> &&cb) noexcept {
  using impl_t = write_bseq_impl<Fd, ConstBufferSeq>;
  async_op<impl_t>(f.context(), forward<callback<ec_or<size_t>>>(cb),
                   make_unique<typename impl_t::locals_t>(f, b))
      .run();
}

} // namespace async
} // namespace iasr
