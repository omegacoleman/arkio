#pragma once

namespace ark {
/*!
 * \brief contains apis that blocks until completion
 */
namespace sync {}
} // namespace ark

#include <ark/bindings.hpp>

#include <ark/io/completion_condition.hpp>
#include <ark/io/nonseekable/sync.hpp>
#include <ark/io/seekable/sync.hpp>
