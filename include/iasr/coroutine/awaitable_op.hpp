#pragma once

#include <iasr/bindings.hpp>
#include <iasr/coro_bindings.hpp>

#include <iasr/async/callback.hpp>

namespace iasr {

template <typename Ret> struct awaitable_op {
private:
  optional<Ret> ret_{};

public:
  virtual void invoke(callback<Ret> &&cb) noexcept = 0;

  bool await_ready() noexcept { return ret_.has_value(); }

  void await_suspend(coroutine_handle<void> ch) noexcept {
    invoke([ch, this](Ret ret) mutable {
      this->ret_.emplace(move(ret));
      ch.resume();
    });
  }

  auto await_resume() noexcept { return *move(ret_); }
};

template <> struct awaitable_op<void> {
private:
  bool ready{false};

public:
  virtual void invoke(callback<void> &&cb) noexcept = 0;

  bool await_ready() noexcept { return ready; }

  void await_suspend(coroutine_handle<void> ch) noexcept {
    invoke([ch, this]() mutable {
      ready = true;
      ch.resume();
    });
  }

  void await_resume() noexcept {}
};

} // namespace iasr
