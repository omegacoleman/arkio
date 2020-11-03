#include <array>
#include <iostream>

#include <ark/buffer/buffer.hpp>
#include <ark/io/sync.hpp>
#include <ark/net/address.hpp>
#include <ark/net/tcp/sync.hpp>

namespace program {

using namespace ark;

result<void> run() {
  net::inet_address ep;
  OUTCOME_TRY(ep.host("198.199.109.141")); // isocpp.org
  ep.port(80);

  OUTCOME_TRY(s, net::tcp::socket::create());
  OUTCOME_TRY(net::tcp::sync::connect(s, ep));
  const std::string wr_buf{
      "GET /index.html HTTP/1.1\r\nHost: isocpp.org\r\n\r\n"};
  OUTCOME_TRY(sync::write(s, buffer(wr_buf)));
  for (;;) {
    std::array<char, 1024> rd_buf;
    OUTCOME_TRY(sz, sync::read(s, buffer(rd_buf), transfer_at_least(1)));
    std::cout.write(rd_buf.data(), sz);
    std::cout.flush();
  }
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
