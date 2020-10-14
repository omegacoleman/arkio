#pragma once

#include <iasr/bindings.hpp>
#include <iasr/clinux.hpp>

#include <iasr/async/callback.hpp>
#include <iasr/async/io_uring/io_uring.hpp>
#include <iasr/error/ec_or.hpp>

namespace iasr {
namespace io_uring_async {
namespace syscall {
template <class UringContext, clinux::version_t KernVer = clinux::version_code>
inline ec_or<typename UringContext::token_t>
read(UringContext &ctx, int fd, void *buf, unsigned nbytes,
     clinux::off_t offset, syscall_callback_t &&cb) noexcept {
  static_assert(KernVer >= clinux::version(5, 6, 0),
                "this api is not avalible on lower kernels");
  return ctx.add_sqe(
      [&ctx, fd, buf, nbytes, offset](sqe_ref sqe) {
        sqe.prep_read(fd, buf, nbytes, offset);
      },
      forward<syscall_callback_t>(cb));
}

template <class UringContext, clinux::version_t KernVer = clinux::version_code>
inline ec_or<typename UringContext::token_t>
write(UringContext &ctx, int fd, const void *buf, unsigned nbytes,
      clinux::off_t offset, syscall_callback_t &&cb) noexcept {
  static_assert(KernVer >= clinux::version(5, 6, 0),
                "this api is not avalible on lower kernels");
  return ctx.add_sqe(
      [&ctx, fd, buf, nbytes, offset](sqe_ref sqe) {
        sqe.prep_write(fd, buf, nbytes, offset);
      },
      forward<syscall_callback_t>(cb));
}

template <class UringContext, clinux::version_t KernVer = clinux::version_code>
inline ec_or<typename UringContext::token_t>
connect(UringContext &ctx, int fd, const clinux::sockaddr *addr,
        clinux::socklen_t addrlen, syscall_callback_t &&cb) noexcept {
  static_assert(KernVer > clinux::version(5, 5, 0),
                "this api is not avalible on lower kernels");
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

template <class UringContext, clinux::version_t KernVer = clinux::version_code>
inline ec_or<typename UringContext::token_t>
accept(UringContext &ctx, int fd, clinux::sockaddr *addr,
       clinux::socklen_t *addrlen, int flags,
       syscall_callback_t &&cb) noexcept {
  static_assert(KernVer > clinux::version(5, 5, 0),
                "this api is not avalible on lower kernels");
  return ctx.add_sqe(
      [&ctx, fd, addr, addrlen, flags](sqe_ref sqe) {
        sqe.prep_accept(fd, addr, addrlen, flags);
      },
      forward<syscall_callback_t>(cb));
}

} // namespace syscall
} // namespace io_uring_async
} // namespace iasr
