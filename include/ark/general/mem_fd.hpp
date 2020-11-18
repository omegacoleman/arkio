#pragma once

#include <ark/bindings.hpp>

#include <ark/async.hpp>
#include <ark/io.hpp>

namespace ark {

/*! \addtogroup general
 *  @{
 */

/*!
 * \brief wraps fildes created by memfd_create(2) as an io object
 */
class mem_fd : public seekable_fd {
protected:
  /*!
   * \brief constructs from int fildes
   *
   * \param[in] fd_int must be an fildes opened by memfd_create(2)
   */
  mem_fd(int fd_int) : seekable_fd(fd_int) {}

private:
  static result<mem_fd> __create(async_context *ctx, const string &name,
                                 int flags) noexcept {
    int ret = clinux::memfd_create(name.c_str(), flags);
    if (ret == -1) {
      return clinux::errno_ec();
    }
    mem_fd f{ret};
    f.set_async_context(ctx);
    return move(f);
  }

public:
  /*!
   * \brief construct and return an event_fd just like calling memfd_create(2)
   */
  static result<mem_fd> create(const string &name, int flags) noexcept {
    return __create(nullptr, name, flags);
  }

  /*!
   * \brief construct and return an event_fd just like calling memfd_create(2),
   * and bind to the given \ref ::ark::async_context
   */
  static result<mem_fd> create(async_context &ctx, const string &name,
                               int flags) noexcept {
    return __create(&ctx, name, flags);
  }
};

/*! @} */

} // namespace ark
