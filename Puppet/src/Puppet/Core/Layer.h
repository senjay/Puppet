#pragma once
#include "Puppet/Core/Core.h"
#include "Puppet/Events/Event.h"
#include "Puppet/Core/TimeStep.h"
namespace Puppet {
	/// Layer �ɴ�����ʾ��Ҳ���Դ����¼���
	/// ��һ����Ϸ�У��㼶����������ʾ��ͬ�����ݣ���ͬ�Ĳ㼶���Խ��ն������¼�
	class PUPPET_API Layer
	{
	public:
		// ��ǰ�������
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		// ���㱻��������ʱ��
		virtual void OnAttach() {}
		// ���㱻ȡ����ʱ��
		virtual void OnDetach() {}
		// ��Ⱦupdate
		virtual void OnUpdate(TimeStep ts) {}
		// UI��Ⱦ
		virtual void OnUIRender() {}
		// ��ǰ�����¼���ʱ��
		virtual void OnEvent(Event& event) {}
		// ��ǰ�������
		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};
}
