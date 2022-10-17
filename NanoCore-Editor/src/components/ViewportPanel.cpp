#include "ViewportPanel.h"
#include "imgui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>






void NanoCore::ViewportPanel::OnUIRender(bool& isOpen)
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });


	ImGuiWindowFlags viewport_window_flags = 0;
	viewport_window_flags |= ImGuiWindowFlags_NoCollapse;
	viewport_window_flags |= ImGuiWindowFlags_NoTitleBar;
	viewport_window_flags |= ImGuiWindowFlags_NoBackground;

	if (ImGui::Begin("Viewport", NULL, viewport_window_flags)) {
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		EditorLayer::GetEditorContext()->m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		EditorLayer::GetEditorContext()->m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		EditorLayer::GetEditorContext()->m_ViewportFocused = ImGui::IsWindowFocused();
		EditorLayer::GetEditorContext()->m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetUILayer()->BlockEvents(!EditorLayer::GetEditorContext()->m_ViewportFocused && !EditorLayer::GetEditorContext()->m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		EditorLayer::GetEditorContext()->m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };


		uint64_t textureID = EditorLayer::GetEditorContext()->m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ EditorLayer::GetEditorContext()->m_ViewportSize.x,EditorLayer::GetEditorContext()->m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				EditorLayer::GetEditorContext()->OpenScene(std::filesystem::path(Project::GetAssetDirectory()) / path);
			}
			ImGui::EndDragDropTarget();
		}

		// Gizmos

		Entity selectedEntity = EditorLayer::GetEditorContext()->m_HierarchyPanel->GetSelectedEntity();


		if (selectedEntity && EditorLayer::GetEditorContext()->m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(EditorLayer::GetEditorContext()->m_ViewportBounds[0].x, EditorLayer::GetEditorContext()->m_ViewportBounds[0].y, EditorLayer::GetEditorContext()->m_ViewportBounds[1].x - EditorLayer::GetEditorContext()->m_ViewportBounds[0].x, EditorLayer::GetEditorContext()->m_ViewportBounds[1].y - EditorLayer::GetEditorContext()->m_ViewportBounds[0].y);

			// Camera

			// Runtime camera from entity
			// auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
			// const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
			// const glm::mat4& cameraProjection = camera.GetProjection();
			// glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

			// Editor camera
			const glm::mat4& cameraProjection = EditorLayer::GetEditorContext()->m_EditorCamera.GetProjection();
			glm::mat4 cameraView = EditorLayer::GetEditorContext()->m_EditorCamera.GetViewMatrix();

			// Entity transform
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if (EditorLayer::GetEditorContext()->m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)EditorLayer::GetEditorContext()->m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Translation = translation;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}


		ImGui::End();
	}
	ImGui::PopStyleVar();
}


