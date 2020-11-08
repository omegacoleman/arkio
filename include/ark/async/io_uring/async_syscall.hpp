#pragma once

#include <ark/bindings.hpp>
#include <ark/clinux.hpp>

#include <ark/async/callback.hpp>
#include <ark/async/io_uring/io_uring.hpp>
#include <ark/error/ec_or.hpp>

namespace ark {
namespace io_uring_async {
namespace syscall {
template <class UringContext>
inline ec_or<typename UringContext::token_t>
read(UringContext &ctx, int fd, void *buf, unsigned nbytes,
     clinux::off_t offset, syscall_callback_t &&cb) noexcept {
  return ctx.add_sqe(
      [&ctx, fd, buf, nbytes, offset](sqe_ref sqe) {
        sqe.prep_read(fd, buf, nbytes, offset);
      },
      forward<syscall_callback_t>(cb));
}

template <class UringContext>
inline ec_or<typename UringContext::token_t>
write(UringContext &ctx, int fd, const void *buf, unsigned nbytes,
      clinux::off_t offset, syscall_callback_t &&cb) noexcept {
  return ctx.add_sqe(
      [&ctx, fd, buf, nbytes, offset](sqe_ref sqe) {
        sqe.prep_write(fd, buf, nbytes, offset);
      },
      forward<syscall_callback_t>(cb));
}

template <class UringContext>
inline ec_or<typename UringContext::token_t>
connect(UringContext &ctx, int fd, const clinux::sockaddr *addr,
        clinux::socklen_t addrlen, syscall_callback_t &&cb) noexcept {
  return ctx.add_sqe([&ctx, fd, addr, addrlen](
                         sqe_ref sqe) { sqe.prep_connect(fd, addr, addrlen); },
                     forward<syscall_callback_t>(cb));
}

template <class UringContext>
inline ec_or<typename UringContext::token_t>
readv(UringContext &ctx, int fd, const clinux::iovec *iovecs, unsigned nr_vecs,
      clinux::off_t offset, syscall_callback_t &&cb) noexcept {
  return ctx.add_sqe(
      [&ctx, fd, iovecs, nr_vecs, offset](sqe_ref sqe) {
        sqe.prep_readv(fd, iovecs, nr_vecs, offset);
      },
      forward<syscall_callback_t>(cb));
}

template <class UringContext>
inline ec_or<typename UringContext::token_t>
writev(UringContext &ctx, int fd, const clinux::iovec *iovecs, unsigned nr_vecs,
       clinux::off_t offset, syscall_callback_t &&cb) noexcept {
  return ctx.add_sqe(
      [&ctx, fd, iovecs, nr_vecs, offset](sqe_ref sqe) {
        sqe.prep_writev(fd, iovecs, nr_vecs, offset);
      },
      forward<syscall_callback_t>(cb));
}

template <class UringContext>
inline ec_or<typename UringContext::token_t>
accept(UringContext &ctx, int fd, clinux::sockaddr *addr,
       clinux::socklen_t *addrlen, int flags,
       syscall_callback_t &&cb) noexcept {
  return ctx.add_sqe(
      [&ctx, fd, addr, addrlen, flags](sqe_ref sqe) {
        sqe.prep_accept(fd, addr, addrlen, flags);
      },
      forward<syscall_callback_t>(cb));
}

} // namespace syscall
} // namespace io_uring_async
} // namespace ark
