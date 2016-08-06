#pragma once

// This is the only file needed to be included to use the ECS.
// Normal header and header-impl files are needed to allow a header-only
// library while preventing cyclic dependencies.

// the entire API that a user of this library needs
#include "ecs/Entity.hh"
#include "ecs/EntityManager.hh"

// Impl files whose include order doesn't matter
#include "ecs/EntityImpl.hh"
#include "ecs/EntityManagerImpl.hh"
#include "ecs/CommonImpl.hh"
#include "ecs/ComponentManagerImpl.hh"
#include "ecs/ComponentStorageImpl.hh"

