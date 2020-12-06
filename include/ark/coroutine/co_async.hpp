#pragma once

#include <ark/bindings.hpp>

#include <ark/async/context.hpp>
#include <ark/coroutine/fire_and_forget.hpp>
#include <ark/coroutine/task.hpp>

namespace ark {

/*! \addtogroup coroutine
 *  @{
 */

/*!
 * \brief start the task as another coroutine task series.
 *
 * the caller lost ownership of the task once it is started, so discarding the
 * returned future does not abort the task from running.
 *
 * \remark as defined in p1056r0 (with a different name), see \ref info_coro
 */
template <typename T> inline future<T> co_async(task<T> tsk) noexcept {
  promise<T> prom_;
  auto fut = prom_.get_future();
  ([](promise<T> prom_, task<T> tsk) mutable -> fire_and_forget {
    if constexpr (is_void_v<T>) {
      co_await move(tsk);
      prom_.set_value();
    } else {
      prom_.set_value(co_await move(tsk));
    }
  })(move(prom_), move(tsk));
  return fut;
}

/*! @} */

} // namespace ark
