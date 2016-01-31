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
#include <cstdint>

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

		using meter_exponent_ratio = Meter;
		using kilogram_exponent_ratio = Kilogram;
		using second_exponent_ratio = Second;
		using ampere_exponent_ratio = Ampere;
		using kelvin_exponent_ratio = Kelvin;
		using candela_exponent_ratio = Candela;
		using mole_exponent_ratio = Mole;
		using radian_exponent_ratio = Radian;
	};

	//------------------------------
	//	UNIT CATEGORIES
	//------------------------------

	namespace category
	{
		// SI BASE UNIT TYPES	--------------------	METERS			KILOGRAMS		SECONDS			RADIANS			AMPERES			KELVIN			MOLE			CANDELA			
		using length_unit					= base_unit<std::ratio<1>>;
		using mass_unit						= base_unit<std::ratio<0>,	std::ratio<1>>;
		using time_unit						= base_unit<std::ratio<0>,	std::ratio<0>,	std::ratio<1>>;
		using angle_unit					= base_unit<std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>;
		using current_unit					= base_unit<std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>;
		using temperature_unit				= base_unit<std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>;
		using substance_unit				= base_unit<std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>;
		using luminous_intensity_unit		= base_unit<std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>;

		// SI DERIVED UNIT TYPES	---------------		METERS			KILOGRAMS		SECONDS			RADIANS			AMPERES			KELVIN			MOLE			CANDELA			
		using solid_angle_unit				= base_unit<std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<2>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>>;
		using frequency_unit				= base_unit<std::ratio<0>,	std::ratio<0>,	std::ratio<-1>>;
		using velocity_unit					= base_unit<std::ratio<1>,	std::ratio<0>,	std::ratio<-1>>;
		using acceleration_unit				= base_unit<std::ratio<1>,	std::ratio<0>,	std::ratio<-2>>;
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
		using luminous_flux_unit			= base_unit<std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<2>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>;
		using illuminance_unit				= base_unit<std::ratio<-2>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<0>,	std::ratio<1>>;
		using radioactivity_unit			= base_unit<std::ratio<0>,	std::ratio<0>,	std::ratio<-1>>;

		// OTHER UNIT TYPES
		using torque_units					= base_unit<std::ratio<1>,	std::ratio<1>>;
		using area_unit						= base_unit<std::ratio<2>>;
		using volume_unit					= base_unit<std::ratio<3>>;
		using density_unit					= base_unit<std::ratio<-3>,	std::ratio<1>>;
		using concentration_unit			= base_unit<std::ratio<-3>>;
	}

	//------------------------------
	//	UNIT CLASSES
	//------------------------------

	template <class, class, class, class> struct unit;
	template<class Conversion, class... Exponents, class PiExponent, class Translation>
	struct unit<Conversion, base_unit<Exponents...>, PiExponent, Translation> : _unit_t
	{
		static_assert(is_ratio<Conversion>::value, "Template parameter `Conversion` must be a `std::ratio` representing the conversion factor to `BaseUnit`.");
		static_assert(is_ratio<PiExponent>::value, "Template parameter `PiExponent` must be a `std::ratio` representing the exponents of Pi the unit has.");
		static_assert(is_ratio<Translation>::value, "Template parameter `Translation` must be a `std::ratio` representing an additive translation required by the unit conversion.");

		typedef typename base_unit<Exponents...> base_unit_type;
		typedef typename Conversion conversion_ratio;
		typedef typename Translation translation_ratio;
		typedef typename PiExponent pi_exponent_ratio;
	};

	template<class Conversion, class BaseUnit, class PiExponent = std::ratio<0>, class Translation = std::ratio<0>>
	struct unit : _unit_t
	{
		static_assert(is_unit<BaseUnit>::value, "Template parameter `BaseUnit` must be a `unit` type.");
		static_assert(is_ratio<Conversion>::value, "Template parameter `Conversion` must be a `std::ratio` representing the conversion factor to `BaseUnit`.");
		static_assert(is_ratio<PiExponent>::value, "Template parameter `PiExponent` must be a `std::ratio` representing the exponents of Pi the unit has.");

		typedef typename unit_traits<BaseUnit>::base_unit_type base_unit_type;
		typedef typename std::ratio_multiply<typename BaseUnit::conversion_ratio, Conversion> conversion_ratio;
		typedef typename std::ratio_add<typename BaseUnit::pi_exponent_ratio, PiExponent> pi_exponent_ratio;
		typedef typename std::ratio_add<std::ratio_multiply<typename BaseUnit::conversion_ratio, Translation>, typename BaseUnit::translation_ratio> translation_ratio;
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
	template<class Conversion, class BaseUnit, class PiExponent, class Translation>
	struct base_unit_of_impl<unit<Conversion, BaseUnit, PiExponent, Translation>> : base_unit_of_impl<BaseUnit> {};
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
	template<class Unit1, class Unit2>
	struct unit_multiply_impl
	{
		using type = unit<std::ratio_multiply<typename Unit1::conversion_ratio, typename Unit2::conversion_ratio>, 
			base_unit_multiply <base_unit_of<typename Unit1::base_unit_type>, base_unit_of<typename Unit2::base_unit_type>>, 
			std::ratio_add<typename Unit1::pi_exponent_ratio, typename Unit1::pi_exponent_ratio>, 
			std::ratio<0>>;
	};

	template<class U1, class U2>
	using unit_multiply = typename unit_multiply_impl<U1, U2>::type;

	/**
	* @brief
	* @details
	* @TODO		DOCUMENT THIS!
	*/
	template<class Unit1, class Unit2>
	struct unit_divide_impl
	{
		using type = unit<std::ratio_divide<typename Unit1::conversion_ratio, typename Unit2::conversion_ratio>, 
			base_unit_divide<base_unit_of<typename Unit1::base_unit_type>, base_unit_of<typename Unit2::base_unit_type>>, 
			std::ratio_subtract<typename Unit1::pi_exponent_ratio, typename Unit1::pi_exponent_ratio>, 
			std::ratio<0>>;
	};

	template<class U1, class U2>
	using unit_divide = typename unit_divide_impl<U1, U2>::type;

	/**
	 * @brief		
	 * @details		
	 * @TODO		DOCUMENT THIS!
	 */
	template<class Unit>
	struct inverse_impl
	{
		using type = unit<std::ratio<Unit::conversion_ratio::den, Unit::conversion_ratio::num>, 
			inverse_base<base_unit_of<typename Unit::base_unit_type>>, 
			std::ratio_multiply<typename Unit::pi_exponent_ratio, std::ratio<-1>>,
			std::ratio<0>>;	// inverses are rates or change, the translation factor goes away.
	};

	template<class U> using inverse = typename inverse_impl<U>::type;

	/**
	 * @brief
	 * @details
	 * @TODO		DOCUMENT THIS!
	 */
	template<class Unit>
	struct squared_impl
	{	
		static_assert(is_unit<Unit>::value, "Template parameter `Unit` must be a `unit` type.");
		using Conversion = typename Unit::conversion_ratio;
		using type = unit <std::ratio_multiply<Conversion, Conversion>,
			squared_base<base_unit_of<typename Unit::base_unit_type>>,
			std::ratio_multiply<typename Unit::pi_exponent_ratio, std::ratio<2>>,
			std::ratio<0>>;
	};

	template<class U>
	using squared = typename squared_impl<U>::type;

	/**
	* @brief
	* @details
	* @TODO		DOCUMENT THIS!
	*/
	template<class Unit>
	struct cubed_impl
	{
		static_assert(is_unit<Unit>::value, "Template parameter `Unit` must be a `unit` type.");
		using Conversion = typename Unit::conversion_ratio;
		using type = unit<std::ratio_multiply<Conversion, std::ratio_multiply<Conversion, Conversion>>,
			cubed_base<base_unit_of<typename Unit::base_unit_type>>, 
			std::ratio_multiply<typename Unit::pi_exponent_ratio, std::ratio<3>>,
			std::ratio<0>>;
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
		using nanometers = nano<meters>;
		using micrometers = micro<meters>;
		using millimeters = milli<meters>;
		using centimeters = centi<meters>;
		using kilometers = kilo<meters>;
		using feet = unit<std::ratio<381, 1250>, meters>;
		using mils = unit<std::ratio<1000>, feet>;
		using inches = unit<std::ratio<1, 12>, feet>;
		using miles = unit<std::ratio<5280>, feet>;
		using nauticalMiles = unit<std::ratio<1852>, meters>;
		using astronicalUnits = unit<std::ratio<149597870700>, meters>;
		using lightyears = unit<std::ratio<9460730472580800>, meters>;
		using parsecs = unit<std::ratio<648000>, astronicalUnits, std::ratio<-1>>;

		using meter = meters;
		using nanometer = nanometers;
		using micrometer = micrometers;
		using millimeter = millimeters;
		using centimeter = centimeters;
		using kilometer = kilometers;
		using foot = feet;
		using inch = inches;
		using mile = miles;
		using nauticalMile = nauticalMiles;
		using astronicalUnit = astronicalUnits;
		using lightyear = lightyears;
		using parsec = parsecs;

		using m = meters;
		using nm = nanometers;
		using um = micrometers;
		using mm = millimeters;
		using cm = centimeters;
		using km = kilometers;
		using ft = feet;
		using inc = inches;
		using mi = miles;
		using nmi = nauticalMiles;
		using au = astronicalUnits;
		using ly = lightyears;
		using pc = parsecs;
	}
	
	//------------------------------
	//	MASS UNITS
	//------------------------------

	namespace mass
	{
		using kilograms = unit<std::ratio<1>, category::mass_unit>;
		using grams = unit<std::ratio<1, 1000>, kilograms>;
		using micrograms = micro<grams>;
		using milligrams = milli<grams>;		
		using metric_tons = unit<std::ratio<1000>, kilograms>;
		using pounds = unit<std::ratio<45359237, 100000000>, kilograms>;
		using imperial_tons = unit<std::ratio<2240>, pounds>;
		using us_tons = unit<std::ratio<2000>, pounds>;
		using stone = unit<std::ratio<14>, pounds>;
		using ounces = unit<std::ratio<1, 16>, pounds>;
		using carats = unit<std::ratio<200>, milligrams>;
		using slugs = unit<std::ratio<14593903,1000000>, kilograms>;

		using gram = grams;
		using microgram = micrograms;
		using milligram = milligrams;
		using kilogram = kilograms;
		using metric_ton = metric_tons;
		using pound = pounds;
		using imperial_ton = imperial_tons;
		using us_ton = us_tons;
		using ounce = ounces;
		using carat = carats;
		using slug = slugs;

		using g = grams;
		using ug = micrograms;
		using mg = milligrams;
		using kg = kilograms;
		using mt = metric_tons;
		using t = us_tons;
		using Ib = pounds;
		using Ibs = pounds;
		using st = stone;
		using oz = ounces;
		using ct = carats;

	}

	//------------------------------
	//	TIME UNITS
	//------------------------------

	namespace time
	{
		using seconds = unit<std::ratio<1>, category::time_unit>;
		using nanoseconds = nano<seconds>;
		using microseconds = micro<seconds>;
		using millseconds = milli<seconds>;
		using minutes = unit<std::ratio<60>, seconds>;
		using hours = unit<std::ratio<60>, minutes>;
		using days = unit<std::ratio<24>, hours>;
		using weeks = unit<std::ratio<7>, days>;
		using years = unit<std::ratio<365>, days>;

		using second = seconds;
		using nanosecond = nanoseconds;
		using microsecond = microseconds;
		using millsecond = millseconds;
		using minute = minutes;
		using hour = hours;
		using day = days;
		using week = weeks;
		using year = years;
		
		using s = seconds;
		using ns = nanoseconds;
		using us = microseconds;
		using ms = millseconds;
		using m = minutes;
		using hr = hours;
		using d = days;
		using wk = weeks;
		using yr = years;
	}

	//------------------------------
	//	ANGLE UNITS
	//------------------------------

	namespace angle
	{
		using radians = unit<std::ratio<1>, category::angle_unit>;
		using milliradians = milli<radians>;
		using degrees = unit<std::ratio<1,180>, radians, std::ratio<1>>;
		using minutes = unit<std::ratio<1, 60>, degrees>;
		using seconds = unit<std::ratio<1, 60>, minutes>;
		using turns = unit<std::ratio<2>, radians, std::ratio<1>>;
		using mils = unit<std::ratio<1, 6400>, radians>;	// 1/6400 of a circle
		using gradians = unit<std::ratio<1, 400>, turns>;

		using radian = radians;
		using milliradian = milliradians;
		using degree = degrees;
		using minute = minutes;
		using second = seconds;
		using turn = turns;
		using mil = mils;
		using gradian = gradians;

		using rad = radians;
		using mrad = milliradians;
		using deg = degrees;
		using min = minutes;
		using sec = seconds;
		using tr = turn;
		using gon = gradians;
		using grad = gradians;
	}

	//------------------------------
	//	UNITS OF CURRENT
	//------------------------------
	namespace current
	{
		using amperes = unit<std::ratio<1>, category::current_unit>;
		using milliamps = milli<amperes>;
		using microamps = micro<amperes>;
		using nanoamps = nano<amperes>;

		using amps = amperes;
		using amp = amperes;
		
		using A = amperes;
		using mA = milliamps;
		using uA = microamps;
		using nA = nanoamps;
	}

	//------------------------------
	//	UNITS OF TEMPERATURE
	//------------------------------

	namespace temperature
	{
		// NOTE: temperature units have special conversion overloads, since they
		// require translations and aren't a reversible transform.
 		using kelvin = unit<std::ratio<1>, category::temperature_unit>;
 		using celsius = unit<std::ratio<1>, kelvin, std::ratio<0>, std::ratio<27315,100>>;
		using fahrenheit = unit<std::ratio<5, 9>, celsius, std::ratio<0>, std::ratio<-160, 9>>;
		using reaumur = unit<std::ratio<10, 8>, celsius>;
		using rankine = unit<std::ratio<5, 9>, kelvin>;

		using centigrade = celsius;

		using K = kelvin;
		using F = fahrenheit;
		using C = celsius;
		using Ra = rankine;
		using Re = reaumur;
	}

	//------------------------------
	//	UNITS OF AMOUNT OF SUBSTANCE
	//------------------------------

	namespace substance
	{
		using moles = unit<std::ratio<1>, category::substance_unit>;

		using mole = moles;

		using mol = mole;
	}

	//------------------------------
	//	UNITS OF LUMINOUS INTENSITY
	//------------------------------

	namespace luminous_intensity
	{
		using candelas = unit<std::ratio<1>, category::luminous_intensity_unit>;
		using millicandelas = milli<candelas>;

		using candela = candelas;
		using millicandela = millicandelas;

		using cd = candela;
		using mcd = millicandela;
	}

	//------------------------------
	//	UNITS OF SOLID ANGLE
	//------------------------------

	namespace solid_angle
	{
		using steradians = unit<std::ratio<1>, category::solid_angle_unit>;
		using degrees_squared = squared<angle::degrees>;
		using spats = unit<std::ratio<4>, steradians, std::ratio<1>>;

		using steradian = steradians;
		using degree_squared = degrees_squared;
		using spat = spats;

		using sr = steradians;
		using sq_deg = degrees_squared;
		using sp = spat;
	}

	//------------------------------
	//	FREQUENCY UNITS
	//------------------------------

	namespace frequency
	{
		using hertz = unit<std::ratio<1>, category::frequency_unit>;
		using kilohertz = kilo<hertz>;
		using megahertz = mega<hertz>;
		using gigahertz = giga<hertz>;

		using Hz = hertz;
		using kHz = kilohertz;
		using MHz = megahertz;
		using GHz = gigahertz;
	}

	//------------------------------
	//	VELOCITY UNITS
	//------------------------------

	namespace velocity
	{
		using meters_per_second = compound_unit<length::meters, inverse<time::second>>;
		using feet_per_second = compound_unit<length::feet, inverse<time::seconds>>;
		using miles_per_hour = compound_unit<length::miles, inverse<time::hour>>;
		using kilometers_per_hour = compound_unit<length::kilometers, inverse<time::hour>>;
		using knots = compound_unit<length::nauticalMiles, inverse<time::hour>>;
		
		using knot = knots;

		using mps = meters_per_second;
		using mph = miles_per_hour;
		using fps = feet_per_second;
		using kmph = kilometers_per_hour;
	}

	//------------------------------
	//	UNITS OF ACCELERATION
	//------------------------------

	namespace acceleration
	{
		using meters_per_second_squared = compound_unit<length::meters, inverse<squared<time::seconds>>>;
		using feet_per_second_squared = compound_unit<length::feet, inverse<squared<time::seconds>>>;
		using standard_gravity = unit<std::ratio<980665, 100000>, meters_per_second_squared>;
	}

	//------------------------------
	//	UNITS OF FORCE
	//------------------------------

	namespace force
	{
		using newtons = unit<std::ratio<1>, category::force_unit>;
		using pounds = compound_unit<mass::slug, length::foot, inverse<squared<time::seconds>>>;
		using dynes = unit<std::ratio<1, 100000>, newtons>;
		using kiloponds = compound_unit<acceleration::standard_gravity, mass::kilograms>;
		using poundals = compound_unit<mass::pound, length::foot, inverse<squared<time::seconds>>>;

		using newton = newtons;
		using pound = pounds;
		using dyne = dynes;
		using kilopond = kiloponds;
		using poundal = poundals;

		using N = newtons;
		using lbf = pounds;
		using dyn = dynes;
		using kp = kiloponds;
		using pdl = poundals;
	}





	//------------------------------
	//	AREA UNITS
	//------------------------------

	namespace area
	{
		using square_meters = squared<length::meters>;
		using square_feet = squared<length::feet>;
		using square_inches = squared<length::inch>;
		using square_miles = squared<length::miles>;
		using square_kilometers = squared<length::kilometers>;
		using hectares = unit<std::ratio<10000>, square_meters>;
		using acres = unit<std::ratio<43560>, square_feet>;

		using square_meter = square_meters;
		using square_foot = square_feet;
		using square_inch = square_inches;
		using square_mile = square_miles;
		using square_kilometer = square_kilometers;
		using hectare = hectares;
		using acre = acres;

		using ha = hectares;
	}

	//------------------------------
	//	CONVERSION TRAITS
	//------------------------------

	template<class U1, class U2>
	struct are_convertible_units : std::is_same<typename base_unit_of<typename unit_traits<U1>::base_unit_type>, 
		typename base_unit_of<typename unit_traits<U2>::base_unit_type>> {};

	//------------------------------
	//	CONVERSION FUNCTION
	//------------------------------

	/// convert dispatch for units which are both the same
	template<class, class, typename T>
	static inline T _convert(const T& value, std::true_type, std::false_type, std::false_type)
	{
		return value;
	}

	/// convert dispatch for units which are both the same
	template<class, class, typename T>
	static inline T _convert(const T& value, std::true_type, std::false_type, std::true_type)
	{
		return value;
	}

	/// convert dispatch for units which are both the same
	template<class, class, typename T>
	static inline T _convert(const T& value, std::true_type, std::true_type, std::false_type)
	{
		return value;
	}

	/// convert dispatch for units which are both the same
	template<class, class, typename T>
	static inline T _convert(const T& value, std::true_type, std::true_type, std::true_type)
	{
		return value;
	}

	/// convert dispatch for units of different types w/ no translation and no PI
	template<class UnitFrom, class UnitTo, typename T>
	static inline T _convert(const T& value, std::false_type, std::false_type, std::false_type)
	{
		using Ratio = std::ratio_divide<UnitFrom::conversion_ratio, UnitTo::conversion_ratio>;
		return (double(Ratio::num) * value / Ratio::den);
	}

	/// convert dispatch for units of different types w/ no translation, but has PI
	template<class UnitFrom, class UnitTo, typename T>
	static inline T _convert(const T& value, std::false_type, std::true_type, std::false_type)
	{
		using Ratio = std::ratio_divide<UnitFrom::conversion_ratio, UnitTo::conversion_ratio>;
		using PiRatio = std::ratio_subtract<UnitFrom::pi_exponent_ratio, UnitTo::pi_exponent_ratio>;
		return ((double(Ratio::num) * value / Ratio::den) * std::pow(PI, (double(PiRatio::num) / PiRatio::den)));
	}

	/// convert dispatch for units of different types with a translation, but no PI
	template<class UnitFrom, class UnitTo, typename T>
	static inline T _convert(const T& value, std::false_type, std::false_type, std::true_type)
	{
		using Ratio = std::ratio_divide<UnitFrom::conversion_ratio, UnitTo::conversion_ratio>;
		using Translation = std::ratio_divide<std::ratio_subtract<UnitFrom::translation_ratio, UnitTo::translation_ratio>, UnitTo::conversion_ratio>;
		return ((double(Ratio::num) * value / Ratio::den) + (double(Translation::num) / Translation::den));
	}

	/// convert dispatch for units of different types with a translation AND PI
	template<class UnitFrom, class UnitTo, typename T>
	static inline T _convert(const T& value, std::false_type, std::true_type, std::true_type)
	{
		using Ratio = std::ratio_divide<UnitFrom::conversion_ratio, UnitTo::conversion_ratio>;
		using Translation = std::ratio_divide<std::ratio_subtract<UnitFrom::translation_ratio, UnitTo::translation_ratio>, UnitTo::conversion_ratio>;
		using PiRatio = std::ratio_subtract<UnitFrom::pi_exponent_ratio, UnitTo::pi_exponent_ratio>;
		return ((double(Ratio::num) * value / Ratio::den) * std::pow(PI, (double(PiRatio::num) / PiRatio::den)) + (double(Translation::num) / Translation::den));
	}

	/**
	 * @brief		
	 * @details		
	 * @TODO		DOCUMENT THIS!
	 */
	template<class UnitFrom, class UnitTo, typename T = double>
	static inline T convert(const T& value)
	{
		static_assert(is_unit<UnitFrom>::value, "Template parameter `UnitFrom` must be a `unit` type.");
		static_assert(is_unit<UnitTo>::value, "Template parameter `UnitTo` must be a `unit` type.");
		static_assert(are_convertible_units<UnitFrom, UnitTo>::value, "`UnitFrom` is not convertible to `UnitTo`.");

		using isSame = typename std::is_same<typename std::decay<UnitFrom>::type, typename std::decay<UnitTo>::type>::type;
		using piRequired = std::integral_constant<bool, !(std::is_same<std::ratio<0>, UnitFrom::pi_exponent_ratio>::value &&
			std::is_same<std::ratio<0>, UnitTo::pi_exponent_ratio>::value)>;
		using translationRequired = std::integral_constant<bool, !(std::is_same<std::ratio<0>, UnitFrom::translation_ratio>::value &&
			std::is_same<std::ratio<0>, UnitTo::translation_ratio>::value)>;

		return _convert<UnitFrom, UnitTo, T>(value, isSame{}, piRequired{}, translationRequired{});
	}	

};	// end namespace units

#endif // units_h__

