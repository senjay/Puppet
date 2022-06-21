#pragma once
#include <glm/glm.hpp>
#include "VertexArray.h"
namespace Puppet {
	using RendererID = uint32_t;

	enum class PrimitiveType
	{
		None = 0,
		Triangles = 1,
		Lines = 2
	};
	struct RenderAPICapabilities
	{
		std::string Vendor;
		std::string Renderer;
		std::string Version;

		int MaxSamples = 0;
		float MaxAnisotropy = 0.0f;
		int MaxTextureUnits = 0;
	};
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0,
			OpenGL = 1
		};
	public:
		virtual ~RendererAPI() = default;
		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear(const glm::vec4& color) = 0;
		virtual void DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest = true)=0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetLineThickness(float thickness) = 0;
		virtual RenderAPICapabilities& GetCapabilities() = 0;
		static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};

}