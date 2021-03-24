#pragma once

/*! \cond FILE_NOT_DOCUMENTED */

#include <liburing.h>

#include <ark/bindings.hpp>

namespace ark {
namespace io_uring_async {
namespace liburing {
using ::io_uring;
using ::io_uring_cqe;
using ::io_uring_cqe_get_data;
using ::io_uring_cqe_seen;
using ::io_uring_get_sqe;
using ::io_uring_peek_batch_cqe;
using ::io_uring_prep_accept;
using ::io_uring_prep_connect;
using ::io_uring_prep_nop;
using ::io_uring_prep_poll_add;
using ::io_uring_prep_read;
using ::io_uring_prep_readv;
using ::io_uring_prep_write;
using ::io_uring_prep_writev;
using ::io_uring_queue_exit;
using ::io_uring_queue_init;
using ::io_uring_sqe;
using ::io_uring_sqe_set_data;
using ::io_uring_sqe_set_flags;
using ::io_uring_submit;
using ::io_uring_wait_cqe;
} // namespace liburing

class sqe_ref {
private:
  liburing::io_uring_sqe *sqe_;

public:
  sqe_ref(not_null<liburing::io_uring_sqe *> sqe) noexcept : sqe_(sqe) {}

  void prep_nop() noexcept { liburing::io_uring_prep_nop(sqe_); }

  void prep_read(int fd, void *buf, unsigned nbytes,
                 clinux::off_t offset) noexcept {
    liburing::io_uring_prep_read(sqe_, fd, buf, nbytes, offset);
  }

  void prep_write(int fd, const void *buf, unsigned nbytes,
                  clinux::off_t offset) noexcept {
    liburing::io_uring_prep_write(sqe_, fd, buf, nbytes, offset);
  }

  void prep_readv(int fd, const clinux::iovec *iovecs, unsigned nr_vecs,
                  clinux::off_t offset) noexcept {
    liburing::io_uring_prep_readv(sqe_, fd, iovecs, nr_vecs, offset);
  }

  void prep_writev(int fd, const clinux::iovec *iovecs, unsigned nr_vecs,
                   clinux::off_t offset) noexcept {
    liburing::io_uring_prep_writev(sqe_, fd, iovecs, nr_vecs, offset);
  }

  void prep_connect(int fd, const clinux::sockaddr *addr,
                    clinux::socklen_t addrlen) noexcept {
    liburing::io_uring_prep_connect(sqe_, fd, addr, addrlen);
  }

  void prep_accept(int fd, clinux::sockaddr *addr, clinux::socklen_t *addrlen,
                   int flags) noexcept {
    liburing::io_uring_prep_accept(sqe_, fd, addr, addrlen, flags);
  }

  void prep_poll_add(int fd, short poll_mask) noexcept {
    liburing::io_uring_prep_poll_add(sqe_, fd, poll_mask);
  }

  void set_data(void *data) noexcept {
    liburing::io_uring_sqe_set_data(sqe_, data);
  }

