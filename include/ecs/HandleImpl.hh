#pragma once

#include "ecs/Handle.hh"


namespace ecs
{
	template <typename CompType>
	Handle<CompType>::Handle()
		: eId(), compPool(nullptr)
	{
	}

	template <typename CompType>
	Handle<CompType>::Handle(Entity::Id entityId, ComponentPool<CompType> *componentPool)
		: eId(entityId), compPool(componentPool)
	{
	}

	template <typename CompType>
	Handle<CompType>::~Handle()
	{
	}

	template <typename CompType>
	CompType &Handle<CompType>::operator*() const
	{
		if (!compPool)
		{
			throw std::runtime_error("trying to dereference a null Handle!");
		}
		return *compPool->Get(eId);
	}

	template <typename CompType>
	CompType *Handle<CompType>::operator->() const
	{
		if (!compPool)
		{
			throw std::runtime_error("trying to dereference a null Handle!");
		}
		return compPool->Get(eId);
	}
}