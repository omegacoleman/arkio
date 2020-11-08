#pragma once

#include <ark/bindings.hpp>
#include <ark/coro_bindings.hpp>

#include <ark/misc/manual_lifetime.hpp>

namespace ark {

template <class T> class task;

template <class T> class task_promise {
public:
  task_promise() noexcept {}

  ~task_promise() { clear(); }

  task<T> get_return_object() noexcept;

  suspend_always initial_suspend() { return {}; }

  auto final_suspend() noexcept {
    struct awaiter {
      bool await_ready() noexcept { return false; }
      auto await_suspend(coroutine_handle<task_promise> h) noexcept {
        return h.promise().continuation_;
      }
      void await_resume() noexcept {}
    };
    return awaiter{};
  }

  template <class U, enable_if_t<is_convertible_v<U, T>, int> = 0>
  void return_value(U &&value) noexcept {
    clear();
    value_.construct((U &&) value);
    state_ = state_t::value;
  }

  void unhandled_exception() noexcept { terminate(); }

  T get() noexcept { return move(value_).get(); }

private:
  friend class task<T>;

  void clear() noexcept {
    switch (exchange(state_, state_t::empty)) {
    case state_t::empty:
      break;
    case state_t::value:
      value_.destruct();
      break;
    }
  }

  coroutine_handle<void> continuation_;
  enum class state_t { empty, value };
  state_t state_ = state_t::empty;
  union {
    manual_lifetime<T> value_;
  };
};

template <> class task_promise<void> {
public:
  task_promise() noexcept {}

  ~task_promise() { clear(); }

  task<void> get_return_object() noexcept;

  suspend_always initial_suspend() noexcept { return {}; }

  auto final_suspend() noexcept {
    struct awaiter {
      bool await_ready() { return false; }
      auto await_suspend(coroutine_handle<task_promise> h) {
        return h.promise().continuation_;
      }
      void await_resume() {}
    };
    return awaiter{};
  }

  void return_void() noexcept {
    clear();
    value_.construct();
    state_ = state_t::value;
  }

  void unhandled_exception() noexcept { terminate(); }

  void get() noexcept {}

private:
  friend class task<void>;

  void clear() noexcept {
    switch (exchange(state_, state_t::empty)) {
    case state_t::empty:
      break;
    case state_t::value:
      value_.destruct();
      break;
    }
  }

  enum class state_t { empty, value };

  coroutine_handle<void> continuation_;
  state_t state_ = state_t::empty;
  union {
    manual_lifetime<void> value_;
  };
};

template <class T> class task {
public:
  using promise_type = task_promise<T>;
  using handle_t = coroutine_handle<promise_type>;

  explicit task(handle_t h) noexcept : coro_(h) {}

  task(task &&t) noexcept : coro_(exchange(t.coro_, {})) {}

  ~task() {
    if (coro_) {
      coro_.destroy();
    }
  }

  auto operator co_await() &&noexcept {
    struct awaiter {
    public:
      explicit awaiter(handle_t coro) : coro_(coro) {}
      bool await_ready() noexcept { return false; }
      auto await_suspend(coroutine_handle<void> h) noexcept {
        coro_.promise().continuation_ = h;
        return coro_;
      }
      T await_resume() { return coro_.promise().get(); }

    private:
      handle_t coro_;
    };
    return awaiter(coro_);
  }

private:
  handle_t coro_;
};

template <class T> task<T> task_promise<T>::get_return_object() noexcept {
  return task<T>(coroutine_handle<task_promise<T>>::from_promise(*this));
}

inline task<void> task_promise<void>::get_return_object() noexcept {
  return task<void>(coroutine_handle<task_promise<void>>::from_promise(*this));
}

} // namespace ark
