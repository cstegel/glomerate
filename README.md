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
		ecs::Handle<Position> position = e.Get<Position>();
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