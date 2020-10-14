#include <iasr/buffer/buffer.hpp>
#include <iasr/buffer/buffer_view.hpp>
#include <iasr/error/ec_or.hpp>
#include <iasr/io/stdio.hpp>
#include <iasr/io/sync.hpp>
#include <iasr/net/address.hpp>
#include <iasr/net/tcp/acceptor.hpp>
#include <iasr/net/tcp/general.hpp>
#include <iasr/net/tcp/socket.hpp>
#include <iasr/net/tcp/sync.hpp>

#define PRINT_ACCESS_LOG

int main(void) {
  using iasr::buffer;
  using iasr::buffer_view;
  using iasr::fd_stdout;
  using iasr::panic_on_ec;
  namespace sync = iasr::sync;
  namespace net = iasr::net;
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
      buffer buf{1024};
      size_t sz = panic_on_ec(sync::read_some(s, buf));
      if (sz == 0)
        break;
      panic_on_ec(sync::write(s, buffer_view{buf.data(), buf.data() + sz}));
    }
#ifdef PRINT_ACCESS_LOG
    std::cout << "connection ended" << std::endl;
#endif
  }

  return 0;
}
