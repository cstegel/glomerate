.PHONY: auto compile linux windows clean unit-tests \
	integration-tests tests astyle dependencies test-cmake \
	unix-release unix-debug vs14-release vs14-debug debug release

auto: release

debug: unix-debug compile
release: unix-release compile

compile: build
	cd build; make -j5

linux: unix-release
unix-release: build
	cd build; cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
unix-debug: build
	cd build; cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..

windows: vs14
vs14-release: build
	cd build; cmake -G "Visual Studio 14" -DCMAKE_BUILD_TYPE=Release ..
vs14-debug: build
	cd build; cmake -G "Visual Studio 14" -DCMAKE_BUILD_TYPE=Debug ..

clean:
	rm -rf build bin lib

build:
	mkdir -p build

unit-tests: build test-cmake
	cd build; make unit-tests

integration-tests: build test-cmake
	cd build; make integration-tests

test-cmake: build dependencies
	cd build; cmake -G "Unix Makefiles" -DBUILD_TESTS=ON ..

tests: unit-tests integration-tests

astyle:
	astyle --options="extra/astyle.config" "src/*.hh" "src/*.cc"

dependencies:
	git submodule sync
	git submodule update --init --recursive

