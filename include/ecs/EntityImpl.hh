#pragma once

#include "ecs/Entity.hh"
#include "ecs/EntityManager.hh"
#include "ecs/Handle.hh"

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

	void Entity::Destroy()
	{
		em->Destroy(this->eid);
	}

	bool Entity::Valid() const
	{
		return em->Valid(this->eid);
	}

	void Entity::RemoveAllComponents()
	{
		em->RemoveAllComponents(this->eid);
	}

	EntityManager *Entity::GetManager()
	{
		return this->em;
	}

	Entity::Id Entity::GetId() const
	{
		return this->eid;
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
