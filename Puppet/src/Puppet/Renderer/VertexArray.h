#pragma once
#include <memory>
#include "Puppet/Renderer/VertexBuffer.h"
#include "Puppet/Renderer/IndexBuffer.h"
namespace Puppet
{
	class VertexArray :public RefCounted
	{
	public:
		virtual ~VertexArray() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		// 添加一组需要渲染的顶点数据
		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
		// 每一次渲染，标记为一个index
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

		static Ref<VertexArray> Create();
	};

}

