#include "Entity.hh"
#include "EntityManager.hh"

namespace ecs
{
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
