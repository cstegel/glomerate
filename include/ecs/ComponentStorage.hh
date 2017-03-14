#pragma once

#include <bitset>
#include <queue>
#include <iterator>
#include <stdexcept>
#include "ecs/Common.hh"
#include "Entity.hh"

#define MAX_COMPONENTS 64


namespace ecs
{
	class BaseComponentPool;

	// Object to capture the state of entities at a given time.  Used for iterating over a
	// ComponentPool's Entities
	class ComponentPoolEntityCollection
	{
	public:
		class Iterator : public std::iterator<std::input_iterator_tag, Entity::Id>
		{
		public:
			Iterator(BaseComponentPool &pool, size_t compIndex);
			Iterator &operator++();
			bool operator==(const Iterator &other);
			bool operator!=(const Iterator &other);
			Entity::Id operator*();
		private:
			BaseComponentPool &pool;
			size_t compIndex;
		};

		ComponentPoolEntityCollection(BaseComponentPool &pool);
		Iterator begin();
		Iterator end();
	private:
		BaseComponentPool &pool;
		size_t lastCompIndex;
	};

	// These classes are meant to be internal to the ECS system and should not be known
	// to code outside of the ECS system
	class BaseComponentPool
	{
		friend class ComponentPoolEntityCollection::Iterator;
	public:
		/**
		* Creating this lock will enable "soft remove" mode on the given ComponentPool.
		* The destruction of this lock will re-enable normal deletion mode.
		*/
		class IterateLock : public NonCopyable
		{
		public:
			IterateLock(BaseComponentPool &pool);
			~IterateLock();
		private:
			BaseComponentPool &pool;
		};
		friend IterateLock;

		virtual ~BaseComponentPool() {}

		virtual void Remove(Entity::Id e) = 0;
		virtual bool HasComponent(Entity::Id e) const = 0;
		virtual size_t Size() const = 0;
		virtual ComponentPoolEntityCollection Entities() = 0;

		// as long as the resultant lock is not destroyed, the order that iteration occurs
		// over the components must stay the same.
		virtual unique_ptr<IterateLock> CreateIterateLock() = 0;

	private:
		// when toggleSoftRemove(true) is called then any Remove(e) calls
		// must guarentee to not alter the internal ordering of components.
		// When toggleSoftRemove(false) is called later then removals are allowed to rearrange
		// internal ordering again
		virtual void toggleSoftRemove(bool enabled) = 0;

		// method used by ComponentPoolEntityCollection::Iterator to find the next Entity
		virtual Entity::Id entityAt(size_t compIndex) = 0;

	};

	/**
	 * ComponentPool is a storage container for Entity components.
	 * It stores all components with a vector and so it only grows when new components are added.
	 * It "recycles" and keeps storage unfragmented by swapping components to the end when they are removed
	 * but does not guarantee the internal ordering of components based on insertion or Entity index.
	 * It allows efficient iteration since there are no holes in its component storage.
	 *
	 * TODO-cs: an incremental allocator instead of a vector will be better once the number
	 * components is very large; this should be implemeted.
	 */
	template <typename CompType>
	class ComponentPool : public BaseComponentPool
	{

	public:
		ComponentPool();

		// DO NOT CACHE THIS POINTER, a component's pointer may change over time
		template <typename ...T>
		CompType *NewComponent(Entity::Id e, T... args);

		// DO NOT CACHE THIS POINTER, a component's pointer may change over time
		CompType *Get(Entity::Id e);
		void Remove(Entity::Id e) override;
		bool HasComponent(Entity::Id e) const override;
		size_t Size() const override;
		ComponentPoolEntityCollection Entities() override;

		unique_ptr<BaseComponentPool::IterateLock> CreateIterateLock() override;

	private:
		static const size_t INVALID_COMP_INDEX = static_cast<size_t>(-1);

		vector<std::pair<Entity::Id, CompType> > components;
		size_t lastCompIndex;
		GLOMERATE_MAP_TYPE<eid_t, size_t> entIndexToCompIndex;
		bool softRemoveMode;
		std::queue<size_t> softRemoveCompIndexes;

		void toggleSoftRemove(bool enabled) override;

		void softRemove(size_t compIndex);
		void remove(size_t compIndex);

		Entity::Id entityAt(size_t compIndex) override;
	};
}
