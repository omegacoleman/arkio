#include <array>
#include <iostream>
#include <string>
#include <utility>

#include <ark.hpp>

namespace program {

using namespace ark;

void read_and_print(net::tcp::socket &s) {
  static std::array<char, 1024> rd_buf;
  async::read(s, buffer(rd_buf), transfer_at_least(1),
              [&s](result<size_t> ret) {
                if (ret.has_error()) {
                  s.context().exit(ret.as_failure());
                  return;
                }
                size_t sz = ret.value();
                std::cout.write(rd_buf.data(), sz);
                std::cout.flush();
              });
}

result<void> run() {
  net::inet_address ep;
  OUTCOME_TRY(ep.host("198.199.109.141")); // isocpp.org
  ep.port(80);

  async_context ctx;
  OUTCOME_TRY(ctx.init());

  OUTCOME_TRY(s, net::tcp::socket::create(ctx));

  net::tcp::async::connect(s, ep, [&ctx, &s](result<void> ret) {
    if (ret.has_error()) {
      ctx.exit(ret.as_failure());
      return;
    }
    static const std::string wr_buf{
        "GET /index.html HTTP/1.1\r\nHost: isocpp.org\r\n\r\n"};
    async::write(s, buffer(wr_buf), [&ctx, &s](result<size_t> ret) {
      if (ret.has_error()) {
        ctx.exit(ret.as_failure());
        return;
      }
      read_and_print(s);
    });
  });

  OUTCOME_TRY(ctx.run());
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
