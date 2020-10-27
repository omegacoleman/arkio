#pragma once

#include <iasr/bindings.hpp>

namespace iasr {
template <typename T> class ec_or {
private:
  error_code ec_;
  optional<T> value_;

public:
  ec_or(const T &value) noexcept : value_(value), ec_() {}
  ec_or(T &&value) noexcept : value_(forward<T>(value)), ec_() {}

  ec_or(error_code ec) noexcept : value_(), ec_(ec) {}

  T get() noexcept {
    Expects(value_.has_value());
    return *(move(value_));
  }

  error_code ec() const noexcept { return ec_; }

  explicit operator bool() const noexcept { return !ec_; }
};

namespace detail {
#if defined(__clang__)
#define NO_WTERMINATE_START                                                    \
  _Pragma("clang diagnostic push")                                             \
      _Pragma("clang diagnostic ignored \"-Wexceptions\"")

#define NO_WTERMINATE_END _Pragma("clang diagnostic pop")
#elif defined(__GNUC__)
#define NO_WTERMINATE_START                                                    \
  _Pragma("GCC diagnostic push")                                               \
      _Pragma("GCC diagnostic ignored \"-Wterminate\"")

#define NO_WTERMINATE_END _Pragma("GCC diagnostic pop")
#else
#define NO_WTERMINATE_START
#define NO_WTERMINATE_END
#endif
[[noreturn]] void inline ec_panic(error_code ec) noexcept {
  NO_WTERMINATE_START
#ifdef __cpp_esxceptions
  throw system_error(ec); // throwing in a noexcept function makes program panic
#else
  cerr << "Error : " << ec.message() << "\n";
  terminate();
#endif
  NO_WTERMINATE_END
}
#undef NO_WTERMINATE_START
#undef NO_WTERMINATE_END
} // namespace detail

template <typename T> inline T panic_on_ec(ec_or<T> ret) noexcept {
  if (!ret) {
    detail::ec_panic(ret.ec());
  }
  return move(ret.get());
}

inline void panic_on_ec(error_code ec) noexcept {
  if (ec) {
    detail::ec_panic(ec);
  }
}

} // namespace iasr
