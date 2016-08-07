#pragma once

#include "ecs/Entity.hh"
#include "ecs/EntityManager.hh"
#include "ecs/Handle.hh"

// Entity::Id
namespace ecs
{
	inline uint64 Entity::Id::Index() const
	{
		return id & INDEX_MASK;
	}

	inline uint64 Entity::Id::Generation() const
	{
		return (id >> INDEX_BITS);
	}

	inline bool Entity::Id::operator==(const Id &other) const
	{
		return id == other.id;
	}

	inline bool Entity::Id::operator!=(const Id &other) const
	{
		return !(*this == other);
	}

	inline bool Entity::Id::operator<(const Id &other) const
	{
		return this->id < other.id;
	}

	inline std::ostream &operator<<(std::ostream &os, const Entity::Id e)
	{
		os << "(Index: " << e.Index() << ", Gen: " << e.Generation() << ")";
		return os;
	}

	inline Entity::Id::Id(uint64 index, uint16 generation)
	{
		id = (static_cast<uint64>(generation) << INDEX_BITS) + index;
		Assert((id & INDEX_MASK) == index);
	}
}

// Entity
namespace ecs
{
	inline Entity::Entity() : em(nullptr), eid()
	{}

	inline Entity::Entity(EntityManager *em, Entity::Id eid) : em(em), eid(eid)
	{}

	inline bool Entity::operator==(const Entity &other) const
	{
		return this->eid == other.eid && this->em == other.em;
	}

	inline bool Entity::operator!=(const Entity &other) const
	{
		return !(*this == other);
	}

	inline bool Entity::operator<(const Entity &other) const
	{
		return this->eid < other.eid;
	}

	inline std::ostream &operator<<(std::ostream &os, const Entity e)
	{
		os << e.eid;
		return os;
	}

	inline uint64 Entity::Index() const
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

	inline void Entity::Destroy()
	{
		em->Destroy(this->eid);
	}

	inline bool Entity::Valid() const
	{
		return em->Valid(this->eid);
	}

	inline void Entity::RemoveAllComponents()
	{
		em->RemoveAllComponents(this->eid);
	}

	inline EntityManager *Entity::GetManager()
	{
		return this->em;
	}

	inline Entity::Id Entity::GetId() const
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
