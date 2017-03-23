cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION} FATAL_ERROR)
project(ecs-perf)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ext/entityx)

foreach(type glomerate entityx common)
	file(GLOB_RECURSE ${type}_sources
		${CMAKE_CURRENT_SOURCE_DIR}/src/${type}/*.cc
	)
	list(REMOVE_DUPLICATES ${type}_sources)

	file(GLOB_RECURSE ${type}_headers
		${CMAKE_CURRENT_SOURCE_DIR}/src/${type}/*.hh
	)
	list(REMOVE_DUPLICATES ${type}_headers)
endforeach(type)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/ext/entityx
)

add_executable(benchmark-glomerate ${glomerate_sources} ${common_sources})

add_executable(benchmark-entityx ${entityx_sources} ${common_sources})
target_link_libraries(benchmark-entityx entityx)
