enable_testing()

add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/googletest googletest)

include(GoogleTest)

set(TEST_SRCS
	test_net_address.cpp;test_general.cpp)

foreach(test_src IN ITEMS ${TEST_SRCS})
	get_filename_component(test_target ${test_src} NAME_WE)
	add_executable(${test_target} ${test_src})
	target_link_libraries(${test_target} PUBLIC gtest gtest_main arkio)

	gtest_discover_tests(${test_target})
endforeach()


