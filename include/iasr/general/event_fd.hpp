#pragma once

#include <iasr/bindings.hpp>

#include <iasr/error/ec_or.hpp>
#include <iasr/io/fd.hpp>

namespace iasr {
class event_fd : public nonseekable_fd {
protected:
  event_fd(int fd_int) : seekable_fd(fd_int) {}

private:
  static ec_or<event_fd> __create(async_context *ctx, unsigned int count,
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
  static ec_or<event_fd> create(unsigned int count, int flags) noexcept {
    return __create(nullptr, count, flags);
  }

  static ec_or<event_fd> create(async_context &ctx, unsigned int count,
                                int flags) noexcept {
    return __create(&ctx, count, flags);
  }
};
} // namespace iasr
