#pragma once

namespace Puppet
{
	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;
		// ��ʼ����Ⱦ����
		virtual void Init() = 0;
		// ��Ⱦ����
		virtual void SwapBuffers() = 0;
	};
}
