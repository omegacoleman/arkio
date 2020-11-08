#include <array>
#include <iostream>

#include <ark/buffer/buffer.hpp>
#include <ark/error/ec_or.hpp>
#include <ark/general/normal_file.hpp>
#include <ark/io/sync.hpp>

int main(int argc, char **argv) {
  using ark::buffer;
  using ark::mutable_buffer;
  using ark::normal_file;
  using ark::panic_on_ec;
  using ark::transfer_at_least;
  namespace sync = ark::sync;

  for (int i = 1; i < argc; i++) {
    normal_file f{panic_on_ec(normal_file::open({argv[i]}, O_RDONLY))};
    std::array<char, 1024> buf;
    for (;;) {
      size_t sz = panic_on_ec(sync::read(f, buffer(buf), transfer_at_least(1)));
      if (sz == 0) {
        break;
      }
      std::cout.write(buf.data(), sz);
    }
  }

  return 0;
}
