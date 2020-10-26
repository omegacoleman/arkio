#include <array>
#include <string>

#include "gtest/gtest.h"

#include <iasr/error/ec_or.hpp>

#include <iasr/general/event_fd.hpp>
#include <iasr/general/mem_fd.hpp>
#include <iasr/general/pipe_fd.hpp>

#include <iasr/io/sync.hpp>

TEST(general, event_fd) {
  using iasr::buffer_view;
  using iasr::event_fd;
  namespace sync = iasr::sync;

  uint64_t data = 1;
  auto fd = panic_on_ec(event_fd::create(0, 0));
  sync::write(fd, buffer_view{reinterpret_cast<char *>(&data), sizeof(data)});
  std::array<char, 4> rd_buf;
  sync::read(fd, rd_buf);
}

TEST(general, mem_fd) {
  using iasr::mem_fd;
  namespace sync = iasr::sync;

  std::string data = "hello";
  auto fd = panic_on_ec(mem_fd::create("test_mfd", 0));
  sync::write(fd, data);
  fd.seek(0);
  std::array<char, 5> rd_buf;
  sync::read(fd, rd_buf);
  std::string got_s{rd_buf.begin(), rd_buf.end()};
  ASSERT_EQ(got_s, data);
}

TEST(general, pipe_fd) {
  using iasr::pipe_fd;
  namespace sync = iasr::sync;

  std::string data = "hello";
  auto ends = panic_on_ec(pipe_fd::create());
  sync::write(ends.second, data);
  std::array<char, 5> rd_buf;
  sync::read(ends.first, rd_buf);
  std::string got_s{rd_buf.begin(), rd_buf.end()};
  ASSERT_EQ(got_s, data);
}
