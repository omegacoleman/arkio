#pragma once

#ifdef USING_DOXYGEN

/*!
 * \mainpage arkio
 *
 * arkio is a next-generation c++ io library, implementing async io-uring based
 * kernel io.
 *
 * This library is a modern C++ wrapper for the io interface of linux kernel. It
 * provides async interface for kernel io, along with the sync ones. What's
 * more, it supports C++20 Coroutines TS, too.
 *
 * The async model is based on kernel's new io-uring interface, and implements
 * the _proactor_ design pattern.
 *
 * The _proactor_ design pattern demultiplexes and dispatches events
 * asynchronously, and was made famous by Boost.ASIO in the c++ world. If you
 * are familar with asio, then you will find this library very similar in some
 * way. In fact, some parts of this library is coded to the standard draft asio
 * is working on, which is called the Networking TS.
 *
 * \htmlonly
 * <aside class="m-block m-info">
 *   <h3>Tip</h3>
 *   If you're new here, you can start with
 *   <a class="m-doc" href="modules.html">Modules</a> or
 *   <a class="m-doc" href="page_examples.html">Examples</a>
  </aside>
 * <div class="m-button m-success">
 *   <a href="https://github.com/omegacoleman/arkio">
 *     <div class="m-big">Follow ARKIO on Github</div>
 *     <div class="m-small">* pls star if you like it *</div>
 *   </a>
 * </div>
 * \endhtmlonly
 */

/*!
 * \example coro_echo_server.cpp
 *
 * This is an echo server, implemented using coroutines. It is able to handle
 * multiple connections in a single thread.
 */

/*!
 * \example async_echo_server.cpp
 *
 * This is an echo server, implemented using callbacks. It is able to handle
 * multiple connections in a single thread.
 */

/*!
 * \example sync_echo_server.cpp
 *
 * This is an echo server, implemented using sync apis. Unable to handle
 * multiple connections at once.
 */

/*!
 * \example coro_cat.cpp
 *
 * This is a simplified cat(1) utility which prints files to screen, implemented
 * using coroutines.
 */

/*!
 * \example sync_cat.cpp
 *
 * This is a simplified cat(1) utility which prints files to screen, implemented
 * using sync apis.
 */

/*!
 * \page page_examples Examples
 *
 * \section echo_server echo server
 *
 * - \ref coro_echo_server.cpp
 * - \ref async_echo_server.cpp
 * - \ref sync_echo_server.cpp
 *
 * The echo server is a tcp server which send back everything it received. These
 * examples demonstrate how network io is performed with arkio.
 *
 * \section cat_utility cat utility
 *
 * - \ref coro_cat.cpp
 * - \ref sync_cat.cpp
 *
 * The cat(1) utility reads a series of filenames from command line and print
 * their content to the console. These examples demonstrates how normal file io
 * is performed with arkio. Notice how file-based async io is fencelessly
 * combined with network io. This could only be done with io-uring based
 * libraries.
 *
 */

/*!
 * \page info_coro Coroutines TS and Coroutine Task Type
 *
 * The coroutine function of arkio requires C++20 coroutines ts. However, the
 * original technical standard alone is not enough for writing nested
 * coroutines, and would normally require libraries like cppcoro() to provide
 * aiding classes and functions.
 *
 * This library does not use cppcoro, however, but implemented a set of tools
 * introduced in another paper,
 * p1056r0(http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1056r0.html)
 * to support coroutine nesting and spawning, for details, see \ref ::ark::task
 * and \ref ::ark::co_async
 *
 * For complete examples on how to write coroutine programs with arkio, see:
 *
 * - \ref coro_echo_server.cpp
 * - \ref coro_cat.cpp
 */

/*!
 * \page info_network the Networking TS
 *
 * This library implemented some parts of networking ts, if you are familar with
 * asio, you will find the apis very similar in some way. However, there are a
 * few differences:
 *
 * - eof is not an error (in read operations), will just return when eof
 * encountered (catering to the linux kernel interface)
 * - error handling via outcome, see \ref info_error
 * - much simpler async_context, comparing to io_context/executors, no scheduler
 * nor executor indeed.
 * - support for general fd io, not only networking
 * - cancellations not supported (linux kernel support for cancellations is
 * buggy)
 * ...
 *
 * In addition, some parts of this library is coded 100% compatible to the
 * latest draft standard of networking ts,
 * n4771(http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/n4771.pdf):
 *
 * - the whole \ref buffer module
 * - the CompletionCondition
 */

/*!
 * \page info_error Error Handling and Outcome
 *
 * It's recommended to use -fno-exceptions with this library. How does it handle
 * errors? The answer is that it uses the outcome
 * library(https://github.com/ned14/outcome).
 *
 * Internally, \ref ::ark::result is aliased to
 * outcome::result(outcome::std_result in fact), and these macros are also
 * aliased to match the style of GSL macros:
 * - \ref ::Try -> OUTCOME_TRY
 * - \ref ::CoTry -> OUTCOME_CO_TRY
 *
 * The other parts of outcome library is not used for simplicity.
 *
 * Refer to the documents of outcome library(https://ned14.github.io/outcome/)
 * for usage.
 *
 * This is (in my opinion) not the optimal state of c++ error handling, will
 * switch to
 * herbceptions(http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0709r1.pdf)
 * when the time is right.
 */

#endif
