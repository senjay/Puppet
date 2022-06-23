#pragma once
#include "RendererAPI.h"
#include "RenderCommand.h"
#include "RenderCommandQueue.h"
#include "RenderPass.h"
#include "Puppet/Camera/EditorCamera.h"
#include "Shader.h"
#include "Texture.h"
#include "Puppet/Mesh/Mesh.h"
namespace Puppet {
	class Renderer
	{
	public:
		typedef void(*RenderCommandFn)(void*);
		static void Init();
		
		static void Clear(const glm::vec4& color = glm::vec4{0.0f,0.0f,0.0f,1.0f});
		static void SetClearColor(const glm::vec4& color);

		static void OnWindowResize(uint32_t width, uint32_t height);
		static void Shutdown();
		
		template<typename FuncT>
		static void Submit(FuncT&& func)
		{
			auto renderCmd = [](void* ptr) {
				auto pFunc = (FuncT*)ptr;
				(*pFunc)();

				// NOTE: Instead of destroying we could try and enforce all items to be trivally destructible
				// however some items like uniforms which contain std::strings still exist for now
				// static_assert(std::is_trivially_destructible_v<FuncT>, "FuncT must be trivially destructible");
				pFunc->~FuncT();//destruct lambda object
			};
			auto storageBuffer = GetRenderCommandQueue().Allocate(renderCmd, sizeof(func));
			new (storageBuffer) FuncT(std::forward<FuncT>(func));
		}
		static void WaitAndRender();
		
		static void BeginRenderPass(Ref<RenderPass> renderPass, bool clear = true);
		static void EndRenderPass();
		static void SubmitQuad(Ref<MaterialInstance> material, const glm::mat4& transform = glm::mat4(1.0f));
		static void SubmitFullscreenQuad(Ref<MaterialInstance> material);
		static void SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform, Ref<MaterialInstance> overrideMaterial = nullptr);
		static void SubmitMeshWithShader(Ref<Mesh> mesh, const glm::mat4& transform, Ref<Shader> shader);
		static void DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest = true);
		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
		static std::pair<uint32_t, uint32_t>GetCommandQueueStatus() { return GetRenderCommandQueue().GetCommandQueueStatus(); }
	private:
		static RenderCommandQueue& GetRenderCommandQueue();
	private:


	};

}