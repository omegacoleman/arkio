#pragma once

#include <iasr/bindings.hpp>
#include <iasr/clinux.hpp>

#include <iasr/io/fd.hpp>

namespace iasr {
namespace net {
namespace tcp {

class socket;

inline socket wrap_accepted_socket(int fd) noexcept;

class socket : public fd {
protected:
  socket(int fd_int)
      : fd(fd_int, fd_offset::offset_manager{fd_offset::socket}) {}

public:
  friend inline socket wrap_accepted_socket(int fd) noexcept;

  static ec_or<socket> create(bool use_ipv6 = false) noexcept {
    int ret = clinux::socket(use_ipv6 ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
    if (ret == -1) {
      return clinux::errno_ec();
    }
    return socket(ret);
  }
};

inline socket wrap_accepted_socket(int fd) noexcept { return socket(fd); }

} // namespace tcp
} // namespace net
} // namespace iasr
