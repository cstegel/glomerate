#include "common/Time.hh"

namespace perf
{
	StopWatch::StopWatch(const string &name, TimeKeeper &keeper)
		: name(name), keeper(keeper)
	{
		this->keeper.Start(*this);
	}

	StopWatch::~StopWatch()
	{
		this->keeper.Stop(*this);
	}
}

namespace perf
{
	void TimeKeeper::Start(StopWatch &sw)
	{
		sw.resultIndex = this->results.size();
		PendingTimer newPending;
		newPending.depth = this->depth;
		newPending.resultIndex = sw.resultIndex;
		newPending.start = std::chrono::high_resolution_clock::now();
		this->pending.push(newPending);
		this->results.emplace_back();

		this->depth += 1;
	}

	void TimeKeeper::Stop(StopWatch &sw)
	{
		time_point end = std::chrono::high_resolution_clock::now();
		PendingTimer timer = this->pending.top();
		this->pending.pop();

		if (timer.resultIndex != sw.resultIndex)
		{
			throw std::runtime_error("mismatch on timer stack");
		}

		this->depth -= 1;

		TimeRecord record;
		record.name = sw.name;
		record.start = timer.start;
		record.elapsed = end - timer.start;
		record.depth = this->depth;
		this->results[timer.resultIndex] = record;
	}

	void TimeKeeper::Clear() {
		this->results.clear();
	}
}