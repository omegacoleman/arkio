#include <iasr/buffer/buffer.hpp>
#include <iasr/buffer/buffer_view.hpp>
#include <iasr/coroutine/co_async.hpp>
#include <iasr/coroutine/task.hpp>
#include <iasr/error/ec_or.hpp>
#include <iasr/general/normal_file.hpp>
#include <iasr/io/coro.hpp>
#include <iostream>

using iasr::async_context;
using iasr::buffer;
using iasr::buffer_view;
using iasr::ec_or;
using iasr::error_code;
using iasr::normal_file;
using iasr::panic_on_ec;
using iasr::task;
namespace coro = iasr::coro;

task<error_code> to_stdout(normal_file &f) {
  buffer buf{1024};
  for (;;) {
    auto read_ret = co_await coro::read_some(f, buf);
    if (!read_ret)
      co_return read_ret.ec();
    size_t sz = read_ret.get();
    if (sz == 0) {
      co_return error_code{};
    }
    std::cout << std::string_view{buf.data(), sz};
  }
}

task<void> coro_cat(async_context &ctx,
                    const std::vector<std::string> &filenames) {

  for (auto &filename : filenames) {
    normal_file f{panic_on_ec(normal_file::open(ctx, filename, O_RDONLY))};
    panic_on_ec(co_await to_stdout(f));
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
