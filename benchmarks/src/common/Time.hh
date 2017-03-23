#pragma once

#include <chrono>
#include <vector>
#include <string>
#include <stack>
#include <stdexcept>


namespace perf
{
	using std::vector;
	using std::string;

	typedef std::chrono::time_point<std::chrono::high_resolution_clock> time_point;
	typedef std::chrono::microseconds us;

	struct TimeRecord
	{
		double GetElapsedMicro() const {
			return std::chrono::duration_cast<us>(elapsed).count();
		}

		string name;
		uint depth;
		time_point start;
		std::chrono::duration<double> elapsed;
	};

	struct PendingTimer
	{
		size_t resultIndex;
		uint depth;
		time_point start;
	};

	class TimeKeeper;
	class StopWatch
	{
		friend class TimeKeeper;
	public:
		StopWatch(const string &name, TimeKeeper &keeper);
		~StopWatch();

	private:
		string name;
		TimeKeeper &keeper;
		size_t resultIndex;
	};

	class TimeKeeper
	{
	public:
		const vector<TimeRecord> &GetTimes() const
		{
			return results;
		}

		void Start(StopWatch &sw);

		void Stop(StopWatch &sw);

		void Clear();

	private:
		uint depth = 0;
		vector<TimeRecord> results;
		std::stack<PendingTimer> pending;
	};
}