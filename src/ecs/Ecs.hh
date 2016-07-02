#pragma once

// This is the only file needed to be included to use the ECS.
// Impl is needed to be included since many functions are templated
// Defn/Impl is split between headers because of cyclic dependencies

#include "Entity.hh"
#include "EntityManager.hh"

#include "EntityImpl.hh"
#include "EntityManagerImpl.hh"
