#pragma once

#include "Puppet/Layer.h"
#include "Puppet/Events/ApplicationEvent.h"
#include "Puppet/Events/KeyEvent.h"
#include "Puppet/Events/MouseEvent.h"
namespace Puppet {

    class HAZEL_API ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        void OnAttach();
        void OnDetach();
        void OnUpdate();
        void OnEvent(Event& event);
    private:
        // 定义UI需要的事件，这里的事件只会作用到UI
        // 游戏的事件流程 硬件->系统->OpenGL->Game Engine->ImGui->User
        bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
        bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
        bool OnMouseMovedEvent(MouseMovedEvent& e);
        bool OnMouseScrolledEvent(MouseScrolledEvent& e);
        bool OnKeyPressedEvent(KeyPressedEvent& e);
        bool OnKeyReleasedEvent(KeyReleasedEvent& e);
        bool OnKeyTypedEvent(KeyTypedEvent& e);
        bool OnWindowResizeEvent(WindowResizeEvent& e);
    private:
        float m_Time = 0.0f;
    };
}

