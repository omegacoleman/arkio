#pragma once

#include <iasr/bindings.hpp>
#include <iasr/clinux.hpp>

#include <iasr/async/async_op.hpp>
#include <iasr/async/context.hpp>
#include <iasr/net/address.hpp>
#include <iasr/net/tcp/acceptor.hpp>
#include <iasr/net/tcp/socket.hpp>

namespace iasr {
namespace net {
namespace tcp {
namespace async {

template <clinux::version_t KernVer = clinux::version_code>
inline void connect(async_context &ctx, socket &f, const address &endpoint,
                    callback<void> &&cb) noexcept {
  async_syscall::connect<async_context, KernVer>(
      ctx, f.get(), endpoint.sa_ptr(), endpoint.sa_len(),
      [cb(forward<callback<void>>(cb))](ec_or<long> ret) mutable {
        if (!ret)
          cb(ret.ec());
        cb({});
      });
}

template <clinux::version_t KernVer = clinux::version_code>
struct accept_with_address_impl {
  struct locals_t {
    acceptor &f_;
    address &endpoint_;

    buffer addr_buf{sizeof(clinux::sockaddr)};
    clinux::socklen_t addrlen_buf{sizeof(clinux::sockaddr)};

    locals_t(acceptor &f, address &endpoint) : f_(f), endpoint_(endpoint) {}
  };
  using ret_t = socket;
  using op_t = async_op<accept_with_address_impl>;

  static void run(op_t &op) noexcept {
    async_syscall::accept<async_context, KernVer>(
        op.ctx_, op.locals_->f_.get(),
        reinterpret_cast<clinux::sockaddr *>(op.locals_->addr_buf.data()),
        addressof(op.locals_->addrlen_buf), 0,
        op.yield_syscall(accept_with_address_impl::finish));
  }

  static void finish(op_t &op, ec_or<long> ret) noexcept {
    if (!ret) {
      op.complete(ret.ec());
    }
    op.locals_->endpoint_ =
        address(buffer(op.locals_->addr_buf.begin(),
                       op.locals_->addr_buf.begin() + op.locals_->addrlen_buf));
    op.complete(wrap_accepted_socket(static_cast<int>(ret.get())));
  }
};

template <clinux::version_t KernVer = clinux::version_code>
inline void accept(async_context &ctx, acceptor &srv, address &endpoint,
                   callback<socket> &&cb) noexcept {
  using impl_t = accept_with_address_impl<KernVer>;
  async_op<impl_t>(ctx, forward<callback<socket>>(cb),
                   make_unique<typename impl_t::locals_t>(srv, endpoint))
      .run();
}

template <clinux::version_t KernVer = clinux::version_code>
inline void accept(async_context &ctx, acceptor &srv,
                   callback<socket> &&cb) noexcept {
  async_syscall::accept<async_context, KernVer>(
      ctx, srv.get(), NULL, NULL, 0,
      [cb(forward<callback<socket>>(cb))](ec_or<long> ret) mutable {
        if (!ret)
          cb(ret.ec());
        cb(wrap_accepted_socket(static_cast<int>(ret.get())));
      });
}

} // namespace async
} // namespace tcp
} // namespace net
} // namespace iasr
