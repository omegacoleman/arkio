#pragma once

#include <ark/bindings.hpp>
#include <ark/clinux.hpp>

namespace ark {
namespace net {

class address {
protected:
  clinux::sockaddr_storage sa_;

public:
  address() { sa_ptr()->sa_family = AF_UNSPEC; }

  clinux::sa_family_t sa_family() const noexcept { return sa_ptr()->sa_family; }
  clinux::sockaddr *sa_ptr() noexcept {
    return reinterpret_cast<clinux::sockaddr *>(addressof(sa_));
  }
  const clinux::sockaddr *sa_ptr() const noexcept {
    return reinterpret_cast<const clinux::sockaddr *>(addressof(sa_));
  }
  clinux::socklen_t sa_len() const noexcept { return sizeof(sa_); }
};

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

  sockaddr_ptr_t sa_ptr() noexcept {
    return reinterpret_cast<sockaddr_ptr_t>(address::sa_ptr());
  }

  const_sockaddr_ptr_t sa_ptr() const noexcept {
    return reinterpret_cast<const_sockaddr_ptr_t>(address::sa_ptr());
  }

  size_type sa_len() const noexcept { return size; }

  address to_address() noexcept { return *(this); }
};

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

  result<string> host() const noexcept {
    char buff[16];
    const char *s = clinux::inet_ntop(AF_INET, &(base_type::sa_ptr()->sin_addr),
                                      buff, sizeof(buff));
    if (s == nullptr) {
      return clinux::errno_ec();
    }
    return {s};
  }

  result<void> host(string host_s) noexcept {
    int ret = clinux::inet_pton(address_family, host_s.c_str(),
                                &(base_type::sa_ptr()->sin_addr));
    if (ret <= 0)
      return error_code(EINVAL, system_category());
    return success();
  }

  unsigned short port() const noexcept {
    return clinux::ntohs(base_type::sa_ptr()->sin_port);
  }

  void port(unsigned short p) noexcept {
    base_type::sa_ptr()->sin_port = clinux::htons(p);
  }

  result<string> to_string() const noexcept {
    ostringstream oss;
    OUTCOME_TRY(host_s, host());
    oss << host_s << ":" << port();
    return oss.str();
  }

  static result<inet_address> from_address(address addr) noexcept {
    if (addr.sa_ptr()->sa_family != address_family)
      return error_code(EAFNOSUPPORT, system_category());
    return inet_address{addr};
  }
};

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

  result<string> host() const noexcept {
    char buff[64];
    const char *s = clinux::inet_ntop(
        AF_INET6, &(base_type::sa_ptr()->sin6_addr), buff, sizeof(buff));
    if (s == nullptr) {
      return clinux::errno_ec();
    }
    return {s};
  }

  result<void> host(string host_s) noexcept {
    int ret = clinux::inet_pton(address_family, host_s.c_str(),
                                &(base_type::sa_ptr()->sin6_addr));
    if (ret <= 0)
      return error_code(EINVAL, system_category());
    return success();
  }

  unsigned short port() const noexcept {
    return clinux::ntohs(base_type::sa_ptr()->sin6_port);
  }

  void port(unsigned short p) noexcept {
    base_type::sa_ptr()->sin6_port = clinux::htons(p);
  }

  result<string> to_string() const noexcept {
    ostringstream oss;
    OUTCOME_TRY(host_s, host());
    oss << "[" << host_s << "]:" << port();
    return oss.str();
  }

  static result<inet6_address> from_address(address addr) noexcept {
    if (addr.sa_ptr()->sa_family != address_family)
      return error_code(EAFNOSUPPORT, system_category());
    return inet6_address{addr};
  }
};

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
