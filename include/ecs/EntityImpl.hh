#pragma once

#include "Entity.hh"
#include "EntityManager.hh"
#include "Handle.hh"

namespace ecs
{
	template <typename CompType, typename ...T>
	Handle<CompType> Entity::Assign(T... args)
	{
		return em->Assign<CompType>(this->eid, args...);
	}

	template <typename CompType>
	void Entity::Remove()
	{
		em->Remove<CompType>(this->eid);
	}

	template <typename CompType>
	bool Entity::Has() const
	{
		return em->Has<CompType>(this->eid);
	}

	template <typename CompType>
	Handle<CompType> Entity::Get()
	{
		return em->Get<CompType>(this->eid);
	}
}

namespace std
{
	// allow Entity class to be used in hashed data structures
	template <>
	struct hash<ecs::Entity>
	{
	public:
		size_t operator()(const ecs::Entity &e) const
		{
			return hash<uint64>()(e.eid.id);
		}
	};
}
