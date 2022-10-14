#pragma once

#include <atomic>



namespace NanoCore {
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

}

