#pragma once
#include "Puppet/Core/Core.h"
#include "Puppet/Events/Event.h"
#include "Puppet/Core/TimeStep.h"
namespace Puppet {
	/// Layer 可处理显示，也可以处理事件；
	/// 在一个游戏中，层级可以用于显示不同的内容，不同的层级可以接收独立的事件
	class PUPPET_API Layer
	{
	public:
		// 当前层的名称
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		// 当层被附件到的时候
		virtual void OnAttach() {}
		// 当层被取消的时候
		virtual void OnDetach() {}
		// 渲染update
		virtual void OnUpdate(TimeStep ts) {}
		// UI渲染
		virtual void OnUIRender() {}
		// 当前层有事件的时候
		virtual void OnEvent(Event& event) {}
		// 当前层的名字
		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};
}
