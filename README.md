# Overview

Glomerate is a header-only, type-safe Entity Component System (ECS) created with C++11.  It aims to provide a simple API with scalable performance.  

# Setup

Glomerate is a header-only library with no external dependencies (outside of testing) so all you need to do to use it is the following:

  1. add Glomerate's ```include``` directory onto your include path when compiling your project.
  2. ```#include <Ecs.hh>``` in any files you wish to use the library

It should work on any OS when using a C++11 compliant compiler.

# Example Usage

Below is a very simple example of using the ECS.  For the full API, see Entity.hh and EntityManager.hh.  Other practical examples can be found in "tests/integration/ecs.cc".

```c++
// glomerate's "include" dir must be on your project's include path
#include <Ecs.hh>

#include <iostream>

struct Position {
	Position() {}
	Position(float x, float y, float z) : x(x), y(y), z(z) {}
	float x, y, z;
};

int main(int argc, char **argv)
{
	ecs::EntityManager entityManager;

	// not needed, but recommended
	entityManager.RegisterComponentType<Position>();

	ecs::Entity entity = entityManager.NewEntity();
	entity.Assign<Position>(1, 2, 3);

	for (ecs::Entity e : entityManager.EntitiesWith<Position>())
	{
		auto position = e.Get<Position>(); // returns an ecs::Handle<Position>
		std::cout << e
		          << " has x: " << position->x
		          << ", y: " << position->y
		          << ", z: " << position->z
		          << std::endl;
	}

	return 0;
}

```

Output:

```c++
(Index: 1, Gen: 0) has x: 1, y: 2,  z: 3
```

# Tests

Existing tests can be run on linux with "make tests" to run integration and unit tests or "make integration-tests" / "make unit-tests" to run the individual suites.

googletest library is used to help with testing and it will automatically be cloned from github into the ```ext/``` directory when compiling the tests for the first time.

# Performance

By default, Glomerate uses std::unordered_map for storing indexes. On some
platforms (in particular VC++), std::unordered_map performs poorly and should
be replaced. This can be done by defining the GLOMERATE_MAP_TYPE macro before
including any Glomerate header. For instance:

```c++
#include <boost/unordered_map.hpp>
#define GLOMERATE_MAP_TYPE boost::unordered_map
#include <Ecs.hh>
```

Any type with a similar interface can be used.

# License

Glomerate uses the MIT license. See LICENSE for more details.

It also uses part of boost so the Boost license also applies. This can be seen in ext/boost/LICENSE_1_0.txt

If you build the tests then googletest and googlemock will be retrieved which are governed by their own open source licenses. This can be seen in ext/googletest/googletest/LICENSE and ext/googletest/googlemock/LICENSE.