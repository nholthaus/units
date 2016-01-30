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
	//	PI
	//------------------------------

	static const double PI = 3.14159265358979323846264338327950288419716939937510;

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

	template<class T, typename = void>
	struct unit_traits
	{
		typedef void base_unit_type;
	};

	template<class T>
	struct unit_traits<T, std::void_t<typename T::base_unit_type>>
	{
		typedef typename T::base_unit_type base_unit_type;
	};

	struct _base_unit_t {};

	template<class T>
	struct is_base_unit : std::is_base_of<_base_unit_t, T> {};

	struct _unit_t {};

	template<class T>
	struct is_unit : std::is_base_of<_unit_t, T>::type {};

	template<class U1, class U2>
	struct are_convertible_units : std::is_same<typename unit_traits<U1>::base_unit_type, typename unit_traits<U2>::base_unit_type> {};

	//------------------------------
	//	BASE UNIT CLASSES
	//------------------------------

	/**
	 * @brief		
	 * @details		
	 * @TODO		DOCUMENT THIS!
	 */
	template<class Meter		= std::ratio<0>,
			 class Kilogram		= std::ratio<0>,
			 class Second		= std::ratio<0>,
			 class Radian		= std::ratio<0>,	 
			 class Ampere		= std::ratio<0>,
			 class Kelvin		= std::ratio<0>,		
			 class Mole			= std::ratio<0>,
			 class Candela		= std::ratio<0>,
			 class Steradian	= std::ratio<0>>
	struct base_unit : _base_unit_t
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

		using meter_exponent_ratio = Meter;
		using kilogram_exponent_ratio = Kilogram;
		using second_exponent_ratio = Second;
		using ampere_exponent_ratio = Ampere;
		using kelvin_exponent_ratio = Kelvin;
		using candela_exponent_ratio = Candela;
		using mole_exponent_ratio = Mole;
		using radian_exponent_ratio = Radian;
		using steradian_exponent_ratio = Steradian;
	};

	//------------------------------
	//	UNIT CATEGORIES
	//------------------------------

	namespace category
	{
		// SI BASE UNIT TYPES	--------------------	METERS			KILOGRAMS		SECONDS			RADIANS			AMPERES			KELVIN			MOLE			CANDELA			STERADIAN
		using length_unit					= base_unit<std::ratio<1>>;
		using mass_unit						= base_unit<std::ratio<0>,	std::ratio<1>>;
		using time_unit						= base_unit<std::ratio<0>,	std::ratio<0>,	std::ratio<1>>;
		using angle_unit					= base_unit<std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>;
		using current_unit					= base_unit<std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>;
		using temperature_unit				= base_unit<std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>;
		using concentration_unit			= base_unit<std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>;
		using luminous_intensity_unit		= base_unit<std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>;
		using solid_angle_unit				= base_unit<std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>;

		// SI DERIVED UNIT TYPES	---------------		METERS			KILOGRAMS		SECONDS			RADIANS			AMPERES			KELVIN			MOLE			CANDELA			STERADIAN
		using frequency_unit				= base_unit<std::ratio<0>,	std::ratio<0>,	std::ratio<-1>>;
		using force_unit					= base_unit<std::ratio<1>,	std::ratio<1>,	std::ratio<-2>>;
		using pressure_unit					= base_unit<std::ratio<-1>,	std::ratio<1>,	std::ratio<-2>>;
		using energy_unit					= base_unit<std::ratio<2>,	std::ratio<1>,	std::ratio<-2>>;
		using power_unit					= base_unit<std::ratio<2>,	std::ratio<1>,	std::ratio<-3>>;
		using charge_unit					= base_unit<std::ratio<0>,	std::ratio<0>,	std::ratio<1>,	std::ratio<0>,	std::ratio<1>>;
		using voltage_unit					= base_unit<std::ratio<2>,	std::ratio<1>,	std::ratio<-3>,	std::ratio<0>,	std::ratio<-1>>;
		using capacitance_unit				= base_unit<std::ratio<-2>,	std::ratio<-1>,	std::ratio<4>,	std::ratio<0>,	std::ratio<2>>;
		using impedance_unit				= base_unit<std::ratio<2>,	std::ratio<1>,	std::ratio<-3>,	std::ratio<0>,	std::ratio<-2>>;
		using conductance_unit				= base_unit<std::ratio<-2>,	std::ratio<-1>,	std::ratio<3>,	std::ratio<0>,	std::ratio<2>>;
		using magnetic_flux_unit			= base_unit<std::ratio<2>,	std::ratio<1>,	std::ratio<-2>,	std::ratio<0>,	std::ratio<-1>>;
		using magnetic_filed_strength_unit	= base_unit<std::ratio<0>,	std::ratio<1>,	std::ratio<-2>,	std::ratio<0>,	std::ratio<-1>>;
		using inductance_unit				= base_unit<std::ratio<2>,	std::ratio<1>,	std::ratio<-2>,	std::ratio<0>,	std::ratio<-2>>;
		using luminous_flux_unit			= base_unit<std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>,	std::ratio<1>>;
		using illuminance_unit				= base_unit<std::ratio<-2>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>;
		using radioactivity_unit			= base_unit<std::ratio<0>,	std::ratio<0>,	std::ratio<-1>>;

		// OTHER UNIT TYPES
		using velocity_unit					= base_unit<std::ratio<1>,	std::ratio<0>,	std::ratio<-1>>;
		using acceleration_unit				= base_unit<std::ratio<1>,	std::ratio<0>,	std::ratio<-2>>;
		using area_unit						= base_unit<std::ratio<2>>;
		using volume_unit					= base_unit<std::ratio<3>>;
		using density_unit					= base_unit<std::ratio<3>,	std::ratio<1>>;
		using inverse_unit					= base_unit<std::ratio<-1>,	std::ratio<-1>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>;
	}

	//------------------------------
	//	UNIT CLASSES
	//------------------------------

	template <class, class, class> struct unit;
	template<class Conversion, class... Exponents, class PiExponent>
	struct unit<Conversion, base_unit<Exponents...>, PiExponent> : _unit_t
	{
		static_assert(is_ratio<Conversion>::value, "Template parameter `Conversion` must be a `std::ratio` representing the conversion factor to `BaseUnit`.");
		static_assert(is_ratio<PiExponent>::value, "Template parameter `PiExponent` must be a `std::ratio` representing the exponents of Pi the unit has.");

		typedef typename base_unit<Exponents...> base_unit_type;
		typedef typename Conversion conversion_ratio;
		typedef typename PiExponent pi_exponent_ratio;

		/**
		 * @brief		Unit conversion factor.
		 * @details		Ratio of units to base units. Example: for feet, returns 0.3048, as in "0.3048 feet per meter".
		 */
		static constexpr double conversionFactor() 
		{ 
			return (double(conversion_ratio::num) / conversion_ratio::den) * std::pow(PI, (double(pi_exponent_ratio::num) / pi_exponent_ratio::den));
		}
	};

	template<class Conversion, class BaseUnit, class PiExponent = std::ratio<0>>
	struct unit : _unit_t
	{
		static_assert(is_unit<BaseUnit>::value, "Template parameter `BaseUnit` must be a `unit` type.");
		static_assert(is_ratio<Conversion>::value, "Template parameter `Conversion` must be a `std::ratio` representing the conversion factor to `BaseUnit`.");
		static_assert(is_ratio<PiExponent>::value, "Template parameter `PiExponent` must be a `std::ratio` representing the exponents of Pi the unit has.");

		typedef typename unit_traits<BaseUnit>::base_unit_type base_unit_type;
		typedef typename std::ratio_multiply<Conversion, typename BaseUnit::conversion_ratio> conversion_ratio;
		typedef typename std::ratio_add<PiExponent, typename BaseUnit::pi_exponent_ratio> pi_exponent_ratio;

		/**
		* @brief		Unit conversion factor.
		* @details		Ratio of units to base units. Example: for feet, returns 0.3048, as in "0.3048 feet per meter".
		*/
		static constexpr double conversionFactor()
		{
			return (double(conversion_ratio::num) / conversion_ratio::den) * std::pow(PI, (double(pi_exponent_ratio::num) / pi_exponent_ratio::den));
		}
	};

	//------------------------------
	//	BASE UNIT MANIPULATORS
	//------------------------------

	/**
	 * @brief		
	 * @details		
	 * @TODO		DOCUMENT THIS!
	 */
	template<class> struct base_unit_of_impl;
	template<class Conversion, class BaseUnit, class PiExponent>
	struct base_unit_of_impl<unit<Conversion, BaseUnit, PiExponent>> : base_unit_of_impl<BaseUnit> {};
	template<class... Exponents>
	struct base_unit_of_impl<base_unit<Exponents...>>
	{
		typedef base_unit<Exponents...> type;
	};

	template<class U>
	using base_unit_of = typename base_unit_of_impl<U>::type;

	/**
	* @brief
	* @details
	* @TODO		DOCUMENT THIS!
	*/
	template<class, class> struct base_unit_multiply_impl;
	template<class... Exponents1, class... Exponents2>
	struct base_unit_multiply_impl<base_unit<Exponents1...>, base_unit<Exponents2...>> {
		using type = base_unit<std::ratio_add<Exponents1, Exponents2>...>;
	};

	template<class U1, class U2>
	using base_unit_multiply = typename base_unit_multiply_impl<U1, U2>::type;

	/**
	* @brief
	* @details
	* @TODO		DOCUMENT THIS!
	*/
	template<class, class> struct base_unit_divide_impl;
	template<class... Exponents1, class... Exponents2>
	struct base_unit_divide_impl<base_unit<Exponents1...>, base_unit<Exponents2...>> {
		using type = base_unit<std::ratio_subtract<Exponents1, Exponents2>...>;
	};

	template<class U1, class U2>
	using base_unit_divide = typename base_unit_divide_impl<U1, U2>::type;

	/**
	* @brief
	* @details
	* @TODO		DOCUMENT THIS!
	*/
	template<class> struct inverse_base_impl;

	template<class... Exponents>
	struct inverse_base_impl<base_unit<Exponents...>> {
		using type = base_unit<std::ratio_multiply<Exponents, std::ratio<-1>>...>;
	};

	template<class U> using inverse_base = typename inverse_base_impl<U>::type;

	/**
	* @brief
	* @details
	* @TODO		DOCUMENT THIS!
	*/
	template<class U> struct squared_base_impl;
	template<class... Exponents>
	struct squared_base_impl<base_unit<Exponents...>> {
		using type = base_unit<std::ratio_multiply<Exponents, std::ratio<2>>...>;
	};

	template<class U> using squared_base = typename squared_base_impl<U>::type;

	/**
	* @brief
	* @details
	* @TODO		DOCUMENT THIS!
	*/
	template<class U> struct cubed_base_impl;
	template<class... Exponents>
	struct cubed_base_impl<base_unit<Exponents...>> {
		using type = base_unit<std::ratio_multiply<Exponents, std::ratio<3>>...>;
	};

	template<class U> using cubed_base = typename cubed_base_impl<U>::type;

	//------------------------------
	//	UNIT MANIPULATORS
	//------------------------------

	/**
	 * @brief
	 * @details
	 * @TODO		DOCUMENT THIS!
	 */
	template<class, class> struct unit_multiply_impl;
	template<class Conversion1, class BaseUnit1, class PiExponent1, class Conversion2, class BaseUnit2, class PiExponent2>
	struct unit_multiply_impl<unit<Conversion1, BaseUnit1, PiExponent1>, unit<Conversion2, BaseUnit2, PiExponent2>> {
		using type = unit<std::ratio_multiply<Conversion1, Conversion2>, base_unit_multiply<base_unit_of<BaseUnit1>, base_unit_of<BaseUnit2>>, std::ratio_add<PiExponent1, PiExponent2>>;
	};

	template<class U1, class U2>
	using unit_multiply = typename unit_multiply_impl<U1, U2>::type;

	/**
	* @brief
	* @details
	* @TODO		DOCUMENT THIS!
	*/
	template<class, class> struct unit_divide_impl;
	template<class Conversion1, class BaseUnit1, class PiExponent1, class Conversion2, class BaseUnit2, class PiExponent2>
	struct unit_divide_impl<unit<Conversion1, BaseUnit1, PiExponent1>, unit<Conversion2, BaseUnit2, PiExponent2>> {
		using type = unit<std::ratio_divide<Conversion1, Conversion2>, base_unit_divide<base_unit_of<BaseUnit1>, base_unit_of<BaseUnit2>>, std::ratio_subtract<PiExponent1, PiExponent2>>;
	};

	template<class U1, class U2>
	using unit_divide = typename unit_divide_impl<U1, U2>::type;

	/**
	 * @brief		
	 * @details		
	 * @TODO		DOCUMENT THIS!
	 */
	template<class U> struct inverse_impl;
	template <class Conversion, class BaseUnit, class PiExponent>
	struct inverse_impl<unit<Conversion, BaseUnit, PiExponent>> {
		using type = unit<std::ratio<Conversion::den, Conversion::num>, inverse_base<base_unit_of<BaseUnit>>, std::ratio_multiply<PiExponent, std::ratio<-1>>>;
	};

	template<class U> using inverse = typename inverse_impl<U>::type;

	/**
	 * @brief
	 * @details
	 * @TODO		DOCUMENT THIS!
	 */
	template<class> struct squared_impl;
	template<class Conversion, class BaseUnit, class PiExponent>
	struct squared_impl<unit<Conversion, BaseUnit, PiExponent>>
	{	
		using type = unit<std::ratio_multiply<Conversion, Conversion>, squared_base<base_unit_of<BaseUnit>>, std::ratio_multiply<PiExponent, std::ratio<2>>>;
	};

	template<class U>
	using squared = typename squared_impl<U>::type;

	/**
	* @brief
	* @details
	* @TODO		DOCUMENT THIS!
	*/
	template<class> struct cubed_impl;
	template<class Conversion, class BaseUnit, class PiExponent>
	struct cubed_impl<unit<Conversion, BaseUnit, PiExponent>>
	{
		using type = unit<std::ratio_multiply<Conversion, std::ratio_multiply<Conversion, Conversion>>, cubed_base<base_unit_of<BaseUnit>>, std::ratio_multiply<PiExponent, std::ratio<3>>>;
	};

	template<class U>
	using cubed = typename cubed_impl<U>::type;

	//------------------------------
	//	COMPOUND UNITS
	//------------------------------

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

	template<class Ratio, class Unit>
	struct prefix
	{
		static_assert(is_ratio<Ratio>::value, "Template parameter `Ratio` must be a `std::ratio`.");
		static_assert(is_unit<Unit>::value, "Template parameter `Unit` must be a `unit` type.");
		typedef typename unit<Ratio, Unit> type;
	};

	template<class U> using atto = typename prefix<std::atto, U>::type;
	template<class U> using femto = typename prefix<std::femto, U>::type;
	template<class U> using pico = typename prefix<std::pico, U>::type;
	template<class U> using nano = typename prefix<std::nano, U>::type;
	template<class U> using micro = typename prefix<std::micro, U>::type;
	template<class U> using milli = typename prefix<std::milli, U>::type;
	template<class U> using centi = typename prefix<std::centi, U>::type;
	template<class U> using deci = typename prefix<std::deci, U>::type;
	template<class U> using deca = typename prefix<std::deca, U>::type;
	template<class U> using hecto = typename prefix<std::hecto, U>::type;
	template<class U> using kilo = typename prefix<std::kilo, U>::type;
	template<class U> using mega = typename prefix<std::mega, U>::type;
	template<class U> using giga = typename prefix<std::giga, U>::type;
	template<class U> using tera = typename prefix<std::tera, U>::type;
	template<class U> using peta = typename prefix<std::peta, U>::type;
	template<class U> using exa = typename prefix<std::exa, U>::type;

	//------------------------------
	//	LENGTH UNITS
	//------------------------------

	namespace length
	{
		using meters = unit<std::ratio<1>, category::length_unit>;
		using millimeters = milli<meters>;
		using feet = unit<std::ratio<381, 1250>, meters>;
		using mils = unit<std::ratio<1000>, feet>;
		using inches = unit<std::ratio<1, 12>, feet>;
		using miles = unit<std::ratio<5280>, feet>;
		using nauticalMiles = unit<std::ratio<1852>, meters>;
		using astronicalUnits = unit<std::ratio<149597870700>, meters>;
		using lightyears = unit<std::ratio<9460730472580800>, meters>;
		using parsecs = unit<std::ratio<648000>, astronicalUnits, std::ratio<-1>>;

		using meter = meters;
		using millimeter = millimeters;
		using foot = feet;
		using inch = inches;
		using mile = miles;
		using nauticalMile = nauticalMiles;
		using astronicalUnit = astronicalUnits;
		using lightyear = lightyears;
		using parsec = parsecs;

		using m = meters;
		using mm = millimeters;
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
		using seconds = unit<std::ratio<1>, category::time_unit>;
		using minutes = unit<std::ratio<60>, seconds>;
		using hours = unit<std::ratio<60>, minutes>;
		using days = unit<std::ratio<24>, hours>;
		using weeks = unit<std::ratio<7>, days>;
		using years = unit<std::ratio<365>, days>;

		using second = seconds;
		using minute = minutes;
		using hour = hours;
		using day = days;
		using week = weeks;
		using year = years;
		
		using s = seconds;
		using m = minutes;
		using hr = hours;
		using d = days;
		using wk = weeks;
		using yr = years;
	}

	//------------------------------
	//	FREQUENCY UNITS
	//------------------------------

	namespace frequency
	{
		using hertz = unit<std::ratio<1>, category::frequency_unit>;

		using hz = hertz;
	}

	//------------------------------
	//	VELOCITY UNITS
	//------------------------------

	namespace velocity
	{
		using meters_per_second = compound_unit<length::meters, inverse<time::second>>;
		using meters_per_minute = compound_unit<length::meters, inverse<time::minute>>;
		using meters_per_hour = compound_unit<length::meters, inverse<time::hour>>;
		using meters_per_year = compound_unit<length::meters, inverse<time::year>>;
		using miles_per_hour = compound_unit<length::miles, inverse<time::hour>>;
		using knots = compound_unit<length::nauticalMiles, inverse<time::hour>>;
		
		using knot = knots;

		using mps = meters_per_second;
		using mph = miles_per_hour;
	}

	//------------------------------
	//	AREA UNITS
	//------------------------------

	namespace area
	{
		using square_meters = squared<length::meters>;
		using square_feet = squared<length::feet>;
		using hectares = unit<std::ratio<10000>, square_meters>;
		using acres = unit<std::ratio<43560>, square_feet>;

		using hectare = hectares;
		using acre = acres;

		using ha = hectares;
	}

	//------------------------------
	//	CONVERSION FUNCTION
	//------------------------------



};	// end namespace units

#endif // units_h__

