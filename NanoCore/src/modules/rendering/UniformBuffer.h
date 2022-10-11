#pragma once

#include "modules/utils/Base.h"

namespace NanoCore{

	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() {}
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		static Shared<UniformBuffer> Create(uint32_t size, uint32_t binding);
	};

}