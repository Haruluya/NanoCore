#include "ncpch.h"
#include "Entity.h"

namespace NanoCore{

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}

}