#include <array>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <ark.hpp>

namespace program {

using namespace ark;

result<void> run(int argc, char **argv) {

  for (int i = 1; i < argc; i++) {
    auto f = TryX(normal_file::open({argv[i]}, O_RDONLY));
    std::array<char, 1024> buf;
    for (;;) {
      size_t sz = TryX(sync::read(f, buffer(buf), transfer_at_least(1)));
      if (sz == 0) {
        break;
      }
      std::cout.write(buf.data(), sz);
    }
  }

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
