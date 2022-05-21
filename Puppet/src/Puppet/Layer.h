#pragma once
#include "Puppet/Core.h"
#include "Puppet/Events/Event.h"

namespace Puppet {
	/// Layer �ɴ�����ʾ��Ҳ���Դ����¼���
	/// ��һ����Ϸ�У��㼶����������ʾ��ͬ�����ݣ���ͬ�Ĳ㼶���Խ��ն������¼�
	class HAZEL_API Layer
	{
	public:
		// ��ǰ�������
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		// ���㱻��������ʱ��
		virtual void OnAttach() {}
		// ���㱻ȡ����ʱ��
		virtual void OnDetach() {}
		// update
		virtual void OnUpdate() {}
		// ��ǰ�����¼���ʱ��
		virtual void OnEvent(Event& event) {}
		// ��ǰ�������
		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};
}
