#pragma once

#include <ark/bindings.hpp>
#include <ark/clinux.hpp>

#include <ark/async/context.hpp>

namespace ark {

class seekable_fd : public with_async_context {
private:
  struct base_type {
    int fd_;
    clinux::off_t offset_;

    base_type(int fd_int) noexcept : fd_(fd_int) {}

    error_code close() noexcept {
      if (clinux::close(fd_) != 0) {
        return clinux::errno_ec();
      }
      return {};
    }

    int get() const noexcept { return fd_; }

    ~base_type() noexcept { static_cast<void>(this->close()); }

    clinux::off_t offset() const noexcept { return offset_; }
    void offset(clinux::off_t o) noexcept { offset_ = o; }
  };
  unique_ptr<base_type> base_;

public:
  static constexpr bool seekable = true;

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

  clinux::off_t offset() const noexcept {
    Expects(base_);
    return base_->offset();
  }

  void seek(clinux::off_t off) noexcept {
    Expects(base_);
    base_->offset(off);
  }

  void feed(clinux::off_t rel_off) noexcept {
    Expects(base_);
    base_->offset(base_->offset() + rel_off);
  }

protected:
  seekable_fd() noexcept {}
  seekable_fd(int fd_int) noexcept : base_(make_unique<base_type>(fd_int)) {}
};

} // namespace ark
