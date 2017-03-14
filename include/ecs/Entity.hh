#pragma once

#include <ostream>
#include "ecs/Common.hh"
#include "ecs/Subscription.hh"



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
		friend struct std::hash<Entity>;
	public:
		class Id
		{
			friend struct std::hash<Entity>;
			friend struct std::hash<Entity::Id>;
			friend class EntityManager;
		public:

#ifdef GLOMERATE_32BIT_ENTITIES
			static const uint8 INDEX_BITS = 22;
#else
			static const uint8 INDEX_BITS = 48;
#endif

			// the rest of the bits are for the generation
			static const eid_t INDEX_MASK = ((eid_t)1 << INDEX_BITS) - 1;

			Id() : Id(NULL_ID) {};
			Id(const Id &) = default;
			Id(eid_t index, gen_t generation);
			Id(eid_t id): id(id) {}
			Id &operator=(const Id &) & = default;

			eid_t Index() const;
			gen_t Generation() const;
			eid_t GetId() const;

			string ToString() const;

			bool operator==(const Id &other) const;
			bool operator!=(const Id &other) const;
			bool operator<(const Id &other) const;

			friend std::ostream &operator<<(std::ostream &os, const Id e);

		private:
			static const eid_t NULL_ID = 0;

			eid_t id;
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

		/**
		 * Retrieve the unique identifier for this Entity
		 */
		eid_t Index() const;

		/**
		 * Retrieve the generation of this Entity
		 * (should not matter to API users)
		 */
		gen_t Generation() const;

		/**
		 * Retrieve a unique string representation of this Entity
		 */
		string ToString() const;

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
		Subscription Subscribe(
			std::function<void(Entity, const Event &)> callback);

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

namespace std
{
	// allow Entity class to be used in hashed data structures
	template <>
	struct hash<ecs::Entity>
	{
		size_t operator()(const ecs::Entity &e) const
		{
			return hash<ecs::eid_t>()(e.eid.id);
		}
	};

	// allow Entity::Id class to be used in hashed data structures
	template <>
	struct hash<ecs::Entity::Id>
	{
		size_t operator()(const ecs::Entity::Id &e) const
		{
			return hash<ecs::eid_t>()(e.id);
		}
	};
}
