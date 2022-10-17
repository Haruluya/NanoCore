#pragma once
#include "ncpch.h"
#include "modules/info/Project.h"
#include "modules/entity/Scene.h"


namespace NanoCore {
	class NanoPanel : public RefCount
	{
	public:
		virtual ~NanoPanel() {}

		virtual void OnUIRender(bool& isOpen) = 0;
		virtual void OnEvent(Event& e){}
		virtual void OnProjectChanged(const Shared<Project>& project) {}
		virtual void SetScene(const Shared<Scene>& context) {}
	};
}