#include <ark/buffer/buffer.hpp>
#include <ark/coroutine/co_async.hpp>
#include <ark/coroutine/task.hpp>
#include <ark/io/coro.hpp>
#include <ark/misc/context_exit_guard.hpp>
#include <ark/net/address.hpp>
#include <ark/net/tcp/acceptor.hpp>
#include <ark/net/tcp/coro.hpp>
#include <ark/net/tcp/general.hpp>
#include <ark/net/tcp/socket.hpp>
#include <functional>

// #define PRINT_ACCESS_LOG

namespace program {

using namespace ark;
namespace tcp = net::tcp;

task<result<void>> handle_conn(tcp::socket s) {
  for (;;) {
    std::array<char, 1024> buf;

    OUTCOME_CO_TRY(sz,
                   co_await coro::read(s, buffer(buf), transfer_at_least(1)));

    if (sz == 0)
      break;

    OUTCOME_CO_TRY(co_await coro::write(s, buffer(buf, sz)));
  }
  co_return success();
}

task<void> run_handle_conn(tcp::socket s) {
  auto ret = co_await handle_conn(std::move(s));
  if (ret.has_error())
    std::cerr << ret.error().message() << std::endl;
}

task<result<void>> echo_srv(tcp::acceptor &ac) {
  context_exit_guard g_(ac.context());

  for (;;) {
    OUTCOME_CO_TRY(s, co_await tcp::coro::accept(ac));
    co_async(run_handle_conn(std::move(s)));
  }
}

result<void> run() {
  async_context ctx;
  OUTCOME_TRY(ctx.init());

  net::inet_address ep;
  OUTCOME_TRY(ep.host("127.0.0.1"));
  ep.port(8080);

  OUTCOME_TRY(ac, tcp::acceptor::create(ctx));
  OUTCOME_TRY(tcp::bind(ac, ep));
  OUTCOME_TRY(tcp::listen(ac));

  auto fut = co_async(echo_srv(ac));
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
