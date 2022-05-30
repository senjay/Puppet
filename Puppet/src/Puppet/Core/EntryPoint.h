#pragma once
#ifdef  PP_PLATFORM_WINDOWS

extern Puppet::Application* Puppet::CreateApplication();

int main(int argc, char** argv)
{
	Puppet::Log::Init();
	PP_CORE_WARN("Initialized Log!");

	PP_PROFILE_BEGIN_SESSION("Startup","Startup.json");
	Puppet::Application* app=Puppet::CreateApplication();
	PP_PROFILE_END_SESSION();

	PP_PROFILE_BEGIN_SESSION("Runtime", "Runtime.json");
	app->Run();
	PP_PROFILE_END_SESSION();

	PP_PROFILE_BEGIN_SESSION("Shutdown","Shutdown.json");
	delete app;
	PP_PROFILE_END_SESSION();
	return 0;
}

#endif //  PP_PLATFORM_WINDOWS
