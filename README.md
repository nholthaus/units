# units
a compile-time, header-only, unit conversion library built on c++14 with no dependencies.

Releases
--------

v1.3.0 - Adds ostream support. bug fixes.  Tested with gcc-4.9.2, msvc2013, msvc2015.

v1.2.2 - Bug fixes (#1) and namespace cleanup. Tested with msvc2015, gcc 5.2.1

v1.2.0 - Adds angular velocity units. Tested with gcc-4.9.2, msvc2013, msvc2015.

v1.1.1 - Adds Doxygen and additional type traits. Tested with gcc-4.9.2, msvc2013, msvc2015.

v1.0.0 - Initial release. Tested with msvc2015

Description
-----------

The library consists of a single file (include/units.h), plus unit tests. To incorporate the library into your project, simply copy the header into a location in your include path. Use the included CMake project to build the tests and documentation.

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

Defining new units is simple, as they can be recursively defined as ratio of previously-defined units in a way that mimics natural language and is highly readable:

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
	
The preferred method of conversion is implicitly though the use of unit containers, however unit conversion can be accomplished using `units::convert` for arithmetic types:

	double val_in = convert<feet, inches>(1.0);	// val_in == 12.0
	
For type-safe conversion, see the next section.

Unit containers
---------------

In addition to providing unit tags to perform conversion, the library also provides container types. Containers are derived from the `unit_t` class, and have the form `[unitname]_t`, e.g. `meter_t`. Containers provide additional advanges over template tag conversions, without any increase in overhead, thanks to the c++ type system.

Unit containers are defined in terms of the units they represent, the underlying type of the container, and an optional non-linear scale (think decibels or richter scale). For example, `meter_t` would be defined: 

   using meter_t = unit_t<length::meter, double, linear_scale>

or simply 

    using meter_t = unit_t<length::meter>

since the underlying type and scale parameters default to `double` and `linear_scale` respectively. Defifining your own units is simple, and the standard SI prefixes, as well as `inverse`, `squared`, and `cubed` templates are provided to make it even easier.

Units of compatible types (e.g length units) can be implicitely converted/assigned to one another. Units (with the exception of dimensionless types) cannot be implicitely converted to/from built-in types. They can be constructed from built-in types, and operator() can be used to retrieve a built-in type value. That said, the user should prefer to operate within the unit type-space as much as is practical. 

Unit containers provide type safety and dimensional analysis for mathematical operations. for instance, the velocity of an object can be calculated:

    auto objectVelocity = meter_t(100.0) / second_t(2.0);

The resulting velocity type will be deduced to be `velocity::meters_per_second` with a value of 50.0. Additionally, if the return type if specified, the type system wll verify that the units are compatible. For example, the following will fail to compile:

    velocity::meters_per_second objectVelocity = square_meter_t(100.0) / second_t(2.0); // Error: cannot convert.`

Unit containers can also be used to perform implicit conversions:

	second_t a;
	minute_t b(1.0);
	
	a = b;	// a == 60.0

Unsupported arithmetic, or improper return types will result in compiler errors:

	meter_t a_m(1.0), b_m(2.0), c_m;
	foot_t	a_ft(1.0), b_ft(2.0), c_ft;
	
	c_m = a_m + b_m;							// OK. c == 3m
	c_ft = a_m + b_m;							// OK. resulting 3m is converted to ft.
	auto result = a_m * b_ft;					// OK. result is `meter_t` (left-most unit)
	
	c_m = a_m + 5.0;							// Error. can't add scalars to dimensioned units.
	c_m = a_m + scalar_t(5.0);					// Error. can't add scalars to dimensioned units.
	auto result = a_m + square_meter_t(1.0);	// Error. Incompatible units.
	
	auto result = a_m * square_meter_t(1.0);	// OK. units can always be multiplied. Result is `cubed<meter_t>`.
	auto result = a_m * scalar_t(1.0); 			// OK. units can always be multiplied. Result is `meter_t`.
	
Exponentials
------------

Many functions require units to be raised to some power. This can be accomplished using the units::pow function:

		square_meter_t m2 = units::pow<2>(meter_t(5.0));	// m2 == 25.0
		
The only constraint is that the exponential power (given in the template argument) must be known at compile time, so that the type system can deduce the output type.

Namespaces
----------

Unit tags and containers are split into separate namespaces to avoid conflicting unit names which represent different physical quantities. The currently defined namespaces are:

- units::length
- units::mass
- units::time
- units::angle (plane)
- units::current
- units::temperature
- units::substance (amount of, i.e. moles)
- units::luminous_intensity
- units::solid_angle
- units::frequency
- units::velocity
- units::angular_velocity
- units::acceleration
- units::force
- units::pressure
- units::charge
- units::energy
- units::power
- units::voltage
- units::capacitance
- units::impedance
- units::magnetic_flux
- units::magnetic_field_strength
- units::inductance
- units::luminous_flux
- units::illuminance
- units::radiation
- units::torque
- units::area
- units::volume
- units::density
- units::concentration
- units::constants (scalar and non-scalar physical constants like Avagadro's number)

Build Instructions
------------------

The library itself consists of a single header (include/units.h), and can be included into your project without being built. The unit tests and documentation can be built with CMake. A doxygen installation is required to generate the documentation, and a Tex install is needed if pdf documentation is desired.

To build the tests:

1. create a `build` directory inside or outside of the source tree.
2. from the build directory, call `cmake [relative path to build direcory]`
3. This will generate makefiles or a visual studio solution, depending on your platform. From here the project can be built in the usual platform-specific way.
