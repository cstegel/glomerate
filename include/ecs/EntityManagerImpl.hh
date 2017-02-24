#pragma once

#include "ecs/EntityManager.hh"
#include "ecs/Entity.hh"
#include "ecs/Handle.hh"
#include "ecs/EntityDestruction.hh"

// EntityManager
namespace ecs
{
	template <typename CompType, typename ...T>
	Handle<CompType> EntityManager::Assign(Entity::Id e, T... args)
	{
		return compMgr.Assign<CompType>(e, args...);
	}

	template <typename CompType>
	void EntityManager::Remove(Entity::Id e)
	{
		compMgr.Remove<CompType>(e);
	}

	template <typename CompType>
	bool EntityManager::Has(Entity::Id e) const
	{
		return compMgr.Has<CompType>(e);
	}

	template <typename CompType>
	Handle<CompType> EntityManager::Get(Entity::Id e)
	{
		return compMgr.Get<CompType>(e);
	}

	template <typename ...CompTypes>
	EntityManager::EntityCollection EntityManager::EntitiesWith()
	{
		return EntitiesWith(compMgr.CreateMask<CompTypes...>());
	}

	template<typename CompType>
	void EntityManager::RegisterComponentType()
	{
		compMgr.RegisterComponentType<CompType>();
	}

	template <typename ...CompTypes>
	ComponentManager::ComponentMask EntityManager::CreateComponentMask()
	{
		return compMgr.CreateMask<CompTypes...>();
	}

	template <typename ...CompTypes>
	ComponentManager::ComponentMask &EntityManager::SetComponentMask(ComponentManager::ComponentMask &mask)
	{
		return compMgr.SetMask<CompTypes...>(mask);
	}

	inline EntityManager::EntityManager()
	{
		// entity 0 is reserved for the NULL Entity
		nextEntityIndex = 1;

		// update data structures for the NULL Entity
		compMgr.entCompMasks.resize(1);
		entIndexToGen.push_back(0);
	}

	inline Entity EntityManager::NewEntity()
	{
		uint64 i;
		uint16 gen;
		if (freeEntityIndexes.size() >= RECYCLE_ENTITY_COUNT)
		{
			i = freeEntityIndexes.front();
			freeEntityIndexes.pop();
			gen = entIndexToGen.at(i);  // incremented at Entity destruction
			Assert(compMgr.entCompMasks[i] == ComponentManager::ComponentMask(),
				"expected ent comp mask to be reset at destruction but it wasn't");
			compMgr.entCompMasks[i] = ComponentManager::ComponentMask();
		}
		else
		{
			i = nextEntityIndex++;
			gen = 0;
			entIndexToGen.push_back(gen);

			// add a blank comp mask without copying one in
			compMgr.entCompMasks.resize(compMgr.entCompMasks.size() + 1);

			Assert(entIndexToGen.size() == nextEntityIndex);
			Assert(compMgr.entCompMasks.size() == nextEntityIndex);
		}

		return Entity(this, Entity::Id(i, gen));
	}

	inline void EntityManager::Destroy(Entity::Id e)
	{
		typedef boost::signals2::signal<void(Entity, void *)> GenericSig;

		if (!Valid(e))
		{
			std::stringstream ss;
			ss << "entity " << e << " is not valid; it may have already been destroyed.";
			throw std::invalid_argument(ss.str());
		}

		// notify any subscribers of this entity's death before killing it
		this->Emit(e, EntityDestruction());

		// detach any subscribers listening for events on this entity
		if (entityEventSignals.count(e) > 0) {
			for (auto &kv : entityEventSignals[e]) {
				GenericSig &signal = kv.second;
				signal.disconnect_all_slots();
			}

			entityEventSignals.erase(e);
		}

		RemoveAllComponents(e);
		entIndexToGen.at(e.Index())++;
		freeEntityIndexes.push(e.Index());
	}

	inline bool EntityManager::Valid(Entity::Id e) const
	{
		return e.Generation() == entIndexToGen.at(e.Index());
	}

	inline void EntityManager::RemoveAllComponents(Entity::Id e)
	{
		compMgr.RemoveAll(e);
	}

