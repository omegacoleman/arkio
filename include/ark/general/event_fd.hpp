#pragma once

#include <ark/bindings.hpp>

#include <ark/async.hpp>
#include <ark/io.hpp>

namespace ark {

/*! \addtogroup general
 *  @{
 */

/*!
 * \brief wraps fildes created by eventfd(2) as an io object
 */
class event_fd : public fd {
protected:
  /*!
   * \brief constructs from int fildes
   *
   * \param[in] fd_int must be an fildes opened by eventfd(2)
   */
  event_fd(int fd_int) : fd(fd_int) {}

private:
  static result<event_fd> __create(async_context *ctx, unsigned int count,
                                   int flags) noexcept {
    int ret = clinux::eventfd(count, flags);
    if (ret == -1) {
      return errno_ec();
    }
    event_fd f{ret};
    f.set_async_context(ctx);
    return move(f);
  }

public:
  /*!
   * \brief construct and return an event_fd just like calling eventfd(2)
   */
  static result<event_fd> create(unsigned int count, int flags) noexcept {
    return __create(nullptr, count, flags);
  }

  /*!
   * \brief construct and return an event_fd just like calling eventfd(2), and
   * bind to the given \ref ::ark::async_context
   */
  static result<event_fd> create(async_context &ctx, unsigned int count,
                                 int flags) noexcept {
    return __create(&ctx, count, flags);
  }
};

/*! @} */

} // namespace ark
