#pragma once
#include <type_traits>
namespace Puppet {

	template<typename Derived >
	class PUPPET_API CoreSingleton 
	{
	protected:
		CoreSingleton() = default;
	public:
		static Derived& getInstance()noexcept(std::is_nothrow_constructible<Derived>::value)
		{
			static Derived instance;
			return instance;
		}
		virtual ~CoreSingleton() noexcept = default;
		CoreSingleton(const CoreSingleton&) = delete;
		CoreSingleton& operator=(const CoreSingleton&) = delete;
	};
}
