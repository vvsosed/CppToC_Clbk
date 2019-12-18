#include <iostream>

// Took this amazing code from https://codereview.stackexchange.com/questions/79612/c-ifying-a-capturing-lambda
namespace
{
	template <typename F, int I, typename L, typename R, typename ...A>
	inline F cify(L&& l, R(*)(A...) noexcept(noexcept(
		::std::declval<F>()(::std::declval<A>()...))))
	{
		static thread_local L l_(::std::forward<L>(l));
		static thread_local bool full;

		if (full)
		{
			l_.~L();
			new (static_cast<void*>(&l_)) L(::std::forward<L>(l));
		}
		else
		{
			full = true;
		}

		struct S
		{
			static R f(A... args) noexcept(noexcept(
				::std::declval<F>()(::std::forward<A>(args)...)))
			{
				return l_(::std::forward<A>(args)...);
			}
		};
		return &S::f;
	}
}

template <typename F, int I = 0, typename L>
F cify(L&& l)
{
	return cify<F, I>(::std::forward<L>(l), F());
}

struct SimpleClass {
	void printStr() {
		std::cout << "This string was printed from class method" << std::endl;
	}
};

void printStringFromSimpleFunction() {
	std::cout << "This string was printed from c function" << std::endl;
}


typedef void(*C_CallbackType)(void);

int main(int argc, char**argv) {
	SimpleClass printer;

	C_CallbackType c_clbk1 = printStringFromSimpleFunction;
	C_CallbackType c_clbk2 = nullptr;

	auto const printStringFromClassMethod(cify<C_CallbackType>([&printer](void)
	{
		printer.printStr();
	}));
	c_clbk2 = printStringFromClassMethod;

	c_clbk1();
	c_clbk2();

	return 0;
}

