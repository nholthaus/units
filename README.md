# units
a compile-time, header-only, unit conversion library built on c++14 with no dependencies.

Releases
--------
v1.0.0 - 02/07/2016    - Initial release, tested with msvc2015
v1.1.0 - TBD (Feb '16) - Adds doxygen, testing with gcc-4.9.2, msvc2013, msvc2015.

Description
-----------

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
	
Unit containers
---------------

In addition to providing unit tags to perform conversion, the library also provides container types. Containers are derived from the `unit_t` class, and have the form [unitname]_t, e.g. `meter_t`. Containers provide additional advanges over template tag conversions, without any increase in overhead, thanks to the c++ type system.

unit containers are defined in terms of the units they represent, the underlying type of the container, and an optional non-linear scale (think decibels or richter scale). For example, `meter_t` would be defined: `using meter_t = unit_t<length::meter, double, linear_scale>`, or simply `using meter_t = unit_t<length::meter>`, since the underlying type and scale parameters default to `double` and `linear_scale` respectively.

Units of compatible types (e.g length units) can be implicitely converted/assigned to one another. Units (with the exception of dimensionless types) cannot be implicitely converted to/from built-in types. They can be constructed from built-in types, and operator() can be used to retrieve a built-in type value. That said, the user should prefer to operate within the unit type-space as much as is practical. 

Unit containers provide type safety for 