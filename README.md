# Overview

Conglomerate is an Entity Component System (ECS) created with C++11.  It aims to provide a simple API with scalable performance.  

# Setup

Conglomerate should build on Linux and Windows but has only currently been tested on Ubuntu 14.04.

If your project uses CMake you can simply include the CMakeLists.txt file in the root directory.  
Otherwise, the project can be built by invoking "make" in the top level directory which serves as a wrapper to the CMake setup.

The library will be compiled to "bin/Debug/libconglomerate.a" on linux by default.

# Example Usage

Below is a very simple example of using the ECS.  For the full API, see Entity.hh and EntityManager.hh.  Other practical examples can be found in "tests/integration/ecs.cc".

```c++
struct Position {
	Position() {}
	Position(float x, y, z) : x(x), y(y), z(z) {}
	float x, y, z;
};

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
	          << ", y: " << position-y
	          << ", z: " << position->z
	          << std::endl;
}

```

# Tests

Existing tests can be run with "make tests" to run integration and unit tests or "make integration-tests" / "make unit-tests" to run the individual suites.

googletest library is used to help with testing.