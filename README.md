arkio
=====

**async io-uring based kernel io library**

# 0. preface

## 0.0 proactor pattern and io-uring

This library is a modern C++ wrapper for the io interface of linux kernel. It provides a set of async interface along with the sync ones, and supports C++20 Coroutines TS. The async model is based on kernel's new io-uring interface, and implements the _proactor_ design pattern.

The _proactor_ design pattern demultiplexes and dispatches events asynchronously, and was made famous by Boost.ASIO in the c++ world. If you are familar with asio, then you will find this library very similar in some way. In fact, some parts of this library is coded to the standard draft asio is working on, which is called the Networking TS.

However, by the time of writing, asio is still using epoll as its low-level kernel interface, which imitates the proactor pattern with reactor pattern. This is inaffective and requires many epoll-only magic techniques. What's more, the asio interface was designed cross-platform, but it is affected greatly by the iocp interface of Microsoft windows. From 5.1, kernel introduced a set of new API named io-uring, which enables us to create real proactor implementions with a great performance boost. This library aims to port that functionality to c++, with APIs tailored just for linux.

## 0.1 requirements

In order to use io-uring you will need kernel version >= 5.1. On lower versions of kernels, io-uring got some known bugs making it unable to use for production, and fewer calls were supported, too. To use this library without pain, kernel >= 5.6 is suggested.

On linux, Coroutine TS support is only mature enough with clang++ and libcxx. If you don't need coroutines, it's not necessary.

You will also need CMake > 3.1 and liburing.

## 0.2 compiling

On Fedora 32:

```
sudo dnf install -y cmake liburing-devel clang libcxx

cd arkio
git submodule init
git submodule update -r

mkdir build
cd build
cmake -H.. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ -DWITH_COROUTINES=YES
make
```

as you may have already known, remove `-DWITH_COROUTINES=YES` to disable coroutines.

## 0.3 usage

This is a header-only library, you just need to add these to your include paths:

```
include
vendor/GSL/include
vendor/function2/include
```

And link your program with liburing.

Coroutine TS demands c++20 to compile, other parts are written in c++17.

## 0.4 dependencies

This project is depened on microsoft/GSL and Naios/function2, and uses google/googletest for testing.

This project is licensed in MIT License.
microsoft/GSL is also licensed in MIT License.
Naios/function2 is licensed in BSL 1.0.
google/googletest is licensed in BSD License.

## 0.5 dialect

This library was written in `-fno-rtti -fno-exceptions` dialect of c++.

It does not require users to turn off that too, however. But it's strongly advised to do so, as it will lead to great performance boost and reduce size of generated binaries.

# 1 examples

# 1.1 coroutine echo server

``` c++
...

void handle_conn_err(error_code ec) { std::cerr << ec.message() << std::endl; }

task<void> handle_conn(tcp::socket s) {
  for (;;) {
    std::array<char, 1024> buf;

    auto ret = co_await coro::read(s, buffer(buf), transfer_at_least(1));
    if (!ret)
      co_return handle_conn_err(ret.ec());

    size_t sz = ret.get();
    if (sz == 0)
      break;

    auto wr_ret = co_await coro::write(s, buffer(buf, sz));
    if (!wr_ret)
      co_return handle_conn_err(wr_ret.ec());
  }
}

task<void> echo_srv(tcp::acceptor &ac) {
  for (;;) {
    auto s = panic_on_ec(co_await tcp::coro::accept(ac));
    co_async(handle_conn(std::move(s)));
  }
}

int main(void) {
  async_context ctx;
  panic_on_ec(ctx.init());

  net::inet_address ep;
  ep.host("127.0.0.1");
  ep.port(8080);

  tcp::acceptor ac{panic_on_ec(tcp::acceptor::create(ctx))};
  panic_on_ec(tcp::bind(ac, ep));
  panic_on_ec(tcp::listen(ac));

  co_async(echo_srv(ac));
  ctx.run();

  return 0;
}
```

see full example at `examples/coro_echo_server.cpp`

