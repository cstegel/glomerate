#pragma once

#include "ecs/Subscription.hh"

namespace ecs {

	inline Subscription::Subscription(boost::signals2::connection c)
		: connection(c)
	{}

	inline bool Subscription::IsActive() const
	{
		return connection.connected();
	}

	inline void Subscription::Unsubscribe() const
	{
		connection.disconnect();
	}
}
