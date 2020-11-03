#include "gtest/gtest.h"

#include <ark/misc/test_r.hpp>
#include <ark/net/address.hpp>

using namespace ark;

TEST_R(net_address, ipv4) {
  net::inet_address addr;
  OUTCOME_TRY(addr.host("127.0.0.1"));
  addr.port(8080);
  OUTCOME_TRY(got_host, addr.host());
  EXPECT_EQ(got_host, "127.0.0.1");
  EXPECT_EQ(addr.port(), 8080);

  OUTCOME_TRY(str, addr.to_string());
  EXPECT_EQ(str, "127.0.0.1:8080");

  return success();
}

TEST_R(net_address, ipv6) {
  net::inet6_address addr;
  OUTCOME_TRY(addr.host("::1"));
  addr.port(8080);
  OUTCOME_TRY(got_host, addr.host());
  EXPECT_EQ(got_host, "::1");
  EXPECT_EQ(addr.port(), 8080);

  OUTCOME_TRY(str, addr.to_string());
  EXPECT_EQ(str, "[::1]:8080");
  return success();
}

TEST_R(net_address, upcast_downcast) {
  net::inet_address addr;
  OUTCOME_TRY(addr.host("127.0.0.1"));
  addr.port(8080);

  net::address p = addr.to_address();
  OUTCOME_TRY(addr2, net::inet_address::from_address(p));

  OUTCOME_TRY(str1, addr.to_string());
  OUTCOME_TRY(str2, addr2.to_string());
  EXPECT_EQ(str1, str2);

  return success();
}
