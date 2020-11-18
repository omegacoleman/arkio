#pragma once

/*! \cond FILE_NOT_DOCUMENTED */

#include <ark/bindings.hpp>

namespace ark {

/*!
 * \brief coroutine functions returning this type is internally cut-off with the
 * caller.
 *
 * Even if the caller discard the returning value, the undelying coroutine won't
 * get halted.
 *
 * Not exposed in concerns the caller would be 'too forgetful', use co_async
 * instead.
 */
class fire_and_forget {
public:
  class promise_type {
  public:
    fire_and_forget get_return_object() noexcept { return {}; }

    auto initial_suspend() noexcept { return suspend_never{}; }

    auto final_suspend() noexcept { return suspend_never{}; }

    void return_void() noexcept {}

    void unhandled_exception() noexcept { terminate(); }

    void get() noexcept {}
  };
};

} // namespace ark

/*! \endcond */
