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
  buffer addr_buf{sizeof(clinux::sockaddr)};
  clinux::socklen_t addrlen_buf = addr_buf.size();
  int ret = clinux::accept4(
      srv.get(), reinterpret_cast<clinux::sockaddr *>(addr_buf.data()),
      addressof(addrlen_buf), 0);
  if (ret == -1) {
    return clinux::errno_ec();
  }
  endpoint = address(buffer(addr_buf.begin(), addr_buf.begin() + addrlen_buf));
  return wrap_accepted_socket(nullptr, ret);
}

} // namespace sync
} // namespace tcp
} // namespace net
} // namespace iasr
