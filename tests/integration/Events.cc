#include "gtest/gtest.h"

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

	// class Gravedigger
	// {
	// public:
	// 	void operator()(ecs::Entity e, const ecs::EntityDestroyed &d)
	// 	{
	// 		gravesDug += 1;
	// 	}
	//
	// 	int gravesDug = 0;
	// };

	class HitReceiver
	{
	public:
		void operator()(ecs::Entity e, const Hit &hit)
		{
			e.Get<Character>()->health -= hit.weapon->damage;
		}
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

	// TEST_F(EcsEvents, ReceiveEventForSingleEntity)
	// {
	// 	HitReceiver hitReceiver;
	// 	player1.Subscribe<Hit>(hitReceiver);
	// 	player1.Emit(Hit(player2.Get<Weapon>()));
	//
	// 	ASSERT_EQ(8, defender.Get<Character>().health)
	// 		<< "subscriber was never triggered";
	// }

	// TEST_F(EcsEvents, ReceiveEventForSingleEntityWithLambda)
	// {
	// 	struct T {
	// 		bool rekt;
	// 	};
	//
	// 	bool rekt = false;
	// 	player1.Subscribe<T>([&rekt](ecs::Entity e, const T &t) {
	// 		rekt = t.rekt;
	// 	});
	//
	// 	ASSERT_FALSE(rekt)
	// 		<< "premature subscriber triggered";
	//
	// 	player1.Emit((struct T){.rekt = true});
	//
	// 	ASSERT_TRUE(rekt)
	// 		<< "lambda subscriber was never triggered";
	// }

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

	// TEST_F(EcsEvents, MultiReceiveEventForSingleEntity)
	// {
	// 	// player1 gets hit twice per Hit event
	// 	HitReceiver hitReceiver1;
	// 	HitReceiver hitReceiver2;
	// 	player1.Subscribe<Hit>(hitReceiver1);
	// 	player1.Subscribe<Hit>(hitReceiver2);
	//
	// 	player1.Emit(Hit(player2.Get<Weapon>()));
	//
	// 	ASSERT_EQ(8, player1.Get<Character>().health)
	// 		<< "not all subscribers were triggered";
	// }
	//
	// TEST_F(EcsEvents, MultiReceiveEventForAllEntities)
	// {
	// 	// everyone gets hit twice per Hit event
	// 	HitReceiver hitReceiver1;
	// 	HitReceiver hitReceiver2;
	// 	em.Subscribe<Hit>(hitReceiver);
	//
	// 	player1.Emit(Hit(player2.Get<Weapon>()));
	// 	player2.Emit(Hit(player1.Get<Weapon>()));
	//
	// 	ASSERT_EQ(6, player1.Get<Character>().health)
	// 		<< "not all subscribers were triggered";
	//
	// 	ASSERT_EQ(8, player2.Get<Character>().health)
	// 		<< "not all subscribers were triggered";
	// }
	//
	// TEST_F(EcsEvents, MultiReceiveEventForSingleEntityWithSingleReceiver)
	// {
	// 	// player1 gets hit twice per Hit event (same receiver subscribed twice)
	// 	HitReceiver hitReceiver;
	// 	player1.Subscribe<Hit>(hitReceiver);
	// 	player1.Subscribe<Hit>(hitReceiver);
	//
	// 	player1.Emit(Hit(player2.Get<Weapon>()));
	//
	// 	ASSERT_EQ(6, player1.Get<Character>().health)
	// 		<< "not all subscribers were triggered";
	// }
	//
	// TEST_F(EcsEvents, ReceiveSingleEntityDestroyedEvent)
	// {
	// 	Gravedigger gravedigger;
	// 	player1.Subscribe<ecs::EntityDestroyed>(gravedigger);
	//
	// 	ASSERT_EQ(0, gravedigger.gravesDug);
	//
	// 	player1.Destroy();
	// 	ASSERT_EQ(1, gravedigger.gravesDug)
	// 		<< "Entity destruction event not seen by subscriber";
	//
	// 	player2.Destroy();
	// 	ASSERT_EQ(1, gravedigger.gravesDug)
	// 		<< "subscriber saw an event it wasn't subscribed to";
	// }
	//
	// TEST_F(EcsEvents, ReceiveAllEntityDestroyedEvents)
	// {
	// 	Gravedigger gravedigger;
	// 	em.Subscribe<ecs::EntityDestroyed>(gravedigger);
	//
	// 	ASSERT_EQ(0, gravedigger.gravesDug);
	// 	player1.Destroy();
	// 	ASSERT_EQ(1, gravedigger.gravesDug)
	// 		<< "Entity destruction event not seen by subscriber";
	// }
	//
	// TEST_F(EcsEvents, UnsubscribeFromSingleEntityEvent)
	// {
	// 	HitReceiver hitReceiver;
	// 	player1.Subscribe<Hit>(hitReceiver);
	// 	player1.Unsubscribe<Hit>(hitReceiver);
	//
	// 	player1.Emit(Hit(player2.Get<Weapon>()));
	//
	// 	ASSERT_EQ(10, player1.Get<Character>().health)
	// 		<< "subscriber was triggered after it unsubscribed";
	// }
	//
	// TEST_F(EcsEvents, UnsubscribeFromAllEntitiesEvent)
	// {
	// 	HitReceiver hitReceiver;
	// 	em.Subscribe<Hit>(hitReceiver);
	// 	em.Unsubscribe<Hit>(hitReceiver);
	//
	// 	player1.Emit(Hit(player2.Get<Weapon>()));
	//
	// 	ASSERT_EQ(10, player1.Get<Character>().health)
	// 		<< "subscriber was triggered after it unsubscribed";
	// }
}
