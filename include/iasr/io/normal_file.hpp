#pragma once

#include <iasr/bindings.hpp>

#include <iasr/error/ec_or.hpp>
#include <iasr/io/fd.hpp>

namespace iasr {
class normal_file : public fd {
protected:
  normal_file(int fd_int)
      : fd(fd_int, fd_offset::offset_manager{fd_offset::userland}) {}

public:
  static ec_or<normal_file> open(string path, int flags) noexcept {
    int ret = clinux::open(path.c_str(), flags);
    if (ret == -1) {
      return clinux::errno_ec();
    }
    return normal_file(ret);
  }

  static ec_or<normal_file> open(string path, int flags, mode_t mode) noexcept {
    int ret = clinux::open(path.c_str(), flags, mode);
    if (ret == -1) {
      return clinux::errno_ec();
    }
    return normal_file(ret);
  }

  static ec_or<normal_file> mkostemp(string templ, int flags) noexcept {
    int ret = clinux::mkostemp(const_cast<char *>(templ.c_str()), flags);
    if (ret == -1) {
      return clinux::errno_ec();
    }
    return normal_file(ret);
  }
};
} // namespace iasr
