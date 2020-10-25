set(EXAMPLE_SRCS
   sync_cat.cpp;sync_tcp_client.cpp;sync_echo_server.cpp;async_echo_server.cpp;async_tcp_client.cpp)

if(${WITH_COROUTINES})
    list(APPEND EXAMPLE_SRCS coro_cat.cpp;coro_tcp_client.cpp;coro_echo_server.cpp)
endif()

foreach(example_src IN ITEMS ${EXAMPLE_SRCS})
	get_filename_component(example_target ${example_src} NAME_WE)
	add_executable(${example_target} ${example_src})
	target_include_directories(${example_target} PUBLIC ${GSL_SOURCE_DIR}/include ${function2_SOURCE_DIR}/include)
	target_link_libraries(${example_target} PUBLIC GSL function2)
endforeach()

