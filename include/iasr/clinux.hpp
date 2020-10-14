#pragma once

#include <iasr/bindings.hpp>

extern "C" {
#include <arpa/inet.h>
#include <fcntl.h>
#include <linux/version.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <poll.h>
#include <signal.h>
#include <sys/eventfd.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
}

namespace iasr {
namespace clinux {
using ::accept4;
using ::bind;
using ::close;
using ::connect;
using ::eventfd;
using ::htons;
using ::inet_ntop;
using ::inet_pton;
using ::iovec;
using ::listen;
using ::lseek;
using ::memfd_create;
using ::mkostemp;
using ::ntohs;
using ::off_t;
using ::open;
using ::preadv2;
using ::pwritev2;
using ::sa_family_t;
using ::signal;
using ::sockaddr;
using ::sockaddr_in;
using ::sockaddr_in6;
using ::socket;
using ::socklen_t;

inline error_code errno_ec() noexcept {
  return error_code(errno, system_category());
}

using version_t = long;

static const inline version_t version_code = LINUX_VERSION_CODE;

inline const constexpr version_t version(const int major, const int minor,
                                         const int micro) noexcept {
  return KERNEL_VERSION(major, minor, micro);
}

} // namespace clinux
} // namespace iasr
