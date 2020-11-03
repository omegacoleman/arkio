#pragma once

#include <ark/bindings.hpp>

#include <ark/io/fd.hpp>

namespace ark {
class pipe_fd : public nonseekable_fd {
protected:
  pipe_fd(int fd_int) : nonseekable_fd(fd_int) {}

private:
  static result<pair<pipe_fd, pipe_fd>> __create(async_context *ctx) noexcept {
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
  static result<pair<pipe_fd, pipe_fd>> create() noexcept {
    return __create(nullptr);
  }

  static result<pair<pipe_fd, pipe_fd>> create(async_context &ctx) noexcept {
    return __create(&ctx);
  }
};
} // namespace ark
