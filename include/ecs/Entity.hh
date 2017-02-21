#pragma once

#include <ostream>
#include "ecs/Common.hh"

namespace ecs
{
	class EntityManager;

	template<typename CompType>
	class Handle;

	/**
	 * Convenience class for operations on entities instead of
	 * going through the EntityManager
	 */
	class Entity
	{
	public:
		friend struct std::hash<Entity>;
		class Id
		{
			friend struct std::hash<Entity>;
			friend class EntityManager;
		public:

			// the rest of the bits are for the generation
			static const uint32 INDEX_BITS = 48;
			static const uint64 INDEX_MASK = ((uint64)1 << INDEX_BITS) - 1;

			Id() : Id(NULL_ID) {};
			Id(const Id &) = default;
			Id &operator=(const Id &) & = default;

			uint64 Index() const;

			uint64 Generation() const;

			bool operator==(const Id &other) const;
			bool operator!=(const Id &other) const;
			bool operator<(const Id &other) const;

			friend std::ostream &operator<<(std::ostream &os, const Id e);

		private:
			Id(uint64 index, uint16 generation);
			Id(uint64 id): id(id) {}

			static const uint64 NULL_ID = 0;

			uint64 id;
		};

		Entity();
		Entity(EntityManager *em, Entity::Id eid);
		Entity(const Entity &) = default;

		Entity &operator=(const Entity &) & = default;

		bool operator==(const Entity &other) const;
		bool operator!=(const Entity &other) const;
		bool operator<(const Entity &other) const;

		friend std::ostream &operator<<(std::ostream &os, const Entity e);

		EntityManager *GetManager();
		Id GetId() const;

		// retrieves the unique identifier for this entity
		uint64 Index() const;

		/**
		 * Remove the given entity from the ECS.
		 * THIS MAKES NO GUARENTEE to call destructors on components that were assigned
		 * to this entity.
		 */
		void Destroy();

		/**
		 * Check if the entity is still valid in the system.
		 * An entity is invalid if it is not currently present in the system.
		 * Since Entity::Id and Entity objects are often passed by value it is useful
		 * to check if the underlying entity they represent is still valid before operating on them.
		 */
		bool Valid() const;

		/**
		 * Construct a new component of type "CompType" with the given arguments
		 * and attach it to the entity.
		 *
		 * Returns a handle to the created component.
		 */
		template <typename CompType, typename ...T>
		Handle<CompType> Assign(T... args);

		/**
		 * Remove the given component type from this entity.
		 * Throws an error if it doesn't have this component type.
		 */
		template <typename CompType>
		void Remove();

		/**
		 * Remove all components from this entity.
		 */
		void RemoveAllComponents();

		/**
		 * Return true if this Entity has the given component type.
		 */
		template <typename CompType>
		bool Has() const;

		/**
		 * Get a handle to an entity's component of type "CompType"
		 * Throws an error if it doesn't have a component of that type.
		 */
		template <typename CompType>
		Handle<CompType> Get();

		/**
		 * Register @callback to be called when an event of type Event
		 * occurs on this Entity.
		 */
		template <typename Event>
		void Subscribe(std::function<void(Entity, const Event &)> callback);

		/**
		 * Unregister @callback for this type of Event. Throws a runtime_error
		 * if it was never registered in the first place.
		 */
		template <typename Event>
		void Unsubscribe(std::function<void(Entity, const Event &)> callback);

		/**
		 * Emit an Event on this Entity that associate subscribers will see.
		 */
		template <typename Event>
		void Emit(const Event &event);

	private:
		EntityManager *em;
		Entity::Id eid;
	};
}
