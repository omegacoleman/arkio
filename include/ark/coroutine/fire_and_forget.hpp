#pragma once

#include <ark/bindings.hpp>
#include <ark/coro_bindings.hpp>

#include <ark/misc/manual_lifetime.hpp>

namespace ark {

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
