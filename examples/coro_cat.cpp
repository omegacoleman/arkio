#include <ark/buffer/buffer.hpp>
#include <ark/coroutine/co_async.hpp>
#include <ark/coroutine/task.hpp>
#include <ark/error/ec_or.hpp>
#include <ark/general/normal_file.hpp>
#include <ark/io/coro.hpp>
#include <iostream>

using ark::async_context;
using ark::buffer;
using ark::ec_or;
using ark::error_code;
using ark::mutable_buffer;
using ark::normal_file;
using ark::panic_on_ec;
using ark::task;
using ark::transfer_at_least;
namespace coro = ark::coro;

task<error_code> to_stdout(normal_file &f) {
  std::array<char, 1024> buf;
  for (;;) {
    auto read_ret = co_await coro::read(f, buffer(buf), transfer_at_least(1));
    if (!read_ret)
      co_return read_ret.ec();
    size_t sz = read_ret.get();
    if (sz == 0) {
      co_return error_code{};
    }
    std::cout.write(buf.data(), sz);
    std::cout.flush();
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
