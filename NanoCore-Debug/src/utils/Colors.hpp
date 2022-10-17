#pragma once
#include "NanoCore.h"
#include "Imgui/imgui.h"

namespace NanoCore::UI
{

	namespace Color {

		constexpr auto accent = IM_COL32(236, 158, 36, 255);
		constexpr auto highlight = IM_COL32(39, 185, 242, 255);
		constexpr auto niceBlue = IM_COL32(83, 232, 254, 255);
		constexpr auto compliment = IM_COL32(78, 151, 166, 255);
		constexpr auto background = IM_COL32(36, 36, 36, 255);
		constexpr auto backgroundDark = IM_COL32(26, 26, 26, 255);
		constexpr auto titlebar = IM_COL32(21, 21, 21, 255);
		constexpr auto propertyField = IM_COL32(15, 15, 15, 255);
		constexpr auto text = IM_COL32(192, 192, 192, 255);
		constexpr auto textBrighter = IM_COL32(210, 210, 210, 255);
		constexpr auto textDarker = IM_COL32(128, 128, 128, 255);
		constexpr auto muted = IM_COL32(77, 77, 77, 255);
		constexpr auto groupHeader = IM_COL32(47, 47, 47, 255);
		constexpr auto selection = IM_COL32(237, 192, 119, 255);
		constexpr auto selectionMuted = IM_COL32(237, 201, 142, 23);
		constexpr auto backgroundPopup = IM_COL32(50, 50, 50, 255); // most blue
	}

	static inline float Convert_sRGB_FromLinear(float theLinearValue)
	{
		return theLinearValue <= 0.0031308f
			? theLinearValue * 12.92f
			: powf(theLinearValue, 1.0f / 2.2f) * 1.055f - 0.055f;
	}

	static inline float Convert_sRGB_ToLinear(float thesRGBValue)
	{
		return thesRGBValue <= 0.04045f
			? thesRGBValue / 12.92f
			: powf((thesRGBValue + 0.055f) / 1.055f, 2.2f);
	}

	ImVec4 ConvertFromSRGB(ImVec4 colour)
	{
		return ImVec4(Convert_sRGB_FromLinear(colour.x),
			Convert_sRGB_FromLinear(colour.y),
			Convert_sRGB_FromLinear(colour.z),
			colour.w);
	}

	ImVec4 ConvertToSRGB(ImVec4 colour)
	{
		return ImVec4(std::pow(colour.x, 2.2f),
			std::pow(colour.y, 2.2f),
			std::pow(colour.z, 2.2f),
			colour.w);
	}


}