#pragma once

#include <ark/bindings.hpp>

#include <ark/async.hpp>

namespace ark {

/*! \addtogroup io
 *  @{
 */

/*!
 * \brief An io object, denotes a lseek()-able fd as defined in kernel.
 *
 * Stores the offset within, as the kernel offset has race problems and
 * undefined behaviors on error. The offset is auto increased on completion of
 * io operations.
 */
class seekable_fd : public with_async_context {
private:
  struct base_type {
    int fd_;
    clinux::off_t offset_;

    base_type(int fd_int) noexcept : fd_(fd_int) {}

    result<void> close() noexcept {
      if (clinux::close(fd_) != 0) {
        return clinux::errno_ec();
      }
      return success();
    }

    int get() const noexcept { return fd_; }

    ~base_type() noexcept { static_cast<void>(this->close()); }

    clinux::off_t offset() const noexcept { return offset_; }
    void offset(clinux::off_t o) noexcept { offset_ = o; }
  };
  unique_ptr<base_type> base_;

public:
  static constexpr bool seekable = true;

  /*!
   * \brief call close() on the fd
   *
   * if the object is empty, the operation succeeds.
   */
  result<void> close() noexcept {
    if (base_) {
      return base_->close();
    }
    return success();
  }

  /*!
   * \brief returns the int fildes
   */
  int get() const noexcept {
    Expects(base_);
    return base_->get();
  }

  /*!
   * \brief returns the offset of next io operations
   */
  clinux::off_t offset() const noexcept {
    Expects(base_);
    return base_->offset();
  }

  /*!
   * \brief sets the offset of next io operations
   */
  void seek(clinux::off_t off) noexcept {
    Expects(base_);
    base_->offset(off);
  }

  /*!
   * \brief similar to seek(offset() + rel_off)
   */
  void feed(clinux::off_t rel_off) noexcept {
    Expects(base_);
    base_->offset(base_->offset() + rel_off);
  }

protected:
  /*!
   * \brief constructs an empty seekable_fd
   */
  seekable_fd() noexcept {}

  /*!
   * \brief constructs an seekable_fd with fildes fd_int and offset set to 0
   */
  seekable_fd(int fd_int) noexcept : base_(make_unique<base_type>(fd_int)) {}
};

/*! @} */

} // namespace ark