  void set_flags(unsigned flags) noexcept {
    liburing::io_uring_sqe_set_flags(sqe_, flags);
  }

#ifdef ARK_ADVANCED_DEBUG_VERBOSITY
  void dump() noexcept {
    cerr << "===SQE INFO===" << endl;
    cerr << "opcode=" << static_cast<int>(sqe_->opcode) << endl;
    cerr << "flags=" << static_cast<int>(sqe_->flags) << endl;
    cerr << "ioprio=" << sqe_->ioprio << endl;
    cerr << "fd=" << sqe_->fd << endl;
    cerr << "addr=" << sqe_->addr << endl;
    cerr << "len=" << sqe_->len << endl;
    cerr << "rw_flags=" << sqe_->rw_flags << endl;
    cerr << "user_data=" << static_cast<long>(sqe_->user_data) << endl;
    cerr << "===END SQE INFO===" << endl;
  }
#endif
};

class io_uring;

template <bool Owning> class cqe_ref {
private:
  class empty_class {};
  conditional_t<Owning, io_uring *, empty_class> parent_;
  liburing::io_uring_cqe *cqe_;

  friend cqe_ref<!Owning>;
  friend io_uring;

  void seen() noexcept {
    if constexpr (Owning) {
      if (cqe_ == nullptr) {
        return;
      }
      liburing::io_uring_cqe_seen(&parent_->ring_, cqe_);
    }
  }

public:
  cqe_ref() noexcept : cqe_(nullptr) {}

  template <bool UOwning = Owning, enable_if_t<UOwning, int> = 0>
  cqe_ref(not_null<liburing::io_uring_cqe *> cqe, io_uring &parent) noexcept
      : cqe_(cqe), parent_(&parent) {}

  template <bool UOwning = Owning, enable_if_t<!UOwning, int> = 0>
  cqe_ref(cqe_ref<true> &from) noexcept : cqe_{from.cqe_} {}

  template <bool UOwning = Owning, enable_if_t<UOwning, int> = 0>
  cqe_ref(cqe_ref &&other) noexcept : cqe_(other.cqe_), parent_(other.parent_) {
    other.cqe_ = nullptr;
    other.parent_ = nullptr;
  }

  template <bool UOwning = Owning, enable_if_t<!UOwning, int> = 0>
  cqe_ref(cqe_ref &&other) noexcept : cqe_(other.cqe_) {}

  template <bool UOwning = Owning, enable_if_t<!UOwning, int> = 0>
  cqe_ref(const cqe_ref &other) noexcept {
    cqe_ = other.cqe_;
  }

  template <bool UOwning = Owning, enable_if_t<UOwning, int> = 0>
  cqe_ref &operator=(cqe_ref &&other) noexcept {
    seen();
    cqe_ = other.cqe_;
    parent_ = other.parent_;
    other.cqe_ = nullptr;
    other.parent_ = nullptr;
    return *this;
  }

  template <bool UOwning = Owning, enable_if_t<!UOwning, int> = 0>
  cqe_ref &operator=(cqe_ref &&other) noexcept {
    seen();
    cqe_ = other.cqe_;
    return *this;
  }

  template <bool UOwning = Owning, enable_if_t<!UOwning, int> = 0>
  cqe_ref &operator=(const cqe_ref &other) noexcept {
    seen();
    cqe_ = other.cqe_;
    return *this;
  }

  ~cqe_ref() { seen(); }

  void *get_data() noexcept { return liburing::io_uring_cqe_get_data(cqe_); }

  template <typename ResType> result<ResType> to_result() noexcept {
    if (cqe_->res < 0) {
      return error_code{-cqe_->res, system_category()};
    }
    return static_cast<ResType>(cqe_->res);
  }
};

using owning_cqe_ref = cqe_ref<true>;
using unowning_cqe_ref = cqe_ref<false>;

class io_uring {
private:
  liburing::io_uring ring_;
  bool inited_{false};

  friend owning_cqe_ref;

public:
  io_uring() noexcept {}

  result<void> queue_init(unsigned entries, unsigned flags) noexcept {
    Expects(!inited_);
    int ret = liburing::io_uring_queue_init(entries, &ring_, flags);
    if (ret < 0) {
      return as_ec(-ret);
    }
    inited_ = true;
    return success();
  }

  result<sqe_ref> get_sqe() noexcept {
    Expects(inited_);
    liburing::io_uring_sqe *ret = liburing::io_uring_get_sqe(&ring_);
    if (ret != nullptr) {
      return sqe_ref{ret};
    }
    return as_ec(ENOBUFS);
  }

  int submit() noexcept {
    Expects(inited_);
    return liburing::io_uring_submit(&ring_);
  }

  result<void> wait() noexcept {
    Expects(inited_);
    liburing::io_uring_cqe *p_discarded;
    int ret = liburing::io_uring_wait_cqe(&ring_, &p_discarded);
    if (ret < 0) {
      return as_ec(-ret);
    }
    return success();
  }

  template <class OutputIt>
  OutputIt peek_batch_cqe(OutputIt it, size_t batch) noexcept {
    Expects(inited_);
    vector<liburing::io_uring_cqe *> peeked{batch, nullptr};
    const size_t peeked_n =
        liburing::io_uring_peek_batch_cqe(&ring_, peeked.data(), batch);
    for (int i = 0; i < peeked_n; i++) {
      *(it++) = owning_cqe_ref{peeked[i], *this};
    }
    return it;
  }

  ~io_uring() {
    if (inited_) {
      liburing::io_uring_queue_exit(&ring_);
    }
  }

  io_uring(io_uring &&) = delete;
  io_uring(const io_uring &) = delete;
  io_uring &operator=(io_uring &&) = delete;
  io_uring &operator=(const io_uring &) = delete;
};
} // namespace io_uring_async
} // namespace ark

/*! \endcond */
