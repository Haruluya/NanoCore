#pragma once

#include <glm/glm.hpp>

namespace NanoCore {

	struct AxisAlignedBB
	{
		glm::vec3 Min, Max;

		AxisAlignedBB()
			: Min(0.0f), Max(0.0f) {}

		AxisAlignedBB(const glm::vec3& min, const glm::vec3& max)
			: Min(min), Max(max) {}

	};


}