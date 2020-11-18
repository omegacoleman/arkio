#pragma once

/*! \addtogroup general
 *  \brief This module contains general io objects like pipes or files
 *
 * While network related io objects has many arbitary dependencies like
 * addresses, some of others remain simple like pipes, files, etc. Those io
 * objects inherit from \ref ::ark::seekable_fd and \ref ::ark::nonseekable_fd,
 * and provides functions for constructing them.
 */

#include <ark/general/event_fd.hpp>
#include <ark/general/mem_fd.hpp>
#include <ark/general/normal_file.hpp>
#include <ark/general/pipe_fd.hpp>
