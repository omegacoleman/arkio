#pragma once

#include <iasr/bindings.hpp>
#include <iasr/clinux.hpp>
#include <iasr/coro_bindings.hpp>

#include <iasr/async/context.hpp>
#include <iasr/coroutine/awaitable_op.hpp>
#include <iasr/error/ec_or.hpp>
#include <iasr/net/address.hpp>
#include <iasr/net/tcp/async.hpp>
#include <iasr/net/tcp/socket.hpp>

namespace iasr {
namespace net {
namespace tcp {
namespace coro {

struct connect_awaitable : public awaitable_op<error_code> {
  async_context &ctx_;
  socket &f_;
  const address &endpoint_;

  connect_awaitable(async_context &ctx, socket &f,
                    const address &endpoint) noexcept
      : ctx_(ctx), f_(f), endpoint_(endpoint) {}

  void invoke(callback<error_code> &&cb) noexcept override {
    async::connect(ctx_, f_, endpoint_, forward<callback<error_code>>(cb));
  }
};

inline auto connect(async_context &ctx, socket &f,
                    const address &endpoint) noexcept {
  return connect_awaitable(ctx, f, endpoint);
}

struct accept_with_ep_awaitable : public awaitable_op<ec_or<socket>> {
  async_context &ctx_;
  acceptor &srv_;
  address &endpoint_;

  accept_with_ep_awaitable(async_context &ctx, acceptor &srv,
                           address &endpoint) noexcept
      : ctx_(ctx), srv_(srv), endpoint_(endpoint) {}

  void invoke(callback<ec_or<socket>> &&cb) noexcept override {
    async::accept(ctx_, srv_, endpoint_, forward<callback<ec_or<socket>>>(cb));
  }
};

struct accept_awaitable : public awaitable_op<ec_or<socket>> {
  async_context &ctx_;
  acceptor &srv_;

  accept_awaitable(async_context &ctx, acceptor &srv) noexcept
      : ctx_(ctx), srv_(srv) {}

  void invoke(callback<ec_or<socket>> &&cb) noexcept override {
    async::accept(ctx_, srv_, forward<callback<ec_or<socket>>>(cb));
  }
};

inline auto accept(async_context &ctx, acceptor &srv,
                   address &endpoint) noexcept {
  return accept_with_ep_awaitable(ctx, srv, endpoint);
}

inline auto accept(async_context &ctx, acceptor &srv) noexcept {
  return accept_awaitable(ctx, srv);
}

} // namespace coro
} // namespace tcp
} // namespace net
} // namespace iasr
