#pragma once

#include <iasr/bindings.hpp>

#include <iasr/buffer/buffer.hpp>
#include <iasr/buffer/traits.hpp>

namespace iasr {
template <typename Byte> class base_buffer_view : public span<Byte> {
public:
  using span<Byte>::span;

  /**
    base_buffer_view(buffer &b) : span<Byte>(b.data(), b.size()) {}

    template <typename U = Byte, typename T = enable_if_t<is_const_v<U>>>
    base_buffer_view(const buffer &b) : span<Byte>(b.data(), b.size()) {}
  **/

  template <typename Buffer, typename T = enable_if_t<is_buffer_like_v<Buffer>>>
  base_buffer_view(Buffer &b) noexcept
      : span<Byte>(reinterpret_cast<Byte *>(b.data()),
                   static_cast<size_t>(b.size())) {}

  template <typename Buffer, typename T = enable_if_t<is_buffer_like_v<Buffer>>,
            typename U = Byte, typename UT = enable_if_t<is_const_v<U>>>
  base_buffer_view(const Buffer &b) noexcept
      : span<Byte>(reinterpret_cast<const Byte *>(b.data()),
                   static_cast<size_t>(b.size())) {}

  template <typename T>
  base_buffer_view(const span<T> &s) noexcept
      : span<Byte>(reinterpret_cast<Byte *>(s.data()), s.size_bytes()) {}

  base_buffer_view<Byte> subview(size_t offset,
                                 size_t count = dynamic_extent) const noexcept {
    auto sp = this->subspan(offset, count);
    return base_buffer_view<Byte>(sp);
  }
};

using buffer_view = base_buffer_view<byte>;
using const_buffer_view = base_buffer_view<const byte>;
} // namespace iasr
