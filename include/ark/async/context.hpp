#pragma once

#include <ark/bindings.hpp>
#include <ark/clinux.hpp>

#include <ark/async/callback.hpp>
#include <ark/async/io_uring/async_syscall.hpp>
#include <ark/async/io_uring/context.hpp>

namespace ark {

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

} // namespace ark
