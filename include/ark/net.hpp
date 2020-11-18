#pragma once

/*! \addtogroup net
 *  \brief This module implements network related stuff.
 *
 * Due to the complexity of network stack, the common stuff, like network
 * address, is placed under namespace \ref ::ark::net, while specific protocol
 * implementions placed under sub-namespaces, like \ref ::ark::net::tcp.
 */

namespace ark {
/*!
 * \brief namespace for module \ref net
 */
namespace net {}
} // namespace ark

#include <ark/net/address.hpp>

#include <ark/net/tcp.hpp>
