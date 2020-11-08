#pragma once

#include <ark/bindings.hpp>
#include <ark/coro_bindings.hpp>

#include <ark/async/context.hpp>
#include <ark/coroutine/fire_and_forget.hpp>
#include <ark/coroutine/task.hpp>

namespace ark {

// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1056r0.html
// mentioned task varaint of std::async
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

} // namespace ark
