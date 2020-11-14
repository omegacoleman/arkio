cmake_minimum_required(VERSION 3.1)
project(arkio)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
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
    else()
        add_definitions(-DARK_NO_COROUTINES)
    endif()
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(URING REQUIRED liburing)
find_package (Threads)
add_subdirectory(vendor/GSL)
add_subdirectory(vendor/function2)
# ned14/outcome uses quickcpplib which do lots of extra work
# here we use a little magic to include the single-header version only ..
add_library(outcome INTERFACE IMPORTED GLOBAL)
target_include_directories(outcome INTERFACE ${PROJECT_SOURCE_DIR}/vendor/outcome/single-header)

add_library(arkio INTERFACE IMPORTED GLOBAL)
target_include_directories(arkio INTERFACE ${PROJECT_SOURCE_DIR}/include ${URING_INCLUDE_DIRS})
target_link_libraries(arkio INTERFACE ${URING_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} GSL function2 outcome)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	add_subdirectory(examples)

	enable_testing()
	add_subdirectory(tests)
endif()