# 1.2 async echo server

``` c++
...

struct echo_service : public std::enable_shared_from_this<echo_service> {
  tcp::socket s_;
  std::array<char, 1024> buf_;

#ifdef PRINT_ACCESS_LOG
  net::address addr_;

  echo_service(tcp::socket s, net::address addr)
      : s_(std::move(s)), addr_(std::move(addr)) {
    std::cout << "connected with peer " << peer_name() << std::endl;
  }

  ~echo_service() {
    std::cout << "closed connection to " << peer_name() << std::endl;
  }

  std::string peer_name() {
    auto ret = to_string(addr_);
    if (!ret)
      return std::string{"["} + ret.ec().message() + "]";
    return ret.get();
  }

#else
  echo_service(async_context &ctx, tcp::socket s) : s_(std::move(s)) {}
#endif

  void do_echo() {
    async::read(s_, buffer(buf_), transfer_at_least(1),
                std::bind(&echo_service::handle_read, shared_from_this(),
                          std::placeholders::_1));
  }

  void handle_write(ec_or<size_t> ret) {
    if (!ret) {
      std::cerr << ret.ec().message() << std::endl;
      return;
    }
    do_echo();
  }

  void handle_read(ec_or<size_t> ret) {
    if (!ret) {
      std::cerr << ret.ec().message() << std::endl;
      return;
    }
    size_t sz = ret.get();
    if (sz == 0)
      return;
    async::write(s_, buffer(buf_, sz),
                 std::bind(&echo_service::handle_write, shared_from_this(),
                           std::placeholders::_1));
  }
};

struct echo_server : public std::enable_shared_from_this<echo_server> {
  tcp::acceptor ac_;
  net::address addr_;

  echo_server(tcp::acceptor ac) : ac_(std::move(ac)) {}

  void run() {
#ifdef PRINT_ACCESS_LOG
    tcp::async::accept(ac_, addr_,
                       std::bind(&echo_server::handle_connection,
                                 shared_from_this(), std::placeholders::_1));
#else
    tcp::async::accept(ac_,
                       std::bind(&echo_server::handle_connection,
                                 shared_from_this(), std::placeholders::_1));
#endif
  }

  void handle_connection(ec_or<tcp::socket> ret) {
    if (!ret) {
      std::cerr << ret.ec().message() << std::endl;
      return;
    }
#ifdef PRINT_ACCESS_LOG
    auto svc = std::make_shared<echo_service>(ret.get(), std::move(addr_));
#else
    auto svc = std::make_shared<echo_service>(ret.get());
#endif
    svc->do_echo();

    run();
  }
};

int main(void) {
  async_context ctx;
  panic_on_ec(ctx.init());

  net::inet_address ep;
  ep.host("127.0.0.1");
  ep.port(8080);

  tcp::acceptor ac{panic_on_ec(tcp::acceptor::create(ctx))};
  panic_on_ec(tcp::bind(ac, ep));
  panic_on_ec(tcp::listen(ac));

  auto srv = std::make_shared<echo_server>(std::move(ac));
  srv->run();

  ctx.run();

  return 0;
}
```

see full example at `examples/async_echo_server.cpp`

# 2 buffers

## 2.0 buffers

TODO

## 2.1 buffer sequences

TODO

## 2.2 buffer operations

TODO

# 3 async

## 3.0 `async_context` and callbacks

TODO

## 3.1 async operations infra

TODO

# 4 io infra

## 4.0 `seekable_fd` and `nonseekable_fd`

TODO

## 4.1 io operations

TODO

## 4.2 general fds

TODO

## 4.3 networking

TODO

# 5 coroutines

## 5.0 `task` and `co_async`

TODO

## 5.1 awaitable operations infra

TODO

# 999 at last

This repo is in a pre-alpha state, and all the apis are unstable.

The `ec_or<T>` was not documented here because a big refactor which replace it with ned14/outcome is ongoing.

Please give a star if you like it, that would really encourage me to go on with the project.

