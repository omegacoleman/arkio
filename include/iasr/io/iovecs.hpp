#pragma once

#include <iasr/bindings.hpp>
#include <iasr/clinux.hpp>

namespace iasr {
template <class Buffer>
inline clinux::iovec to_iovec(const Buffer &buf) noexcept {
  static_assert(is_buffer_like_v<Buffer>);
  return clinux::iovec{
      .iov_base = static_cast<void *>(
          const_cast<typename Buffer::value_type *>(buf.data())),
      .iov_len = buf.size()};
}

template <class OutputIt, class BufferSeq>
inline OutputIt transform_to_iovecs(const BufferSeq &bseq,
                                    OutputIt d_first) noexcept {
  static_assert(is_buffer_seq_v<BufferSeq>);
  return transform(cbegin(bseq), cend(bseq), d_first,
                   to_iovec<typename BufferSeq::value_type>);
}

template <class BufferSeq>
inline auto to_iovec_array_ptr(const BufferSeq &bseq) noexcept {
  auto ret = vector<clinux::iovec>(bseq.size());
  transform_to_iovecs(bseq, ret.begin());
  return move(ret);
}
} // namespace iasr
