#pragma once

#include <iasr/bindings.hpp>

#include <iasr/io/fd.hpp>

namespace iasr {
class stdin_t : public fd {
public:
  stdin_t() noexcept : fd(0) {}
};

class stdout_t : public fd {
public:
  stdout_t() noexcept : fd(1) {}
};

class stderr_t : public fd {
public:
  stderr_t() noexcept : fd(2) {}
};

// names start with fd_ to avoid conflict with c FILE* descs
inline static stdin_t fd_stdin{};
inline static stdout_t fd_stdout{};
inline static stderr_t fd_stderr{};
} // namespace iasr
