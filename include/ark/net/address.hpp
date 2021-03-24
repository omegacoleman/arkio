#pragma once

#include <ark/bindings.hpp>

namespace ark {
namespace net {

/*! \addtogroup net
 *  @{
 */

/*!
 * \brief a network address, of unspecified address family
 */
class address {
protected:
  clinux::sockaddr_storage sa_;

public:
  /*!
   * \brief create an address object, able to hold a network address
   *
   * sets sa_family to AF_UNSPEC
   */
  address() { sa_ptr()->sa_family = AF_UNSPEC; }

  /*!
   * \brief returns the address family
   */
  clinux::sa_family_t sa_family() const noexcept { return sa_ptr()->sa_family; }

  /*!
   * \brief returns a pointer to the underlying sockaddr
   */
  clinux::sockaddr *sa_ptr() noexcept {
    return reinterpret_cast<clinux::sockaddr *>(addressof(sa_));
  }

  /*!
   * \brief returns a pointer to the underlying sockaddr
   */
  const clinux::sockaddr *sa_ptr() const noexcept {
    return reinterpret_cast<const clinux::sockaddr *>(addressof(sa_));
  }

  /*!
   * \brief always return size of sockaddr_storage
   */
  clinux::socklen_t sa_len() const noexcept { return sizeof(sa_); }
};

/*!
 * \brief Provides common members and types for network address of specific
 * address family
 */
template <class SockAddr, clinux::sa_family_t AddrFamily>
class address_with_family : public address {
public:
  using size_type = clinux::socklen_t;
  static const clinux::sa_family_t address_family = AddrFamily;
  static const clinux::socklen_t size = sizeof(SockAddr);
  using sockaddr_ptr_t = add_pointer_t<SockAddr>;
  using const_sockaddr_ptr_t = add_pointer_t<add_const_t<SockAddr>>;

  address_with_family() : address() {
    address::sa_ptr()->sa_family = address_family;
  }
  address_with_family(const address &addr) : address(addr) {
    Expects(address::sa_ptr()->sa_family == address_family);
  }

  /*!
   * \brief returns a pointer to the underlying sockaddr
   */
  sockaddr_ptr_t sa_ptr() noexcept {
    return reinterpret_cast<sockaddr_ptr_t>(address::sa_ptr());
  }

  /*!
   * \brief returns a pointer to the underlying sockaddr
   */
  const_sockaddr_ptr_t sa_ptr() const noexcept {
    return reinterpret_cast<const_sockaddr_ptr_t>(address::sa_ptr());
  }

  /*!
   * \brief returns size of the corresponding sockaddr structure
   */
  size_type sa_len() const noexcept { return size; }

  /*!
   * \brief erase specific address family bound type info, upcast to an \ref
   * ::ark::net::address
   */
  address to_address() noexcept { return *(this); }
};

/*!
 * \brief denotes an IPV4 network address
 */
class inet_address : public address_with_family<clinux::sockaddr_in, AF_INET> {
private:
  using base_type = address_with_family<clinux::sockaddr_in, AF_INET>;

public:
  using base_type::address_family;
  using base_type::base_type;
  using base_type::const_sockaddr_ptr_t;
  using base_type::size;
  using base_type::size_type;
  using base_type::sockaddr_ptr_t;

  /*!
   * \brief returns the string representation of the host
   *
   * error if the host is invalid
   */
  result<string> host() const noexcept {
    char buff[16];
    const char *s = clinux::inet_ntop(AF_INET, &(base_type::sa_ptr()->sin_addr),
                                      buff, sizeof(buff));
    if (s == nullptr) {
      return errno_ec();
    }
    return {s};
  }

  /*!
   * \brief sets the host
   *
   * error if the given string is invalid
   *
   * \param[in] host_s ipv4 string representation, like '127.0.0.1'
   */
  result<void> host(string host_s) noexcept {
    int ret = clinux::inet_pton(address_family, host_s.c_str(),
                                &(base_type::sa_ptr()->sin_addr));
    if (ret <= 0)
      return as_ec(EINVAL);
    return success();
  }

