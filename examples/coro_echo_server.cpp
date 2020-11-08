#include <functional>
#include <ark/buffer/buffer.hpp>
#include <ark/coroutine/co_async.hpp>
#include <ark/coroutine/task.hpp>
#include <ark/error/ec_or.hpp>
#include <ark/io/coro.hpp>
#include <ark/net/address.hpp>
#include <ark/net/tcp/acceptor.hpp>
#include <ark/net/tcp/coro.hpp>
#include <ark/net/tcp/general.hpp>
#include <ark/net/tcp/socket.hpp>

// #define PRINT_ACCESS_LOG

using ark::async_context;
using ark::buffer;
using ark::ec_or;
using ark::error_code;
using ark::panic_on_ec;
using ark::task;
using ark::transfer_at_least;
namespace coro = ark::coro;
namespace net = ark::net;
namespace tcp = net::tcp;

void handle_conn_err(error_code ec) { std::cerr << ec.message() << std::endl; }

task<void> handle_conn(tcp::socket s) {
  for (;;) {
    std::array<char, 1024> buf;

    auto ret = co_await coro::read(s, buffer(buf), transfer_at_least(1));
    if (!ret)
      co_return handle_conn_err(ret.ec());

    size_t sz = ret.get();
    if (sz == 0)
      break;

    auto wr_ret = co_await coro::write(s, buffer(buf, sz));
    if (!wr_ret)
      co_return handle_conn_err(wr_ret.ec());
  }
}

task<void> echo_srv(tcp::acceptor &ac) {
  for (;;) {
    auto s = panic_on_ec(co_await tcp::coro::accept(ac));
    co_async(handle_conn(std::move(s)));
  }
}

int main(void) {
  async_context ctx;
  panic_on_ec(ctx.init());

  net::inet_address ep;
  ep.host("127.0.0.1");
  ep.port(8080);

  tcp::acceptor ac{panic_on_ec(tcp::acceptor::create(ctx))};
  panic_on_ec(tcp::bind(ac, ep));
  panic_on_ec(tcp::listen(ac));

  co_async(echo_srv(ac));
  ctx.run();

  return 0;
}
