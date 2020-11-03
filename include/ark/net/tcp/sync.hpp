#pragma once

#include <ark/bindings.hpp>
#include <ark/clinux.hpp>

#include <ark/net/tcp/acceptor.hpp>
#include <ark/net/tcp/socket.hpp>

namespace ark {
namespace net {
namespace tcp {
namespace sync {

inline result<void> connect(socket &f, const address &endpoint) noexcept {
  int ret = clinux::connect(f.get(), endpoint.sa_ptr(), endpoint.sa_len());
  if (ret == -1) {
    return clinux::errno_ec();
  }
  return success();
}

inline result<socket> accept(acceptor &srv) noexcept {
  int ret = clinux::accept4(srv.get(), NULL, NULL, 0);
  if (ret == -1) {
    return clinux::errno_ec();
  }
  return wrap_accepted_socket(nullptr, ret);
}

inline result<socket> accept(acceptor &srv, address &endpoint) noexcept {
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
} // namespace ark
