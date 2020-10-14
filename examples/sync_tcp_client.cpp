#include <iasr/buffer/buffer.hpp>
#include <iasr/buffer/buffer_view.hpp>
#include <iasr/error/ec_or.hpp>
#include <iasr/io/stdio.hpp>
#include <iasr/io/sync.hpp>
#include <iasr/net/address.hpp>
#include <iasr/net/tcp/sync.hpp>

int main(void) {
  using iasr::buffer;
  using iasr::buffer_view;
  using iasr::fd_stdout;
  using iasr::panic_on_ec;
  namespace sync = iasr::sync;
  namespace net = iasr::net;

  net::inet_address ep;
  ep.host("198.199.109.141"); // isocpp.org
  ep.port(80);

  net::tcp::socket s{panic_on_ec(net::tcp::socket::create())};
  panic_on_ec(net::tcp::sync::connect(s, ep));
  const std::string wr_buf{
      "GET /index.html HTTP/1.1\r\nHost: isocpp.org\r\n\r\n"};
  panic_on_ec(sync::write(s, wr_buf));
  for (;;) {
    buffer rd_buf{1024};
    size_t sz = panic_on_ec(sync::read_some(s, rd_buf));
    panic_on_ec(sync::write(fd_stdout, buffer_view(rd_buf.data(), sz)));
  }
}
