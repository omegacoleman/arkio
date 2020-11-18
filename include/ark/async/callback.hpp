#pragma once

#include <ark/bindings.hpp>

namespace ark {

/*! \addtogroup async
 *  @{
 */

/*!
 * \brief a movable noncopyable unique_function invoked to signal completion
 *
 * arkio callbacks are function2::unique_function, which is just like
 * std::function but movable and non-copyable. So, it is safe to move capture
 * values, and use the async io operation to prolong their lifetime inside
 * async_context.
 *
 * ahout function2 : https://github.com/Naios/function2
 * the document for unique_function is inside readme
 *
 * \tparam RetType if void, the signature of callback function is void(),
 * otherwise void(RetType ret)
 */
template <typename RetType>
struct callback : public unique_function<void(RetType ret)> {
  using unique_function<void(RetType ret)>::unique_function;
  using ret_type = RetType;
};

/*!
 * \copydoc callback
 */
template <> struct callback<void> : public unique_function<void()> {
  using unique_function<void()>::unique_function;
  using ret_type = void;
};

/*! \cond INTERNAL_CLASSES */

using syscall_callback_t = callback<result<long>>;

/*! \endcond */

/*! @} */

} // namespace ark
