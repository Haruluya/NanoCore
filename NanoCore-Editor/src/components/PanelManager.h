#pragma once
#include "NanoCore.h"
#include <unordered_map>
#include <utility>
#include "NanoPanel.h"

namespace NanoCore {

	struct PanelData
	{
		const char* ID = "";
		const char* Name = "";
		Shared<NanoPanel> Panel = nullptr;
		bool IsOpen = false;
	};

	// NOTE(Peter): For now this just corresponds to what menu the panel will be listed in (in Hazelnut)
	enum class PanelCategory
	{
		Edit, View, _COUNT
	};

	class PanelManager
	{
	public:
		PanelManager() = default;
		~PanelManager()
		{
			for (auto& map : m_Panels)
				map.clear();
		}

		template<typename TPanel>
		Shared<TPanel> AddPanel(PanelCategory category, const PanelData& panelData)
		{
			static_assert(std::is_base_of<NanoPanel, TPanel>::value, "PanelManager::AddPanel requires TPanel to inherit from NanoPanel");

			auto& panelMap = m_Panels[(size_t)category];

			uint32_t id = Hash::GenerateFNVHash(panelData.ID);
			if (panelMap.find(id) != panelMap.end())
			{
				NANO_ENGINE_LOG_ERROR("PanelManager", "A panel with the id '{0}' has already been added.", panelData.ID);
				return nullptr;
			}

			panelMap[id] = panelData;
			return panelData.Panel.As<TPanel>();
		}

		template<typename TPanel, typename... TArgs>
		Shared<TPanel> AddPanel(PanelCategory category, const char* strID, bool isOpenByDefault, TArgs&&... args)
		{
			return AddPanel<TPanel>(category, PanelData{ strID, strID, Shared<TPanel>::Create(std::forward<TArgs>(args)...), isOpenByDefault });
		}

		template<typename TPanel, typename... TArgs>
		Shared<TPanel> AddPanel(PanelCategory category, const char* strID, const char* displayName, bool isOpenByDefault, TArgs&&... args)
		{
			return AddPanel<TPanel>(category, PanelData{ strID, displayName, Shared<TPanel>::Create(std::forward<TArgs>(args)...), isOpenByDefault });
		}

		template<typename TPanel>
		Shared<TPanel> GetPanel(const char* strID)
		{
			static_assert(std::is_base_of<NanoPanel, TPanel>::value, "PanelManager::AddPanel requires TPanel to inherit from NanoPanel");

			uint32_t id = Hash::GenerateFNVHash(strID);

			for (const auto& panelMap : m_Panels)
			{
				if (panelMap.find(id) == panelMap.end())
					continue;

				return panelMap.at(id).Panel.As<TPanel>();
			}

			NANO_ENGINE_LOG_ERROR("PanelManage: Couldn't find panel with id '{0}'", strID);
			return nullptr;
		}

		void RemovePanel(const char* strID)
		{
			uint32_t id = Hash::GenerateFNVHash(strID);
			for (auto& panelMap : m_Panels)
			{
				if (panelMap.find(id) == panelMap.end())
					continue;

				panelMap.erase(id);
				return;
			}

			NANO_ENGINE_LOG_ERROR("PanelManager", "Couldn't find panel with id '{0}'", strID);

		}

		void OnUIRender()
		{

			for (auto& panelMap : m_Panels)
			{

				for (auto& [id, panelData] : panelMap)
				{

					if (panelData.IsOpen)
						panelData.Panel->OnUIRender(panelData.IsOpen);
				}
			}
		}

		void OnEvent(Event& e)
		{
			for (auto& panelMap : m_Panels)
			{
				for (auto& [id, panelData] : panelMap)
					panelData.Panel->OnEvent(e);
			}
		}

		void SetScene(const Shared<Scene>& context)
		{
			for (auto& panelMap : m_Panels)
			{
				for (auto& [id, panelData] : panelMap)
					panelData.Panel->SetScene(context);
			}
		}

		void OnProjectChanged(const Shared<Project>& project)
		{
			for (auto& panelMap : m_Panels)
			{
				for (auto& [id, panelData] : panelMap)
					panelData.Panel->OnProjectChanged(project);
			}
		}

		std::unordered_map<uint32_t, PanelData>& GetPanels(PanelCategory category) { return m_Panels[(size_t)category]; }
		const std::unordered_map<uint32_t, PanelData>& GetPanels(PanelCategory category) const { return m_Panels[(size_t)category]; }

	private:
		std::array<std::unordered_map<uint32_t, PanelData>, (size_t)PanelCategory::_COUNT> m_Panels;
	};

}
