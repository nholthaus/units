# units
a compile-time, header-only, unit conversion library built on c++14 with no dependencies.

The library consists of a single file (include/units.h), plus unit tests. To incorporate the library into your project, simply copy the header into a location in your include path. Use the included CMake project to build the tests.

unitTests/main.cpp provides comprehensive examples of how to use the library.

Complex, recurively-defined conversions are performed in just 5 processor instructions:

		auto test2 = convert<years, weeks>(2.0);
	00007FF6D6475ECC  mov         eax,16Dh  
	00007FF6D6475ED1  xorps       xmm1,xmm1  
	00007FF6D6475ED4  cvtsi2sd    xmm1,rax  
	00007FF6D6475ED9  mulsd       xmm1,mmword ptr [__real@4000000000000000 (07FF6D64AFE38h)]  
	00007FF6D6475EE1  divsd       xmm1,mmword ptr [__real@401c000000000000 (07FF6D64AFE58h)] 
		EXPECT_NEAR(104.357143, test2, 5.0e-7);
	00007FF6D6475EE9  ...

An explanation of the instructions can be found at: http://stackoverflow.com/questions/35103741/what-is-the-purpose-of-xorps-on-the-same-register/35103871#35103871

In the library, the year to week conversion is defined in terms of
years -> days -> hours -> minutes -> seconds -> minutes -> hours -> days -> weeks
but the total conversion ratio is computed at compile-time and the math is optimized to two floating-point operations.

Unit conversions between equivalent types are optimized away completely, and generate no machine code.

Defining new units is simple, as they can be recusively defined as ratio of previously-defined units in a way that mimicks natural language and is highly readable:

	namespace time
	{
		using seconds = unit<std::ratio<1>, category::time_unit>;
		using minutes = unit<std::ratio<60>, seconds>;
		using hours = unit<std::ratio<60>, minutes>;
		using days = unit<std::ratio<24>, hours>;
		using weeks = unit<std::ratio<7>, days>;
		using years = unit<std::ratio<365>, days>;
	}

Compound units are defined in a similar manner, with additional helper functions for polynomials:

	using acceleration = compound_unit<meters, inverse<squared<seconds>>>;