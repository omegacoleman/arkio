#pragma once

/*! \addtogroup io
 *  \brief This module provides base class for io objects and corresponding read
 * / write functions.
 *
 *  The read / write functions were provided in three flavours, under three
 * namespaces:
 *  - \ref ::ark::sync
 *  - \ref ::ark::async
 *  - \ref ::ark::coro
 *
 *  The async io functions takes an additional \ref ::ark::callback which was
 * invoked by \ref ::ark::async_context on completion. The coro io functions
 * are Awaitables, which could be used with co_await, and will be resumed in
 * \ref ::ark::async_context on completion.
 *
 *  The io objects can be bundled with an \ref ::ark::async_context (by
 * inheriting \ref ::ark::with_async_context), which is necessary for usage with
 * async or coro variants of io functions.
 */

#include <ark/io/async.hpp>
#include <ark/io/completion_condition.hpp>
#include <ark/io/fd.hpp>
#include <ark/io/sync.hpp>

#ifndef ARK_NO_COROUTINES
#include <ark/io/coro.hpp>
#endif
