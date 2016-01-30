//--------------------------------------------------------------------------------------------------
// 
/// @PROJECT	units
/// @AUTHORS	Nic Holthaus
/// @DATE		2016/01/29
// 
//--------------------------------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
// and associated documentation files (the "Software"), to deal in the Software without 
// restriction, including without limitation the rights to use, copy, modify, merge, publish, 
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or 
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//--------------------------------------------------------------------------------------------------
//
// ATTRIBUTION:
// Parts of this work have been adapted from: 
// http://stackoverflow.com/questions/35069778/create-comparison-trait-for-template-classes-whose-parameters-are-in-a-different
//
//--------------------------------------------------------------------------------------------------
// 
// Copyright (c) 2016 Nic Holthaus
// 
//--------------------------------------------------------------------------------------------------

#ifndef units_h__
#define units_h__

//--------------------
//	INCLUDES
//--------------------

#include <ratio>
#include <type_traits>

//--------------------
//	UNITS NAMESPACE
//--------------------
namespace units
{
	//------------------------------
	//	RATIO TRAITS
	//------------------------------

	/**
	* @brief		trait which checks for the existence of a static numerator
	*/
	template<class T>
	struct has_num_impl
	{
		template<class U>
		static constexpr auto test(U*)->std::is_integral<decltype(U::num)>;
		template<typename>
		static constexpr std::false_type test(...);

		using type = decltype(test<T>(0));
	};

	template<class T>
	struct has_num : has_num_impl<T>::type {};

	/**
	* @brief		Trait which checks for the existence of a static denominator
	*/
	template<class T>
	struct has_den_impl
	{
		template<class U>
		static constexpr auto test(U*)->std::is_integral<decltype(U::den)>;
		template<typename>
		static constexpr std::false_type test(...);

		using type = decltype(test<T>(0));
	};

	template<class T>
	struct has_den : has_den_impl<T>::type {};

	/**
	* @brief		Trait that tests whether a type represents a ratio.
	*/
	template<class T>
	struct is_ratio : std::integral_constant<bool,
		has_num<T>::value &&
		has_den<T>::value>
	{};

	//------------------------------
	//	UNIT TRAITS
	//------------------------------

	struct _unit_t {};

	template<class T>
	struct is_unit : std::is_base_of<_unit_t, T> {};

	//------------------------------
	//	UNIT  CLASSES
	//------------------------------

	/**
	 * @brief		
	 * @details		
	 * @TODO		DOCUMENT THIS!
	 */
	template<class Conversion,
			 class Pi			= std::ratio<0>,	// Pi isn't a unit, but since it's irrational (i.e. can't be represented with std::ratio), we need to treat it as one.
			 class Meter		= std::ratio<0>,
			 class Kilogram		= std::ratio<0>,
			 class Second		= std::ratio<0>,
			 class Radian		= std::ratio<0>,	 
			 class Ampere		= std::ratio<0>,
			 class Kelvin		= std::ratio<0>,
			 class Candela		= std::ratio<0>,
			 class Mole			= std::ratio<0>,
			 class Steradian	= std::ratio<0>>
	struct unit : _unit_t
	{
		static_assert(is_ratio<Meter>::value,		"Template parameter `Meter` must be a `std::ratio` representing the exponent of meters the unit has");
		static_assert(is_ratio<Kilogram>::value,	"Template parameter `Kilogram` must be a `std::ratio` representing the exponent of kilograms the unit has");
		static_assert(is_ratio<Second>::value,		"Template parameter `Second` must be a `std::ratio` representing the exponent of seconds the unit has");
		static_assert(is_ratio<Ampere>::value,		"Template parameter `Ampere` must be a `std::ratio` representing the exponent of amperes the unit has");
		static_assert(is_ratio<Kelvin>::value,		"Template parameter `Kelvin` must be a `std::ratio` representing the exponent of kelvin the unit has");
		static_assert(is_ratio<Candela>::value,		"Template parameter `Candela` must be a `std::ratio` representing the exponent of candelas the unit has");
		static_assert(is_ratio<Mole>::value,		"Template parameter `Mole` must be a `std::ratio` representing the exponent of moles the unit has");
		static_assert(is_ratio<Radian>::value,		"Template parameter `Radian` must be a `std::ratio` representing the exponent of radians the unit has");
		static_assert(is_ratio<Steradian>::value,	"Template parameter `Steradian` must be a `std::ratio` representing the exponent of steradians the unit has");
		static_assert(is_ratio<Pi>::value,			"Template parameter `Pi` must be a `std::ratio` representing the exponent of pi the unit has");
		static_assert(is_ratio<Conversion>::value,	"Template parameter `Conversion` must be a `std::ratio` representing the conversion factor to base unit types");

		using meter_exponent_ratio = Meter;
		using kilogram_exponent_ratio = Kilogram;
		using second_exponent_ratio = Second;
		using ampere_exponent_ratio = Ampere;
		using kelvin_exponent_ratio = Kelvin;
		using candela_exponent_ratio = Candela;
		using mole_exponent_ratio = Mole;
		using radian_exponent_ratio = Radian;
		using steradian_exponent_ratio = Steradian;
		using pi_exponent_ratio = Pi;
		using conversion_ratio = Conversion;

