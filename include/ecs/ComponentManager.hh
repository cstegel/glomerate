#pragma once

#include <typeindex>
#include <unordered_map>
#include <bitset>
#include <sstream>

#include "ecs/Common.hh"
#include "ecs/Entity.hh"
#include "ecs/ComponentStorage.hh"
#include "ecs/UnrecognizedComponentType.hh"
#include "ecs/Handle.hh"

#define MAX_COMPONENT_TYPES 64

namespace ecs
{
	class ComponentManager
	{
		// TODO-cs: should probably just merge these two classes
		friend class EntityManager;
	public:
		typedef std::bitset<MAX_COMPONENT_TYPES> ComponentMask;

		template <typename CompType, typename ...T>
		Handle<CompType> Assign(Entity::Id e, T... args);

		template <typename CompType>
		void Remove(Entity::Id e);

		void RemoveAll(Entity::Id e);

		template <typename CompType>
		bool Has(Entity::Id e) const;

		template <typename CompType>
		Handle<CompType> Get(Entity::Id e);

		size_t ComponentTypeCount() const
		{
			return componentPools.size();
		}

		template <typename ...CompTypes>
		ComponentMask &SetMask(ComponentMask &mask);

		template <typename ...CompTypes>
		ComponentMask CreateMask();

		/**
		 * Register the given type as a valid "Component type" in the system.
		 * This means that operations to search for entities with this component
		 * as well as checking if an entity has that component will not fail with
		 * an exception.
		 *
		 * It is good practice to do this with all intended component types during
		 * program initialization to prevent errors when checking for component types
		 * that have yet to be assigned to an entity.
		 *
		 * This will throw an std::runtime_error if any of the types are already registered.
		 */
		template<typename CompType>
		void RegisterComponentType();

	private:
		template <typename TypeId>
		ComponentMask &setMask(ComponentManager::ComponentMask &mask, const TypeId &stdTypeId);

		template <typename TypeId, typename ...OtherTypeIds>
		ComponentMask &setMask(ComponentManager::ComponentMask &mask, const TypeId &stdTypeId, const OtherTypeIds &... stdTypeIds);

		// it is REALLY a vector of ComponentPool<T>* where each pool is the storage
		// for a different type of component.  I'm not sure of a type-safe way to store
		// this while still allowing dynamic addition of new component types.
		vector<void *> componentPools;

		// map the typeid(T) of a component type, T, to the "index" of that
		// component type. Any time each component type stores info in a vector, this index
		// will identify which component type it corresponds to
		std::unordered_map<std::type_index, uint32> compTypeToCompIndex;

		// An entity's index gives a bitmask for the components that it has. If bitset[i] is set
		// then it means this entity has the component with component index i
		vector<ComponentMask> entCompMasks;
	};
}