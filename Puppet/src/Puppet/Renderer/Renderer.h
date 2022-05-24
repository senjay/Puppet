#pragma once

namespace Puppet {

	// ��Ⱦ����
	enum class RendererAPI
	{
		None = 0,
		OpenGL = 1
	};

	// ��Ⱦ�ӿ�
	class Renderer
	{
	public:
		static RendererAPI GetAPI() { return s_RendererAPI; }
		static void SetAPI(RendererAPI apiType) {s_RendererAPI = apiType;}
	private:
		static RendererAPI s_RendererAPI;
	};

}