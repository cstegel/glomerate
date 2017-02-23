#include <gtest/gtest.h>
#include <boost/signals2.hpp>

#include <functional>

#include "Ecs.hh"

namespace test
{

	typedef struct Position
	{
		Position() {}
		Position(int x, int y) : x(x), y(y) {}
		bool operator==(const Position & other) const
		{
			return x == other.x && y == other.y;
		}

		int x;
		int y;
	} Position;

	struct Character
	{
		Character(int health) : health(health) {}
		int health;
	};

	struct Weapon
	{
		Weapon(int damage) : damage(damage) {}
		int damage;
	};

	// An event
	struct Hit
	{
		Hit(ecs::Handle<Weapon> wep) : weapon(wep) {}
		ecs::Handle<Weapon> weapon;
	};

	class Gravedigger
	{
	public:
		void operator()(ecs::Entity e, const ecs::EntityDestroyed &d)
		{
			gravesDug += 1;
		}

		int gravesDug = 0;
	};

	class HitReceiver
	{
	public:
		void operator()(ecs::Entity e, const Hit &hit)
		{
			e.Get<Character>()->health -= hit.weapon->damage;
			totalDamage += hit.weapon->damage;
			totalDamageSq = totalDamage * totalDamage;
		}

		int totalDamage = 0;
		int totalDamageSq = 0;
		double a;
		double b;
		double c;
		double d;
		double e;
	};

	class EcsEvents : public ::testing::Test
	{
	protected:
		ecs::EntityManager em;
		ecs::Entity player1;
		ecs::Entity player2;

		virtual void SetUp()
		{
			player1 = em.NewEntity();
			player1.Assign<Character>(10);
			player1.Assign<Weapon>(1);

			player2 = em.NewEntity();
			player2.Assign<Character>(10);
			player2.Assign<Weapon>(2);
		}
	};

	TEST_F(EcsEvents, ReceiveEventForSingleEntity)
	{
		HitReceiver hitReceiver;
		player1.Subscribe<Hit>(hitReceiver);
		player1.Emit(Hit(player2.Get<Weapon>()));

		ASSERT_EQ(8, player1.Get<Character>()->health)
			<< "subscriber was never triggered";
	}

	TEST_F(EcsEvents, ReceiveEventForSingleEntityWithLambda)
	{
		struct T {
			bool rekt;
		};

		bool rekt = false;
		player1.Subscribe<T>([&rekt](ecs::Entity e, const T &t) {
			rekt = t.rekt;
		});

		ASSERT_FALSE(rekt)
			<< "premature subscriber triggered";

		player1.Emit((struct T){.rekt = true});

		ASSERT_TRUE(rekt)
			<< "lambda subscriber was never triggered";
	}

	TEST_F(EcsEvents, ReceiveEventForAllEntities)
	{
		HitReceiver hitReceiver;
		em.Subscribe<Hit>(hitReceiver);

		player1.Emit(Hit(player2.Get<Weapon>()));
		player2.Emit(Hit(player1.Get<Weapon>()));

		ASSERT_EQ(8, player1.Get<Character>()->health)
			<< "subscriber was never triggered";

		ASSERT_EQ(9, player2.Get<Character>()->health)
			<< "subscriber was not triggered for all entities";
	}

	TEST_F(EcsEvents, ReceiveEventForAllEntitiesWithLambdasAndFunctors)
	{
		HitReceiver hitReceiver;
		em.Subscribe<Hit>(hitReceiver);

		bool rekt = false;
		em.Subscribe<Hit>([&rekt](ecs::Entity, const Hit &h) {
			rekt = true;
		});

		player1.Emit(Hit(player2.Get<Weapon>()));

		EXPECT_EQ(8, player1.Get<Character>()->health)
			<< "functor was not triggered";

		EXPECT_TRUE(rekt) << "lambda was not triggered";
	}

	/**
	 * Test that multiple large functors can be stored and called properly
	 * without any of their values overwritten. More of a sanity test
	 * casting between signal2::signal types doesn't do anything strange
	 * (this is done internally).
	 */
	TEST_F(EcsEvents, ReceiveEventForAllEntitiesWithLargeFunctors)
	{
		class Functor1 {
		public:
			void operator()(ecs::Entity e, const Hit &unused) {
				for (int64 &x : nums) {
					x += 1;
				}
			}
			std::array<int64, 32> nums = {0};
		};

		class Functor2 {
		public:
			void operator()(ecs::Entity e, const Hit &unused) {
				for (int64 &x : nums) {
					x += 2;
				}
			}
			std::array<int64, 16> nums = {0};
		};

		auto f1a = Functor1();
		auto f2  = Functor2();
		auto f1b = Functor1();

		em.Subscribe<Hit>(std::ref(f1a));
		em.Subscribe<Hit>(std::ref(f2));
		em.Subscribe<Hit>(std::ref(f1b));

		player1.Emit(Hit(player2.Get<Weapon>()));

		for (auto x : f1a.nums) {
			EXPECT_EQ(1, x);
		}
		for (auto x : f2.nums) {
			EXPECT_EQ(2, x);
		}
		for (auto x : f1b.nums) {
			EXPECT_EQ(1, x);
		}
	}

