#include <entityx/entityx.h>

#include <iostream>
#include <cstdint>
#include <string>

#include "common/Time.hh"

using std::cin;
using std::cout;
using std::endl;
using std::uint64_t;
using std::string;

#define DEFINE_COMPONENT(CompName, size) \
struct CompName \
{ \
	char data[size]; \
}

DEFINE_COMPONENT(A10, 10);
DEFINE_COMPONENT(A30, 30);
DEFINE_COMPONENT(A60, 60);
DEFINE_COMPONENT(A90, 90);

uint64_t BenchFindAll(uint64_t numEnts, uint64_t loops, perf::TimeKeeper &timer)
{
	entityx::EventManager events;
	entityx::EntityManager em(events);

	perf::StopWatch sw1("find all entities", timer);
	{
		perf::StopWatch sw2("create", timer);
		for (uint64_t i = 0; i < numEnts; ++i)
		{
			entityx::Entity e = em.create();
			e.assign<A10>();
			e.assign<A30>();
			e.assign<A60>();
		}
	}

	// iters stops loop from being removed by compiler
	uint64_t iters = 0;
	{
		perf::StopWatch sw3("iterate", timer);
		for (uint64_t i = 0; i < loops; ++i)
		{
			perf::StopWatch sw4("loop", timer);
			for (entityx::Entity e : em.entities_with_components<A10, A30, A60>())
			{
				iters += 1;
			}
		}
	}

	return iters;
}

/**
 * Benchmark where many entities are create but only one exists with
 * the type we are looking for.
 */
uint64_t BenchFind1AmongMany(uint64_t numEnts, uint64_t loops, perf::TimeKeeper &timer)
{
	entityx::EventManager events;
	entityx::EntityManager em(events);

	perf::StopWatch sw1("find 1 entity among many", timer);
	{
		perf::StopWatch sw2("create", timer);
		entityx::Entity e = em.create();
		e.assign<A10>();
		e.assign<A90>();

		for (uint64_t i = 1; i < numEnts; ++i)
		{
			entityx::Entity ei = em.create();
			ei.assign<A10>();
			ei.assign<A30>();
			ei.assign<A60>();
		}
	}

	// iters stops loop from being removed by compiler
	uint64_t iters = 0;
	{
		perf::StopWatch sw3("iterate", timer);
		for (uint64_t i = 0; i < loops; ++i)
		{
			perf::StopWatch sw4("loop", timer);
			for (entityx::Entity e : em.entities_with_components<A10, A90>())
			{
				iters += 1;
			}
		}
	}

	return iters;
}

int main(int argc, char **argv)
{

	if (argc < 3 || (argc >= 2 &&
		((string(argv[1]) == "--help")
		|| (string(argv[1]) == "-h"))))
	{
		cout << "Usage: " << argv[0] << " numEntities numLoops" << endl;
		return -1;
	}

	uint64_t numEnts = std::stoull(string(argv[1]));
	uint64_t loops = std::stoull(string(argv[2]));

	perf::TimeKeeper timer;
	BenchFindAll(numEnts, loops, timer);
	BenchFind1AmongMany(numEnts, loops, timer);

	for (const perf::TimeRecord &r : timer.GetTimes())
	{
		for (uint i = 0; i < r.depth; ++i)
		{
			cout << "  ";
		}
		cout << r.name << ": " << (r.GetElapsedMicro()/1000.0) << "ms" << endl;
	}

	return 0;
}














