#pragma once

#include <ark/bindings.hpp>

#include <ark/io/fd.hpp>

namespace ark {
namespace net {

/*! \addtogroup net
 *  @{
 */

namespace tcp {

/*! \cond SOCKET_WRAP_INTERNALS */

class socket;

inline socket wrap_accepted_socket(async_context *ctx, int fd) noexcept;

/*! \endcond */

/*!
 * \brief denotes a tcp socket
 *
 * will be available for io after a successful connect operation, sync or async,
 * or if the socket is retrieved by accepting from an \ref
 * ::ark::net::tcp::acceptor
 */
class socket : public fd {
protected:
  /*!
   * \brief constructs from int fildes
   *
   * \param[in] fd_int must be an fildes opened by socket(2)
   */
  socket(int fd_int) : fd(fd_int) {}

private:
  static result<socket> __create(async_context *ctx,
                                 bool use_ipv6 = false) noexcept {
    int ret = clinux::socket(use_ipv6 ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
    if (ret == -1) {
      return errno_ec();
    }
    socket ret_fd(ret);
    ret_fd.set_async_context(ctx);
    return move(ret_fd);
  }

public:
  /*! \cond SOCKET_WRAP_INTERNALS */
  friend inline socket wrap_accepted_socket(async_context *ctx,
                                            int fd) noexcept;
  /*! \endcond */

  /*!
   * \brief constructs a socket available for connecting
   *
   * \param[in] use_ipv6 if set to true, associated connect should use \ref
   * ark::net::inet6_address
   */
  static result<socket> create(bool use_ipv6 = false) noexcept {
    return __create(nullptr, use_ipv6);
  }

  /*!
   * \brief constructs a socket available for connecting
   *
   * \param[in] ctx bound to this \ref ark::async_context in addition, notice
   * that it would also be bound for accepted sockets \param[in] use_ipv6 if set
   * to true, associated connect should use \ref ark::net::inet6_address
   */
  static result<socket> create(async_context &ctx,
                               bool use_ipv6 = false) noexcept {
    return __create(&ctx, use_ipv6);
  }
};

/*! \cond SOCKET_WRAP_INTERNALS */

inline socket wrap_accepted_socket(async_context *ctx, int fd) noexcept {
  socket ret(fd);
  ret.set_async_context(ctx);
  return move(ret);
}

/*! \endcond */
} // namespace tcp

/*! @} */

} // namespace net
} // namespace ark