	inline EntityManager::EntityCollection EntityManager::EntitiesWith(ComponentManager::ComponentMask compMask)
	{
		// find the smallest size component pool to iterate over
		uint64 minSize;
		int minSizeCompIndex = -1;

		for (size_t i = 0; i < compMgr.ComponentTypeCount(); ++i)
		{
			if (!compMask.test(i))
			{
				continue;
			}

			uint64 compSize = compMgr.componentPools.at(i)->Size();

			if (minSizeCompIndex == -1 || compSize < minSize)
			{
				minSize = compSize;
				minSizeCompIndex = i;
			}
		}

		auto smallestCompPool = compMgr.componentPools.at(minSizeCompIndex);

		return EntityManager::EntityCollection(
			*this,
			compMask,
			smallestCompPool->Entities(),
			smallestCompPool->CreateIterateLock()
		);
	}

	template <typename Event>
	void EntityManager::registerEventType()
	{
		std::type_index eventType = typeid(Event);

		if (eventTypeToEventIndex.count(eventType) != 0)
		{
			std::stringstream ss;
			ss << "event type " << string(eventType.name())
			   << " is already registered";
			throw std::runtime_error(ss.str());
		}

		uint32 eventIndex = eventSignals.size();
		eventTypeToEventIndex[eventType] = eventIndex;
		eventSignals.push_back({});
	}

	template <typename Event>
	void EntityManager::registerNonEntityEventType()
	{
		std::type_index eventType = typeid(Event);

		if (eventTypeToNonEntityEventIndex.count(eventType) != 0)
		{
			std::stringstream ss;
			ss << "event type " << string(eventType.name())
			   << " is already registered";
			throw std::runtime_error(ss.str());
		}

		uint32 nonEntityEventIndex = nonEntityEventSignals.size();
		eventTypeToNonEntityEventIndex[eventType] = nonEntityEventIndex;
		nonEntityEventSignals.push_back({});
	}

	template <typename Event>
	Subscription EntityManager::Subscribe(
		std::function<void(const Event &e)> callback)
	{
		// TODO-cs: this shares a lot of code in common with
		// Subscribe(function<void(Entity, const Event &)>), find a way
		// to eliminate the duplicate code.
		std::type_index eventType = typeid(Event);

		uint32 nonEntityEventIndex;

		try
		{
			nonEntityEventIndex = eventTypeToNonEntityEventIndex.at(eventType);
		}
		// Non-Entity Event never seen before, add it to the collection
		catch (const std::out_of_range &e)
		{
			registerNonEntityEventType<Event>();
			nonEntityEventIndex = eventTypeToNonEntityEventIndex.at(eventType);
		}

		auto &signal = nonEntityEventSignals.at(nonEntityEventIndex);
		boost::signals2::connection c = signal.connect(callback);

		return Subscription(c);
	}

	template <typename Event>
	Subscription EntityManager::Subscribe(
		std::function<void(Entity, const Event &)> callback)
	{
		typedef boost::signals2::signal<void(Entity, const Event &)> EventSignal;
		std::type_index eventType = typeid(Event);

		uint32 eventIndex;

		try
		{
			eventIndex = eventTypeToEventIndex.at(eventType);
		}
		// Event never seen before, add it to the collection
		catch (const std::out_of_range &e)
		{
			registerEventType<Event>();
			eventIndex = eventTypeToEventIndex.at(eventType);
		}

		EventSignal &signal = getSignal<Event>(eventSignals.at(eventIndex));
		boost::signals2::connection c = signal.connect(callback);

		return Subscription(c);
	}

	template <typename Event>
	Subscription EntityManager::Subscribe(
		std::function<void(Entity, const Event &e)> callback,
		Entity::Id entity)
	{
		auto &eventSignal = getOrCreateEntitySignal<Event>(entity);
		boost::signals2::connection &&c = eventSignal.connect(callback);

		return Subscription(c);
	}

	template <typename Event>
	boost::signals2::signal<void(Entity, const Event &)> &
	EntityManager::getOrCreateEntitySignal(Entity::Id entity)
	{
		typedef boost::signals2::signal<void(Entity, void *)> GenericSig;
		typedef boost::signals2::signal<void(Entity, const Event &)> EventSig;

		GenericSig &genSignal = entityEventSignals[entity][typeid(Event)];
		EventSig &eventSignal = getSignal<Event>(genSignal);
		return eventSignal;
	}