		/**
		 * @brief		Unit conversion factor.
		 * @details		Ratio of units to base units. Example: for feet, returns 0.3048, as in "0.3048 feet per meter".
		 */
		 static constexpr double conversionFactor() { return double(conversion_ratio::num) / conversion_ratio::den; }
	};

	/**
	 * @brief		
	 * @details		
	 * @TODO		DOCUMENT THIS!
	 */
	template<class, class, class> struct derived_unit_impl;
	template<class Conversion, class BaseUnitConversion, class BaseUnitPiExponent, class... Exponents, class PiExponent>
	struct derived_unit_impl<Conversion, unit<BaseUnitConversion, BaseUnitPiExponent, Exponents...>, PiExponent>
	{
		static_assert(is_unit<unit<BaseUnitConversion, BaseUnitPiExponent, Exponents...>>::value, "Template parameter `BaseUnit` must be a unit type.");
		static_assert(is_ratio<Conversion>::value, "Template parameter `Conversion` must be a `std::ratio` representing the conversion factor to `BaseUnit`.");
		static_assert(is_ratio<PiExponent>::value, "Template parameter `PiExponent` must be a `std::ratio` representing the exponents of Pi the unit has.");

		using type = unit<std::ratio_multiply<Conversion, BaseUnitConversion>, std::ratio_add<PiExponent, BaseUnitPiExponent>, Exponents...>;
	};

	template<class Conversion, class BaseUnit, class PiExponent = std::ratio<0>>
	using derived_unit = typename derived_unit_impl<Conversion, BaseUnit, PiExponent>::type;

	/**
	 * @brief
	 * @details
	 * @TODO		DOCUMENT THIS!
	 */
	template<class, class> struct unit_multiply_impl;
	template<class... Exponents1, class... Exponents2>
	struct unit_multiply_impl<unit<Exponents1...>, unit<Exponents2...>> {
		using type = unit<std::ratio_add<Exponents1, Exponents2>...>;
	};

	template<class U1, class U2>
	using unit_multiply = typename unit_multiply_impl<U1, U2>::type;

	/**
	 * @brief		
	 * @details		
	 * @TODO		DOCUMENT THIS!
	 */
	template<class U> struct inverse_impl;
	template<class... Exponents>
	struct inverse_impl<unit<Exponents...>> {
		using type = unit<std::ratio_multiply<Exponents, std::ratio<-1>>...>;
	};

	template<class U> using inverse = typename inverse_impl<U>::type;

	/**
	 * @brief
	 * @details
	 * @TODO		DOCUMENT THIS!
	 */
	template<class U, class... Us> struct compound_impl;
	template<class U> struct compound_impl<U> { using type = U; };
	template<class U1, class U2, class...Us>
	struct compound_impl<U1, U2, Us...>
		: compound_impl<unit_multiply<U1, U2>, Us...> {};

	template<class U, class... Us>
	using compound_unit = typename compound_impl<U, Us...>::type;

	//------------------------------
	//	PREFIXES
	//------------------------------

// 	template<class U> struct prefix_impl;
// 	template<class... Exponents>
// 	struct inverse_impl<unit<Exponents...>> {
// 		using type = unit<std::ratio_multiply<Exponents, std::ratio<-1>>...>;
// 	};

// 	template<class T>
// 	struct centi 
	//------------------------------
	//	LENGTH UNITS
	//------------------------------

	namespace length
	{
		using meters = unit<std::ratio<1>, std::ratio<0>, std::ratio<1>>;
		using feet = derived_unit<std::ratio<381, 1250>, meters>;
		using inches = derived_unit<std::ratio<1, 12>, feet>;
		using miles = derived_unit<std::ratio<5280>, feet>;
		using nauticalMiles = derived_unit<std::ratio<1852>, meters>;
		using astronicalUnits = derived_unit<std::ratio<149597870700>, meters>;
		using lightyears = derived_unit<std::ratio<9460730472580800>, meters>;
		using parsecs = derived_unit<std::ratio<648000>, astronicalUnits, std::ratio<-1>>;

		using meter = meters;
		using foot = feet;
		using inch = inches;
		using mile = miles;
		using nauticalMile = nauticalMiles;
		using astronicalUnit = astronicalUnits;
		using lightyear = lightyears;
		using parsec = parsecs;

		using m = meters;
		using ft = feet;
		using inc = inches;
		using mi = miles;
		using nmi = nauticalMiles;
		using au = astronicalUnits;
		using ly = lightyears;
		using pc = parsecs;
	}
	
	//------------------------------
	//	TIME UNITS
	//------------------------------

	namespace time
	{
		using s
	}

	//------------------------------
	//	CONVERSION FUNCTION
	//------------------------------



};	// end namespace units

#endif // units_h__

