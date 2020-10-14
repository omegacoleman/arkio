#pragma once

#include <iasr/bindings.hpp>

#include <iasr/error/ec_or.hpp>

namespace iasr {

template <typename RetType>
struct callback : public unique_function<void(ec_or<RetType> result)> {
  using unique_function<void(ec_or<RetType> result)>::unique_function;
};

template <>
struct callback<void> : public unique_function<void(error_code result)> {
  using unique_function<void(error_code result)>::unique_function;
};

using syscall_callback_t = callback<long>;

} // namespace iasr
