#pragma once

#include <boost/signals2.hpp>

using namespace boost;

namespace ecs {

	/**
	 * Lightweight class that represents a subscription to a type of event
	 */
	class Subscription {
	public:
		Subscription(signals2::connection c);

		/**
		 * Returns true if the registered callback will still be called
		 * when its subscribed event occurs.
		 * Always safe to call.
		 */
		bool IsActive() const;

		/**
		 * Terminates this subscription so that the registered callback will
		 * stop being called when new events are generated. It is safe to
		 * deallocate the associated callback after calling this function.
		 * Always safe to call, even if the subscription is not active.
		 */
		void Unsubscribe() const;
	private:
		signals2::connection connection;
	};
}