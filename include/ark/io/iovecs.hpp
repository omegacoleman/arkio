#pragma once

#include <ark/bindings.hpp>
#include <ark/clinux.hpp>

namespace ark {

inline clinux::iovec to_iovec(const const_buffer &buf) noexcept {
  return clinux::iovec{.iov_base =
                           static_cast<void *>(const_cast<char *>(buf.data())),
                       .iov_len = buf.size()};
}

template <class OutputIt, class ConstBufferSequence>
inline OutputIt transform_to_iovecs(const ConstBufferSequence &bseq,
                                    size_t skip, size_t max_len,
                                    OutputIt d_it) noexcept {
  for (auto it = buffer_sequence_begin(bseq); it != buffer_sequence_end(bseq);
       ++it) {
    const_buffer b{*it};
    if (b.size() <= skip) {
      skip -= b.size();
      continue;
    }
    b += skip;
    skip = 0;

    if (max_len < b.size()) {
      b = buffer(b, max_len);
    }

    d_it = to_iovec(b);
    d_it++;
    max_len -= b.size();
    if (max_len == 0) {
      break;
    }
  }
  return d_it;
}

template <class ConstBufferSequence>
inline auto to_iovecs(const ConstBufferSequence &bseq, size_t skip,
                      size_t max_len) noexcept {
  vector<clinux::iovec> ret;
  ret.reserve(bseq.size());
  transform_to_iovecs(bseq, skip, max_len, back_inserter(ret));
  return move(ret);
}
} // namespace ark
