#pragma once

#include <iasr/bindings.hpp>

namespace iasr {

template <class T> struct manual_lifetime {
public:
  manual_lifetime() noexcept {}
  ~manual_lifetime() noexcept {}

  template <class... Args> void construct(Args &&... args) noexcept {
    ::new (static_cast<void *>(addressof(value)))
        T(static_cast<Args &&>(args)...);
  }

  void destruct() noexcept { value.~T(); }

  T &get() & { return value; }
  const T &get() const & { return value; }
  T &&get() && { return (T &&) value; }
  const T &&get() const && { return (const T &&)value; }

private:
  union {
    T value;
  };
};

template <class T> struct manual_lifetime<T &> {
  manual_lifetime() noexcept : ptr(nullptr) {}

  void construct(T &value) noexcept { ptr = addressof(value); }
  void destruct() noexcept { ptr = nullptr; }

  T &get() const noexcept { return *ptr; }

private:
  T *ptr;
};

template <class T> struct manual_lifetime<T &&> {
  manual_lifetime() noexcept : ptr(nullptr) {}

  void construct(T &&value) noexcept { ptr = addressof(value); }
  void destruct() noexcept { ptr = nullptr; }

  T &&get() const noexcept { return *ptr; }

private:
  T *ptr;
};

template <> struct manual_lifetime<void> {
  void construct() noexcept {}
  void destruct() noexcept {}
  void get() const noexcept {}
};

} // namespace iasr
