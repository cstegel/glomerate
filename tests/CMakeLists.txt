cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION} FATAL_ERROR)
project(ecs-test)

################################
# Test Configuration
################################

include_directories(${project_include_dirs} ${GOOGLETEST_DIR}/include)

################################
# Test targets
################################

if (FORCE_32BIT)
	set(test_suffix "_32")
endif()

foreach(test_type unit integration)
	set(test_exe ecs-${test_type}-tests${test_suffix})

	file(GLOB_RECURSE test_sources ${CMAKE_CURRENT_SOURCE_DIR}/${test_type}/*.cc)
	list(APPEND test_sources ${GOOGLETEST_DIR}/src/gtest_main.cc)
	list(REMOVE_DUPLICATES test_sources)

	file(GLOB_RECURSE test_headers ${CMAKE_CURRENT_SOURCE_DIR}/${test_type}/*.hh)
	list(REMOVE_DUPLICATES test_headers)

	add_executable(${test_exe} ${test_sources} ${test_headers})
	target_link_libraries(${test_exe} gtest gtest_main)

	if (FORCE_32BIT)
		set_target_properties(${test_exe} PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
		set_target_properties(gtest PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
		set_target_properties(gtest_main PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
	endif()

	# target to run the tests
	add_custom_target(
		${test_type}-tests${test_suffix}
		COMMAND ${test_exe}
		DEPENDS ${test_exe}
	COMMENT "Run ${test_type} tests")

endforeach(test_type)
