#pragma once

#include <iasr/bindings.hpp>

#include <iasr/error/ec_or.hpp>
#include <iasr/io/fd.hpp>

namespace iasr {
class mem_fd : public fd {
protected:
  mem_fd(int fd_int)
      : fd(fd_int, fd_offset::offset_manager{fd_offset::userland}) {}

public:
  static ec_or<mem_fd> create(const string &name, int flags) noexcept {
    int ret = clinux::memfd_create(name.c_str(), flags);
    if (ret == -1) {
      return clinux::errno_ec();
    }
    return mem_fd(ret);
  }
};
} // namespace iasr