	TEST_F(EcsEvents, MultiReceiveEventForSingleEntity)
	{
		// player1 gets hit twice per Hit event
		HitReceiver hitReceiver1;
		HitReceiver hitReceiver2;
		player1.Subscribe<Hit>(hitReceiver1);
		player1.Subscribe<Hit>(hitReceiver2);

		player1.Emit(Hit(player2.Get<Weapon>()));

		ASSERT_EQ(8, player1.Get<Character>()->health)
			<< "not all subscribers were triggered";
	}

	TEST_F(EcsEvents, MultiReceiveEventForAllEntities)
	{
		// everyone gets hit twice per Hit event
		HitReceiver hitReceiver1;
		HitReceiver hitReceiver2;
		em.Subscribe<Hit>(hitReceiver1);
		em.Subscribe<Hit>(hitReceiver2);

		player1.Emit(Hit(player2.Get<Weapon>()));
		player2.Emit(Hit(player1.Get<Weapon>()));

		ASSERT_EQ(6, player1.Get<Character>()->health)
			<< "not all subscribers were triggered";

		ASSERT_EQ(8, player2.Get<Character>()->health)
			<< "not all subscribers were triggered";
	}

	TEST_F(EcsEvents, MultiReceiveEventForSingleEntityWithSingleReceiver)
	{
		// player1 gets hit twice per Hit event (same receiver subscribed twice)
		HitReceiver hitReceiver;
		player1.Subscribe<Hit>(hitReceiver);
		player1.Subscribe<Hit>(hitReceiver);

		player1.Emit(Hit(player2.Get<Weapon>()));

		ASSERT_EQ(6, player1.Get<Character>()->health)
			<< "not all subscribers were triggered";
	}

	TEST_F(EcsEvents, ReceiveSingleEntityDestroyedEvent)
	{
		Gravedigger gravedigger;
		player1.Subscribe<ecs::EntityDestroyed>(std::ref(gravedigger));

		ASSERT_EQ(0, gravedigger.gravesDug);

		player1.Destroy();
		ASSERT_EQ(1, gravedigger.gravesDug)
			<< "Entity destruction event not seen by subscriber";

		player2.Destroy();
		ASSERT_EQ(1, gravedigger.gravesDug)
			<< "subscriber saw an event it wasn't subscribed to";
	}

	TEST_F(EcsEvents, ReceiveAllEntityDestroyedEvents)
	{
		Gravedigger gravedigger;
		em.Subscribe<ecs::EntityDestroyed>(std::ref(gravedigger));

		ASSERT_EQ(0, gravedigger.gravesDug);
		player1.Destroy();
		ASSERT_EQ(1, gravedigger.gravesDug)
			<< "Entity destruction event not seen by subscriber";
	}

	TEST_F(EcsEvents, UnsubscribeFromSingleEntityEvent)
	{
		HitReceiver hitReceiver;
		player1.Subscribe<Hit>(hitReceiver);
		player1.Unsubscribe<Hit>(hitReceiver);

		player1.Emit(Hit(player2.Get<Weapon>()));

		ASSERT_EQ(10, player1.Get<Character>()->health)
			<< "subscriber was triggered after it unsubscribed";
	}

	TEST_F(EcsEvents, UnsubscribeFromAllEntitiesEvent)
	{
		HitReceiver hitReceiver;
		ecs::Subscription sub = em.Subscribe<Hit>(hitReceiver);
		sub.Unsubscribe();

		player1.Emit(Hit(player2.Get<Weapon>()));

		ASSERT_EQ(10, player1.Get<Character>()->health)
			<< "subscriber was triggered after it unsubscribed";
	}

	/**
	 * Context: There are 2 subscribers for an event which has just been
	 * triggered.
	 *
	 * If the 1st subscriber unsubscribes we need to ensure the 2nd one is still
	 * called. This might not happen when we just do a "swap-to-back and delete"
	 * approach to removing a subscriber. This should be handled similar to
	 * deleting components while iterating over them where they are flagged for
	 * deletion and then only do the swap-delete after we are done iterating
	 * over subscribers.
	 */
	TEST_F(EcsEvents, UnsubscribeFromAllEntitiesEventDoesNotCauseOtherSubscribersToMissAnEvent)
	{
		bool triggered1 = false;
		bool triggered2 = false;

		ecs::Subscription sub = em.Subscribe<bool>([&](ecs::Entity e, bool _){
			triggered1 = true;
			sub.Unsubscribe();
		});

		em.Subscribe<bool>([&](ecs::Entity e, bool unused){
			triggered2 = true;
		});

		player1.Emit(true);

		EXPECT_TRUE(triggered1);
		EXPECT_TRUE(triggered2);
	}
}
