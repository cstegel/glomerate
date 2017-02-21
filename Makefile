.PHONY: auto compile linux windows clean unit-tests unit-tests-32 \
	integration-tests integration-tests-32 tests tests-32 astyle dependencies test-cmake \
	test-cmake-32

auto: tests

clean:
	rm -rf build bin lib bin_32

build:
	mkdir -p build

unit-tests: build test-cmake
	cd build; make unit-tests

unit-tests: build test-cmake-32
	cd build; make unit-tests

integration-tests: build test-cmake
	cd build; make integration-tests

integration-tests-32: build test-cmake-32
	cd build; make integration-tests_32

test-cmake: build dependencies
	cd build; cmake -G "Unix Makefiles" -DBUILD_TESTS=ON -DFORCE_32BIT=0 \
		-DCMAKE_BUILD_TYPE=Debug ..

test-cmake-32: build dependencies
	cd build; cmake -G "Unix Makefiles" -DBUILD_TESTS=ON -DFORCE_32BIT=1 \
		-DCMAKE_BUILD_TYPE=Debug ..

tests: unit-tests integration-tests

tests-32: unit-tests-32 integration-tests-32

astyle:
	astyle --options="extra/astyle.config" "src/*.hh" "src/*.cc"

dependencies:
	git submodule sync
	git submodule update --init --recursive

