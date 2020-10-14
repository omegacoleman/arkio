#include <functional>
#include <iasr/buffer/buffer.hpp>
#include <iasr/buffer/buffer_view.hpp>
#include <iasr/coroutine/co_async.hpp>
#include <iasr/coroutine/task.hpp>
#include <iasr/error/ec_or.hpp>
#include <iasr/io/coro.hpp>
#include <iasr/io/stdio.hpp>
#include <iasr/net/address.hpp>
#include <iasr/net/tcp/acceptor.hpp>
#include <iasr/net/tcp/coro.hpp>
#include <iasr/net/tcp/general.hpp>
#include <iasr/net/tcp/socket.hpp>

// #define PRINT_ACCESS_LOG

using iasr::async_context;
using iasr::buffer;
using iasr::buffer_view;
using iasr::ec_or;
using iasr::fd_stdout;
using iasr::panic_on_ec;
using iasr::task;
namespace coro = iasr::coro;
namespace net = iasr::net;
namespace tcp = net::tcp;

task<void> handle_conn(async_context &ctx, tcp::socket s) {
  for (;;) {
    buffer buf{1024};
    auto ret = co_await coro::read_some(ctx, s, buf);
    if (!ret) {
      std::cerr << ret.ec().message() << std::endl;
      break;
    }
    size_t sz = ret.get();
    if (sz == 0)
      break;
    auto wr_ret =
        co_await coro::write(ctx, s, buffer_view{buf.data(), buf.data() + sz});
    if (!wr_ret) {
      std::cerr << wr_ret.ec().message() << std::endl;
      break;
    }
  }
}

task<void> echo_srv(async_context &ctx, tcp::acceptor &ac) {
  for (;;) {
    auto s = panic_on_ec(co_await tcp::coro::accept(ctx, ac));
    co_async(handle_conn(ctx, std::move(s)));
  }
}

int main(void) {
  async_context ctx;
  panic_on_ec(ctx.init());

  net::inet_address ep;
  ep.host("127.0.0.1");
  ep.port(8080);

  tcp::acceptor ac{panic_on_ec(tcp::acceptor::create())};
  panic_on_ec(tcp::bind(ac, ep));
  panic_on_ec(tcp::listen(ac));

  co_async(echo_srv(ctx, ac));
  ctx.run();

  return 0;
}
