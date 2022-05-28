#pragma once
#include "PPpch.h"
namespace Puppet
{
	//����ĳ�����
	//����ʵ���ɸ���ƽ̨��
	class  Input
	{
	public:
		virtual bool IsKeyPressedImpl(int keycode) = 0;
		virtual bool IsMouseButtonPressedImpl(int button) = 0;
		virtual std::pair<float, float> GetMousePositionImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
	};
}
