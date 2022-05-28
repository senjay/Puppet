#pragma once
#ifdef  PP_PLATFORM_WINDOWS

extern Puppet::Application* Puppet::CreateApplication();

int main(int argc, char** argv)
{
	Puppet::Log::Init();
	PP_CORE_WARN("Initialized Log!");
	PP_INFO("Hello! var={0}",888);
	Puppet::Application* app=Puppet::CreateApplication();
	app->Run();
	delete app;
	return 0;
}

#endif //  PP_PLATFORM_WINDOWS
