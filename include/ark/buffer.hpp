#pragma once

/*! \addtogroup buffer
 *  \brief This module provides buffer implemention as defined in N4771
 * Networking TS draft's 'Buffer' section.
 *
 * \see \ref info_network
 *
 *  Buffers are built upon gsl::span, and the buffers all inherit from spans.
 * See \ref ::ark::base_buffer for details. gsl::span has bound checks, which
 * also make buffers of this module 'safe buffers' with bound checks, too.
 *
 * for more information about gsl: https://github.com/microsoft/GSL/
 */

#include <ark/buffer/buffer.hpp>
#include <ark/buffer/concepts.hpp>
#include <ark/buffer/sequence.hpp>
