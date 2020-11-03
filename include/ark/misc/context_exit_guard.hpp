#pragma once

#include <ark/async/context.hpp>
#include <ark/bindings.hpp>

namespace ark {

class context_exit_guard {
private:
  async_context &ctx_;

public:
  context_exit_guard(async_context &ctx) : ctx_(ctx) {}

  ~context_exit_guard() { ctx_.exit(); }
};

} // namespace ark
