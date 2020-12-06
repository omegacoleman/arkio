#pragma once

#include <ark/bindings.hpp>

namespace ark {

/*! \addtogroup buffer
 *  @{
 */

/*!
 * \brief base type for mutable_buffer and const_buffer
 *
 * base_buffer is a buffer of specified type of chars. It is inherited from
 * gsl::span and could be constructed from it, too. As defined in N4771,
 * buffer types are very similar to spans, so only a few extra functions are
 * added here. Most common functions, such as begin() or operator[], were
 * implemented and documented by gsl::span.
 *
 * Notice that the base_buffer represents an unowning view of the underlying
 * type, and marks it for usage of buffered io operations like ark::sync::read
 * and ark::sync:write. It's the user's responsibility to keep the underlying
 * memory available before any io operation completes.
 *
 * \tparam CharT type of chars, if const qualified, then the buffer is immutable
 * and the data should not get modified. Ill-formed if sizeof(CharT) != 1
 */
template <typename CharT> class base_buffer : public span<CharT> {
public:
  using span<CharT>::span;

  static_assert(sizeof(CharT) == 1);

  /*! \brief construct from a gsl::span */
  base_buffer(span<CharT> sp) : span<CharT>(move(sp)) {}

  /*! \brief construct from pointers with size */
  base_buffer(void *p, size_t n) : span<CharT>(static_cast<CharT *>(p), n) {}

  /*! \brief construct from const pointers with size
   *
   * internally uses static_cast, so won't work with non-const CharT
   */
  base_buffer(const void *p, size_t n)
      : span<CharT>(static_cast<CharT *>(p), n) {}

  /*! \brief ascending the buffer */
  base_buffer<CharT> &operator+=(size_t offset) noexcept {
    *this = this->subspan(offset);
    return *this;
  }
};

/*!
 * \brief a view to a span of memory as mutable buffer
 *
 * \remark [buffer.mutable] as defined in N4771, see \ref info_network
 */
using mutable_buffer = base_buffer<char>;

/*!
 * \brief a view to a span of memory as const buffer
 *
 * \remark [buffer.const] as defined in N4771, see \ref info_network
 */
using const_buffer = base_buffer<const char>;

/*!
 * \brief returns an ascended buffer by n bytes
 *
 * \remark [buffer.arithmetic] as defined in N4771, see \ref info_network
 */
inline mutable_buffer operator+(const mutable_buffer &b, size_t n) noexcept {
  return {static_cast<char *>(b.data()) + min(n, b.size()),
          b.size() - min(n, b.size())};
}

/*! \copydoc operator+(const mutable_buffer &,size_t) */
inline mutable_buffer operator+(size_t n, const mutable_buffer &b) noexcept {
  return operator+(b, n);
}

/*! \copydoc operator+(const mutable_buffer &,size_t) */
inline const_buffer operator+(const const_buffer &b, size_t n) noexcept {
  return {static_cast<const char *>(b.data()) + min(n, b.size()),
          b.size() - min(n, b.size())};
}

/*! \copydoc operator+(const mutable_buffer &,size_t) */
inline const_buffer operator+(size_t n, const const_buffer &b) noexcept {
  return operator+(b, n);
}

/*!
 * \brief creates a \ref ::ark::mutable_buffer or \ref ::ark::const_buffer
 *
 * depending on the undelying type, creates a \ref ::ark::mutable_buffer or \ref
 * ::ark::const_buffer, an additional size_t could also be served to limit the
 * size of the buffer returned.
 *
 * \remark [buffer.creation] as defined in N4771, see \ref info_network
 */
inline mutable_buffer buffer(void *p, size_t n) noexcept { return {p, n}; }

/*! \copydoc buffer(void *,size_t) */
inline const_buffer buffer(const void *p, size_t n) noexcept { return {p, n}; }

/*! \copydoc buffer(void *,size_t) */
inline mutable_buffer buffer(const mutable_buffer &b) noexcept { return b; }

/*! \copydoc buffer(void *,size_t) */
inline mutable_buffer buffer(const mutable_buffer &b, size_t n) noexcept {
  return {b.data(), min(b.size(), n)};
}

/*! \copydoc buffer(void *,size_t) */
inline const_buffer buffer(const const_buffer &b) noexcept { return b; }

/*! \copydoc buffer(void *,size_t) */
inline const_buffer buffer(const const_buffer &b, size_t n) noexcept {
  return {b.data(), min(b.size(), n)};
}

/*! \cond DETAIL_MACROS */

#define ARK_BUFFER_CONSTRAIN_T                                                 \
  static_assert(is_trivially_copyable_v<T> || is_standard_layout_v<T>,         \
                "T must be TriviallyCopyable or StandardLayout")

#define ARK_BUFFER_IMPL_DATA                                                   \
  return buffer(begin(data) != end(data) ? std::addressof(*begin(data))        \
                                         : nullptr,                            \
                (end(data) - begin(data)) * sizeof(*begin(data)));

#define ARK_BUFFER_IMPL_DATA_CONSTRAIN_T                                       \
  ARK_BUFFER_CONSTRAIN_T;                                                      \
  return buffer(begin(data) != end(data) ? std::addressof(*begin(data))        \
                                         : nullptr,                            \
                (end(data) - begin(data)) * sizeof(*begin(data)));

#define ARK_BUFFER_IMPL_DATA_N return buffer(buffer(data), n);

#define ARK_BUFFER_IMPL_DATA_N_CONSTRAIN_T                                     \
  ARK_BUFFER_CONSTRAIN_T;                                                      \
  return buffer(buffer(data), n);

/*! \endcond */

/*! \copydoc buffer(void *,size_t) */
template <class T, size_t N> mutable_buffer buffer(T (&data)[N]) noexcept {
  ARK_BUFFER_IMPL_DATA_CONSTRAIN_T
}

/*! \copydoc buffer(void *,size_t) */
template <class T, size_t N> const_buffer buffer(const T (&data)[N]) noexcept {
  ARK_BUFFER_IMPL_DATA_CONSTRAIN_T
}

/*! \copydoc buffer(void *,size_t) */
template <class T, size_t N> mutable_buffer buffer(array<T, N> &data) noexcept {
  ARK_BUFFER_IMPL_DATA_CONSTRAIN_T
}

/*! \copydoc buffer(void *,size_t) */
template <class T, size_t N>
const_buffer buffer(array<const T, N> &data) noexcept {
  ARK_BUFFER_IMPL_DATA_CONSTRAIN_T
}

/*! \copydoc buffer(void *,size_t) */
template <class T, size_t N>
const_buffer buffer(const array<T, N> &data) noexcept {
  ARK_BUFFER_IMPL_DATA_CONSTRAIN_T
}

/*! \copydoc buffer(void *,size_t) */
template <class T, class Allocator>
mutable_buffer buffer(vector<T, Allocator> &data) noexcept {
  ARK_BUFFER_IMPL_DATA_CONSTRAIN_T
}

/*! \copydoc buffer(void *,size_t) */
template <class T, class Allocator>
const_buffer buffer(const vector<T, Allocator> &data) noexcept {
  ARK_BUFFER_IMPL_DATA_CONSTRAIN_T
}

/*! \copydoc buffer(void *,size_t) */
template <class CharT, class Traits, class Allocator>
mutable_buffer buffer(basic_string<CharT, Traits, Allocator> &data) noexcept {
  ARK_BUFFER_IMPL_DATA
}

/*! \copydoc buffer(void *,size_t) */
template <class CharT, class Traits, class Allocator>
const_buffer
buffer(const basic_string<CharT, Traits, Allocator> &data) noexcept {
  ARK_BUFFER_IMPL_DATA
}

/*! \copydoc buffer(void *,size_t) */
template <class CharT, class Traits>
const_buffer buffer(basic_string_view<CharT, Traits> data) noexcept {
  ARK_BUFFER_IMPL_DATA
}

/*! \copydoc buffer(void *,size_t) */
template <class T, size_t N>
mutable_buffer buffer(T (&data)[N], size_t n) noexcept {
  ARK_BUFFER_IMPL_DATA_N_CONSTRAIN_T
}

/*! \copydoc buffer(void *,size_t) */
template <class T, size_t N>
const_buffer buffer(const T (&data)[N], size_t n) noexcept {
  ARK_BUFFER_IMPL_DATA_N_CONSTRAIN_T
}

/*! \copydoc buffer(void *,size_t) */
template <class T, size_t N>
mutable_buffer buffer(array<T, N> &data, size_t n) noexcept {
  ARK_BUFFER_IMPL_DATA_N_CONSTRAIN_T
}

/*! \copydoc buffer(void *,size_t) */
template <class T, size_t N>
const_buffer buffer(array<const T, N> &data, size_t n) noexcept {
  ARK_BUFFER_IMPL_DATA_N_CONSTRAIN_T
}

/*! \copydoc buffer(void *,size_t) */
template <class T, size_t N>
const_buffer buffer(const array<T, N> &data, size_t n) noexcept {
  ARK_BUFFER_IMPL_DATA_N_CONSTRAIN_T
}

/*! \copydoc buffer(void *,size_t) */
template <class T, class Allocator>
mutable_buffer buffer(vector<T, Allocator> &data, size_t n) noexcept {
  ARK_BUFFER_IMPL_DATA_N_CONSTRAIN_T
}

/*! \copydoc buffer(void *,size_t) */
template <class T, class Allocator>
const_buffer buffer(const vector<T, Allocator> &data, size_t n) noexcept {
  ARK_BUFFER_IMPL_DATA_N_CONSTRAIN_T
}

/*! \copydoc buffer(void *,size_t) */
template <class CharT, class Traits, class Allocator>
mutable_buffer buffer(basic_string<CharT, Traits, Allocator> &data,
                      size_t n) noexcept {
  ARK_BUFFER_IMPL_DATA_N
}

/*! \copydoc buffer(void *,size_t) */
template <class CharT, class Traits, class Allocator>
const_buffer buffer(const basic_string<CharT, Traits, Allocator> &data,
                    size_t n) noexcept {
  ARK_BUFFER_IMPL_DATA_N
}

/*! \copydoc buffer(void *,size_t) */
template <class CharT, class Traits>
const_buffer buffer(basic_string_view<CharT, Traits> data, size_t n) noexcept {
  ARK_BUFFER_IMPL_DATA_N
}

#undef ARK_BUFFER_CONSTRAIN_T
#undef ARK_BUFFER_IMPL_DATA_CONSTRAIN_T
#undef ARK_BUFFER_IMPL_DATA_N_CONSTRAIN_T
#undef ARK_BUFFER_IMPL_DATA
#undef ARK_BUFFER_IMPL_DATA_N

/*! @} */

} // namespace ark
