#pragma once

/*!
 * \brief contains symbols normally placed in linux-headers
 *
 * arkio demands a few symbols from linux-headers, which is the c header files
 * for linux kernel. Functions and structs are aliased by keyword 'using', while
 * macros remain in the global.
 *
 * This approach is to avoid naming conflicts.
 */
namespace clinux {}

/*! \cond FILE_NOT_DOCUMENTED */

#include <ark/bindings/bindings.hpp>

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

namespace ark {
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
using ::loff_t;
using ::lseek;
using ::memfd_create;
using ::mkostemp;
using ::ntohs;
using ::off_t;
using ::open;
using ::pipe2;
using ::preadv2;
using ::pwritev2;
using ::read;
using ::readv;
using ::sa_family_t;
using ::signal;
using ::sockaddr;
using ::sockaddr_in;
using ::sockaddr_in6;
using ::sockaddr_storage;
using ::socket;
using ::socklen_t;
using ::splice;
using ::vmsplice;
using ::write;
using ::writev;

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
} // namespace ark

/*! \endcond */
