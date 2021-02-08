#pragma once

#include <ark/bindings.hpp>

#include <ark/misc/concepts_polyfill.hpp>

namespace ark {

namespace concepts {

/*! \addtogroup io
 *  @{
 */

/*!\class ark::concepts::Fd
 * \remark this is a c++20 concept
 * \brief denotes a type which resembles a fildes to kernel
 */

/*!\fn int ark::concepts::Fd::get() noexcept
 * \brief returns the undelying fildes
 */

/*! \cond CXX20_CONCEPTS */
template <class T> concept Fd = requires(T f) {
  { f.get() }
  noexcept->same_as<int>;
};
/*! \endcond */

/*!\class ark::concepts::Seekable
 * \remark this is a c++20 concept
 * \brief has a set of interface related to offsets for io operations
 */

/*!\fn void ark::concepts::Seekable::seek() noexcept
 * \brief sets the current offset
 */

/*!\fn void ark::concepts::Seekable::feed(off_t len) noexcept
 * \brief denotes an io operation succeeded with length
 *
 * often implemented as seek(offset() + len)
 */

/*!\fn off_t ark::concepts::Seekable::offset() noexcept
 * \brief returns the current offset
 */

/*! \cond CXX20_CONCEPTS */
template <class T> concept Seekable = requires(T u, clinux::off_t off) {
  u.seek(off);
  u.feed(off);
  { u.offset() }
  noexcept->same_as<clinux::off_t>;
};
/*! \endcond */

/*!\class ark::concepts::SeekableFd
 * \remark this is a c++20 concept
 * \brief a type which satisfies \ref ::ark::concepts::Seekable and \ref
 * ::ark::concepts::Fd
 */

/*! \cond CXX20_CONCEPTS */
template <class T> concept SeekableFd = Fd<T> &&Seekable<T>;
/*! \endcond */

/*!\class ark::concepts::NonseekableFd
 * \remark this is a c++20 concept
 * \brief a type which does NOT satisfy \ref ::ark::concepts::Seekable but do
 * satisfy \ref ::ark::concepts::Fd
 */

/*! \cond CXX20_CONCEPTS */
template <class T> concept NonseekableFd = Fd<T> && (!Seekable<T>);
/*! \endcond */

/*!\class ark::concepts::CompletionCondition
 * \remark this is a c++20 concept
 * \brief used with io operations like \ref ::ark::write to resemble where
 * should it stop
 */

/*! \cond CXX20_CONCEPTS */
template <class T>
concept CompletionCondition = requires(T cond, size_t buffer_sz, size_t done) {
  { cond(buffer_sz, done) }
  noexcept->same_as<size_t>;
};
/*! \endcond */

/*! @} */

} // namespace concepts

/*! \cond HIDDEN_CLASSES */

// overload resoilution guidance classes
namespace io_operation {
class read {};
class write {};
} // namespace io_operation

namespace concepts {

namespace internal {

template <class T>
concept IoOperation =
    is_same_v<T, io_operation::read> || is_same_v<T, io_operation::write>;

}

} // namespace concepts

/* \endcond */

} // namespace ark
