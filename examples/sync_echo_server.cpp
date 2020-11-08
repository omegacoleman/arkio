#include <array>
#include <iostream>

#include <ark/buffer/buffer.hpp>
#include <ark/error/ec_or.hpp>
#include <ark/io/sync.hpp>
#include <ark/net/address.hpp>
#include <ark/net/tcp/acceptor.hpp>
#include <ark/net/tcp/general.hpp>
#include <ark/net/tcp/socket.hpp>
#include <ark/net/tcp/sync.hpp>

#define PRINT_ACCESS_LOG

int main(void) {
  using ark::buffer;
  using ark::mutable_buffer;
  using ark::panic_on_ec;
  using ark::transfer_at_least;
  namespace sync = ark::sync;
  namespace net = ark::net;
  namespace tcp = net::tcp;

  net::inet_address ep;
  ep.host("127.0.0.1");
  ep.port(8080);

  tcp::acceptor ac{panic_on_ec(tcp::acceptor::create())};
  panic_on_ec(tcp::bind(ac, ep));
  panic_on_ec(tcp::listen(ac));

  for (;;) {
#ifdef PRINT_ACCESS_LOG
    net::address addr;
    tcp::socket s{panic_on_ec(tcp::sync::accept(ac, addr))};
    std::cout << "accepted connection from "
              << panic_on_ec(panic_on_ec(net::inet_address::from_address(addr))
                                 .to_string())
              << std::endl;
#else
    tcp::socket s{panic_on_ec(tcp::sync::accept(ac))};
#endif
    for (;;) {
      std::array<char, 1024> buf;
      size_t sz = panic_on_ec(sync::read(s, buffer(buf), transfer_at_least(1)));
      if (sz == 0)
        break;
      panic_on_ec(sync::write(s, buffer(buf, sz)));
    }
#ifdef PRINT_ACCESS_LOG
    std::cout << "connection ended" << std::endl;
#endif
  }

  return 0;
}