	template <typename Event>
	boost::signals2::signal<void(Entity, const Event &)> &
	EntityManager::getSignal(boost::signals2::signal<void(Entity, void *)> &sig)
	{
		// reinterpret_cast is okay here since only difference is the
		// call signature of the stored functions, which does not affect size
		typedef boost::signals2::signal<void(Entity, const Event &)> EventSignal;
		return *reinterpret_cast<EventSignal *>(&sig);
	}

	template <typename Event>
	void EntityManager::Emit(Entity::Id e, const Event &event)
	{
		typedef boost::signals2::signal<void(Entity, const Event &)> EventSignal;
		std::type_index eventType = typeid(Event);
		Entity entity(this, e);

		// signal the generic Event subscribers
		if (eventTypeToEventIndex.count(eventType) > 0)
		{
			auto eventIndex = eventTypeToEventIndex.at(eventType);
			EventSignal &signal = getSignal<Event>(eventSignals.at(eventIndex));
			signal(entity, event);
		}

		// now signal the entity-specific Event subscribers
		if (entityEventSignals.count(e) > 0) {
			if (entityEventSignals[e].count(eventType) > 0) {
				auto &eventSignal = getOrCreateEntitySignal<Event>(e);
				eventSignal(entity, event);
			}
		}
	}

	template <typename Event>
	void EntityManager::Emit(const Event &event)
	{
		typedef boost::signals2::signal<void(const Event &)> EventSignal;
		std::type_index eventType = typeid(Event);

		if (eventTypeToNonEntityEventIndex.count(eventType) > 0)
		{
			auto eventIndex = eventTypeToNonEntityEventIndex.at(eventType);
			auto &sig = nonEntityEventSignals.at(eventIndex);
			EventSignal &signal = *reinterpret_cast<EventSignal *>(&sig);
			signal(event);
		}
	}
}

// EntityManager::EntityCollection
namespace ecs
{
	inline EntityManager::EntityCollection::EntityCollection(EntityManager &em,
			const ComponentManager::ComponentMask &compMask,
			ComponentPoolEntityCollection compEntColl,
			unique_ptr<BaseComponentPool::IterateLock> &&iLock)
		: em(em), compMask(compMask), compEntColl(compEntColl), iLock(std::move(iLock))
	{}

	inline EntityManager::EntityCollection::Iterator EntityManager::EntityCollection::begin()
	{
		return EntityManager::EntityCollection::Iterator(em, compMask, &compEntColl, compEntColl.begin());
	}

	inline EntityManager::EntityCollection::Iterator EntityManager::EntityCollection::end()
	{
		return EntityManager::EntityCollection::Iterator(em, compMask, &compEntColl, compEntColl.end());
	}
}

// EntityManager::EntityCollection::Iterator
namespace ecs
{
	inline EntityManager::EntityCollection::Iterator::Iterator(EntityManager &em,
			const ComponentManager::ComponentMask &compMask,
			ComponentPoolEntityCollection *compEntColl,
			ComponentPoolEntityCollection::Iterator compIt)
		: em(em), compMask(compMask), compEntColl(compEntColl), compIt(compIt)
	{
		// might need to advance this iterator to the first entity that satisfies the mask
		// since *compIt is not guarenteed to satisfy the mask right now
		if (compIt != compEntColl->end())
		{
			Entity::Id e = *compIt;
			auto entCompMask = em.compMgr.entCompMasks.at(e.Index());
			if ((entCompMask & compMask) != compMask)
			{
				this->operator++();
			}
		}
	}

	inline EntityManager::EntityCollection::Iterator &EntityManager::EntityCollection::Iterator::operator++()
	{
		// find the next entity that has all the components specified by this->compMask
		while (++compIt != compEntColl->end())
		{
			Entity::Id e = *compIt;
			auto entCompMask = em.compMgr.entCompMasks.at(e.Index());
			if ((entCompMask & compMask) == compMask)
			{
				break;
			}
		}
		return *this;
	}

	inline bool EntityManager::EntityCollection::Iterator::operator==(const Iterator &other)
	{
		return compMask == other.compMask && compIt == other.compIt;
	}

	inline bool EntityManager::EntityCollection::Iterator::operator!=(const Iterator &other)
	{
		return !(*this == other);
	}

	inline Entity EntityManager::EntityCollection::Iterator::operator*()
	{
		return Entity(&this->em, *compIt);
	}
}
