#pragma once

#include <ark/bindings.hpp>

#include <ark/async/context.hpp>
#include <ark/coroutine/awaitable_op.hpp>
#include <ark/net/address.hpp>
#include <ark/net/tcp/async.hpp>
#include <ark/net/tcp/socket.hpp>

namespace ark {
namespace net {

/*! \addtogroup net
 *  @{
 */

namespace tcp {

/*!
 * \brief contains apis that returns an Awaitable
 */
namespace coro {

/*! \cond HIDDEN_CLASSES */

struct connect_awaitable : public awaitable_op<result<void>> {
  socket &f_;
  const address &endpoint_;

  connect_awaitable(socket &f, const address &endpoint) noexcept
      : f_(f), endpoint_(endpoint) {}

  void invoke(callback<result<void>> &&cb) noexcept override {
    async::connect(f_, endpoint_, forward<callback<result<void>>>(cb));
  }
};

/*! \endcond */

/*!
 * \brief connect socket to the given endpoint
 *
 * returns an Awaitable which yields an result<void> when co_awaited.
 */
inline auto connect(socket &f, const address &endpoint) noexcept {
  return connect_awaitable(f, endpoint);
}

/*! \cond HIDDEN_CLASSES */

struct accept_with_ep_awaitable : public awaitable_op<result<socket>> {
  acceptor &srv_;
  address &endpoint_;

  accept_with_ep_awaitable(acceptor &srv, address &endpoint) noexcept
      : srv_(srv), endpoint_(endpoint) {}

  void invoke(callback<result<socket>> &&cb) noexcept override {
    async::accept(srv_, endpoint_, forward<callback<result<socket>>>(cb));
  }
};

struct accept_awaitable : public awaitable_op<result<socket>> {
  acceptor &srv_;

  accept_awaitable(acceptor &srv) noexcept : srv_(srv) {}

  void invoke(callback<result<socket>> &&cb) noexcept override {
    async::accept(srv_, forward<callback<result<socket>>>(cb));
  }
};

/*! \endcond */

/*!
 * \brief accept a socket connection from the given acceptor
 *
 * returns an Awaitable which yields an result<socket> when co_awaited.
 *
 * \param[out] endpoint the address of accepted socket, on success
 */
inline auto accept(acceptor &srv, address &endpoint) noexcept {
  return accept_with_ep_awaitable(srv, endpoint);
}

/*!
 * \brief accept a socket connection from the given acceptor
 *
 * returns an Awaitable which yields an result<socket> when co_awaited.
 */
inline auto accept(acceptor &srv) noexcept { return accept_awaitable(srv); }

} // namespace coro
} // namespace tcp

/*! @} */

} // namespace net
} // namespace ark
