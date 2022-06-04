#pragma once

namespace Puppet
{
	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;
		// ≥ı ºªØ‰÷»æ“˝«Ê
		virtual void Init() = 0;
		// ‰÷»æª∫¥Ê
		virtual void SwapBuffers() = 0;
	};
}
