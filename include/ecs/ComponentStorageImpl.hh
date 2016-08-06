#pragma once

#include "ecs/ComponentStorage.hh"

// BaseComponentPool::IterateLock
namespace ecs
{
	BaseComponentPool::IterateLock::IterateLock(BaseComponentPool &pool): pool(pool)
	{
		pool.toggleSoftRemove(true);
	}

	BaseComponentPool::IterateLock::~IterateLock()
	{
		pool.toggleSoftRemove(false);
	}
}

// ComponentPoolEntityCollection
namespace ecs
{
	ComponentPoolEntityCollection::ComponentPoolEntityCollection(BaseComponentPool &pool)
		: pool(pool)
	{
		// keep track of the last component at creation time.  This way, if new components
		// are created during iteration they will be added at the end and we will not iterate over them
		lastCompIndex = pool.Size() - 1;
	}

	ComponentPoolEntityCollection::Iterator ComponentPoolEntityCollection::begin()
	{
		return ComponentPoolEntityCollection::Iterator(pool, 0);
	}

	ComponentPoolEntityCollection::Iterator ComponentPoolEntityCollection::end()
	{
		return ComponentPoolEntityCollection::Iterator(pool, lastCompIndex + 1);
	}
}

// ComponentPoolEntityCollection::Iterator
namespace ecs
{
	ComponentPoolEntityCollection::Iterator::Iterator(BaseComponentPool &pool, uint64 compIndex)
		: pool(pool), compIndex(compIndex)
	{}

	ComponentPoolEntityCollection::Iterator &ComponentPoolEntityCollection::Iterator::operator++()
	{
		compIndex++;
		if (compIndex > pool.Size())
		{
			throw std::runtime_error(
				"Cannot increment entity component iterator more than 1 time past the end of its pool. "
				"You are likely calling operator++ on an EntityCollection::Iterator that is at the end.  "
				"Try comparing it to <your EntityCollection>::end()");
		}
		return *this;
	}

	Entity::Id ComponentPoolEntityCollection::Iterator::operator*()
	{
		Assert(compIndex < pool.Size(),
			"trying to access entity that is past the end of this component pool");
		return pool.entityAt(compIndex);
	}

	bool ComponentPoolEntityCollection::Iterator::operator==(
		const ComponentPoolEntityCollection::Iterator &other)
	{
		return compIndex == other.compIndex;
	}

	bool ComponentPoolEntityCollection::Iterator::operator!=(
		const ComponentPoolEntityCollection::Iterator &other)
	{
		return compIndex != other.compIndex;
	}
}

// ComponentPool
namespace ecs
{
	template <typename CompType>
	ComponentPool<CompType>::ComponentPool()
	{
		lastCompIndex = static_cast<uint64>(-1);
		softRemoveMode = false;
	}

	template <typename CompType>
	unique_ptr<BaseComponentPool::IterateLock> ComponentPool<CompType>::CreateIterateLock()
	{
		return unique_ptr<BaseComponentPool::IterateLock>(new BaseComponentPool::IterateLock(*static_cast<BaseComponentPool *>(this)));
	}

	template <typename CompType>
	template <typename ...T>
	CompType *ComponentPool<CompType>::NewComponent(Entity::Id e, T... args)
	{
		uint64 newCompIndex = lastCompIndex + 1;
		lastCompIndex = newCompIndex;

		if (components.size() == newCompIndex)
		{
			components.emplace_back(e, CompType(args...));
		}
		else
		{
			components.at(newCompIndex).second = CompType(args...);
		}

		components.at(newCompIndex).first = e;
		entIndexToCompIndex[e.Index()] = newCompIndex;

		return &components.at(newCompIndex).second;
	}

	template <typename CompType>
	CompType *ComponentPool<CompType>::Get(Entity::Id e)
	{
		if (!HasComponent(e))
		{
			return nullptr;
		}

		uint64 compIndex = entIndexToCompIndex.at(e.Index());
		return &components.at(compIndex).second;
	}

	template <typename CompType>
	void ComponentPool<CompType>::Remove(Entity::Id e)
	{
		if (!HasComponent(e))
		{
			throw std::runtime_error("cannot remove component because the entity does not have one");
		}

		uint64 removeIndex = entIndexToCompIndex.at(e.Index());
		entIndexToCompIndex.at(e.Index()) = ComponentPool<CompType>::INVALID_COMP_INDEX;

		if (softRemoveMode)
		{
			softRemove(removeIndex);
		}
		else
		{
			remove(removeIndex);
		}
	}

	template <typename CompType>
	void ComponentPool<CompType>::remove(uint64 compIndex)
	{
		if (compIndex != lastCompIndex)
		{
			// Swap this component to the end
			auto validComponentPair = components.at(lastCompIndex);
			components.at(lastCompIndex) = components.at(compIndex);
			components.at(compIndex) = validComponentPair;

			// update the entity -> component index mapping of swapped component
			// if it's entity still exists
			// (Entity could have been deleted while iterating over entities so the component was only soft-deleted till now)
			uint64 entityIndex = validComponentPair.first.Index();
			if (entIndexToCompIndex.count(entityIndex) > 0)
			{
				entIndexToCompIndex.at(entityIndex) = compIndex;
			}
		}

		lastCompIndex--;
	}

	template <typename CompType>
	void ComponentPool<CompType>::softRemove(uint64 compIndex)
	{
		// mark the component as the "Null" Entity and add this component index to queue of
		// components to be deleted when "soft remove" mode is disabled.
		// "Null" Entities will never be iterated over
		Assert(compIndex < components.size());

		components.at(compIndex).first = Entity::Id();
		softRemoveCompIndexes.push(compIndex);
	}

	template <typename CompType>
	bool ComponentPool<CompType>::HasComponent(Entity::Id e) const
	{
		auto compIndex = entIndexToCompIndex.find(e.Index());
		return compIndex != entIndexToCompIndex.end() && compIndex->second != ComponentPool<CompType>::INVALID_COMP_INDEX;
	}

	template <typename CompType>
	size_t ComponentPool<CompType>::Size() const
	{
		return lastCompIndex + 1;
	}

	template <typename CompType>
	void ComponentPool<CompType>::toggleSoftRemove(bool enabled)
	{
		if (enabled)
		{
			if (softRemoveMode)
			{
				throw runtime_error("soft remove mode is already active");
			}
		}
		else
		{
			if (!softRemoveMode)
			{
				throw runtime_error("soft remove mode is already inactive");
			}

			// must perform proper removes for everything that has been "soft removed"
			while (!softRemoveCompIndexes.empty())
			{
				uint64 compIndex = softRemoveCompIndexes.front();
				softRemoveCompIndexes.pop();
				remove(compIndex);
			}
		}

		softRemoveMode = enabled;
	}

	template <typename CompType>
	Entity::Id ComponentPool<CompType>::entityAt(uint64 compIndex)
	{
		Assert(compIndex < components.size());
		return components[compIndex].first;
	}

	template <typename CompType>
	ComponentPoolEntityCollection ComponentPool<CompType>::Entities()
	{
		return ComponentPoolEntityCollection(*this);
	}
}
