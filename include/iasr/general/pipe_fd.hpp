#pragma once

#include <iasr/bindings.hpp>

#include <iasr/error/ec_or.hpp>
#include <iasr/io/fd.hpp>

namespace iasr {
class pipe_fd : public nonseekable_fd {
protected:
  pipe_fd(int fd_int) : nonseekable_fd(fd_int) {}

private:
  static ec_or<pair<pipe_fd, pipe_fd>> __create(async_context *ctx) noexcept {
    int pipefd[2];
    int ret = clinux::pipe2(pipefd, 0);
    if (ret == -1) {
      return clinux::errno_ec();
    }
    pipe_fd f_in{pipefd[0]};
    pipe_fd f_out{pipefd[1]};
    f_in.set_async_context(ctx);
    f_out.set_async_context(ctx);
    return move(make_pair(move(f_in), move(f_out)));
  }

public:
  static ec_or<pair<pipe_fd, pipe_fd>> create() noexcept {
    return __create(nullptr);
  }

  static ec_or<pair<pipe_fd, pipe_fd>> create(async_context &ctx) noexcept {
    return __create(&ctx);
  }
};
} // namespace iasr
