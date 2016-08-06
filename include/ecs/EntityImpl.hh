#pragma once

#include "ecs/Entity.hh"
#include "ecs/EntityManager.hh"
#include "ecs/Handle.hh"

// Entity::Id
namespace ecs
{
	uint64 Entity::Id::Index() const
	{
		return id & INDEX_MASK;
	}

	uint64 Entity::Id::Generation() const
	{
		return (id >> INDEX_BITS);
	}

	bool Entity::Id::operator==(const Id &other) const
	{
		return id == other.id;
	}
	bool Entity::Id::operator!=(const Id &other) const
	{
		return !(*this == other);
	}
	bool Entity::Id::operator<(const Id &other) const
	{
		return this->id < other.id;
	}

	std::ostream &operator<<(std::ostream &os, const Entity::Id e)
	{
		os << "(Index: " << e.Index() << ", Gen: " << e.Generation() << ")";
		return os;
	}

	Entity::Id::Id(uint64 index, uint16 generation)
	{
		id = (static_cast<uint64>(generation) << INDEX_BITS) + index;
		Assert((id & INDEX_MASK) == index);
	}
}

// Entity
namespace ecs
{
	Entity::Entity() : em(nullptr), eid()
	{}

	Entity::Entity(EntityManager *em, Entity::Id eid) : em(em), eid(eid)
	{}

	bool Entity::operator==(const Entity &other) const
	{
		return this->eid == other.eid && this->em == other.em;
	}

	bool Entity::operator!=(const Entity &other) const
	{
		return !(*this == other);
	}

	bool Entity::operator<(const Entity &other) const
	{
		return this->eid < other.eid;
	}

	std::ostream &operator<<(std::ostream &os, const Entity e)
	{
		os << e.eid;
		return os;
	}

	uint64 Entity::Index() const
	{
		return eid.Index();
	}

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
