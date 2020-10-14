#pragma once

#include <iasr/bindings.hpp>

namespace iasr {
class buffer {
private:
  unique_ptr<byte[]> data_;
  span<byte> view_;

public:
  using value_type = byte;
  using iterator = span<byte>::iterator;

  buffer() noexcept : data_(), view_() {}

  buffer(const size_t len) noexcept
      : data_(make_unique<byte[]>(len)), view_(data_.get(), len) {}

  // fill ctor
  buffer(const size_t len, const byte f) noexcept
      : data_(make_unique<byte[]>(len)), view_(data_.get(), len) {
    fill(view_.begin(), view_.end(), f);
  }

  // range ctor
  template <class InputIt, typename T = enable_if_t<is_convertible_v<
                               typename InputIt::value_type, byte>>>
  buffer(InputIt first, InputIt last) noexcept
      : data_(make_unique<byte[]>(last - first)),
        view_(data_.get(), (last - first)) {
    copy(first, last, view_.begin());
  }

  size_t size() const noexcept { return view_.size(); }

  byte *data() noexcept { return view_.data(); }

  const byte *data() const noexcept { return view_.data(); }

  auto begin() noexcept { return view_.begin(); }

  auto end() noexcept { return view_.end(); }

  auto cbegin() const noexcept { return view_.begin(); }

  auto cend() const noexcept { return view_.end(); }

  auto rbegin() noexcept { return view_.rbegin(); }

  auto rend() noexcept { return view_.rend(); }
};
} // namespace iasr
