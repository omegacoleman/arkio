#pragma once

#include <ark/bindings.hpp>
#include <ark/clinux.hpp>

#include <ark/async/context.hpp>

namespace ark {

class nonseekable_fd : public with_async_context {
private:
  struct base_type {
    int fd_;

    base_type(int fd_int) noexcept : fd_(fd_int) {}

    error_code close() noexcept {
      if (clinux::close(fd_) != 0) {
        return clinux::errno_ec();
      }
      return {};
    }

    int get() const noexcept { return fd_; }

    ~base_type() noexcept { static_cast<void>(this->close()); }
  };
  unique_ptr<base_type> base_;

public:
  static constexpr bool seekable = false;

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

protected:
  nonseekable_fd() noexcept {}
  nonseekable_fd(int fd_int) noexcept : base_(make_unique<base_type>(fd_int)) {}
};

} // namespace ark
