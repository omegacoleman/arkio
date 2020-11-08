#pragma once

#include <iasr/bindings.hpp>

namespace iasr {
template <typename CharT> class base_buffer : public span<CharT> {
public:
  using span<CharT>::span;

  static_assert(sizeof(CharT) == 1);

  base_buffer(span<CharT> sp) : span<CharT>(move(sp)) {}

  base_buffer(void *p, size_t n) : span<CharT>(static_cast<CharT *>(p), n) {}
  base_buffer(const void *p, size_t n)
      : span<CharT>(static_cast<CharT *>(p), n) {}

  // base_buffer(base_buffer<remove_const_t<CharT>> o) :span<CharT>(o) {}

  base_buffer<CharT> &operator+=(size_t offset) noexcept {
    *this = this->subspan(offset);
    return *this;
  }
};

// N4771 [buffer.mutable]
using mutable_buffer = base_buffer<char>;

// N4771 [buffer.const]
using const_buffer = base_buffer<const char>;

// N4771 [buffer.arithmetic]

mutable_buffer operator+(const mutable_buffer &b, size_t n) noexcept {
  return {static_cast<char *>(b.data()) + min(n, b.size()),
          b.size() - min(n, b.size())};
}

mutable_buffer operator+(size_t n, const mutable_buffer &b) noexcept {
  return operator+(b, n);
}

const_buffer operator+(const const_buffer &b, size_t n) noexcept {
  return {static_cast<const char *>(b.data()) + min(n, b.size()),
          b.size() - min(n, b.size())};
}

const_buffer operator+(size_t n, const const_buffer &b) noexcept {
  return operator+(b, n);
}

// N4771 [buffer.creation]

mutable_buffer buffer(void *p, size_t n) noexcept { return {p, n}; }

const_buffer buffer(const void *p, size_t n) noexcept { return {p, n}; }

mutable_buffer buffer(const mutable_buffer &b) noexcept { return b; }

mutable_buffer buffer(const mutable_buffer &b, size_t n) noexcept {
  return {b.data(), min(b.size(), n)};
}

const_buffer buffer(const const_buffer &b) noexcept { return b; }

const_buffer buffer(const const_buffer &b, size_t n) noexcept {
  return {b.data(), min(b.size(), n)};
}

#define IASR_BUFFER_CONSTRAIN_T                                                \
  static_assert(is_trivially_copyable_v<T> || is_standard_layout_v<T>,         \
                "T must be TriviallyCopyable or StandardLayout")

#define IASR_BUFFER_IMPL_DATA                                                  \
  return buffer(begin(data) != end(data) ? std::addressof(*begin(data))        \
                                         : nullptr,                            \
                (end(data) - begin(data)) * sizeof(*begin(data)));

#define IASR_BUFFER_IMPL_DATA_CONSTRAIN_T                                      \
  IASR_BUFFER_CONSTRAIN_T;                                                     \
  return buffer(begin(data) != end(data) ? std::addressof(*begin(data))        \
                                         : nullptr,                            \
                (end(data) - begin(data)) * sizeof(*begin(data)));

#define IASR_BUFFER_IMPL_DATA_N return buffer(buffer(data), n);

#define IASR_BUFFER_IMPL_DATA_N_CONSTRAIN_T                                    \
  IASR_BUFFER_CONSTRAIN_T;                                                     \
  return buffer(buffer(data), n);

template <class T, size_t N> mutable_buffer buffer(T (&data)[N]) noexcept {
  IASR_BUFFER_IMPL_DATA_CONSTRAIN_T
}

template <class T, size_t N> const_buffer buffer(const T (&data)[N]) noexcept {
  IASR_BUFFER_IMPL_DATA_CONSTRAIN_T
}

template <class T, size_t N> mutable_buffer buffer(array<T, N> &data) noexcept {
  IASR_BUFFER_IMPL_DATA_CONSTRAIN_T
}

template <class T, size_t N>
const_buffer buffer(array<const T, N> &data) noexcept {
  IASR_BUFFER_IMPL_DATA_CONSTRAIN_T
}

template <class T, size_t N>
const_buffer buffer(const array<T, N> &data) noexcept {
  IASR_BUFFER_IMPL_DATA_CONSTRAIN_T
}

template <class T, class Allocator>
mutable_buffer buffer(vector<T, Allocator> &data) noexcept {
  IASR_BUFFER_IMPL_DATA_CONSTRAIN_T
}

template <class T, class Allocator>
const_buffer buffer(const vector<T, Allocator> &data) noexcept {
  IASR_BUFFER_IMPL_DATA_CONSTRAIN_T
}

template <class CharT, class Traits, class Allocator>
mutable_buffer buffer(basic_string<CharT, Traits, Allocator> &data) noexcept {
  IASR_BUFFER_IMPL_DATA
}

template <class CharT, class Traits, class Allocator>
const_buffer
buffer(const basic_string<CharT, Traits, Allocator> &data) noexcept {
  IASR_BUFFER_IMPL_DATA
}

template <class CharT, class Traits>
const_buffer buffer(basic_string_view<CharT, Traits> data) noexcept {
  IASR_BUFFER_IMPL_DATA
}

template <class T, size_t N>
mutable_buffer buffer(T (&data)[N], size_t n) noexcept {
  IASR_BUFFER_IMPL_DATA_N_CONSTRAIN_T
}

template <class T, size_t N>
const_buffer buffer(const T (&data)[N], size_t n) noexcept {
  IASR_BUFFER_IMPL_DATA_N_CONSTRAIN_T
}

template <class T, size_t N>
mutable_buffer buffer(array<T, N> &data, size_t n) noexcept {
  IASR_BUFFER_IMPL_DATA_N_CONSTRAIN_T
}

template <class T, size_t N>
const_buffer buffer(array<const T, N> &data, size_t n) noexcept {
  IASR_BUFFER_IMPL_DATA_N_CONSTRAIN_T
}

template <class T, size_t N>
const_buffer buffer(const array<T, N> &data, size_t n) noexcept {
  IASR_BUFFER_IMPL_DATA_N_CONSTRAIN_T
}

template <class T, class Allocator>
mutable_buffer buffer(vector<T, Allocator> &data, size_t n) noexcept {
  IASR_BUFFER_IMPL_DATA_N_CONSTRAIN_T
}

template <class T, class Allocator>
const_buffer buffer(const vector<T, Allocator> &data, size_t n) noexcept {
  IASR_BUFFER_IMPL_DATA_N_CONSTRAIN_T
}

template <class CharT, class Traits, class Allocator>
mutable_buffer buffer(basic_string<CharT, Traits, Allocator> &data,
                      size_t n) noexcept {
  IASR_BUFFER_IMPL_DATA_N
}

template <class CharT, class Traits, class Allocator>
const_buffer buffer(const basic_string<CharT, Traits, Allocator> &data,
                    size_t n) noexcept {
  IASR_BUFFER_IMPL_DATA_N
}

template <class CharT, class Traits>
const_buffer buffer(basic_string_view<CharT, Traits> data, size_t n) noexcept {
  IASR_BUFFER_IMPL_DATA_N
}

#undef IASR_BUFFER_CONSTRAIN_T
#undef IASR_BUFFER_IMPL_DATA_CONSTRAIN_T
#undef IASR_BUFFER_IMPL_DATA_N_CONSTRAIN_T
#undef IASR_BUFFER_IMPL_DATA
#undef IASR_BUFFER_IMPL_DATA_N

} // namespace iasr
