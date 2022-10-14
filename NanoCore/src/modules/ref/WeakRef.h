#pragma once
#include "Shared.h"

namespace NanoCore {
	template<typename T>
	class WeakRef
	{
	public:
		WeakRef() = default;

		WeakRef(Shared<T> ref)
		{
			m_Instance = ref.Raw();
		}

		WeakRef(T* instance)
		{
			m_Instance = instance;
		}

		T* operator->() { return m_Instance; }
		const T* operator->() const { return m_Instance; }

		T& operator*() { return *m_Instance; }
		const T& operator*() const { return *m_Instance; }

		bool IsValid() const { return m_Instance ? IsLive(m_Instance) : false; }
		operator bool() const { return IsValid(); }
	private:
		T* m_Instance = nullptr;
	};
}