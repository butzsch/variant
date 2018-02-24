#include <iostream>

#include "variant.hxx"

namespace
{
	class X
	{
	public:
		X(X const & other) noexcept
		{}

		X & operator = (X const & other)
		{}
	};
}

int main()
{
	cpe::Variant<int, float, X> x {0};

	std::cout << std::is_nothrow_copy_constructible<decltype(x)>::value << '\n';
	std::cout << std::is_nothrow_copy_assignable<decltype(x)>::value << '\n';
	std::cout << std::is_nothrow_move_constructible<decltype(x)>::value << '\n';
	std::cout << std::is_nothrow_move_assignable<decltype(x)>::value << '\n';

	std::cin.ignore();
}
