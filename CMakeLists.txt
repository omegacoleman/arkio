cmake_minimum_required(VERSION 3.1)
project(arkio)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti")
if(${WITH_COROUTINES})
    set(CMAKE_CXX_STANDARD 20)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines-ts -stdlib=libc++")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(WARNING "gcc support for coroutines is untested, and libstdc++ lacks coroutine support")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")
    endif()
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(URING REQUIRED liburing)
include_directories(${URING_INCLUDE_DIRS})
link_libraries(${URING_LIBRARIES})

find_package (Threads)
link_libraries(${CMAKE_THREAD_LIBS_INIT})

include_directories(include)

add_subdirectory(vendor/GSL)
add_subdirectory(vendor/function2)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	add_subdirectory(examples)

	enable_testing()
	add_subdirectory(tests)
endif()

