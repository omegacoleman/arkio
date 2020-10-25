#pragma once

#include <iasr/bindings.hpp>
#include <iasr/clinux.hpp>

#include <iasr/async/callback.hpp>
#include <iasr/async/io_uring/async_syscall.hpp>
#include <iasr/async/io_uring/context.hpp>
#include <iasr/error/ec_or.hpp>

namespace iasr {

using async_context = io_uring_async::singlethread_uring_async_context;

namespace async_syscall = io_uring_async::syscall;

class with_async_context {
  using context_type = async_context;

private:
  context_type *ctx_{nullptr};

public:
  context_type &context() const noexcept {
    Ensures(ctx_ != nullptr);
    return *ctx_;
  }

protected:
  void set_async_context(context_type *ctx) noexcept { ctx_ = ctx; }
};

} // namespace iasr
