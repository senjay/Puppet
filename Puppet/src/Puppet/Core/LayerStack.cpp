#include "PPpch.h"
#include "LayerStack.h"
namespace Puppet {

	LayerStack::LayerStack()
	{
		
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
		{
			layer->OnDetach();
			delete layer;
		}
	}

	// ��ָ��m_LayerInsert�����һ������
	// emplace ���������ʱ����
	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin()+m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		// ����һ���㣬�������������Ż�Ч��
		auto it = std::find(m_Layers.begin(), m_Layers.begin()+ m_LayerInsertIndex, layer);
		if (it != m_Layers.end())
		{
			layer->OnDetach();
			m_Layers.erase(it);
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto it = std::find(m_Layers.begin()+ m_LayerInsertIndex, m_Layers.end(), overlay);
		if (it != m_Layers.end())
		{
			m_Layers.erase(it);
			overlay->OnDetach();
		}
	}
}
