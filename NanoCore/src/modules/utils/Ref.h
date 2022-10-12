#pragma once
#include <atomic>
namespace NanoCore{

	template<typename T>
	using Unique = std::unique_ptr<T>;

	template<typename T>
	using Shared = std::shared_ptr<T>;


	class RefCount
	{
	public:
		void IncRefCount() const
		{
			++m_RefCount;
		}
		void DecRefCount() const
		{
			--m_RefCount;
		}

		uint32_t GetRefCount() const { return m_RefCount.load(); }
	private:
		mutable std::atomic<uint32_t> m_RefCount = 0;
	};

	//template<typename T>
	//class WeakRef
	//{
	//public:
	//	WeakRef() = default;

	//	WeakRef(Shared<T> ref)
	//	{
	//		m_Instance = ref.Raw();
	//	}

	//	WeakRef(T* instance)
	//	{
	//		m_Instance = instance;
	//	}

	//	T* operator->() { return m_Instance; }
	//	const T* operator->() const { return m_Instance; }

	//	T& operator*() { return *m_Instance; }
	//	const T& operator*() const { return *m_Instance; }

	//	bool IsValid() const { return m_Instance ? RefUtils::IsLive(m_Instance) : false; }
	//	operator bool() const { return IsValid(); }
	//private:
	//	T* m_Instance = nullptr;
	////};
}