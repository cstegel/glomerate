project(ecs)
set(CMAKE_MIN_VERSION 3.1)
cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION} FATAL_ERROR)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

if (UNIX)
	add_definitions(-Wall)
endif()

if (MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS /wd4800 /MP)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

if (WIN32)
	set(_SP_WIN32 1)
elseif (UNIX)
	set(_SP_X11 1)
else()
	message(FATAL_ERROR "Unsupported target platform")
endif()

if (CMAKE_BUILD_TYPE MATCHES Release)
	message(STATUS "ecs target is Release")
elseif (CMAKE_BUILD_TYPE MATCHES Debug)
	message(STATUS "ecs target is Debug")
else()
	message(STATUS "ecs target is default, using Debug")
	set(CMAKE_BUILD_TYPE Debug)
endif()

set(EXT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ext)

set(GOOGLETEST_DIR ${EXT_PATH}/googletest/googletest)
add_subdirectory(${EXT_PATH}/googletest)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${EXT_PATH}/boostconfig/include
	${EXT_PATH}
)


# split into lib and exe so tests can link against lib
set(PROJECT_LIB conglomerate)

file(GLOB_RECURSE all_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
list(REMOVE_DUPLICATES all_sources)

file(GLOB_RECURSE all_headers ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hh)
list(REMOVE_DUPLICATES all_headers)

add_library(${PROJECT_LIB} ${all_sources} ${all_headers})
set_target_properties(${PROJECT_LIB} PROPERTIES
                      OUTPUT_NAME conglomerate
                      POSITION_INDEPENDENT_CODE ON)

# C++11
target_compile_features(${PROJECT_LIB} PRIVATE cxx_range_for)

# Installations
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
	DESTINATION include/conglomerate
	PATTERN "internal" EXCLUDE
)

install(TARGETS conglomerate
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

################################################
# Build tests
################################################
add_subdirectory(tests)
