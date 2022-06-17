#pragma once
#include <type_traits>
namespace Puppet {

	template<typename Derived >
	class BaseSingleton 
	{
	protected:
		BaseSingleton() = default;
	public:
		static Derived& GetInstance()noexcept(std::is_nothrow_constructible<Derived>::value)
		{
			static Derived instance;
			return instance;
		}
		virtual ~BaseSingleton() noexcept = default;
		BaseSingleton(const BaseSingleton&) = delete;
		BaseSingleton& operator=(const BaseSingleton&) = delete;
	};
}
