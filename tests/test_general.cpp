#include <array>
#include <string>

#include "gtest/gtest.h"

#include <ark/error/ec_or.hpp>

#include <ark/general/event_fd.hpp>
#include <ark/general/mem_fd.hpp>
#include <ark/general/pipe_fd.hpp>

#include <ark/io/sync.hpp>

TEST(general, event_fd) {
  using ark::buffer;
  using ark::event_fd;
  namespace sync = ark::sync;

  uint64_t data[1] = {1};
  auto fd = panic_on_ec(event_fd::create(0, 0));
  sync::write(fd, buffer(data));
  std::array<char, 4> rd_buf;
  sync::read(fd, buffer(rd_buf));
}

TEST(general, mem_fd) {
  using ark::buffer;
  using ark::mem_fd;
  namespace sync = ark::sync;

  std::string data = "hello";
  auto fd = panic_on_ec(mem_fd::create("test_mfd", 0));
  sync::write(fd, buffer(data));
  fd.seek(0);
  std::array<char, 5> rd_buf;
  sync::read(fd, buffer(rd_buf));
  std::string got_s{rd_buf.begin(), rd_buf.end()};
  ASSERT_EQ(got_s, data);
}

TEST(general, pipe_fd) {
  using ark::buffer;
  using ark::pipe_fd;
  namespace sync = ark::sync;

  std::string data = "hello";
  auto ends = panic_on_ec(pipe_fd::create());
  sync::write(ends.second, buffer(data));
  std::array<char, 5> rd_buf;
  sync::read(ends.first, buffer(rd_buf));
  std::string got_s{rd_buf.begin(), rd_buf.end()};
  ASSERT_EQ(got_s, data);
}
