.PHONY: auto compile linux unix windows vs14 clean unit-tests \
	integration-tests tests astyle dependencies

auto: build unix compile

compile:
	cd build; make -j5

linux: unix
unix: build
	cd build; cmake -G "Unix Makefiles" ..

windows: vs14
vs14: build
	cd build; cmake -G "Visual Studio 14" ..

clean:
	rm -rf build bin lib

build:
	mkdir -p build

unit-tests: build dependencies unix
	cd build; make unit-tests

integration-tests: build dependencies unix
	cd build; make integration-tests

tests: unit-tests integration-tests

astyle:
	astyle --options="extra/astyle.config" "src/*.hh" "src/*.cc"

dependencies:
	git submodule sync
	git submodule update --init --recursive

