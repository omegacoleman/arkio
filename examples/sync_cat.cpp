#include <array>
#include <iostream>

#include <iasr/buffer/buffer.hpp>
#include <iasr/error/ec_or.hpp>
#include <iasr/general/normal_file.hpp>
#include <iasr/io/sync.hpp>

int main(int argc, char **argv) {
  using iasr::buffer;
  using iasr::mutable_buffer;
  using iasr::normal_file;
  using iasr::panic_on_ec;
  using iasr::transfer_at_least;
  namespace sync = iasr::sync;

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
