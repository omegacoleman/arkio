#pragma once

/*! \addtogroup coroutine
 *  \brief This module provides tools used by coroutines ts related programming.
 *
 * Unlike other coroutine libraries, e.g. libunifex or cppcoro, this module is
 * centered with some most basic and simple tools, like proposed by p1056,
 * task<> and co_async.
 */

#include <ark/coroutine/awaitable_op.hpp>
#include <ark/coroutine/co_async.hpp>
#include <ark/coroutine/fire_and_forget.hpp>
#include <ark/coroutine/task.hpp>
