#pragma once

#include <ark/bindings.hpp>

#include <ark/async/callback.hpp>
#include <ark/async/io_uring/async_syscall.hpp>
#include <ark/async/io_uring/context.hpp>

namespace ark {

/*! \addtogroup async
 *  @{
 */

#ifdef USING_DOXYGEN

/*!
 * \brief context for running async functions
 *
 * The async_context could be bound to io objects, often as their first argument
 * to static construct functions. Then if async io operations got completed, the
 * callbacks would get invoked inside run() of the bound context.
 *
 * Currently, only single-threaded context run() invocation is supported,
 * however, calling its functions from other thread or submitting io operations
 * is supported.
 */
class async_context {
public:
  /*!
   * \brief constructor
   *
   * the default constructed async_context is in an unusable state, you'll have
   * to call init() before doing anything
   */
  async_context() noexcept;

  /*!
   * \brief init the context
   */
  result<void> init() noexcept;

  /*!
   * \brief start the event loop
   *
   * start the event loop on current thread, continuously wait for and process
   * events, and invoke callbacks
   */
  result<void> run() noexcept;

  /*!
   * \brief let run() successfully return
   */
  void exit() noexcept;

  /*!
   * \brief let run() return with given result
   */
  void exit(result<void> ret);
};

#else

using async_context = io_uring_async::singlethread_uring_async_context;

namespace async_syscall = io_uring_async::syscall;

#endif

/*!
 * \brief able to bound to an async_context
 *
 * io objects inherited from this class is able to store a nullable reference to
 * an async_context, then the context will be used for async io operations.
 */
class with_async_context {
private:
  async_context *ctx_{nullptr};

public:
  /*!
   * \brief retrieves the context
   *
   * \pre a not-null async_context must have been bound to this object using
   * \ref set_async_context
   */
  async_context &context() const noexcept {
    Ensures(ctx_ != nullptr);
    return *ctx_;
  }

protected:
  /*!
   * \brief bound this io object to an async_context
   *
   * \pre must get called before any async operation has started, and must not
   * get called after that
   */
  void set_async_context(async_context *ctx) noexcept { ctx_ = ctx; }
};

/*! @} */

} // namespace ark
