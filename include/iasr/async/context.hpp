#pragma once

#include <iasr/bindings.hpp>
#include <iasr/clinux.hpp>

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 1, 0)
#error "your kernel version is below 5.1, the async interface would not work"
#endif

#include <iasr/async/callback.hpp>
#include <iasr/async/io_uring/async_syscall.hpp>
#include <iasr/async/io_uring/context.hpp>
#include <iasr/error/ec_or.hpp>

namespace iasr {
using async_context = io_uring_async::singlethread_uring_async_context;

namespace async_syscall = io_uring_async::syscall;
} // namespace iasr
