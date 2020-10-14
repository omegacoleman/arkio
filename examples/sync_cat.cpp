#include <iasr/buffer/buffer.hpp>
#include <iasr/buffer/buffer_view.hpp>
#include <iasr/error/ec_or.hpp>
#include <iasr/io/normal_file.hpp>
#include <iasr/io/stdio.hpp>
#include <iasr/io/sync.hpp>

int main(int argc, char **argv) {
  using iasr::buffer;
  using iasr::buffer_view;
  using iasr::fd_stdout;
  using iasr::normal_file;
  using iasr::panic_on_ec;
  namespace sync = iasr::sync;

  for (int i = 1; i < argc; i++) {
    normal_file f{panic_on_ec(normal_file::open({argv[i]}, O_RDONLY))};
    buffer buf{1024};
    for (;;) {
      size_t sz = panic_on_ec(sync::read_some(f, buf));
      if (sz == 0) {
        goto next;
      }
      panic_on_ec(sync::write(fd_stdout, buffer_view(buf.data(), sz)));
    }
  next:;
  }

  return 0;
}
