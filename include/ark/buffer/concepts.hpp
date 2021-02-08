#pragma once

#include <ark/bindings.hpp>

#include <ark/buffer/sequence.hpp>
#include <ark/misc/concepts_polyfill.hpp>

namespace ark {

namespace concepts {

template <class T> concept MutableBufferSequence = requires(T bseq) {
  requires convertible_to<decltype(*buffer_sequence_begin(bseq)),
                          mutable_buffer>;
  requires convertible_to<decltype(*buffer_sequence_end(bseq)), mutable_buffer>;
};

template <class T> concept ConstBufferSequence = requires(T bseq) {
  requires convertible_to<decltype(*buffer_sequence_begin(bseq)), const_buffer>;
  requires convertible_to<decltype(*buffer_sequence_end(bseq)), const_buffer>;
};

} // namespace concepts

} // namespace ark
