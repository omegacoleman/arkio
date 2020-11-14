#include <array>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <ark.hpp>

namespace program {

using namespace ark;

task<result<void>> to_stdout(normal_file &f) {
  std::array<char, 1024> buf;
  for (;;) {
    OUTCOME_CO_TRY(sz,
                   co_await coro::read(f, buffer(buf), transfer_at_least(1)));
    if (sz == 0) {
      co_return success();
    }
    std::cout.write(buf.data(), sz);
    std::cout.flush();
  }
}

task<result<void>> coro_cat(async_context &ctx,
                            const std::vector<std::string> &filenames) {
  context_exit_guard g_(ctx);

  for (auto &filename : filenames) {
    OUTCOME_CO_TRY(f, normal_file::open(ctx, filename, O_RDONLY));
    OUTCOME_CO_TRY(co_await to_stdout(f));
  }

  co_return success();
}

result<void> run(int argc, char **argv) {
  async_context ctx;
  OUTCOME_TRY(ctx.init());

  std::vector<std::string> filenames{};
  for (int i = 1; i < argc; ++i)
    filenames.emplace_back(argv[i]);

  auto fut = co_async(coro_cat(ctx, filenames));
  OUTCOME_TRY(ctx.run());
  OUTCOME_TRY(fut.get());

  return success();
}

} // namespace program

int main(int argc, char **argv) {
  auto ret = program::run(argc, argv);
  if (ret.has_error()) {
    std::cerr << "error : " << ret.error().message() << std::endl;
    std::abort();
  }
  return 0;
}
