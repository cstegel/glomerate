#include <gtest/gtest.h>

#include "Ecs.hh"

namespace test
{
	TEST(Entity_Id, EntitySize)
	{
#ifdef GLOMERATE_32BIT_ENTITIES
		ASSERT_EQ(4u, sizeof(ecs::eid_t));
		ASSERT_EQ(4u, sizeof(ecs::Entity::Id));
		ASSERT_EQ(4u, sizeof(ecs::Entity::Id().Index()));
#else
		ASSERT_EQ(8u, sizeof(ecs::eid_t));
		ASSERT_EQ(8u, sizeof(ecs::Entity::Id));
		ASSERT_EQ(8u, sizeof(ecs::Entity::Id().Index()));
#endif
	}
}
