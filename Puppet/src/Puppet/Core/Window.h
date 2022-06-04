// Window.h
#pragma once
#include "Puppet/Core/Core.h"
#include "Puppet/Events/Event.h"

namespace Puppet {
	// ����window����
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "Puppet Engine",
			uint32_t width = 1280,
			uint32_t height = 720)
			:Title(title), Width(width), Height(height)
		{
		}
	};

	// ���崰�ڻ���
	class PUPPET_API Window
	{
	public:
		// �����¼��ص���������
		using EventCallbackFn = std::function<void(Event&)>;
		virtual ~Window() {}
		// ��ǰ�����������ǻ���PC������ʵ�֣��������Ǵ��壬����update
		// -> ����update
		virtual void OnUpdate() = 0;


		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enable) = 0;
		virtual bool IsVSync() const = 0;
		virtual void* GetNativeWindow()const = 0;

		// ���ﶨ��һ����̬��Create����������ΪWindowProps��ȫ��ʹ�õ�Ĭ������
		// ���������WindowsWindow.cpp����ʵ��
		static Scope<Window> Create(const WindowProps& prop = WindowProps());
	};
}

