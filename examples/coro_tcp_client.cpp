#include <iasr/buffer/buffer.hpp>
#include <iasr/coroutine/co_async.hpp>
#include <iasr/coroutine/task.hpp>
#include <iasr/error/ec_or.hpp>
#include <iasr/io/coro.hpp>
#include <iasr/net/address.hpp>
#include <iasr/net/tcp/coro.hpp>

using iasr::async_context;
using iasr::error_code;
using iasr::panic_on_ec;
using iasr::task;
using iasr::transfer_at_least;

task<void> do_tcp_client(async_context &ctx) {
  using iasr::buffer;
  using iasr::mutable_buffer;
  namespace coro = iasr::coro;
  namespace net = iasr::net;

  net::inet_address ep;
  ep.host("198.199.109.141"); // isocpp.org
  ep.port(80);

  net::tcp::socket s{panic_on_ec(net::tcp::socket::create(ctx))};
  panic_on_ec(co_await net::tcp::coro::connect(s, ep));

  const std::string wr_buf{
      "GET /index.html HTTP/1.1\r\nHost: isocpp.org\r\n\r\n"};
  panic_on_ec(co_await coro::write(s, buffer(wr_buf)));

  for (;;) {
    std::array<char, 1024> rd_buf;
    size_t sz = panic_on_ec(
        co_await coro::read(s, buffer(rd_buf), transfer_at_least(1)));
    std::cout.write(rd_buf.data(), sz);
  }

  ctx.exit();
}

int main(void) {
  using iasr::co_async;
  using iasr::panic_on_ec;

  async_context ctx;
  panic_on_ec(ctx.init());

  co_async(do_tcp_client(ctx));
  ctx.run();

  return 0;
}
