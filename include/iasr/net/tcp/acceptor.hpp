#pragma once

#include <iasr/bindings.hpp>
#include <iasr/clinux.hpp>

#include <iasr/io/fd.hpp>

namespace iasr {
namespace net {
namespace tcp {

class acceptor : public fd {
protected:
  acceptor(int fd_int)
      : fd(fd_int, fd_offset::offset_manager{fd_offset::socket}) {}

public:
  static ec_or<acceptor> create(bool use_ipv6 = false) noexcept {
    int ret = clinux::socket(use_ipv6 ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
    if (ret == -1) {
      return clinux::errno_ec();
    }
    return acceptor(ret);
  }
};

} // namespace tcp
} // namespace net
} // namespace iasr
