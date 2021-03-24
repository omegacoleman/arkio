#pragma once

/*! \cond FILE_NOT_DOCUMENTED */

#include <ark/bindings.hpp>

#include <ark/async/io_uring/io_uring.hpp>

namespace ark {
namespace io_uring_async {
using callback_t = unique_function<void(result<long> ret)>;

class base_singlethread_uring_async_context {
public:
  using token_t = uintptr_t;
  using callback_t = unique_function<void(result<long> ret)>;

private:
  io_uring r_;
  map<token_t, callback_t> callbacks_;

  mutex m_submission_;
  mutex m_callbacks_;

  token_t callbacks_idx_;

  static const constexpr unsigned batch_size = 1024;

  int waker_evfd_;

  bool inited_;
  bool exiting_;
  error_code exiting_error_;

  template <typename PrepSqeCallable // void prep_sqe(sqe_ref) noexcept
            >
  result<token_t> base_add_sqe(const PrepSqeCallable &prep_sqe) noexcept {
    OUTCOME_TRY(sqe, r_.get_sqe());
    prep_sqe(sqe);
    token_t tok = callbacks_idx_;
    sqe.set_data(reinterpret_cast<void *>(callbacks_idx_++));
#ifdef ARK_ADVANCED_DEBUG_VERBOSITY
    sqe.dump();
#endif
    auto ret = wake();
    if (ret.has_error()) {
      cancel(tok);
      return ret.as_failure();
    }
    return tok;
  }

  result<void> add_waker() noexcept {
    auto ret =
        add_sqe([this](sqe_ref sqe) { sqe.prep_poll_add(waker_evfd_, POLLIN); },
                [this](result<long> ret) {
                  if (!ret) {
                    this->exit(ret.error());
                    return;
                  }
                  array<char, sizeof(uint64_t)> b;
                  auto read_ret = clinux::read(waker_evfd_, b.data(), b.size());
                  if (read_ret == -1) {
                    this->exit(errno_ec());
                    return;
                  }
                  auto next_ret = this->add_waker();
                  if (next_ret.has_error())
                    this->exit(next_ret.error());
                });
    if (ret.has_error())
      return ret.as_failure();
    return success();
  }

  void submit() noexcept {
    lock_guard<mutex> g_submission(m_submission_);
    int s = r_.submit();
#ifdef ARK_ADVANCED_DEBUG_VERBOSITY
    cerr << "### SUBMITTED : " << s << endl;
#endif
  }

public:
  base_singlethread_uring_async_context() noexcept
      : inited_(false), exiting_(false), callbacks_idx_(0) {}

  result<void> init() noexcept {
    Expects(!inited_);

    int waker_ret = clinux::eventfd(0, 0);
    if (waker_ret == -1)
      return errno_ec();
    waker_evfd_ = waker_ret;

    auto ret = r_.queue_init(batch_size, 0);
    if (ret.has_error())
      return ret.as_failure();

    inited_ = true;
    return add_waker();
  }

  void exit() noexcept {
    exiting_ = true;
    auto ret = wake();
    if (ret.has_error()) {
      cerr << "error occurred, waking up thread failed : "
           << ret.error().message() << endl;
      abort();
    }
  }

  void exit(result<void> ret) noexcept {
    if (ret.has_error()) {
      exiting_error_ = ret.error();
    }
    exit();
  }

  result<void> wake() noexcept {
    static uint64_t d{1};
    auto write_ret = clinux::write(waker_evfd_, &d, sizeof(d));
    if (write_ret == -1) {
      return errno_ec();
    }
    return success();
  }

  template <typename PrepSqeCallable // void prep_sqe(sqe_ref) noexcept
            >
  result<token_t> add_sqe(const PrepSqeCallable &prep_sqe) noexcept {
    lock_guard<mutex> g_submission(m_submission_);
    return base_add_sqe(prep_sqe);
  }

