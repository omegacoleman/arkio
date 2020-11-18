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
  TryX(ep.host("127.0.0.1"));
  ep.port(8080);

  auto ac = TryX(tcp::acceptor::create());
  TryX(tcp::bind(ac, ep));
  TryX(tcp::listen(ac));

  for (;;) {
#ifdef PRINT_ACCESS_LOG
    net::address addr;
    tcp::socket s = TryX(tcp::sync::accept(ac, addr));
    auto in_addr = TryX(net::inet_address::from_address(addr));
    std::string in_addr_s = TryX(to_string(in_addr));
    std::cout << "accepted connection from " << in_addr_s << std::endl;
#else
    tcp::socket s = TryX(tcp::sync::accept(ac).value());
#endif
    for (;;) {
      std::array<char, 1024> buf;
      size_t sz = TryX(sync::read(s, buffer(buf), transfer_at_least(1)));
      if (sz == 0)
        break;
      TryX(sync::write(s, buffer(buf, sz)));
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
