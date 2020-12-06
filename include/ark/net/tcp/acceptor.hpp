#pragma once

#include <ark/bindings.hpp>

#include <ark/io/fd.hpp>

namespace ark {
namespace net {

/*! \addtogroup net
 *  @{
 */

namespace tcp {

/*!
 * \brief special io object available to bond, listen, and accept \ref
 * ark::net::tcp::socket from
 */
class acceptor : public nonseekable_fd {
protected:
  /*!
   * \brief constructs from int fildes
   *
   * \param[in] fd_int must be an fildes opened by socket(2)
   */
  acceptor(int fd_int) : nonseekable_fd(fd_int) {}

private:
  static result<acceptor> __create(async_context *ctx,
                                   bool use_ipv6 = false) noexcept {
    int ret = clinux::socket(use_ipv6 ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
    if (ret == -1) {
      return clinux::errno_ec();
    }
    acceptor ret_fd(ret);
    ret_fd.set_async_context(ctx);
    return move(ret_fd);
  }

public:
  /*!
   * \brief constructs an acceptor
   *
   * \param[in] use_ipv6 if set to true, associated bond and accept calls should
   * use \ref ark::net::inet6_address
   */
  static result<acceptor> create(bool use_ipv6 = false) noexcept {
    return __create(nullptr, use_ipv6);
  }

  /*!
   * \brief constructs an acceptor
   *
   * \param[in] ctx bound to this \ref ark::async_context in addition, notice
   * that it would also be bound for accepted sockets \param[in] use_ipv6 if set
   * to true, associated bond and accept calls should use \ref
   * ark::net::inet6_address
   */
  static result<acceptor> create(async_context &ctx,
                                 bool use_ipv6 = false) noexcept {
    return __create(&ctx, use_ipv6);
  }
};
} // namespace tcp

/*! @} */

} // namespace net
} // namespace ark
