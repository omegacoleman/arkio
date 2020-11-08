#pragma once

#include <iasr/bindings.hpp>
#include <iasr/clinux.hpp>

#include <iasr/net/tcp/acceptor.hpp>
#include <iasr/net/tcp/socket.hpp>

namespace iasr {
namespace net {
namespace tcp {
namespace sync {

inline error_code connect(socket &f, const address &endpoint) noexcept {
  int ret = clinux::connect(f.get(), endpoint.sa_ptr(), endpoint.sa_len());
  if (ret == -1) {
    return clinux::errno_ec();
  }
  return {};
}

inline ec_or<socket> accept(acceptor &srv) noexcept {
  int ret = clinux::accept4(srv.get(), NULL, NULL, 0);
  if (ret == -1) {
    return clinux::errno_ec();
  }
  return wrap_accepted_socket(nullptr, ret);
}

inline ec_or<socket> accept(acceptor &srv, address &endpoint) noexcept {
  clinux::socklen_t addrlen_buf = endpoint.sa_len();
  int ret =
      clinux::accept4(srv.get(), endpoint.sa_ptr(), addressof(addrlen_buf), 0);
  if (ret == -1) {
    return clinux::errno_ec();
  }
  return wrap_accepted_socket(nullptr, ret);
}

} // namespace sync
} // namespace tcp
} // namespace net
} // namespace iasr
