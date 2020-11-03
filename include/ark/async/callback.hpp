#pragma once

#include <ark/bindings.hpp>

namespace ark {

template <typename RetType>
struct callback : public unique_function<void(RetType ret)> {
  using unique_function<void(RetType ret)>::unique_function;
  using ret_type = RetType;
};

template <> struct callback<void> : public unique_function<void()> {
  using unique_function<void()>::unique_function;
  using ret_type = void;
};

using syscall_callback_t = callback<result<long>>;

} // namespace ark