  template <typename PrepSqeCallable // void prep_sqe(sqe_ref) noexcept
            >
  result<token_t> add_sqe(const PrepSqeCallable &prep_sqe,
                          callback_t &&callback) noexcept {
    lock_guard<mutex> g_submission(m_submission_);
    lock_guard<mutex> g_callbacks(m_callbacks_);
    OUTCOME_TRY(tok, base_add_sqe(prep_sqe));
    callbacks_[tok] = forward<callback_t>(callback);
    return tok;
  }

  void cancel(const token_t token) noexcept {
    lock_guard<mutex> g_callbacks(m_callbacks_);
    callbacks_.erase(token);
  }

  result<void> run() noexcept {
    for (;;) {
      if (exiting_) {
        exiting_ = false;
        if (exiting_error_) {
          error_code ret = exiting_error_;
          exiting_error_ = {};
          return ret;
        } else {
          return success();
        }
      }
      submit();
      auto ret = r_.wait();
      if (ret.has_error())
        return ret.as_failure();
#ifdef ARK_ADVANCED_DEBUG_VERBOSITY
      cerr << "### WOKE" << endl;
#endif

      array<owning_cqe_ref, batch_size> cqe_buffer;
      auto cqe_end = r_.peek_batch_cqe(cqe_buffer.begin(), batch_size);
      list<pair<callback_t, result<long>>> run_callbacks;
      {
        lock_guard<mutex> g_callbacks(m_callbacks_);
        for (auto it = cqe_buffer.begin(); it != cqe_end; ++it) {
          unowning_cqe_ref cqe{*it};

          token_t tok = reinterpret_cast<token_t>(cqe.get_data());
#ifdef ARK_ADVANCED_DEBUG_VERBOSITY
          cerr << "### GOT TOK IN CQE : " << tok << endl;
#endif
          auto it_callback = callbacks_.find(tok);
          if (it_callback != callbacks_.end()) {
#ifdef ARK_ADVANCED_DEBUG_VERBOSITY
            cerr << "### FOUND CALLBACK" << endl;
#endif
            run_callbacks.emplace_back(forward<callback_t>(it_callback->second),
                                       cqe.to_result<long>());
            callbacks_.erase(it_callback);
          }

          *it = {};
        }
      }
      for (auto &it : run_callbacks) {
        it.first(it.second);
      }
    }
  }

  base_singlethread_uring_async_context(
      base_singlethread_uring_async_context &&) = delete;
  base_singlethread_uring_async_context &
  operator=(base_singlethread_uring_async_context &&) = delete;
  base_singlethread_uring_async_context(
      const base_singlethread_uring_async_context &) = delete;
  base_singlethread_uring_async_context &
  operator=(const base_singlethread_uring_async_context &) = delete;
};

class singlethread_uring_async_context {
public:
  using base_t = base_singlethread_uring_async_context;
  using token_t = base_t::token_t;
  using callback_t = base_t::callback_t;

private:
  unique_ptr<base_t> base_;

public:
  singlethread_uring_async_context() noexcept
      : base_(make_unique<base_singlethread_uring_async_context>()) {}

  result<void> init() noexcept { return base_->init(); }

  result<void> wake() noexcept { return base_->wake(); }

  template <typename PrepSqeCallable // void prep_sqe(sqe_ref) noexcept
            >
  result<void> add_sqe(const PrepSqeCallable &prep_sqe) noexcept {
    return base_->add_sqe(prep_sqe);
  }

  template <typename PrepSqeCallable // void prep_sqe(sqe_ref) noexcept
            >
  result<token_t> add_sqe(const PrepSqeCallable &prep_sqe,
                          callback_t &&callback) noexcept {
    return base_->add_sqe(prep_sqe, forward<callback_t>(callback));
  }

  void cancel(const token_t token) noexcept { base_->cancel(token); }

  result<void> run() noexcept { return base_->run(); }

  void exit() noexcept { return base_->exit(); }

  void exit(result<void> ret) noexcept { return base_->exit(move(ret)); }
};
} // namespace io_uring_async
} // namespace ark

/*! \endcond */
