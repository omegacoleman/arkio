#pragma once

#include <ark/bindings.hpp>

#include <ark/net/address.hpp>
#include <ark/net/tcp/acceptor.hpp>
#include <ark/net/tcp/socket.hpp>

namespace ark {
namespace net {

/*! \addtogroup net
 *  @{
 */

namespace tcp {

/*!
 * \brief bind acceptor to the given endpoint
 *
 * see bind(2)
 *
 * won't block
 */
inline result<void> bind(acceptor &f, const address &endpoint) noexcept {
  int ret = clinux::bind(f.get(), endpoint.sa_ptr(), endpoint.sa_len());
  if (ret == -1) {
    return errno_ec();
  }
  return success();
}

/*!
 * \brief make acceptor start to accept connections
 *
 * see listen(2)
 *
 * won't block
 */
inline result<void> listen(acceptor &f,
                           int backlog = numeric_limits<int>::max()) noexcept {
  int ret = clinux::listen(f.get(), backlog);
  if (ret == -1) {
    return errno_ec();
  }
  return success();
}

} // namespace tcp

/*! @} */

} // namespace net
} // namespace ark
