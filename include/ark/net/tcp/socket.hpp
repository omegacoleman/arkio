#pragma once

#include <ark/bindings.hpp>
#include <ark/clinux.hpp>

#include <ark/io/fd.hpp>

namespace ark {
namespace net {
namespace tcp {

class socket;

inline socket wrap_accepted_socket(async_context *ctx, int fd) noexcept;

class socket : public nonseekable_fd {
protected:
  socket(int fd_int) : nonseekable_fd(fd_int) {}

private:
  static ec_or<socket> __create(async_context *ctx,
                                bool use_ipv6 = false) noexcept {
    int ret = clinux::socket(use_ipv6 ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
    if (ret == -1) {
      return clinux::errno_ec();
    }
    socket ret_fd(ret);
    ret_fd.set_async_context(ctx);
    return move(ret_fd);
  }

public:
  friend inline socket wrap_accepted_socket(async_context *ctx,
                                            int fd) noexcept;

  static ec_or<socket> create(bool use_ipv6 = false) noexcept {
    return __create(nullptr, use_ipv6);
  }

  static ec_or<socket> create(async_context &ctx,
                              bool use_ipv6 = false) noexcept {
    return __create(&ctx, use_ipv6);
  }
};

inline socket wrap_accepted_socket(async_context *ctx, int fd) noexcept {
  socket ret(fd);
  ret.set_async_context(ctx);
  return move(ret);
}

} // namespace tcp
} // namespace net
} // namespace ark
