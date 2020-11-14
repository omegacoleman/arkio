arkio
=====

*async io-uring based kernel io library*

This library is a modern C++ wrapper for the io interface of linux kernel. It provides async interface for kernel io, along with the sync ones. What's more, it supports C++20 Coroutines TS, too.

The async model is based on kernel's new io-uring interface, and implements the _proactor_ design pattern.

The _proactor_ design pattern demultiplexes and dispatches events asynchronously, and was made famous by Boost.ASIO in the c++ world. If you are familar with asio, then you will find this library very similar in some way. In fact, some parts of this library is coded to the standard draft asio is working on, which is called the Networking TS.

# examples

## coroutine echo server

``` c++
namespace program {

using namespace ark;
namespace tcp = net::tcp;

task<result<void>> handle_conn(tcp::socket s) {
  for (;;) {
    std::array<char, 1024> buf;

    OUTCOME_CO_TRY(sz,
                   co_await coro::read(s, buffer(buf), transfer_at_least(1)));

    if (sz == 0)
      break;

    OUTCOME_CO_TRY(co_await coro::write(s, buffer(buf, sz)));
  }
  co_return success();
}

task<void> run_handle_conn(tcp::socket s) {
  auto ret = co_await handle_conn(std::move(s));
  if (ret.has_error())
    std::cerr << ret.error().message() << std::endl;
}

task<result<void>> echo_srv(tcp::acceptor &ac) {
  context_exit_guard g_(ac.context());

  for (;;) {
    OUTCOME_CO_TRY(s, co_await tcp::coro::accept(ac));
    co_async(run_handle_conn(std::move(s)));
  }
}

result<void> run() {
  async_context ctx;
  OUTCOME_TRY(ctx.init());

  net::inet_address ep;
  OUTCOME_TRY(ep.host("127.0.0.1"));
  ep.port(8080);

  OUTCOME_TRY(ac, tcp::acceptor::create(ctx));
  OUTCOME_TRY(tcp::bind(ac, ep));
  OUTCOME_TRY(tcp::listen(ac));

  auto fut = co_async(echo_srv(ac));
  OUTCOME_TRY(ctx.run());
  OUTCOME_TRY(fut.get());

  return success();
}

} // namespace program

int main(void) {
  auto ret = program::run();
  if (ret.has_error()) {
    std::cerr << "error : " << ret.error().message() << std::endl;
    std::abort();
  }
  return 0;
}
```

see full example at `examples/coro_echo_server.cpp`

# usage

## requirements

In order to use io-uring you will need kernel version >= 5.1. On lower versions of kernels, io-uring has some known bugs making it unable to use for production, and fewer calls were supported, too. To use this library without pain, kernel >= 5.6 is suggested.

On linux, Coroutine TS support is only mature enough with clang++ and libcxx. If you don't need coroutines, it's not necessary.

You will also need CMake > 3.1 and liburing.

Coroutine TS related parts demands c++20 to compile, the others are written in c++17. This library was written in `-fno-rtti -fno-exceptions` dialect of c++. It does not require users to turn off that too, however. But it's strongly advised to do so, as it will lead to great performance boost and reduce size of generated binaries.

## compiling tests and examples

### on fedora 32

```
sudo dnf install -y cmake liburing-devel clang libcxx

cd arkio
git submodule init
git submodule update -r

mkdir build
cd build
cmake -H.. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ -DWITH_COROUTINES=YES
make
make test
```

as you may have already known, remove `-DWITH_COROUTINES=YES` to disable coroutines.

## compiling projects using arkio

### with cmake

first clone this repo as a submodule or subtree, to `vendor/arkio` for example

``` cmake
add_subdirectory(vendor/arkio)

...

target_link_libraries(your_program_name PUBLIC arkio)
```

### without cmake

add these to your including paths

```
include
vendor/GSL/include
vendor/function2/include
vendor/outcome/single-header
```

remember to include and link liburing by yourself.

# motivation

By the time of writing, asio is still using epoll as its low-level kernel interface, which imitates the proactor pattern with reactor pattern. This is inaffective and requires many epoll-only magic techniques. What's more, the asio interface was designed cross-platform, but it is affected greatly by the iocp interface of Microsoft windows. From 5.1, kernel introduced a set of new API named io-uring, which enables us to create real proactor implementions with a great performance boost. This library aims to port that functionality to c++, with APIs tailored just for linux.

This library aims to get away with those historical burden and a few others. By making use of Coroutine TS and GSL and outcome, modern c++ programming is getting powerful and free as it had never be. IO programming is a major use case of c++, and this library exist to port that enlightment to it.

# a few notes

This repo is in a pre-alpha state, and all the APIs are unstable.

Please give a star if you like it, that would really encourage me to go on with the project.

