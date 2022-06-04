#include <Puppet.h>
#include <imgui.h>
#include <Puppet/Core/EntryPoint.h>
#include "EditorLayer.h"
namespace Puppet{

	class PuppetEditor :public Application
	{
	public:
		PuppetEditor():Application("Puppet Editor")
		{
			PushLayer(new EditorLayer());
		}
		~PuppetEditor(){}

	};
	Application* CreateApplication()
	{
		return new PuppetEditor();
	}
}
