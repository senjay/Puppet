#pragma once
namespace Puppet {

	class RenderCommandQueue
	{
	public:
		typedef void(*RenderCommandFn)(void*);

		RenderCommandQueue();
		~RenderCommandQueue();


		/*
		*					  (placement new)
		*						return ptr
		*							|
		| __cmdFun_ | _submitSize__ | ___submitFun___|
		cmdFun:对submitFun包装一层的lambda object
		submitSize: submitFun lambda object 's size
		submitFun:cmdFun的参数,真正提交的lambda object,在ptr处调用placement new 保存
		*/
		void* Allocate(RenderCommandFn func, uint32_t size);
		
		void Execute();
		std::pair<uint32_t, uint32_t>GetCommandQueueStatus();
	private:
		uint8_t* m_CommandBuffer;
		uint8_t* m_CommandBufferPtr;
		uint32_t m_CommandCount = 0;
	};
}
