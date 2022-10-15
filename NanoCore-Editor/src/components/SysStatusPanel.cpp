#include "SysStatusPanel.h"
#include "Imgui/imgui.h"
NanoCore::SysStatusPanel::SysStatusPanel()
{
}

void NanoCore::SysStatusPanel::OnImGuiRender()
{
	if (ImGui::Begin("Stats")) {
		std::string name = "None";
		if (m_HoveredEntity)
			name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
		ImGui::Text("Hovered Entity: %s", name.c_str());

		auto stats = RenderUtils::GetStats();
		ImGui::Text("RenderUtils Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::End();
	}
}
