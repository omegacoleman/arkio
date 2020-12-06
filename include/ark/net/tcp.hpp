#pragma once

namespace ark {
namespace net {
/*!
 * \brief implements TCP/IP related classes and functions
 */
namespace tcp {}
} // namespace net
} // namespace ark

#include <ark/net/tcp/acceptor.hpp>
#include <ark/net/tcp/socket.hpp>

#include <ark/net/tcp/general.hpp>

#include <ark/net/tcp/async.hpp>
#include <ark/net/tcp/sync.hpp>
#ifndef ARK_NO_COROUTINES
#include <ark/net/tcp/coro.hpp>
#endif
