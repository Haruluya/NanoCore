#include "ncpch.h"
#include "Project.h"


namespace NanoCore {

	Project::Project()
	{
	}

	Project::Project(ProjectConfig projectConfig)
	{
		this->m_Config = projectConfig;
	}

	Project::~Project()
	{
	}

	void Project::SetActive(Shared<Project> project)
	{
		if (s_ActiveProject)
		{

		}

		s_ActiveProject = project;
		if (s_ActiveProject)
		{

		}
	}

	void Project::OnDeserialized()
	{
	}

}
