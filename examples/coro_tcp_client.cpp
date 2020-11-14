#include <array>
#include <iostream>
#include <string>
#include <utility>

#include <ark.hpp>

namespace program {

using namespace ark;

task<result<void>> do_tcp_client(async_context &ctx) {
  context_exit_guard g_(ctx);

  net::inet_address ep;
  OUTCOME_CO_TRY(ep.host("198.199.109.141")); // isocpp.org
  ep.port(80);

  OUTCOME_CO_TRY(s, net::tcp::socket::create(ctx));
  OUTCOME_CO_TRY(co_await net::tcp::coro::connect(s, ep));

  const std::string wr_buf{
      "GET /index.html HTTP/1.1\r\nHost: isocpp.org\r\n\r\n"};
  OUTCOME_CO_TRY(co_await coro::write(s, buffer(wr_buf)));

  for (;;) {
    std::array<char, 1024> rd_buf;
    OUTCOME_CO_TRY(
        sz, co_await coro::read(s, buffer(rd_buf), transfer_at_least(1)));
    std::cout.write(rd_buf.data(), sz);
  }

  co_return success();
}

result<void> run(void) {
  async_context ctx;
  OUTCOME_TRY(ctx.init());

  auto fut = co_async(do_tcp_client(ctx));
  OUTCOME_TRY(ctx.run());
  OUTCOME_TRY(fut.get());

  return success();
}

} // namespace program

int main(void) {
  auto ret = program::run();
  if (ret.has_error()) {
    std::cerr << "error : " << ret.error().message() << std::endl;
    std::abort();
  }
  return 0;
}
