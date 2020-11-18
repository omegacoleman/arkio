#pragma once

/*! \cond FILE_NOT_DOCUMENTED */

#include <ark/bindings.hpp>
#include <gtest/gtest.h>

#define TEST_R(_x_suite, _x_name)                                              \
  ark::result<void> _x_suite##_##_x_name##_body();                             \
                                                                               \
  TEST(_x_suite, _x_name) {                                                    \
    ark::result<void> ret = _x_suite##_##_x_name##_body();                     \
    if (ret.has_error()) {                                                     \
      ASSERT_FALSE(ret.has_error())                                            \
          << "error returned from TEST_R : " << ret.error().message();         \
    }                                                                          \
  }                                                                            \
                                                                               \
  ark::result<void> _x_suite##_##_x_name##_body()

/*! \endcond */
