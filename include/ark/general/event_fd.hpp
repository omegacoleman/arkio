#pragma once

#include <ark/bindings.hpp>

#include <ark/async.hpp>
#include <ark/io.hpp>

namespace ark {
class event_fd : public nonseekable_fd {
protected:
  event_fd(int fd_int) : nonseekable_fd(fd_int) {}

private:
  static result<event_fd> __create(async_context *ctx, unsigned int count,
                                   int flags) noexcept {
    int ret = clinux::eventfd(count, flags);
    if (ret == -1) {
      return clinux::errno_ec();
    }
    event_fd f{ret};
    f.set_async_context(ctx);
    return move(f);
  }

public:
  static result<event_fd> create(unsigned int count, int flags) noexcept {
    return __create(nullptr, count, flags);
  }

  static result<event_fd> create(async_context &ctx, unsigned int count,
                                 int flags) noexcept {
    return __create(&ctx, count, flags);
  }
};
} // namespace ark
