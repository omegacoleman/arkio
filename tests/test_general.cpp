#include <array>
#include <string>

#include "gtest/gtest.h"

#include <ark/general/event_fd.hpp>
#include <ark/general/mem_fd.hpp>
#include <ark/general/pipe_fd.hpp>

#include <ark/io/sync.hpp>
#include <ark/misc/test_r.hpp>

using ark::success;

TEST_R(general, event_fd) {
  using ark::buffer;
  using ark::event_fd;
  namespace sync = ark::sync;

  uint64_t data[1] = {1};
  OUTCOME_TRY(fd, event_fd::create(0, 0));
  OUTCOME_TRY(sync::write(fd, buffer(data)));
  uint64_t rd_buf[1];
  OUTCOME_TRY(sync::read(fd, buffer(rd_buf)));
  return success();
}

TEST_R(general, mem_fd) {
  using ark::buffer;
  using ark::mem_fd;
  namespace sync = ark::sync;

  std::string data = "hello";
  OUTCOME_TRY(fd, mem_fd::create("test_mfd", 0));
  OUTCOME_TRY(sync::write(fd, buffer(data)));
  fd.seek(0);
  std::array<char, 5> rd_buf;
  OUTCOME_TRY(sync::read(fd, buffer(rd_buf)));
  std::string_view got_s{rd_buf.data(), rd_buf.size()};
  EXPECT_EQ(got_s, data);
  return success();
}

TEST_R(general, pipe_fd) {
  using ark::buffer;
  using ark::pipe_fd;
  namespace sync = ark::sync;

  std::string data = "hello";
  OUTCOME_TRY(ends, pipe_fd::create());
  OUTCOME_TRY(sync::write(ends.second, buffer(data)));
  std::array<char, 5> rd_buf;
  OUTCOME_TRY(sync::read(ends.first, buffer(rd_buf)));
  std::string_view got_s{rd_buf.data(), rd_buf.size()};
  EXPECT_EQ(got_s, data);
  return success();
}
