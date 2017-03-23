.PHONY: \
	auto \
	clean \
	dependencies \
	tests-all-arches \
	tests \
	tests-x86 \
	unit-tests-all-arches \
	unit-tests \
	unit-tests-32bit \
	unit-tests-64bit \
	unit-tests-x86 \
	unit-tests-x86-32bit \
	unit-tests-x86-64bit \
	integration-tests-all-arches \
	integration-tests \
	integration-tests-32bit \
	integration-tests-64bit \
	integration-tests-x86 \
	integration-tests-x86-32bit \
	integration-tests-x86-64bit \
	astyle \
	test-cmake \
	test-cmake-x86 \
	benchmark-tests \
	benchmark-tests-cmake \
	benchmark-tests-glomerate \
	benchmark-tests-entityx

auto: tests

clean:
	rm -rf build bin lib

build:
	mkdir -p build

unit-tests-all-arches: unit-tests unit-tests-x86

unit-tests: unit-tests-32bit unit-tests-64bit

unit-tests-32bit: build test-cmake
	cd build; make unit-tests_32bit-ents

unit-tests-64bit: build test-cmake
	cd build; make unit-tests_64bit-ents

unit-tests-x86: unit-tests-x86-32bit unit-tests-x86-64bit

unit-tests-x86-32bit: build test-cmake-x86
	cd build; make unit-tests_32bit-ents

unit-tests-x86-64bit: build test-cmake-x86
	cd build; make unit-tests_64bit-ents

integration-tests-all-arches: integration-tests integration-tests-x86

integration-tests: integration-tests-32bit integration-tests-64bit

integration-tests-32bit: build test-cmake
	cd build; make integration-tests_32bit-ents

integration-tests-64bit: build test-cmake
	cd build; make integration-tests_64bit-ents

integration-tests-x86: integration-tests-x86-32bit integration-tests-x86-64bit

integration-tests-x86-32bit: build test-cmake-x86
	cd build; make integration-tests_32bit-ents

integration-tests-x86-64bit: build test-cmake-x86
	cd build; make integration-tests_64bit-ents

test-cmake: build dependencies
	cd build; \
	cmake \
		-G "Unix Makefiles" \
		-DCMAKE_BUILD_TYPE=Debug \
		-DGLOMERATE_BUILD_TESTS=ON \
		-DGLOMERATE_TEST_ARCH=default \
		..

test-cmake-x86: build dependencies
	cd build; \
	cmake \
		-G "Unix Makefiles" \
		-DCMAKE_BUILD_TYPE=Debug \
		-DGLOMERATE_BUILD_TESTS=ON \
		-DGLOMERATE_TEST_ARCH=x86 \
		..

benchmark: benchmark-glomerate benchmark-entityx

benchmark-glomerate: benchmark-cmake
	cd build; make benchmark-glomerate

benchmark-entityx: benchmark-cmake
	cd build; make benchmark-entityx

benchmark-cmake:
	cd build; \
	cmake \
		-G "Unix Makefiles" \
		-DCMAKE_BUILD_TYPE=Release \
		-DGLOMERATE_BUILD_BENCHMARK=ON \
		..

tests-all-arches: tests tests-x86

tests: unit-tests integration-tests

tests-x86: unit-tests-x86 integration-tests-x86

astyle:
	astyle --options="extra/astyle.config" "src/*.hh" "src/*.cc"

dependencies:
	git submodule sync
	git submodule update --init --recursive

