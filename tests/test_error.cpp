#include "gtest/gtest.h"

#include <iasr/error/ec_or.hpp>

using namespace iasr;

ec_or<int> divide_by(int lhs, int rhs) {
  if (rhs == 0) {
    return error_code(static_cast<int>(errc::invalid_argument),
                      system_category());
  }
  return lhs / rhs;
}

TEST(ec_or, retrieve_value) {
  auto ret = divide_by(4, 2);
  ASSERT_TRUE(ret);
  int ret_v = ret.get();
  ASSERT_EQ(ret_v, 2);
}

TEST(ec_or, retrieve_err) {
  auto ret = divide_by(4, 0);
  ASSERT_FALSE(ret);
  error_code ec = ret.ec();
  ASSERT_EQ(ec.value(), static_cast<int>(errc::invalid_argument));
}

ec_or<pair<int, int>> vec2_divide_by(pair<int, int> lhs, pair<int, int> rhs) {
  auto first = divide_by(lhs.first, rhs.first);
  IASR_PASS_EC_ON(first);
  auto second = divide_by(lhs.second, rhs.second);
  IASR_PASS_EC_ON(second);
  return make_pair(first.get(), second.get());
}

TEST(ec_or, pass_on_helper) {
  auto ret = vec2_divide_by({12, 4}, {3, 2});
  ASSERT_TRUE(ret);
  pair<int, int> p = ret.get();
  ASSERT_EQ(p.first, 4);
  ASSERT_EQ(p.second, 2);

  auto ret_err = vec2_divide_by({12, 4}, {0, 2});
  ASSERT_FALSE(ret_err);
  auto ret_err2 = vec2_divide_by({12, 4}, {3, 0});
  ASSERT_FALSE(ret_err2);
  auto ret_err3 = vec2_divide_by({12, 4}, {0, 0});
  ASSERT_FALSE(ret_err3);
}

TEST(ec_or, panic_on_ec) {
  int ret = panic_on_ec(ec_or<int>(5));
  ASSERT_EQ(ret, 5);
}
