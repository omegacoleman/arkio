#pragma once

namespace ark {
/*!
 * \brief contains apis that invokes a given \ref ::ark::callback on completion
 */
namespace async {}
} // namespace ark

#include <ark/bindings.hpp>

#include <ark/io/completion_condition.hpp>
#include <ark/io/nonseekable/async.hpp>
#include <ark/io/seekable/async.hpp>
