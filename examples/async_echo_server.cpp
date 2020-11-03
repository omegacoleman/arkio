#include <ark/buffer/buffer.hpp>
#include <ark/io/async.hpp>
#include <ark/net/address.hpp>
#include <ark/net/tcp/acceptor.hpp>
#include <ark/net/tcp/async.hpp>
#include <ark/net/tcp/general.hpp>
#include <ark/net/tcp/socket.hpp>
#include <functional>

#define PRINT_ACCESS_LOG

namespace program {

using namespace ark;
namespace tcp = net::tcp;

struct echo_service : public std::enable_shared_from_this<echo_service> {
  tcp::socket s_;
  std::array<char, 1024> buf_;

#ifdef PRINT_ACCESS_LOG
  net::address addr_;

  echo_service(tcp::socket s, net::address addr)
      : s_(std::move(s)), addr_(std::move(addr)) {
    std::cout << "connected with peer " << peer_name() << std::endl;
  }

  ~echo_service() {
    std::cout << "closed connection to " << peer_name() << std::endl;
  }

  std::string peer_name() {
    auto ret = to_string(addr_);
    if (!ret)
      return std::string{"["} + ret.error().message() + "]";
    return ret.value();
  }

#else
  echo_service(async_context &ctx, tcp::socket s) : s_(std::move(s)) {}
#endif

  void do_echo() {
    async::read(s_, buffer(buf_), transfer_at_least(1),
                std::bind(&echo_service::handle_read, shared_from_this(),
                          std::placeholders::_1));
  }

  void handle_error(error_code ec) { std::cerr << ec.message() << std::endl; }

  void handle_write(result<size_t> ret) {
    if (ret.has_error()) {
      handle_error(ret.error());
      return;
    }
    do_echo();
  }

  void handle_read(result<size_t> ret) {
    if (ret.has_error()) {
      handle_error(ret.error());
      return;
    }
    size_t sz = ret.value();
    if (sz == 0)
      return;
    async::write(s_, buffer(buf_, sz),
                 std::bind(&echo_service::handle_write, shared_from_this(),
                           std::placeholders::_1));
  }
};

struct echo_server : public std::enable_shared_from_this<echo_server> {
  tcp::acceptor ac_;
  net::address addr_;

  echo_server(tcp::acceptor ac) : ac_(std::move(ac)) {}

  void run() {
#ifdef PRINT_ACCESS_LOG
    tcp::async::accept(ac_, addr_,
                       std::bind(&echo_server::handle_connection,
                                 shared_from_this(), std::placeholders::_1));
#else
    tcp::async::accept(ac_,
                       std::bind(&echo_server::handle_connection,
                                 shared_from_this(), std::placeholders::_1));
#endif
  }

  void handle_connection(result<tcp::socket> ret) {
    if (!ret) {
      ac_.context().exit(ret.as_failure());
      return;
    }
#ifdef PRINT_ACCESS_LOG
    auto svc = std::make_shared<echo_service>(std::move(ret.value()),
                                              std::move(addr_));
#else
    auto svc = std::make_shared<echo_service>(std::move(ret.value()));
#endif
    svc->do_echo();

    run();
  }
};

result<void> run() {
  async_context ctx;
  OUTCOME_TRY(ctx.init());

  net::inet_address ep;
  OUTCOME_TRY(ep.host("127.0.0.1"));
  ep.port(8080);

  OUTCOME_TRY(ac, tcp::acceptor::create(ctx));
  OUTCOME_TRY(tcp::bind(ac, ep));
  OUTCOME_TRY(tcp::listen(ac));

  auto srv = std::make_shared<echo_server>(std::move(ac));
  srv->run();

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
