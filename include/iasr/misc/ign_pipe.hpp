#pragma once

#include <iasr/bindings.hpp>
#include <iasr/clinux.hpp>

namespace iasr {
namespace detail {
class ign_pipe {
public:
  ign_pipe() noexcept { clinux::signal(SIGPIPE, SIG_IGN); }
};

#ifndef IASR_NO_IGN_PIPE
static const inline ign_pipe __ign_pipe{};
#endif
} // namespace detail
} // namespace iasr
