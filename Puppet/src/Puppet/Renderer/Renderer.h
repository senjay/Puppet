#pragma once

namespace Puppet {

	// 渲染类型
	enum class RendererAPI
	{
		None = 0,
		OpenGL = 1
	};

	// 渲染接口
	class Renderer
	{
	public:
		static RendererAPI GetAPI() { return s_RendererAPI; }
		static void SetAPI(RendererAPI apiType) {s_RendererAPI = apiType;}
	private:
		static RendererAPI s_RendererAPI;
	};

}