#include "gtest/gtest.h"

#include <iterator>

#include <iasr/async/io_uring/async_syscall.hpp>
#include <iasr/async/io_uring/context.hpp>
#include <iasr/async/io_uring/io_uring.hpp>
#include <iasr/error/ec_or.hpp>
#include <iasr/io/normal_file.hpp>

using namespace iasr;
using iasr::io_uring_async::owning_cqe_ref;
using iasr::io_uring_async::sqe_ref;
using iasr::io_uring_async::unowning_cqe_ref;

TEST(io_uring, base) {
  iasr::io_uring_async::io_uring r;
  panic_on_ec(r.queue_init(64, 0));
  sqe_ref sqe = panic_on_ec(r.get_sqe());
  sqe.prep_nop();
  sqe.set_data(reinterpret_cast<void *>(123));

  r.submit();
  panic_on_ec(r.wait());

  vector<owning_cqe_ref> cqes;
  r.peek_batch_cqe(std::back_inserter(cqes), 1024);
  ASSERT_EQ(cqes.size(), 1);

  unowning_cqe_ref cqe{cqes[0]};
  ASSERT_EQ(reinterpret_cast<intptr_t>(cqe.get_data()), 123);

  ASSERT_EQ(panic_on_ec(cqe.to_ec_or<int>()), 0);
}

TEST(singlethread_uring_async_context, base_death) {
  iasr::io_uring_async::singlethread_uring_async_context ctx{};
  panic_on_ec(ctx.init());
  normal_file fd_a{panic_on_ec(
      normal_file::mkostemp("/tmp/uring_write_test.XXXXXX", O_WRONLY))};
  string buffer = "hello";
  clinux::iovec iov{
      .iov_base = reinterpret_cast<void *>(buffer.data()),
      .iov_len = buffer.size(),
  };
  io_uring_async::syscall::writev(
      ctx, fd_a.get(), &iov, 1, 0, [&ctx](ec_or<long> ret) {
        panic_on_ec(ret);
        panic_on_ec(ctx.add_sqe([](sqe_ref sqe) { sqe.prep_nop(); },
                                [&ctx](ec_or<long> ret) {
                                  panic_on_ec(ret);
                                  panic_on_ec(ctx.exit());
                                }));
      });
  panic_on_ec(ctx.run());
}
