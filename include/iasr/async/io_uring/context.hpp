#pragma once

#include <iasr/bindings.hpp>
#include <iasr/clinux.hpp>

#include <iasr/async/io_uring/io_uring.hpp>
#include <iasr/error/ec_or.hpp>

namespace iasr {
namespace io_uring_async {
using callback_t = unique_function<void(ec_or<long> result)>;

class base_singlethread_uring_async_context {
public:
  using token_t = uintptr_t;
  using callback_t = unique_function<void(ec_or<long> result)>;

private:
  io_uring r_;
  map<token_t, callback_t> callbacks_;

  mutex m_submission_;
  mutex m_callbacks_;

  token_t callbacks_idx_;

  static const constexpr unsigned batch_size = 1024;

  int waker_evfd_;
  volatile bool need_wake_;

  bool inited_;
  bool exiting_;
  error_code exiting_error_;

  template <typename PrepSqeCallable // void prep_sqe(sqe_ref) noexcept
            >
  ec_or<token_t> base_add_sqe(const PrepSqeCallable &prep_sqe) noexcept {
    auto sqe_ret = r_.get_sqe();
    IASR_PASS_EC_ON(sqe_ret);
    sqe_ref sqe = sqe_ret.get();
    prep_sqe(sqe);
    token_t ret = callbacks_idx_;
    sqe.set_data(reinterpret_cast<void *>(callbacks_idx_++));
#ifdef IASR_ADVANCED_DEBUG_VERBOSITY
    sqe.dump();
#endif
    error_code ec = wake();
    if (ec) {
      cancel(ret);
      return ec;
    }
    return ret;
  }

  error_code add_waker() noexcept {
    auto ret =
        add_sqe([this](sqe_ref sqe) { sqe.prep_poll_add(waker_evfd_, POLLIN); },
                [this](ec_or<long> ret) {
                  if (!ret) {
                    this->exit_error(ret.ec());
                    return;
                  }
                  buffer b{sizeof(uint64_t)};
                  auto read_ret = clinux::read(waker_evfd_, b.data(), b.size());
                  if (read_ret == -1) {
                    this->exit_error(clinux::errno_ec());
                    return;
                  }
                  error_code ec = this->add_waker();
                  if (ec)
                    this->exit_error(ec);
                });
    if (!ret)
      return ret.ec();
    return {};
  }

  void exit_error(error_code ec) noexcept {
    exiting_error_ = ec;
    exiting_ = true;
  }

  void submit() noexcept {
    lock_guard<mutex> g_submission(m_submission_);
    int s = r_.submit();
#ifdef IASR_ADVANCED_DEBUG_VERBOSITY
    cerr << "### SUBMITTED : " << s << endl;
#endif
  }

public:
  base_singlethread_uring_async_context() noexcept
      : inited_(false), exiting_(false), callbacks_idx_(0), need_wake_(false) {}

  error_code init() noexcept {
    Expects(!inited_);

    int waker_ret = clinux::eventfd(0, 0);
    if (waker_ret == -1)
      return clinux::errno_ec();
    waker_evfd_ = waker_ret;

    error_code ec = r_.queue_init(batch_size, 0);
    if (ec)
      return ec;

    inited_ = true;
    return add_waker();
  }

  error_code exit() noexcept {
    exiting_ = true;
    return wake();
  }

  error_code wake() noexcept {
    if (!need_wake_)
      return {};
    static uint64_t d{1};
    auto write_ret = clinux::write(waker_evfd_, &d, sizeof(d));
    if (write_ret == -1) {
      return clinux::errno_ec();
    }
    return {};
  }

  template <typename PrepSqeCallable // void prep_sqe(sqe_ref) noexcept
            >
  ec_or<token_t> add_sqe(const PrepSqeCallable &prep_sqe) noexcept {
    lock_guard<mutex> g_submission(m_submission_);
    return base_add_sqe(prep_sqe);
  }

  template <typename PrepSqeCallable // void prep_sqe(sqe_ref) noexcept
            >
  ec_or<token_t> add_sqe(const PrepSqeCallable &prep_sqe,
                         callback_t &&callback) noexcept {
    lock_guard<mutex> g_submission(m_submission_);
    lock_guard<mutex> g_callbacks(m_callbacks_);
    auto ret = base_add_sqe(prep_sqe);
    IASR_PASS_EC_ON(ret);
    token_t tok = ret.get();
    callbacks_[tok] = forward<callback_t>(callback);
    return tok;
  }

  void cancel(const token_t token) noexcept {
    lock_guard<mutex> g_callbacks(m_callbacks_);
    callbacks_.erase(token);
  }

  error_code run() noexcept {
    for (;;) {
      if (exiting_) {
        exiting_ = false;
        if (exiting_error_) {
          error_code ret = exiting_error_;
          exiting_error_ = {};
          return ret;
        } else {
          return {};
        }
      }
      submit();
      need_wake_ = true;
      error_code ec = r_.wait();
      need_wake_ = false;
      if (ec)
        return ec;
#ifdef IASR_ADVANCED_DEBUG_VERBOSITY
      cerr << "### WOKE" << endl;
#endif

      array<owning_cqe_ref, batch_size> cqe_buffer;
      auto cqe_end = r_.peek_batch_cqe(cqe_buffer.begin(), batch_size);
      list<pair<callback_t, ec_or<long>>> run_callbacks;
      {
        lock_guard<mutex> g_callbacks(m_callbacks_);
        for (auto it = cqe_buffer.begin(); it != cqe_end; ++it) {
          unowning_cqe_ref cqe{*it};

          token_t tok = reinterpret_cast<token_t>(cqe.get_data());
#ifdef IASR_ADVANCED_DEBUG_VERBOSITY
          cerr << "### GOT TOK IN CQE : " << tok << endl;
#endif
          auto it_callback = callbacks_.find(tok);
          if (it_callback != callbacks_.end()) {
#ifdef IASR_ADVANCED_DEBUG_VERBOSITY
            cerr << "### FOUND CALLBACK" << endl;
#endif
            run_callbacks.emplace_back(forward<callback_t>(it_callback->second),
                                       cqe.to_ec_or<long>());
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

  error_code init() noexcept { return base_->init(); }

  error_code wake() noexcept { return base_->wake(); }

  template <typename PrepSqeCallable // void prep_sqe(sqe_ref) noexcept
            >
  error_code add_sqe(const PrepSqeCallable &prep_sqe) noexcept {
    return base_->add_sqe(prep_sqe);
  }

  template <typename PrepSqeCallable // void prep_sqe(sqe_ref) noexcept
            >
  ec_or<token_t> add_sqe(const PrepSqeCallable &prep_sqe,
                         callback_t &&callback) noexcept {
    return base_->add_sqe(prep_sqe, forward<callback_t>(callback));
  }

  void cancel(const token_t token) noexcept { base_->cancel(token); }

  error_code run() noexcept { return base_->run(); }

  error_code exit() noexcept { return base_->exit(); }
};
} // namespace io_uring_async
} // namespace iasr
