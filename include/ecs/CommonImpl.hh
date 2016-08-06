#pragma once

#include "ecs/Common.hh"
#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <intrin.h>
#define os_break __debugbreak
#else
#define os_break __builtin_trap
#endif


namespace ecs
{
	void Assert(bool condition, const string & message)
	{
#ifndef NDEBUG
		if (!condition)
		{
			std::stringstream ss;
			ss << "assertion failed: " << message;
			std::cerr << ss.str() << std::endl;
			os_break();
			throw std::runtime_error(ss.str());
		}
#endif
	}

	void Assert(bool condition)
	{
		return Assert(condition, "assertion failed");
	}
}