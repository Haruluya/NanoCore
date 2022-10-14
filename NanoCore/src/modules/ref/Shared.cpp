#include "ncpch.h"
#include "Shared.h"

namespace NanoCore {


	static std::unordered_set<void*> s_LiveReferences;
	static std::mutex s_LiveReferenceMutex;

	void AddToLiveReferences(void* instance)
	{
		std::scoped_lock<std::mutex> lock(s_LiveReferenceMutex);
		NANO_ENGINE_LOG_ASSERT(instance);
		s_LiveReferences.insert(instance);
	}

	void RemoveFromLiveReferences(void* instance)
	{
		std::scoped_lock<std::mutex> lock(s_LiveReferenceMutex);
		NANO_ENGINE_LOG_ASSERT(instance);
		NANO_ENGINE_LOG_ASSERT(s_LiveReferences.find(instance) != s_LiveReferences.end());
		s_LiveReferences.erase(instance);
	}

	bool IsLive(void* instance)
	{
		NANO_ENGINE_LOG_ASSERT(instance);
		return s_LiveReferences.find(instance) != s_LiveReferences.end();
	}
}