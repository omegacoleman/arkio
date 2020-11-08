#pragma once

#include <iasr/bindings.hpp>

#include <iasr/buffer/buffer.hpp>

namespace iasr {

// N4771 [buffer.seq.access]

const mutable_buffer *buffer_sequence_begin(const mutable_buffer &b) noexcept {
  return addressof(b);
}

const mutable_buffer *buffer_sequence_end(const mutable_buffer &b) noexcept {
  return addressof(b) + 1;
}

const const_buffer *buffer_sequence_begin(const const_buffer &b) noexcept {
  return addressof(b);
}

const const_buffer *buffer_sequence_end(const const_buffer &b) noexcept {
  return addressof(b) + 1;
}

template <class C> auto buffer_sequence_begin(C &c) noexcept {
  return c.begin();
}

template <class C> auto buffer_sequence_begin(const C &c) noexcept {
  return c.begin();
}

template <class C> auto buffer_sequence_end(C &c) noexcept { return c.end(); }

template <class C> auto buffer_sequence_end(const C &c) noexcept {
  return c.end();
}

// N4771 [buffer.size]

template <class ConstBufferSequence>
size_t buffer_size(const ConstBufferSequence &buffers) noexcept {
  size_t total_size = 0;
  auto i = buffer_sequence_begin(buffers);
  auto end = buffer_sequence_end(buffers);
  for (; i != end; ++i) {
    const_buffer b(*i);
    total_size += b.size();
  }
  return total_size;
}

// N4771 [buffer.copy]

template <class MutableBufferSequence, class ConstBufferSequence>
size_t buffer_copy(const MutableBufferSequence &dest,
                   const ConstBufferSequence &source, size_t max_size) {
  auto it_src = buffer_sequence_begin(source);
  auto it_dst = buffer_sequence_begin(dest);
  auto end_src = buffer_sequence_end(source);
  auto end_dst = buffer_sequence_end(dest);

  size_t done_sz = 0;
  const_buffer curr_src(*it_src);
  mutable_buffer curr_dst(*it_dst);
  while ((it_src != end_src) && (it_dst != end_dst)) {
    size_t curr_sz = min(curr_src.size(), curr_dst.size());
    std::memcpy(curr_dst.data(), curr_src.data(), curr_sz);
    done_sz += curr_sz;

    if (max_size == curr_sz) {
      return max_size;
    } else {
      max_size -= curr_sz;
    }
    if (curr_src.size() == curr_sz) {
      it_src++;
      if (it_src != end_src)
        curr_src = const_buffer(*it_src);
    } else {
      curr_src += curr_sz;
    }
    if (curr_dst.size() == curr_sz) {
      it_dst++;
      if (it_dst != end_dst)
        curr_dst = mutable_buffer(*it_dst);
    } else {
      curr_dst += curr_sz;
    }
  }
}

}; // namespace iasr
