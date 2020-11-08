#include <array>
#include <iostream>

#include <ark/buffer/buffer.hpp>
#include <ark/error/ec_or.hpp>
#include <ark/io/sync.hpp>
#include <ark/net/address.hpp>
#include <ark/net/tcp/sync.hpp>

int main(void) {
  using ark::buffer;
  using ark::mutable_buffer;
  using ark::panic_on_ec;
  using ark::transfer_at_least;
  namespace sync = ark::sync;
  namespace net = ark::net;

  net::inet_address ep;
  ep.host("198.199.109.141"); // isocpp.org
  ep.port(80);

  net::tcp::socket s{panic_on_ec(net::tcp::socket::create())};
  panic_on_ec(net::tcp::sync::connect(s, ep));
  const std::string wr_buf{
      "GET /index.html HTTP/1.1\r\nHost: isocpp.org\r\n\r\n"};
  panic_on_ec(sync::write(s, buffer(wr_buf)));
  for (;;) {
    std::array<char, 1024> rd_buf;
    size_t sz =
        panic_on_ec(sync::read(s, buffer(rd_buf), transfer_at_least(1)));
    std::cout.write(rd_buf.data(), sz);
    std::cout.flush();
  }
}
