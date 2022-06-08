#pragma once

#include "Puppet/Core/Layer.h"
#include "Puppet/Events/ApplicationEvent.h"
#include "Puppet/Events/KeyEvent.h"
#include "Puppet/Events/MouseEvent.h"
namespace Puppet {

    class ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        virtual void OnAttach() override;
        virtual void OnDetach()override;
        virtual void OnEvent(Event& event)override;
        void Begin();
        void End();
        void BlockEvents(bool block) { m_BlockEvents = block; }
    private:
        void SetDarkThemeColors();
    private:
        bool m_BlockEvents = true;
    };
}

