#pragma once

#include <iasr/bindings.hpp>

#include <iasr/error/ec_or.hpp>
#include <iasr/io/fd.hpp>

namespace iasr {
class event_fd : public fd {
  using fd::fd;

public:
  static ec_or<event_fd> create(unsigned int count, int flags) noexcept {
    int ret = clinux::eventfd(count, flags);
    if (ret == -1) {
      return clinux::errno_ec();
    }
    return event_fd(ret);
  }
};
} // namespace iasr
