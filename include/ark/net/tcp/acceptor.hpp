#pragma once

#include <ark/bindings.hpp>
#include <ark/clinux.hpp>

#include <ark/io/fd.hpp>

namespace ark {
namespace net {
namespace tcp {

class acceptor : public nonseekable_fd {
protected:
  acceptor(int fd_int) : nonseekable_fd(fd_int) {}

private:
  static result<acceptor> __create(async_context *ctx,
                                   bool use_ipv6 = false) noexcept {
    int ret = clinux::socket(use_ipv6 ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
    if (ret == -1) {
      return clinux::errno_ec();
    }
    acceptor ret_fd(ret);
    ret_fd.set_async_context(ctx);
    return move(ret_fd);
  }

public:
  static result<acceptor> create(bool use_ipv6 = false) noexcept {
    return __create(nullptr, use_ipv6);
  }

  static result<acceptor> create(async_context &ctx,
                                 bool use_ipv6 = false) noexcept {
    return __create(&ctx, use_ipv6);
  }
};

} // namespace tcp
} // namespace net
} // namespace ark
