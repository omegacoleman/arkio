#pragma once

#include <ark/async/context.hpp>
#include <ark/bindings.hpp>

namespace ark {

/*! \addtogroup misc
 *  @{
 */

/*!
 * \brief RAII-style helper for async_context.exit()
 *
 * call async_context.exit() on destruct, often used in coroutine main function
 *
 * could use with co_async to accomplish something like well-known sync_wait for
 * awaitables in coroutines ts
 */
class context_exit_guard {
private:
  async_context &ctx_;

public:
  context_exit_guard(async_context &ctx) : ctx_(ctx) {}

  ~context_exit_guard() { ctx_.exit(); }
};

/*! @} */

} // namespace ark
