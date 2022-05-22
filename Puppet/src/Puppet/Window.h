// Window.h
#pragma once

#include "PPpch.h"

#include "Puppet/Core.h"
#include "Puppet/Events/Event.h"

namespace Puppet {
	// ����window����
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "Puppet Engine",
			unsigned int width = 1280,
			unsigned int height = 720)
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


		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enable) = 0;
		virtual bool IsVSync() const = 0;
		virtual void* GetNativeWindow()const = 0;

		// ���ﶨ��һ����̬��Create����������ΪWindowProps��ȫ��ʹ�õ�Ĭ������
		// ���������WindowsWindow.cpp����ʵ��
		static Window* Create(const WindowProps& prop = WindowProps());
	};
}

