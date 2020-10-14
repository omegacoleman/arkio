#pragma once

#include <iasr/bindings.hpp>
#include <iasr/clinux.hpp>

#include <iasr/error/ec_or.hpp>

namespace iasr {

namespace fd_offset {

struct offset_concept {
  virtual ~offset_concept() = default;

  virtual clinux::off_t
  sync_offset() const noexcept = 0; // passed by preadv2/pwritev2
  virtual clinux::off_t
  async_offset() const noexcept = 0; // passed by io_uring readv/writev
  virtual void seek(const clinux::off_t offset) noexcept = 0;

  virtual void on_read(ec_or<size_t> ret) noexcept = 0;
  virtual void on_write(ec_or<size_t> ret) noexcept = 0;
};

struct offset_ignored : public offset_concept {
  virtual clinux::off_t sync_offset() const noexcept override { return -1; }
  virtual clinux::off_t async_offset() const noexcept override { return 0; }
  virtual void seek(const clinux::off_t offset) noexcept override {}

  virtual void on_read(ec_or<size_t> ret) noexcept override {}
  virtual void on_write(ec_or<size_t> ret) noexcept override {}
};

struct offset_socket : public offset_concept {
  virtual clinux::off_t sync_offset() const noexcept override { return -1; }
  virtual clinux::off_t async_offset() const noexcept override {
    return 0;
  } // strange, maybe this is linux
  virtual void seek(const clinux::off_t offset) noexcept override {}

  virtual void on_read(ec_or<size_t> ret) noexcept override {}
  virtual void on_write(ec_or<size_t> ret) noexcept override {}
};

struct offset_userland : public offset_concept {
  clinux::off_t off_{0};

  virtual clinux::off_t sync_offset() const noexcept override { return off_; }
  virtual clinux::off_t async_offset() const noexcept override { return off_; }
  virtual void seek(const clinux::off_t offset) noexcept override {
    off_ = offset;
  }

  virtual void on_read(ec_or<size_t> ret) noexcept override {
    if (ret)
      off_ += ret.get();
  }
  virtual void on_write(ec_or<size_t> ret) noexcept override {
    if (ret)
      off_ += ret.get();
  }
};

static inline const offset_ignored ignored{};
static inline const offset_socket socket{};
static inline const offset_userland userland{};

struct offset_manager {
  unique_ptr<offset_concept> con_;

  offset_manager() : con_(make_unique<offset_ignored>()) {}

  template <typename Concept>
  offset_manager(const Concept &) : con_(make_unique<Concept>()) {}

  clinux::off_t sync_offset() const noexcept { return con_->sync_offset(); }
  clinux::off_t async_offset() const noexcept { return con_->async_offset(); }
  void seek(const clinux::off_t offset) noexcept { con_->seek(offset); }

  void on_read(ec_or<size_t> ret) noexcept { con_->on_read(ret); }
  void on_write(ec_or<size_t> ret) noexcept { con_->on_write(ret); }
};

}; // namespace fd_offset

class base_fd {
private:
  int fd_;
  fd_offset::offset_manager off_m_;

public:
  base_fd(int fd_int) noexcept : fd_(fd_int) {}
  base_fd(int fd_int, fd_offset::offset_manager off_m) noexcept
      : fd_(fd_int), off_m_(move(off_m)) {}

  error_code close() noexcept {
    if (clinux::close(fd_) != 0) {
      return clinux::errno_ec();
    }
    return {};
  }

  int get() const noexcept { return fd_; }

  clinux::off_t sync_offset() const noexcept { return off_m_.sync_offset(); }
  clinux::off_t async_offset() const noexcept { return off_m_.async_offset(); }
  void seek(const clinux::off_t offset) noexcept { off_m_.seek(offset); }

  void on_read(ec_or<size_t> ret) noexcept { off_m_.on_read(ret); }

  void on_write(ec_or<size_t> ret) noexcept { off_m_.on_read(ret); }

  ~base_fd() noexcept { static_cast<void>(this->close()); }
};

class fd {
protected:
  fd(int fd_int) noexcept : base_(make_unique<base_fd>(fd_int)) {}
  fd(int fd_int, fd_offset::offset_manager off_m) noexcept
      : base_(make_unique<base_fd>(fd_int, move(off_m))) {}

private:
  unique_ptr<base_fd> base_;

public:
  fd() noexcept {}

  error_code close() noexcept {
    if (base_) {
      return base_->close();
    }
    return {};
  }

  int get() const noexcept {
    Expects(base_);
    return base_->get();
  }

  clinux::off_t sync_offset() const noexcept { return base_->sync_offset(); }
  clinux::off_t async_offset() const noexcept { return base_->async_offset(); }
  void seek(const clinux::off_t offset) noexcept { base_->seek(offset); }

  void on_read(ec_or<size_t> ret) noexcept { base_->on_read(ret); }

  void on_write(ec_or<size_t> ret) noexcept { base_->on_read(ret); }
};
} // namespace iasr
