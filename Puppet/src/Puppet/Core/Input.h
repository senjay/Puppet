#pragma once
#include "PPpch.h"
namespace Puppet
{
	//输入的抽象类
	//具体实现由各个平台来
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
