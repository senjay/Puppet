#pragma once

#include "Puppet/Core.h"
#include "Layer.h"

namespace Puppet {

	/// 用于处理层的管理关系
	class PUPPET_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();
		/// 从列表的第一个元素添加一个层级
		void PushLayer(Layer* layer);

		/// 添加一个层到列表
		void PushOverlay(Layer* overlay);
		/// 将最外的层取出来
		void PopLayer(Layer* layer);
		/// 从列表中取出一个层
		void PopOverlay(Layer* overlay);
		/// 开始迭代器
		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }
		std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
		std::vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }
	private:
		/// 所有的层列表
		std::vector<Layer*> m_Layers;
		size_t m_LayerInsertIndex=0;
	};
}

