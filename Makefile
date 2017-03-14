.PHONY: \
	auto \
	clean \
	dependencies \
	tests \
	tests-amd64 \
	tests-x86 \
	unit-tests \
	unit-tests-amd64 \
	unit-tests-amd64-32bit \
	unit-tests-amd64-64bit \
	unit-tests-x86 \
	unit-tests-x86-32bit \
	unit-tests-x86-64bit \
	integration-tests \
	integration-tests-amd64 \
	integration-tests-amd64-32bit \
	integration-tests-amd64-64bit \
	integration-tests-x86 \
	integration-tests-x86-32bit \
	integration-tests-x86-64bit \
	astyle \
	test-cmake-amd64 \
	test-cmake-x86

auto: tests-amd64

clean:
	rm -rf build bin lib

build:
	mkdir -p build

unit-tests: unit-tests-amd64 unit-tests-x86

unit-tests-amd64: unit-tests-amd64-32bit unit-tests-amd64-64bit

unit-tests-amd64-32bit: build test-cmake-amd64
	cd build; make unit-tests_64bit-ents

unit-tests-amd64-64bit: build test-cmake-amd64
	cd build; make unit-tests_64bit-ents

unit-tests-x86: unit-tests-x86-32bit unit-tests-x86-64bit

unit-tests-x86-32bit: build test-cmake-x86
	cd build; make unit-tests_64bit-ents

unit-tests-x86-64bit: build test-cmake-x86
	cd build; make unit-tests_64bit-ents

integration-tests: integration-tests-amd64 integration-tests-x86

integration-tests-amd64: integration-tests-amd64-32bit integration-tests-amd64-64bit

integration-tests-amd64-32bit: build test-cmake-amd64
	cd build; make integration-tests_32bit-ents

integration-tests-amd64-64bit: build test-cmake-amd64
	cd build; make integration-tests_64bit-ents

integration-tests-x86: integration-tests-x86-32bit integration-tests-x86-64bit

integration-tests-x86-32bit: build test-cmake-x86
	cd build; make integration-tests_32bit-ents

integration-tests-x86-64bit: build test-cmake-x86
	cd build; make integration-tests_64bit-ents

test-cmake-amd64: build dependencies
	cd build; \
	cmake \
		-G "Unix Makefiles" \
		-DCMAKE_BUILD_TYPE=Debug \
		-DGLOMERATE_BUILD_TESTS=ON \
		-DGLOMERATE_TEST_ARCH=amd64 \
		..

test-cmake-x86: build dependencies
	cd build; \
	cmake \
		-G "Unix Makefiles" \
		-DCMAKE_BUILD_TYPE=Debug \
		-DGLOMERATE_BUILD_TESTS=ON \
		-DGLOMERATE_TEST_ARCH=x86 \
		..

tests: tests-amd64 tests-x86

tests-amd64: unit-tests-amd64 integration-tests-amd64

tests-x86: unit-tests-x86 integration-tests-x86

astyle:
	astyle --options="extra/astyle.config" "src/*.hh" "src/*.cc"

dependencies:
	git submodule sync
	git submodule update --init --recursive

