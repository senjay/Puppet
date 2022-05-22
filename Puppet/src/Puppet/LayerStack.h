#pragma once

#include "Puppet/Core.h"
#include "Layer.h"

namespace Puppet {

	/// ���ڴ����Ĺ����ϵ
	class PUPPET_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();
		/// ���б�ĵ�һ��Ԫ�����һ���㼶
		void PushLayer(Layer* layer);

		/// ���һ���㵽�б�
		void PushOverlay(Layer* overlay);
		/// ������Ĳ�ȡ����
		void PopLayer(Layer* layer);
		/// ���б���ȡ��һ����
		void PopOverlay(Layer* overlay);
		/// ��ʼ������
		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }
		std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
		std::vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }
	private:
		/// ���еĲ��б�
		std::vector<Layer*> m_Layers;
		size_t m_LayerInsertIndex=0;
	};
}

