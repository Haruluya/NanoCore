#pragma once
#include "NanoCore.h"
#include "modules/entity/Scene.h"
#include "modules/entity/Entity.h"
#include "NanoPanel.h"

namespace NanoCore {

	class HierarchyPanel : public NanoPanel
	{
	public:
		HierarchyPanel() = default;
		HierarchyPanel(const Shared<Scene>& scene);
		virtual void OnUIRender(bool& isOpen) override;
		virtual void SetScene(const Shared<Scene>& context) override;


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