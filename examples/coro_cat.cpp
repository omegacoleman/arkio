#include <iasr/buffer/buffer.hpp>
#include <iasr/buffer/buffer_view.hpp>
#include <iasr/coroutine/co_async.hpp>
#include <iasr/coroutine/task.hpp>
#include <iasr/error/ec_or.hpp>
#include <iasr/io/coro.hpp>
#include <iasr/io/normal_file.hpp>
#include <iasr/io/stdio.hpp>
#include <iostream>

using iasr::async_context;
using iasr::buffer;
using iasr::buffer_view;
using iasr::ec_or;
using iasr::error_code;
using iasr::fd;
using iasr::fd_stdin;
using iasr::fd_stdout;
using iasr::normal_file;
using iasr::panic_on_ec;
using iasr::task;
namespace coro = iasr::coro;

task<error_code> to_stdout(async_context &ctx, fd &f) {
  buffer buf{1024};
  std::cerr << "hey hey" << std::endl;
  for (;;) {
    auto read_ret = co_await coro::read_some(ctx, f, buf);
    std::cerr << "hey hey" << std::endl;
    if (!read_ret)
      co_return read_ret.ec();
    size_t sz = read_ret.get();
    if (sz == 0) {
      co_return error_code{};
    }
    auto write_ret =
        co_await coro::write(ctx, fd_stdout, buffer_view{buf.data(), sz});
    if (!write_ret)
      co_return write_ret.ec();
    std::cerr << "hey hey" << std::endl;
  }
}

task<void> coro_cat(async_context &ctx,
                    const std::vector<std::string> &filenames) {

  if (filenames.empty())
    panic_on_ec(co_await to_stdout(ctx, fd_stdin));

  for (auto &filename : filenames) {
    normal_file f{panic_on_ec(normal_file::open(filename, O_RDONLY))};
    panic_on_ec(co_await to_stdout(ctx, f));
  }

  ctx.exit();
}

int main(int argc, char **argv) {
  async_context ctx;
  panic_on_ec(ctx.init());

  std::vector<std::string> filenames{};
  for (int i = 1; i < argc; ++i)
    filenames.emplace_back(argv[i]);

  co_async(coro_cat(ctx, filenames));
  ctx.run();

  return 0;
}
