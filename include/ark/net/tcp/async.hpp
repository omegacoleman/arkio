#pragma once

#include <ark/bindings.hpp>
#include <ark/clinux.hpp>

#include <ark/async/async_op.hpp>
#include <ark/async/context.hpp>
#include <ark/net/address.hpp>
#include <ark/net/tcp/acceptor.hpp>
#include <ark/net/tcp/socket.hpp>

namespace ark {
namespace net {
namespace tcp {
namespace async {

inline void connect(socket &f, const address &endpoint,
                    callback<error_code> &&cb) noexcept {
  async_syscall::connect(
      f.context(), f.get(), endpoint.sa_ptr(), endpoint.sa_len(),
      [cb(forward<callback<error_code>>(cb))](ec_or<long> ret) mutable {
        if (!ret)
          cb(ret.ec());
        cb({});
      });
}

struct accept_with_address_impl {
  struct locals_t {
    acceptor &f_;
    address &endpoint_;

    clinux::socklen_t addrlen_buf{sizeof(clinux::sockaddr)};

    locals_t(acceptor &f, address &endpoint) : f_(f), endpoint_(endpoint) {}
  };
  using ret_t = ec_or<socket>;
  using op_t = async_op<accept_with_address_impl>;

  static void run(op_t &op) noexcept {
    async_syscall::accept(op.ctx_, op.locals_->f_.get(),
                          op.locals_->endpoint_.sa_ptr(),
                          addressof(op.locals_->addrlen_buf), 0,
                          op.yield_syscall(accept_with_address_impl::finish));
  }

  static void finish(op_t &op, ec_or<long> ret) noexcept {
    if (!ret) {
      op.complete(ret.ec());
    }
    op.complete(wrap_accepted_socket(&op.ctx_, static_cast<int>(ret.get())));
  }
};

inline void accept(acceptor &srv, address &endpoint,
                   callback<ec_or<socket>> &&cb) noexcept {
  using impl_t = accept_with_address_impl;
  async_op<impl_t>(srv.context(), forward<callback<ec_or<socket>>>(cb),
                   make_unique<typename impl_t::locals_t>(srv, endpoint))
      .run();
}

inline void accept(acceptor &srv, callback<ec_or<socket>> &&cb) noexcept {
  async_syscall::accept(
      srv.context(), srv.get(), NULL, NULL, 0,
      [&ctx(srv.context()),
       cb(forward<callback<ec_or<socket>>>(cb))](ec_or<long> ret) mutable {
        if (!ret)
          cb(ret.ec());
        cb(wrap_accepted_socket(&ctx, static_cast<int>(ret.get())));
      });
}

} // namespace async
} // namespace tcp
} // namespace net
} // namespace ark
