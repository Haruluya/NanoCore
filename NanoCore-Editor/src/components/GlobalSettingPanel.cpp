#include "GlobalSettingPanel.h"
#include <imgui/imgui.h>

NanoCore::GlobalSettingPanel::GlobalSettingPanel()
{
}

void NanoCore::GlobalSettingPanel::OnUIRender(bool& isOpen)
{
	if (ImGui::Begin("Settings")) {
		//ImGui::Checkbox("Show physics colliders", &m_ShowPhysicsColliders);
		ImGui::End();
	}

}
