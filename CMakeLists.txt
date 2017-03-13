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

set(EXT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ext)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${EXT_PATH}
)

# Installations
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
	DESTINATION include/glomerate
	PATTERN "internal" EXCLUDE
)

################################################
# Build tests
################################################

OPTION(GLOMERATE_BUILD_TESTS "Build test executables" OFF)
IF (GLOMERATE_BUILD_TESTS)
	set(GOOGLETEST_DIR ${EXT_PATH}/googletest/googletest)
	add_subdirectory(${EXT_PATH}/googletest)
	add_subdirectory(tests)
ENDIF ()