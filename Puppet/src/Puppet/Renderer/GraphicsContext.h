#pragma once

namespace Puppet
{
	class GraphicsContext
	{
	public:
		// ��ʼ����Ⱦ����
		virtual void Init() = 0;
		// ��Ⱦ����
		virtual void SwapBuffers() = 0;
	};
}
