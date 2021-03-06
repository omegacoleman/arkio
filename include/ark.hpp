#pragma once

/*!
 * \brief grand namespace for all non-specific symbols of arkio
 */
namespace ark {} // namespace ark

#include <ark/async.hpp>
#include <ark/buffer.hpp>
#include <ark/general.hpp>
#include <ark/io.hpp>
#include <ark/net.hpp>
#ifndef ARK_NO_COROUTINES
#include <ark/coroutine.hpp>
#endif

#include <ark/misc/context_exit_guard.hpp>
