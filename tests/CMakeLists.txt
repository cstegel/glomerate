cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION} FATAL_ERROR)
project(ecs-test)

################################
# Test Configuration
################################

include_directories(${project_include_dirs} ${GOOGLETEST_DIR}/include)

################################
# Test targets
################################

foreach(test_type unit integration)
	set(test_exe ecs-${test_type}-tests)

	file(GLOB_RECURSE test_sources ${CMAKE_CURRENT_SOURCE_DIR}/${test_type}/*.cc)
	list(APPEND test_sources ${GOOGLETEST_DIR}/src/gtest_main.cc)
	list(REMOVE_DUPLICATES test_sources)

	file(GLOB_RECURSE test_headers ${CMAKE_CURRENT_SOURCE_DIR}/${test_type}/*.hh)
	list(REMOVE_DUPLICATES test_headers)

	add_executable(${test_exe} ${test_sources} ${test_headers})
	target_link_libraries(${test_exe} ${PROJECT_LIB} gtest gtest_main)

	# target to run the tests
	add_custom_target(
		${test_type}-tests
		COMMAND ${test_exe}
		DEPENDS ${test_exe}
	COMMENT "Run ${test_type} tests")

endforeach(test_type)
