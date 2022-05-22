#pragma once
#include <type_traits>
namespace Puppet {

	template<typename T>
	class PUPPET_API CoreSingleton 
	{
	protected:
		CoreSingleton() = default;
	public:
		static T& getInstance()noexcept(std::is_nothrow_constructible<T>::value)
		{
			static T instance;
			return instance;
		}
		virtual ~CoreSingleton() noexcept = default;
		CoreSingleton(const CoreSingleton&) = delete;
		CoreSingleton& operator=(const CoreSingleton&) = delete;
	};
}
