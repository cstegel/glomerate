set(PROJECT glomerate)
project(${PROJECT})
set(CMAKE_MIN_VERSION 2.8)
cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION} FATAL_ERROR)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_FLAGS "-std=c++11")

if (UNIX)
	add_definitions(-Wall)
endif()

if (MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS /wd4800 /MP)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/lib/Release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/lib/Release)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/lib/Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/lib/Debug)

if (WIN32)
	set(_SP_WIN32 1)
elseif (UNIX)
	set(_SP_X11 1)
else()
	message(FATAL_ERROR "Unsupported target platform")
endif()

if (CMAKE_BUILD_TYPE MATCHES Release)
	message(STATUS "${PROJECT} target is Release")
elseif (CMAKE_BUILD_TYPE MATCHES Debug)
	message(STATUS "${PROJECT} target is Debug")
else()
	message(STATUS "${PROJECT} target is default, using Debug")
	set(CMAKE_BUILD_TYPE Debug)
endif()

set(EXT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ext)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${EXT_PATH}/boostconfig/include
	${EXT_PATH}
)


set(PROJECT_LIB glomerate)

file(GLOB_RECURSE all_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
list(REMOVE_DUPLICATES all_sources)

file(GLOB_RECURSE all_headers ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hh)
list(REMOVE_DUPLICATES all_headers)

add_library(${PROJECT_LIB} ${all_sources} ${all_headers})
set_target_properties(${PROJECT_LIB} PROPERTIES
                      OUTPUT_NAME glomerate
                      POSITION_INDEPENDENT_CODE ON)

# Installations
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
	DESTINATION include/glomerate
	PATTERN "internal" EXCLUDE
)

install(TARGETS ${PROJECT_LIB}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

################################################
# Build tests
################################################

OPTION(BUILD_TESTS "Build test executables" OFF)
IF (BUILD_TESTS)
	set(GOOGLETEST_DIR ${EXT_PATH}/googletest/googletest)
	add_subdirectory(${EXT_PATH}/googletest)
	add_subdirectory(tests)
ENDIF (BUILD_TESTS)