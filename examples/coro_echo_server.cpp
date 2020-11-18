#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include <ark.hpp>

#define PRINT_ACCESS_LOG

namespace program {

using namespace ark;
namespace tcp = net::tcp;

task<result<void>> handle_conn(tcp::socket s) {
  for (;;) {
    std::array<char, 1024> buf;

    size_t sz =
        CoTryX(co_await coro::read(s, buffer(buf), transfer_at_least(1)));

    if (sz == 0)
      break;

    CoTryX(co_await coro::write(s, buffer(buf, sz)));
  }
  co_return success();
}

#ifdef PRINT_ACCESS_LOG
task<void> run_handle_conn(tcp::socket s, net::address addr) {
  auto addr_ret = to_string(addr);
  std::string addr_s;
  if (addr_ret.has_error())
    addr_s = "[invalid addr]";
  else
    addr_s = addr_ret.value();
  std::cout << "accepted connection from " << addr_s << std::endl;
#else
task<void> run_handle_conn(tcp::socket s) {
#endif
  auto ret = co_await handle_conn(std::move(s));
  if (ret.has_error())
    std::cerr << ret.error().message() << std::endl;
#ifdef PRINT_ACCESS_LOG
  std::cout << "connection to " << addr_s << " ended" << std::endl;
#endif
}

task<result<void>> echo_srv(tcp::acceptor &ac) {
  context_exit_guard g_(ac.context());

  for (;;) {
#ifdef PRINT_ACCESS_LOG
    net::address addr;
    tcp::socket s = CoTryX(co_await tcp::coro::accept(ac, addr));
    co_async(run_handle_conn(std::move(s), std::move(addr)));
#else
    tcp::socket s = CoTryX(co_await tcp::coro::accept(ac));
    co_async(run_handle_conn(std::move(s)));
#endif
  }
}

result<void> run() {
  async_context ctx;
  TryX(ctx.init());

  net::inet_address ep;
  TryX(ep.host("127.0.0.1"));
  ep.port(8080);

  auto ac = TryX(tcp::acceptor::create(ctx));
  TryX(tcp::bind(ac, ep));
  TryX(tcp::listen(ac));

  auto fut = co_async(echo_srv(ac));
  TryX(ctx.run());
  TryX(fut.get());

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
