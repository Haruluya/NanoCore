#pragma once
#include "NanoCore.h"
#include "modules/entity/Scene.h"
#include "modules/entity/Entity.h"


namespace NanoCore {

	class HierarchyPanel
	{
	public:
		HierarchyPanel() = default;
		HierarchyPanel(const Shared<Scene>& scene);

		void SetContext(const Shared<Scene>& scene);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity);
	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Shared<Scene> m_Context;
		Entity m_SelectionContext;
	};

}