  /*!
   * \brief gets the port
   */
  unsigned short port() const noexcept {
    return clinux::ntohs(base_type::sa_ptr()->sin_port);
  }

  /*!
   * \brief sets the port
   */
  void port(unsigned short p) noexcept {
    base_type::sa_ptr()->sin_port = clinux::htons(p);
  }

  /*!
   * \brief convert address to string
   *
   * format is 'host:port', like '127.0.0.1:8080'
   *
   * error if the address is invalid
   */
  result<string> to_string() const noexcept {
    ostringstream oss;
    OUTCOME_TRY(host_s, host());
    oss << host_s << ":" << port();
    return oss.str();
  }

  /*!
   * \brief downcast from an address
   *
   * error if the address family does not match
   */
  static result<inet_address> from_address(address addr) noexcept {
    if (addr.sa_ptr()->sa_family != address_family)
      return as_ec(EAFNOSUPPORT);
    return inet_address{addr};
  }
};

/*!
 * \brief denotes an IPV6 network address
 */
class inet6_address
    : public address_with_family<clinux::sockaddr_in6, AF_INET6> {
private:
  using base_type = address_with_family<clinux::sockaddr_in6, AF_INET6>;

public:
  using base_type::address_family;
  using base_type::const_sockaddr_ptr_t;
  using base_type::size;
  using base_type::size_type;
  using base_type::sockaddr_ptr_t;

  /*!
   * \brief returns the string representation of the host
   *
   * error if the host is invalid
   */
  result<string> host() const noexcept {
    char buff[64];
    const char *s = clinux::inet_ntop(
        AF_INET6, &(base_type::sa_ptr()->sin6_addr), buff, sizeof(buff));
    if (s == nullptr) {
      return errno_ec();
    }
    return {s};
  }

  /*!
   * \brief sets the host
   *
   * error if the given string is invalid
   *
   * \param[in] host_s ipv6 string representation, like '::1'
   */
  result<void> host(string host_s) noexcept {
    int ret = clinux::inet_pton(address_family, host_s.c_str(),
                                &(base_type::sa_ptr()->sin6_addr));
    if (ret <= 0)
      return as_ec(EINVAL);
    return success();
  }

  /*!
   * \brief gets the port
   */
  unsigned short port() const noexcept {
    return clinux::ntohs(base_type::sa_ptr()->sin6_port);
  }

  /*!
   * \brief sets the port
   */
  void port(unsigned short p) noexcept {
    base_type::sa_ptr()->sin6_port = clinux::htons(p);
  }

  /*!
   * \brief convert address to string
   *
   * format is '[host]:port', like '[::1]:8080'
   *
   * error if the address is invalid
   */
  result<string> to_string() const noexcept {
    ostringstream oss;
    OUTCOME_TRY(host_s, host());
    oss << "[" << host_s << "]:" << port();
    return oss.str();
  }

  /*!
   * \brief downcast from an address
   *
   * error if the address family does not match
   */
  static result<inet6_address> from_address(address addr) noexcept {
    if (addr.sa_ptr()->sa_family != address_family)
      return as_ec(EAFNOSUPPORT);
    return inet6_address{addr};
  }
};

/*!
 * \brief ADL enabled to_string visitor of \ref ::ark::net::address
 *
 * convert the address to its string representation
 *
 * error if the address family is unsupported, or the address is invalid
 */
result<string> to_string(const address &addr) {
  if (addr.sa_family() == AF_INET) {
    OUTCOME_TRY(ret, inet_address::from_address(addr));
    return ret.to_string();
  } else if (addr.sa_family() == AF_INET6) {
    OUTCOME_TRY(ret, inet6_address::from_address(addr));
    return ret.to_string();
  }
  return as_ec(EAFNOSUPPORT);
}

/*! @} */

} // namespace net
} // namespace ark
