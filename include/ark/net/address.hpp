#pragma once

#include <ark/bindings.hpp>
#include <ark/clinux.hpp>

namespace ark {
namespace net {

class in_address_impl {
public:
  static const clinux::sa_family_t address_family = AF_INET;
  static const clinux::socklen_t size = sizeof(clinux::sockaddr_in);
  using sockaddr_ptr_t = clinux::sockaddr_in *;
  using const_sockaddr_ptr_t = const clinux::sockaddr_in *;

  static result<string> host(const_sockaddr_ptr_t sa) noexcept {
    char buff[16];
    const char *s =
        clinux::inet_ntop(AF_INET, &sa->sin_addr, buff, sizeof(buff));
    if (s == nullptr) {
      return clinux::errno_ec();
    }
    return {s};
  }

  static result<void> host(sockaddr_ptr_t sa, string host_s) noexcept {
    int ret = clinux::inet_pton(address_family, host_s.c_str(), &sa->sin_addr);
    if (ret <= 0)
      return error_code(EINVAL, system_category());
    return success();
  }

  static unsigned short port(const_sockaddr_ptr_t sa) noexcept {
    return clinux::ntohs(sa->sin_port);
  }

  static void port(sockaddr_ptr_t sa, unsigned short p) noexcept {
    sa->sin_port = clinux::htons(p);
  }

  static result<string> to_string(const_sockaddr_ptr_t sa) noexcept {
    ostringstream oss;
    OUTCOME_TRY(host_s, host(sa));
    oss << host_s << ":" << port(sa);
    return oss.str();
  }
};

class in6_address_impl {
public:
  static const clinux::sa_family_t address_family = AF_INET6;
  static const clinux::socklen_t size = sizeof(clinux::sockaddr_in6);
  using sockaddr_ptr_t = clinux::sockaddr_in6 *;
  using const_sockaddr_ptr_t = const clinux::sockaddr_in6 *;

  static result<string> host(const_sockaddr_ptr_t sa) noexcept {
    char buff[64];
    const char *s =
        clinux::inet_ntop(AF_INET6, &sa->sin6_addr, buff, sizeof(buff));
    if (s == nullptr) {
      return clinux::errno_ec();
    }
    return {s};
  }

  static result<void> host(sockaddr_ptr_t sa, string host_s) noexcept {
    int ret = clinux::inet_pton(address_family, host_s.c_str(), &sa->sin6_addr);
    if (ret <= 0)
      return error_code(EINVAL, system_category());
    return success();
  }

  static unsigned short port(const_sockaddr_ptr_t sa) noexcept {
    return clinux::ntohs(sa->sin6_port);
  }

  static void port(sockaddr_ptr_t sa, unsigned short p) noexcept {
    sa->sin6_port = clinux::htons(p);
  }

  static result<string> to_string(const_sockaddr_ptr_t sa) noexcept {
    ostringstream oss;
    OUTCOME_TRY(host_s, host(sa));
    oss << "[" << host_s << "]:" << port(sa);
    return oss.str();
  }
};

template <class Impl> class address_with_family;

class address {
protected:
  clinux::sockaddr_storage sa_;

public:
  address() { sa_ptr()->sa_family = AF_UNSPEC; }

  template <class Impl> friend class address_with_family;

  clinux::sa_family_t sa_family() const noexcept { return sa_ptr()->sa_family; }
  clinux::sockaddr *sa_ptr() noexcept {
    return reinterpret_cast<clinux::sockaddr *>(addressof(sa_));
  }
  const clinux::sockaddr *sa_ptr() const noexcept {
    return reinterpret_cast<const clinux::sockaddr *>(addressof(sa_));
  }
  clinux::socklen_t sa_len() const noexcept { return sizeof(sa_); }
};

template <class Impl> class address_with_family : public address {
public:
  using size_type = clinux::socklen_t;

  address_with_family() : address() {
    address::sa_ptr()->sa_family = Impl::address_family;
  }
  address_with_family(const address &addr) : address(addr) {
    Expects(address::sa_ptr()->sa_family == Impl::address_family);
  }

  typename Impl::sockaddr_ptr_t sa_ptr() noexcept {
    return reinterpret_cast<typename Impl::sockaddr_ptr_t>(address::sa_ptr());
  }
  typename Impl::const_sockaddr_ptr_t sa_ptr() const noexcept {
    return reinterpret_cast<typename Impl::const_sockaddr_ptr_t>(
        address::sa_ptr());
  }
  size_type sa_len() const noexcept { return Impl::size; }

  address to_address() noexcept { return *(this); }

  static result<address_with_family<Impl>> from_address(address addr) noexcept {
    if (addr.sa_ptr()->sa_family != Impl::address_family)
      return error_code(EAFNOSUPPORT, system_category());
    return address_with_family<Impl>{addr};
  }

  result<string> host() const noexcept { return Impl::host(sa_ptr()); }

  result<void> host(string host_s) noexcept {
    return Impl::host(sa_ptr(), move(host_s));
  }

  unsigned short port() const noexcept { return Impl::port(sa_ptr()); }

  void port(unsigned short p) noexcept { Impl::port(sa_ptr(), p); }

  result<string> to_string() const noexcept {
    return Impl::to_string(sa_ptr());
  }
};

using inet_address = address_with_family<in_address_impl>;
using inet6_address = address_with_family<in6_address_impl>;

result<string> to_string(const address &addr) {
  if (addr.sa_family() == AF_INET) {
    OUTCOME_TRY(ret, inet_address::from_address(addr));
    return ret.to_string();
  } else if (addr.sa_family() == AF_INET6) {
    OUTCOME_TRY(ret, inet6_address::from_address(addr));
    return ret.to_string();
  }
  return error_code(EAFNOSUPPORT, system_category());
}

} // namespace net
} // namespace ark
