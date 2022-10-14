#pragma once

#include "RefCount.h"


namespace NanoCore {

	void AddToLiveReferences(void* instance);
	void RemoveFromLiveReferences(void* instance);
	bool IsLive(void* instance);

	template<typename T>
	using Unique = std::unique_ptr<T>;

	//template<typename T>
	//using Shared = std::shared_ptr<T>;

	template<typename T>
	class Shared
	{
	public:
		Shared()
			: m_Instance(nullptr)
		{
		}

		Shared(std::nullptr_t n)
			: m_Instance(nullptr)
		{
		}

		Shared(T* instance)
			: m_Instance(instance)
		{
			static_assert(std::is_base_of<RefCount, T>::value, "Class is not SharedCounted!");

			IncShared();
		}

		template<typename T2>
		Shared(const Shared<T2>& other)
		{
			m_Instance = (T*)other.m_Instance;
			IncShared();
		}

		template<typename T2>
		Shared(Shared<T2>&& other)
		{
			m_Instance = (T*)other.m_Instance;
			other.m_Instance = nullptr;
		}

		static Shared<T> CopyWithoutIncrement(const Shared<T>& other)
		{
			Shared<T> result = nullptr;
			result->m_Instance = other.m_Instance;
			return result;
		}

		~Shared()
		{
			DecShared();
		}

		Shared(const Shared<T>& other)
			: m_Instance(other.m_Instance)
		{
			IncShared();
		}

		Shared& operator=(std::nullptr_t)
		{
			DecShared();
			m_Instance = nullptr;
			return *this;
		}

		Shared& operator=(const Shared<T>& other)
		{
			other.IncShared();
			DecShared();

			m_Instance = other.m_Instance;
			return *this;
		}

		template<typename T2>
		Shared& operator=(const Shared<T2>& other)
		{
			other.IncShared();
			DecShared();

			m_Instance = other.m_Instance;
			return *this;
		}

		template<typename T2>
		Shared& operator=(Shared<T2>&& other)
		{
			DecShared();

			m_Instance = other.m_Instance;
			other.m_Instance = nullptr;
			return *this;
		}

		operator bool() { return m_Instance != nullptr; }
		operator bool() const { return m_Instance != nullptr; }

		T* operator->() { return m_Instance; }
		const T* operator->() const { return m_Instance; }

		T& operator*() { return *m_Instance; }
		const T& operator*() const { return *m_Instance; }

		T* Raw() { return  m_Instance; }
		const T* Raw() const { return  m_Instance; }

		void Reset(T* instance = nullptr)
		{
			DecShared();
			m_Instance = instance;
		}

		template<typename T2>
		Shared<T2> As() const
		{
			return Shared<T2>(*this);
		}

		template<typename... Args>
		static Shared<T> Create(Args&&... args)
		{

			return Shared<T>(new T(std::forward<Args>(args)...));
		}

		bool operator==(const Shared<T>& other) const
		{
			return m_Instance == other.m_Instance;
		}

		bool operator!=(const Shared<T>& other) const
		{
			return !(*this == other);
		}

		bool EqualsObject(const Shared<T>& other)
		{
			if (!m_Instance || !other.m_Instance)
				return false;

			return *m_Instance == *other.m_Instance;
		}
	private:
		void IncShared() const
		{
			if (m_Instance)
			{
				m_Instance->IncRefCount();
				AddToLiveReferences((void*)m_Instance);
			}
		}

		void DecShared() const
		{
			if (m_Instance)
			{
				m_Instance->DecRefCount();
				if (m_Instance->GetRefCount() == 0)
				{
					delete m_Instance;
					RemoveFromLiveReferences((void*)m_Instance);
					m_Instance = nullptr;
				}
			}
		}

		template<class T2>
		friend class Shared;
		mutable T* m_Instance;
	};
}



