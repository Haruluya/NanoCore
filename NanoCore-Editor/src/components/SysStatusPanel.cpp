#include "SysStatusPanel.h"
#include "Imgui/imgui.h"

#include "spdlog/fmt/bundled/core.h"
#include "spdlog/fmt/bundled/format.h"
#include "spdlog/fmt/bundled/format-inl.h"

NanoCore::SysStatusPanel::SysStatusPanel()
{
}

void NanoCore::SysStatusPanel::OnUIRender(bool& isOpen)
{
	if (ImGui::Begin("Stats")) {
		std::string name = "None";
		if (EditorLayer::GetEditorContext()->m_HoveredEntity)
			name = EditorLayer::GetEditorContext()->m_HoveredEntity.GetComponent<TagComponent>().Tag;
		ImGui::Text("Hovered Entity: %s", name.c_str());

		auto stats = RenderUtils::GetStats();
		ImGui::Text("RenderUtils Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::Text(fmt::format("Distance: {}", EditorLayer::GetEditorContext()->m_EditorCamera.m_Distance).c_str());
		ImGui::Text(fmt::format("Focal Point: {}, {}, {}", EditorLayer::GetEditorContext()->m_EditorCamera.m_FocalPoint.x, EditorLayer::GetEditorContext()->m_EditorCamera.m_FocalPoint.y, EditorLayer::GetEditorContext()->m_EditorCamera.m_FocalPoint.z).c_str());
		//ImGui::Text(fmt::format("Rotation: {}, {}, {}", EditorLayer::GetEditorContext()->m_EditorCamera.m_WorldRotation.x, EditorLayer::GetEditorContext()->m_EditorCamera.m_WorldRotation.y, EditorLayer::GetEditorContext()->m_EditorCamera.m_WorldRotation.z).c_str());
		ImGui::Text(fmt::format("Up Dir: {}, {}, {}", EditorLayer::GetEditorContext()->m_EditorCamera.GetUpDirection().x, EditorLayer::GetEditorContext()->m_EditorCamera.GetUpDirection().y, EditorLayer::GetEditorContext()->m_EditorCamera.GetUpDirection().z).c_str());
		ImGui::Text(fmt::format("Strafe Dir: {}, {}, {}", EditorLayer::GetEditorContext()->m_EditorCamera.GetRightDirection().x, EditorLayer::GetEditorContext()->m_EditorCamera.GetRightDirection().y, EditorLayer::GetEditorContext()->m_EditorCamera.GetRightDirection().z).c_str());
		ImGui::Text(fmt::format("Yaw: {}", EditorLayer::GetEditorContext()->m_EditorCamera.m_Yaw).c_str());
		//ImGui::Text(fmt::format("Yaw Delta: {}", EditorLayer::GetEditorContext()->m_EditorCamera.m_YawDelta).c_str());
		ImGui::Text(fmt::format("Pitch: {}", EditorLayer::GetEditorContext()->m_EditorCamera.m_Pitch).c_str());
		//ImGui::Text(fmt::format("Pitch Delta: {}", EditorLayer::GetEditorContext()->m_EditorCamera.m_PitchDelta).c_str());
		ImGui::Text(fmt::format("Position: ({}, {}, {})", EditorLayer::GetEditorContext()->m_EditorCamera.m_Position.x, EditorLayer::GetEditorContext()->m_EditorCamera.m_Position.y, EditorLayer::GetEditorContext()->m_EditorCamera.m_Position.z).c_str());
		//ImGui::Text(fmt::format("Position Delta: ({}, {}, {})", EditorLayer::GetEditorContext()->m_EditorCamera.m_PositionDelta.x, EditorLayer::GetEditorContext()->m_EditorCamera.m_PositionDelta.y, EditorLayer::GetEditorContext()->m_EditorCamera.m_PositionDelta.z).c_str());
		ImGui::Text(fmt::format("View matrix: [{}, {}, {}, {}]", EditorLayer::GetEditorContext()->m_EditorCamera.m_ViewMatrix[0].x, EditorLayer::GetEditorContext()->m_EditorCamera.m_ViewMatrix[0].y, EditorLayer::GetEditorContext()->m_EditorCamera.m_ViewMatrix[0].z, EditorLayer::GetEditorContext()->m_EditorCamera.m_ViewMatrix[0].w).c_str());
		ImGui::Text(fmt::format("		      [{}, {}, {}, {}]", EditorLayer::GetEditorContext()->m_EditorCamera.m_ViewMatrix[1].x, EditorLayer::GetEditorContext()->m_EditorCamera.m_ViewMatrix[1].y, EditorLayer::GetEditorContext()->m_EditorCamera.m_ViewMatrix[1].z, EditorLayer::GetEditorContext()->m_EditorCamera.m_ViewMatrix[1].w).c_str());
		ImGui::Text(fmt::format("		      [{}, {}, {}, {}]", EditorLayer::GetEditorContext()->m_EditorCamera.m_ViewMatrix[2].x, EditorLayer::GetEditorContext()->m_EditorCamera.m_ViewMatrix[2].y, EditorLayer::GetEditorContext()->m_EditorCamera.m_ViewMatrix[2].z, EditorLayer::GetEditorContext()->m_EditorCamera.m_ViewMatrix[2].w).c_str());
		ImGui::Text(fmt::format("		      [{}, {}, {}, {}]", EditorLayer::GetEditorContext()->m_EditorCamera.m_ViewMatrix[3].x, EditorLayer::GetEditorContext()->m_EditorCamera.m_ViewMatrix[3].y, EditorLayer::GetEditorContext()->m_EditorCamera.m_ViewMatrix[3].z, EditorLayer::GetEditorContext()->m_EditorCamera.m_ViewMatrix[3].w).c_str());

		ImGui::End();
	}
}
