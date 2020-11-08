#pragma once

#include <ark/bindings.hpp>
#include <ark/clinux.hpp>
#include <ark/coro_bindings.hpp>

#include <ark/async/context.hpp>
#include <ark/coroutine/awaitable_op.hpp>
#include <ark/error/ec_or.hpp>
#include <ark/net/address.hpp>
#include <ark/net/tcp/async.hpp>
#include <ark/net/tcp/socket.hpp>

namespace ark {
namespace net {
namespace tcp {
namespace coro {

struct connect_awaitable : public awaitable_op<error_code> {
  socket &f_;
  const address &endpoint_;

  connect_awaitable(socket &f, const address &endpoint) noexcept
      : f_(f), endpoint_(endpoint) {}

  void invoke(callback<error_code> &&cb) noexcept override {
    async::connect(f_, endpoint_, forward<callback<error_code>>(cb));
  }
};

inline auto connect(socket &f, const address &endpoint) noexcept {
  return connect_awaitable(f, endpoint);
}

struct accept_with_ep_awaitable : public awaitable_op<ec_or<socket>> {
  acceptor &srv_;
  address &endpoint_;

  accept_with_ep_awaitable(acceptor &srv, address &endpoint) noexcept
      : srv_(srv), endpoint_(endpoint) {}

  void invoke(callback<ec_or<socket>> &&cb) noexcept override {
    async::accept(srv_, endpoint_, forward<callback<ec_or<socket>>>(cb));
  }
};

struct accept_awaitable : public awaitable_op<ec_or<socket>> {
  acceptor &srv_;

  accept_awaitable(acceptor &srv) noexcept : srv_(srv) {}

  void invoke(callback<ec_or<socket>> &&cb) noexcept override {
    async::accept(srv_, forward<callback<ec_or<socket>>>(cb));
  }
};

inline auto accept(acceptor &srv, address &endpoint) noexcept {
  return accept_with_ep_awaitable(srv, endpoint);
}

inline auto accept(acceptor &srv) noexcept { return accept_awaitable(srv); }

} // namespace coro
} // namespace tcp
} // namespace net
} // namespace ark
