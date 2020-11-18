#pragma once

#include <ark/bindings.hpp>

#include <ark/async.hpp>

namespace ark {

/*! \addtogroup io
 *  @{
 */

/*!
 * \brief An io object, denotes a non lseek()-able fd as defined in kernel.
 */
class nonseekable_fd : public with_async_context {
private:
  struct base_type {
    int fd_;

    base_type(int fd_int) noexcept : fd_(fd_int) {}

    result<void> close() noexcept {
      if (clinux::close(fd_) != 0) {
        return clinux::errno_ec();
      }
      return success();
    }

    int get() const noexcept { return fd_; }

    ~base_type() noexcept { static_cast<void>(this->close()); }
  };
  unique_ptr<base_type> base_;

public:
  static constexpr bool seekable = false;

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

protected:
  /*!
   * \brief constructs an empty nonseekable_fd
   */
  nonseekable_fd() noexcept {}

  /*!
   * \brief constructs an nonseekable_fd with fildes fd_int
   */
  nonseekable_fd(int fd_int) noexcept : base_(make_unique<base_type>(fd_int)) {}
};

/*! @} */

} // namespace ark
