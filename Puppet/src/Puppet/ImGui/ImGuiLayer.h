#pragma once

#include "Puppet/Core/Layer.h"
#include "Puppet/Events/ApplicationEvent.h"
#include "Puppet/Events/KeyEvent.h"
#include "Puppet/Events/MouseEvent.h"
namespace Puppet {

    class PUPPET_API ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        virtual void OnAttach() override;
        virtual void OnDetach()override;
        virtual void OnEvent(Event& event)override {}
        virtual void OnUIRender()override { OnImGuiRender(); }
        void OnImGuiRender();
        void Begin();
        void End();
    private:
        float m_Time = 0.0f;
    };
}

