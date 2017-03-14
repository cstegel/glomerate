cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION} FATAL_ERROR)
project(ecs-test)

################################
# Test Configuration
################################

include_directories(${project_include_dirs} ${GOOGLETEST_DIR}/include)

################################
# Test targets
################################

foreach(entity_bits 64 32)
	foreach(test_type unit integration)
		set(test_target
		${test_type}-tests_${entity_bits}bit-ents)
		set(test_exe ecs_${test_target})

		file(GLOB_RECURSE test_sources ${CMAKE_CURRENT_SOURCE_DIR}/${test_type}/*.cc)
		list(APPEND test_sources ${GOOGLETEST_DIR}/src/gtest_main.cc)
		list(REMOVE_DUPLICATES test_sources)

		file(GLOB_RECURSE test_headers ${CMAKE_CURRENT_SOURCE_DIR}/${test_type}/*.hh)
		list(REMOVE_DUPLICATES test_headers)

		add_executable(${test_exe} ${test_sources} ${test_headers})
		target_link_libraries(${test_exe} gtest gtest_main)

		if (${GLOMERATE_TEST_ARCH} MATCHES "x86")
			foreach(target ${test_exe} "gtest" "gtest_main")
				set_target_properties(${target}
					PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
			endforeach(target)

		endif()

		if (${entity_bits} EQUAL 32)
			target_compile_definitions(${test_exe}
				PRIVATE "-DGLOMERATE_32BIT_ENTITIES")
		endif()

		# target to run the tests
		add_custom_target(
			${test_target}
			COMMAND ${test_exe}
			DEPENDS ${test_exe}
		COMMENT "Run ${test_type} tests for ${entity_bits} bit entities built for ${GLOMERATE_TEST_ARCH} CPU ARCH")

	endforeach(test_type)
endforeach(entity_bits)
