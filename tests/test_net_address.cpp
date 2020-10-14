#include "gtest/gtest.h"

#include <iasr/net/address.hpp>

using namespace iasr;

TEST(net_address, ipv4) {
  net::inet_address addr;
  addr.host("127.0.0.1");
  addr.port(8080);
  ASSERT_EQ(panic_on_ec(addr.host()), "127.0.0.1");
  ASSERT_EQ(addr.port(), 8080);

  ASSERT_EQ(panic_on_ec(addr.to_string()), "127.0.0.1:8080");
}

TEST(net_address, ipv6) {
  net::inet6_address addr;
  addr.host("::1");
  addr.port(8080);
  ASSERT_EQ(panic_on_ec(addr.host()), "::1");
  ASSERT_EQ(addr.port(), 8080);

  ASSERT_EQ(panic_on_ec(addr.to_string()), "[::1]:8080");
}

TEST(net_address, upcast_downcast) {
  net::inet_address addr;
  addr.host("127.0.0.1");
  addr.port(8080);

  net::address p = addr.to_address();
  net::inet_address addr2{
      panic_on_ec(net::inet_address::from_address(move(p)))};
  ASSERT_EQ(panic_on_ec(addr.to_string()), panic_on_ec(addr2.to_string()));
}
