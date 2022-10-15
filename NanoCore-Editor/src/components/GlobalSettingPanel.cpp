#include "GlobalSettingPanel.h"
#include <imgui/imgui.h>

NanoCore::GlobalSettingPanel::GlobalSettingPanel()
{
}

void NanoCore::GlobalSettingPanel::OnImGuiRender()
{
	if (ImGui::Begin("Settings")) {
		//ImGui::Checkbox("Show physics colliders", &m_ShowPhysicsColliders);
		ImGui::End();
	}

}
