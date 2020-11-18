#pragma once

/*! \cond FILE_NOT_DOCUMENTED */

#include <ark/bindings/bindings.hpp>
#include <ark/bindings/clinux.hpp>

namespace ark {
namespace detail {
class ign_pipe {
public:
  ign_pipe() noexcept { clinux::signal(SIGPIPE, SIG_IGN); }
};

#ifndef ARK_NO_IGN_PIPE
static const inline ign_pipe __ign_pipe{};
#endif
} // namespace detail
} // namespace ark

/*! \endcond */
