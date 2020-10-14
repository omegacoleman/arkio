#include "gtest/gtest.h"

// #define IASR_ADVANCED_DEBUG_VERBOSITY

#include <iasr/async/context.hpp>
#include <iasr/error/ec_or.hpp>
#include <iasr/io/async.hpp>
#include <iasr/io/mem_fd.hpp>
#include <iasr/io/normal_file.hpp>

#include <iostream>

using namespace iasr;

bool cmp_fd_data(fd &f, const const_buffer_view &data) {
  buffer buf{data.size()};
  f.seek(0);
  sync::read(f, buf);
  return std::equal(buf.begin(), buf.end(), data.begin());
}

TEST(async_io, write_some_buffer) {
  mem_fd f{panic_on_ec(mem_fd::create("write_some_buffer", 0))};
  async_context ctx;
  panic_on_ec(ctx.init());
  static const string buffer = "hello";
  async::write_some(ctx, f, buffer, [&ctx](ec_or<size_t> ret) {
    panic_on_ec(ret);
    EXPECT_EQ(ret.get(), 5);
    ctx.exit();
  });
  ctx.run();

  string s{"hello"};
  ASSERT_TRUE(cmp_fd_data(
      f, buffer_view{reinterpret_cast<byte *>(s.data()), s.size()}));
}

TEST(async_io, write_some_buffer_seq) {
  mem_fd f{panic_on_ec(mem_fd::create("write_some_buffer_seq", 0))};
  async_context ctx;
  panic_on_ec(ctx.init());
  static const vector<string> buffer_seq{"hello", " ", "world"};
  async::write_some(ctx, f, buffer_seq, [&ctx](ec_or<size_t> ret) {
    panic_on_ec(ret);
    EXPECT_EQ(ret.get(), 11);
    ctx.exit();
  });
  panic_on_ec(ctx.run());

  string s{"hello world"};
  ASSERT_TRUE(cmp_fd_data(
      f, buffer_view{reinterpret_cast<byte *>(s.data()), s.size()}));
}

TEST(async_io, read_write_to_buffer) {
  static const size_t test_size = 50 * 1024;
  async_context ctx;
  panic_on_ec(ctx.init());
  buffer buf{test_size};
  buffer buf2{test_size};
  normal_file fd_random{
      panic_on_ec(normal_file::open({"/dev/random"}, O_RDONLY))};
  normal_file fd_a{panic_on_ec(
      normal_file::mkostemp({"/tmp/async_rw_test.XXXXXX"}, O_RDWR))};
  async::read(
      ctx, fd_random, buf, [&ctx, &fd_a, &buf, &buf2](ec_or<size_t> ret) {
        ASSERT_EQ(panic_on_ec(ret), test_size);
        async::write(ctx, fd_a, buf,
                     [&ctx, &fd_a, &buf, &buf2](ec_or<size_t> ret) {
                       ASSERT_EQ(panic_on_ec(ret), test_size);
                       fd_a.seek(0);
                       async::read(ctx, fd_a, buf2,
                                   [&ctx, &buf, &buf2](ec_or<size_t> ret) {
                                     ASSERT_EQ(panic_on_ec(ret), test_size);
                                     ASSERT_TRUE(std::equal(
                                         buf.begin(), buf.end(), buf2.begin()));
                                     ctx.exit();
                                   });
                     });
      });
  ctx.run();
}

TEST(async_io, read_write_to_buffer_seq) {
  static const size_t test_size = 10 * 1024;
  static const size_t test_buffer_count = 5;
  async_context ctx;
  panic_on_ec(ctx.init());
  array<buffer, test_buffer_count> buf;
  array<buffer, test_buffer_count> buf2;
  for (auto &it : buf)
    it = buffer(test_size);
  for (auto &it : buf2)
    it = buffer(test_size);
  normal_file fd_random{
      panic_on_ec(normal_file::open({"/dev/random"}, O_RDONLY))};
  normal_file fd_a{panic_on_ec(
      normal_file::mkostemp({"/tmp/async_rw_test.XXXXXX"}, O_RDWR))};
  async::read(
      ctx, fd_random, buf,
      [&ctx, &fd_a, &buf, &buf2](ec_or<size_t> ret) mutable {
        ASSERT_EQ(panic_on_ec(ret), test_size * test_buffer_count);
        async::write(
            ctx, fd_a, buf,
            [&ctx, &fd_a, &buf, &buf2](ec_or<size_t> ret) mutable {
              ASSERT_EQ(panic_on_ec(ret), test_size * test_buffer_count);
              fd_a.seek(0);
              async::read(
                  ctx, fd_a, buf2, [&ctx, &buf, &buf2](ec_or<size_t> ret) {
                    ASSERT_EQ(panic_on_ec(ret), test_size * test_buffer_count);
                    for (int i = 0; i < test_buffer_count; ++i)
                      ASSERT_TRUE(std::equal(buf[i].begin(), buf[i].end(),
                                             buf2[i].begin()));
                    ctx.exit();
                  });
            });
      });
  ctx.run();
}
