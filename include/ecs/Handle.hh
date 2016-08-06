#pragma once

#include "ComponentStorage.hh"
#include "Entity.hh"

namespace ecs
{
	/**
	 * Handles are meant as a substitute for component pointers/references
	 * since the actual memory address of a component may change at many
	 * different times without the user of the ECS expecting it.
	 *
	 * Usage is similar to a pointer (operator*() and operator->()).
	 */
	template <typename CompType>
	class Handle
	{
	public:
		Handle();
		Handle(Entity::Id entityId, ComponentPool<CompType> *componentPool);
		~Handle();

		CompType &operator*() const;
		CompType *operator->() const;

	private:
		Entity::Id eId;
		ComponentPool<CompType> *compPool = nullptr;
	};
}

