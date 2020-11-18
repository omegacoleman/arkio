#pragma once

/*! \cond FILE_NOT_DOCUMENTED */

#include <ark/bindings.hpp>

#include <ark/async/context.hpp>

namespace ark {

template <typename Impl> class async_op {
public:
  using locals_t = typename Impl::locals_t;
  using ret_t = typename Impl::ret_t;
  using callback_t = callback<ret_t>;

private:
  callback_t cb_;

public:
  async_context &ctx_;
  unique_ptr<locals_t> locals_;

  async_op(async_context &ctx, callback_t &&cb,
           unique_ptr<locals_t> locals) noexcept
      : ctx_(ctx), cb_(forward<callback_t>(cb)), locals_(move(locals)) {}

  template <typename CallRet>
  callback<CallRet>
  yield(unique_function<void(async_op &op, CallRet ret)> next) noexcept {
    return [moved_this(move(*this)), next(move(next))](CallRet ret) mutable {
      next(moved_this, ret);
    };
  }

  syscall_callback_t yield_syscall(
      unique_function<void(async_op &op, syscall_callback_t::ret_type ret)>
          next) noexcept {
    return yield<syscall_callback_t::ret_type>(move(next));
  }

  void run() noexcept { Impl::run(*this); }

  template <typename Ret = ret_t, typename T = enable_if_t<!is_void_v<Ret>>>
  void complete(Ret ret) noexcept {
    cb_(move(ret));
  }

  template <typename Ret = ret_t, typename T = enable_if_t<is_void_v<Ret>>>
  void complete() noexcept {
    cb_();
  }
};

} // namespace ark

/*! \endcond */
