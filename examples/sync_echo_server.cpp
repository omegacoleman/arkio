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

result<void> run() {
  net::inet_address ep;
  OUTCOME_TRY(ep.host("127.0.0.1"));
  ep.port(8080);

  OUTCOME_TRY(ac, tcp::acceptor::create());
  OUTCOME_TRY(tcp::bind(ac, ep));
  OUTCOME_TRY(tcp::listen(ac));

  for (;;) {
#ifdef PRINT_ACCESS_LOG
    net::address addr;
    OUTCOME_TRY(s, tcp::sync::accept(ac, addr));
    OUTCOME_TRY(in_addr, net::inet_address::from_address(addr));
    OUTCOME_TRY(in_addr_s, to_string(in_addr));
    std::cout << "accepted connection from " << in_addr_s << std::endl;
#else
    OUTCOME_TRY(s, tcp::sync::accept(ac).value());
#endif
    for (;;) {
      std::array<char, 1024> buf;
      OUTCOME_TRY(sz, sync::read(s, buffer(buf), transfer_at_least(1)));
      if (sz == 0)
        break;
      OUTCOME_TRY(sync::write(s, buffer(buf, sz)));
    }
#ifdef PRINT_ACCESS_LOG
    std::cout << "connection ended" << std::endl;
#endif
  }

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
