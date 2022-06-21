// Buffer.h
#pragma once

namespace Puppet
{
	// ¶¥µãË÷ÒýBuffer
	class IndexBuffer :public RefCounted
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void SetData(const void* data, uint32_t count, uint32_t offset = 0)=0;
		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t count);
		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
	};
}

