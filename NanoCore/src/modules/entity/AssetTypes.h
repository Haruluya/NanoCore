#pragma once
#include <string>
#include "core/log/log.h"
#include "Core/base/Base.h"

namespace NanoCore {

	enum class AssetFlag : uint16_t
	{
		None = 0,
		Missing = BIT(0),
		Invalid = BIT(1)
	};

	enum class AssetType : uint16_t
	{
		None = 0,
		Scene = 1,
		Prefab = 2,
		Mesh = 3,
		StaticMesh = 4,
		MeshSource = 5,
		Material = 6,
		Texture = 7,
		EnvMap = 8,
		Audio = 9,
		PhysicsMat = 10,
		SoundConfig = 11,
		SpatializationConfig = 12,
		Font = 13,
		Script = 14,
		MeshCollider = 15,
		SoundGraphSound = 16,
	};

	namespace Utils {

		inline AssetType AssetTypeFromString(const std::string& assetType)
		{
			if (assetType == "None")			return AssetType::None;
			if (assetType == "Scene")			return AssetType::Scene;
			if (assetType == "Prefab")			return AssetType::Prefab;
			if (assetType == "Mesh")			return AssetType::Mesh;
			if (assetType == "StaticMesh")		return AssetType::StaticMesh;
			if (assetType == "MeshAsset")		return AssetType::MeshSource; // DEPRECATED
			if (assetType == "MeshSource")		return AssetType::MeshSource;
			if (assetType == "Material")		return AssetType::Material;
			if (assetType == "Texture")			return AssetType::Texture;
			if (assetType == "EnvMap")			return AssetType::EnvMap;
			if (assetType == "Audio")			return AssetType::Audio;
			if (assetType == "PhysicsMat")		return AssetType::PhysicsMat;
			if (assetType == "SoundConfig")		return AssetType::SoundConfig;
			if (assetType == "Font")			return AssetType::Font;
			if (assetType == "Script")			return AssetType::Script;
			if (assetType == "MeshCollider")	return AssetType::MeshCollider;
			if (assetType == "SoundGraphSound")	return AssetType::SoundGraphSound;

			NANO_ENGINE_LOG_ASSERT(false, "Unknown Asset Type");
			return AssetType::None;
		}

		inline const char* AssetTypeToString(AssetType assetType)
		{
			switch (assetType)
			{
			case AssetType::None:			return "None";
			case AssetType::Scene:			return "Scene";
			case AssetType::Prefab:			return "Prefab";
			case AssetType::Mesh:			return "Mesh";
			case AssetType::StaticMesh:		return "StaticMesh";
			case AssetType::MeshSource:		return "MeshSource";
			case AssetType::Material:		return "Material";
			case AssetType::Texture:		return "Texture";
			case AssetType::EnvMap:			return "EnvMap";
			case AssetType::Audio:			return "Audio";
			case AssetType::PhysicsMat:		return "PhysicsMat";
			case AssetType::SoundConfig:	return "SoundConfig";
			case AssetType::Font:			return "Font";
			case AssetType::Script:			return "Script";
			case AssetType::MeshCollider:	return "MeshCollider";
			case AssetType::SoundGraphSound: return "SoundGraphSound";
			}

			NANO_ENGINE_LOG_ASSERT(false, "Unknown Asset Type");
			return "None";
		}

	}
}
