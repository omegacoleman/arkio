#pragma once

#include <ark/bindings.hpp>

#include <ark/async.hpp>
#include <ark/io.hpp>

namespace ark {

/*! \addtogroup general
 *  @{
 */

/*!
 * \brief wraps fildes created by pipe2(2) as an io object
 */
class pipe_fd : public nonseekable_fd {
protected:
  /*!
   * \brief constructs from int fildes
   *
   * \param[in] fd_int must be an fildes opened by pipe2(2)
   */
  pipe_fd(int fd_int) : nonseekable_fd(fd_int) {}

private:
  static result<pair<pipe_fd, pipe_fd>> __create(async_context *ctx) noexcept {
    int pipefd[2];
    int ret = clinux::pipe2(pipefd, 0);
    if (ret == -1) {
      return clinux::errno_ec();
    }
    pipe_fd f_in{pipefd[0]};
    pipe_fd f_out{pipefd[1]};
    f_in.set_async_context(ctx);
    f_out.set_async_context(ctx);
    return move(make_pair(move(f_in), move(f_out)));
  }

public:
  /*!
   * \brief construct a std::pair of pipe_fd just like calling pipe2(2)
   *
   * on success, returns the {in, out} end of the pipe as a pair
   */
  static result<pair<pipe_fd, pipe_fd>> create() noexcept {
    return __create(nullptr);
  }

  /*!
   * \brief construct a std::pair of pipe_fd just like calling pipe2(2), and
   * bind both of them to the given \ref ::ark::async_context
   *
   * on success, returns the {in, out} end of the pipe as a pair
   */
  static result<pair<pipe_fd, pipe_fd>> create(async_context &ctx) noexcept {
    return __create(&ctx);
  }
};

/*! @} */

} // namespace ark
