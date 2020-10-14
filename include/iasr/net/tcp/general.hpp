#pragma once

#include <iasr/bindings.hpp>
#include <iasr/clinux.hpp>

#include <iasr/net/tcp/socket.hpp>

namespace iasr {
namespace net {
namespace tcp {

inline error_code bind(acceptor &f, const address &endpoint) noexcept {
  int ret = clinux::bind(f.get(), endpoint.sa_ptr(), endpoint.sa_len());
  if (ret == -1) {
    return clinux::errno_ec();
  }
  return {};
}

inline error_code listen(acceptor &f,
                         int backlog = numeric_limits<int>::max()) noexcept {
  int ret = clinux::listen(f.get(), backlog);
  if (ret == -1) {
    return clinux::errno_ec();
  }
  return {};
}

} // namespace tcp
} // namespace net
} // namespace iasr
