#include <iostream>

#include "variant.hxx"

namespace
{
	class X
	{
	public:
		X(X const &) noexcept
		{}

		X & operator = (X const &)
		{}
	};
}

int main()
{
	std::cin.ignore();
}
