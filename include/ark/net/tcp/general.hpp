#pragma once

#include <ark/bindings.hpp>
#include <ark/clinux.hpp>

#include <ark/net/tcp/socket.hpp>

namespace ark {
namespace net {
namespace tcp {

inline result<void> bind(acceptor &f, const address &endpoint) noexcept {
  int ret = clinux::bind(f.get(), endpoint.sa_ptr(), endpoint.sa_len());
  if (ret == -1) {
    return clinux::errno_ec();
  }
  return success();
}

inline result<void> listen(acceptor &f,
                           int backlog = numeric_limits<int>::max()) noexcept {
  int ret = clinux::listen(f.get(), backlog);
  if (ret == -1) {
    return clinux::errno_ec();
  }
  return success();
}

} // namespace tcp
} // namespace net
} // namespace ark
