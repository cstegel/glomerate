# Overview

Glomerate is a header-only, type-safe Entity Component System (ECS) created with C++11.  It aims to provide a simple API with scalable performance.  

It includes an event system which allows callbacks to be registered for user-defined events (Observer design pattern).

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
1@0 has x: 1, y: 2,  z: 3
```

# Entity IDs

An `Entity::Id` is an opaque integer (defaults to 64-bit) that uniquely represents an entity.
It may be passed by value or cached and will remain usable until the entity is
destroyed.

Applications usually treat `Entity::Id` as an indivisible type, however it is
useful to understand the implementation details. IDs are composed of an *index*
and *generation*. When an entity is destroyed, the index is returned to the
pool of available indexes, and the generation for that index is incremented.
To check if an entity is destroyed, use `Entity::Valid()`, which compares the
entity's generation with the current generation for its index.

The string representation of an Entity is in the format `index@generation`.

# Events

The event system can be used to easily allow callbacks to be triggered when specific events occur. Events are identified by any user-defined data type.  Callbacks can be registered to trigger when an event occurs...

1. on a single (specified) entity
2. on any entity
3. in isolation (not associated with any entity)

Callbacks registered for scenarios 1 and 2 above have the call signature ```void(ecs::Entity, const Event &)```.

Callbacks registered for scenario 3 have the call signature ```void(const Event &)```.

The ECS emits a special ```ecs::EntityDestruction``` event just before an entity is destroyed.

Below is a complete example showcasing most of the event system. For more examples, see the tests in ```tests/integration/Events.cc```

```c++
#include <Ecs.hh>

#include <functional>
#include <iostream>

using std::cout;
using std::endl;

// Event
struct IncomingMissile {
    IncomingMissile(int x, int y) : x(x), y(y) {}
    int x, y;
};

// Event
struct Explosion {
    Explosion(int x, int y) : x(x), y(y) {}
    int x, y;
};

// Component
struct Character {
    Character(int x, int y, string name) : x(x), y(y), name(name) {}
    int x, y;
    string name;
};

// Callback for when Explosion events occur
class ExplosionHandler {
public:
    void operator()(ecs::Entity entity, const Explosion &explosion) {
        explosionsSeen += 1;

        if (entity.Has<Character>()) {
            ecs::Handle<Character> character = entity.Get<Character>();

            if (character->x == explosion.x && character->y == explosion.y) {
                entity.Destroy();
            }
        }
    }
    int explosionsSeen = 0;
};

int main(int argc, char **argv)
{
    ecs::EntityManager em;

    // announce character deaths when they occur
    typedef ecs::EntityDestruction Destruction;
    em.Subscribe<Destruction>([](ecs::Entity e, const Destruction &d) {
        if (e.Has<Character>()) {
            cout << e.Get<Character>()->name << " has died" << endl;
        }
    });

    ecs::Entity player = em.NewEntity();
    auto playerChar = player.Assign<Character>(1, 1, "John Cena");

    // player will be smart and moves out of the way of missiles
    auto intelligence = [](ecs::Entity e, const IncomingMissile &missile) {
        ecs::Handle<Character> character = e.Get<Character>();
        if (character->x == missile.x && character->y == missile.y) {
            // we better move...
            cout << character->name << " has moved out of the way!" << endl;
            character->x += 10;
        }
    };

    ecs::Subscription sub = player.Subscribe<IncomingMissile>(intelligence);

    // We can also use functors (be sure to pass by std::ref to maintain state)
    ExplosionHandler explosionHandler;
    em.Subscribe<Explosion>(std::ref(explosionHandler));

    cout << "Firing a missile at " << playerChar->name << endl;
    int missileX = playerChar->x;
    int missileY = playerChar->y;
    player.Emit(IncomingMissile(missileX, missileY));
    player.Emit(Explosion(missileX, missileY));

    cout << playerChar->name << " stops paying attention (Uh oh)" << endl;
    sub.Unsubscribe();

    cout << "Firing a missile at " << playerChar->name << endl;
    missileX = playerChar->x;
    missileY = playerChar->y;
    player.Emit(IncomingMissile(missileX, missileY));
    player.Emit(Explosion(missileX, missileY));

    cout << "The explosion handler saw " << explosionHandler.explosionsSeen
         << " explosions" << endl;

    return 0;
}

```

This program produces the following output:

```
Firing a missile at John Cena
John Cena has moved out of the way!
John Cena stops paying attention (Uh oh)
Firing a missile at John Cena
John Cena has died
The explosion handler saw 2 explosions
```

# Tests

Existing tests can be run on linux with "make tests" to run integration and unit tests or "make integration-tests" / "make unit-tests" to run the individual suites.

googletest library is used to help with testing and it will automatically be cloned from github into the ```ext/``` directory when compiling the tests for the first time.

# Customizations

## Entity Id size

32 bit Entity Ids can be used instead of 64 bit Ids by defining  `GLOMERATE_32BIT_ENTITIES` before including any Glomerate header. For instance:

```c++
#define GLOMERATE_32BIT_ENTITIES
#include <Ecs.hh>
```

By default, Glomerate uses 64 bit unsigned integers to represent an Entity. Most of the bits are used for the _index_ and the rest for the _generation_. For 32 bit Ids 22 bits are used for the _index_ and 10 for the _generation_. This allows roughly 4 million simultaneous entities whereas 48 bits (the default for 64 bit Ids) allows about 281 trillion simulataneous entities. If you don't need more than a couple million entities or you are building your application for a 32 bit platform you may see a speedup by using 32 bit Ids.

## Performance

By default, Glomerate uses std::unordered_map for storing indexes. On some
platforms (in particular VC++), std::unordered_map performs poorly and should
be replaced. This can be done by defining the `GLOMERATE_MAP_TYPE` macro before
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
