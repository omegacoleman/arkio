
#define TEST_R(_x_suite, _x_name) \
ark::result<void> _x_suite##_##_x_name##_body(); \
 \
TEST(_x_suite, _x_name) { \
  auto ret = _x_suite##_##_x_name##_body(); \
  ASSERT_FALSE(ret.has_error()) << "ark::result returning test returned error: " << ret.error().message(); \
} \
 \
ark::result<void> _x_suite##_##_x_name##_body()

