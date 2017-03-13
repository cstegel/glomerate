.PHONY: auto compile linux windows clean unit-tests unit-tests-32 \
	integration-tests \
	integration-tests-32bit \
	integration-tests-64bit \
	integration-tests-x86 \
	integration-tests-32bit-x86 \
	integration-tests-64bit-x86 \
	tests tests-x86 astyle dependencies test-cmake \
	test-cmake-32

auto: tests

clean:
	rm -rf build bin lib bin_32

build:
	mkdir -p build

unit-tests: build test-cmake
	cd build; make unit-tests

unit-tests: build test-cmake-x86
	cd build; make unit-tests

integration-tests: integration-tests-32bit integration-tests-64bit

integration-tests-32bit: build test-cmake
	cd build; make integration-tests_32bit-ents

integration-tests-64bit: build test-cmake
	cd build; make integration-tests_64bit-ents

integration-tests-x86: integration-tests-32bit-x86 integration-tests-64bit-x86

integration-tests-32bit-x86: build test-cmake-x86
	cd build; make integration-tests_32bit-ents

integration-tests-64bit-x86: build test-cmake-x86
	cd build; make integration-tests_64bit-ents

test-cmake: build dependencies
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

tests: unit-tests integration-tests

tests-x86: unit-tests-x86 integration-tests-x86

astyle:
	astyle --options="extra/astyle.config" "src/*.hh" "src/*.cc"

dependencies:
	git submodule sync
	git submodule update --init --recursive

