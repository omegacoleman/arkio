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

template <clinux::version_t KernVer = clinux::version_code>
struct connect_awaitable : public awaitable_op<void> {
  async_context &ctx_;
  socket &f_;
  const address &endpoint_;

  connect_awaitable(async_context &ctx, socket &f,
                    const address &endpoint) noexcept
      : ctx_(ctx), f_(f), endpoint_(endpoint) {}

  void invoke(callback<void> &&cb) noexcept override {
    async::connect<KernVer>(ctx_, f_, endpoint_, forward<callback<void>>(cb));
  }
};

inline auto connect(async_context &ctx, socket &f,
                    const address &endpoint) noexcept {
  return connect_awaitable(ctx, f, endpoint);
}

template <clinux::version_t KernVer = clinux::version_code>
struct accept_with_ep_awaitable : public awaitable_op<socket> {
  async_context &ctx_;
  acceptor &srv_;
  address &endpoint_;

  accept_with_ep_awaitable(async_context &ctx, acceptor &srv,
                           address &endpoint) noexcept
      : ctx_(ctx), srv_(srv), endpoint_(endpoint) {}

  void invoke(callback<socket> &&cb) noexcept override {
    async::accept<KernVer>(ctx_, srv_, endpoint_,
                           forward<callback<socket>>(cb));
  }
};

template <clinux::version_t KernVer = clinux::version_code>
struct accept_awaitable : public awaitable_op<socket> {
  async_context &ctx_;
  acceptor &srv_;

  accept_awaitable(async_context &ctx, acceptor &srv) noexcept
      : ctx_(ctx), srv_(srv) {}

  void invoke(callback<socket> &&cb) noexcept override {
    async::accept<KernVer>(ctx_, srv_, forward<callback<socket>>(cb));
  }
};

template <clinux::version_t KernVer = clinux::version_code>
inline auto accept(async_context &ctx, acceptor &srv,
                   address &endpoint) noexcept {
  return accept_with_ep_awaitable<KernVer>(ctx, srv, endpoint);
}

template <clinux::version_t KernVer = clinux::version_code>
inline auto accept(async_context &ctx, acceptor &srv) noexcept {
  return accept_awaitable<KernVer>(ctx, srv);
}

} // namespace coro
} // namespace tcp
} // namespace net
} // namespace iasr
