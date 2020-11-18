#pragma once

#include <ark/bindings.hpp>

#include <ark/async/async_op.hpp>
#include <ark/async/context.hpp>
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
 * \brief contains apis that invokes a given \ref ::ark::callback on completion
 */
namespace async {

/*!
 * \brief connect socket to the given endpoint
 *
 * returns instantly, cb is invoked on completion or error.
 */
inline void connect(socket &f, const address &endpoint,
                    callback<result<void>> &&cb) noexcept {
  auto ret = async_syscall::connect(
      f.context(), f.get(), endpoint.sa_ptr(), endpoint.sa_len(),
      [cb(forward<callback<result<void>>>(cb))](result<long> ret) mutable {
        if (!ret)
          cb(ret.error());
        cb(success());
      });
  if (ret.has_error())
    cb(ret.as_failure());
}

/*! \cond HIDDEN_CLASSES */

struct accept_with_address_impl {
  struct locals_t {
    acceptor &f_;
    address &endpoint_;

    clinux::socklen_t addrlen_buf{sizeof(clinux::sockaddr)};

    locals_t(acceptor &f, address &endpoint) : f_(f), endpoint_(endpoint) {}
  };
  using ret_t = result<socket>;
  using op_t = async_op<accept_with_address_impl>;

  static void run(op_t &op) noexcept {
    auto ret = async_syscall::accept(
        op.ctx_, op.locals_->f_.get(), op.locals_->endpoint_.sa_ptr(),
        addressof(op.locals_->addrlen_buf), 0,
        op.yield_syscall(accept_with_address_impl::finish));
    if (ret.has_error())
      return op.complete(ret.as_failure());
  }

  static void finish(op_t &op, result<long> ret) noexcept {
    if (!ret) {
      op.complete(ret.error());
    }
    op.complete(wrap_accepted_socket(&op.ctx_, static_cast<int>(ret.value())));
  }
};

/*! \endcond */

/*!
 * \brief accept a socket connection from the given acceptor
 *
 * returns instantly, cb is invoked on completion or error.
 *
 * \param[out] endpoint the address of accepted socket, on success
 */
inline void accept(acceptor &srv, address &endpoint,
                   callback<result<socket>> &&cb) noexcept {
  using impl_t = accept_with_address_impl;
  async_op<impl_t>(srv.context(), forward<callback<result<socket>>>(cb),
                   make_unique<typename impl_t::locals_t>(srv, endpoint))
      .run();
}

/*!
 * \brief accept a socket connection from the given acceptor
 *
 * returns instantly, cb is invoked on completion or error.
 */
inline void accept(acceptor &srv, callback<result<socket>> &&cb) noexcept {
  auto ret = async_syscall::accept(
      srv.context(), srv.get(), NULL, NULL, 0,
      [&ctx(srv.context()),
       cb(forward<callback<result<socket>>>(cb))](result<long> ret) mutable {
        if (!ret)
          cb(ret.error());
        cb(wrap_accepted_socket(&ctx, static_cast<int>(ret.value())));
      });
  if (ret.has_error())
    cb(ret.as_failure());
}

} // namespace async
} // namespace tcp

/*! @} */

} // namespace net
} // namespace ark
