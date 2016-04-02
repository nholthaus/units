//--------------------------------------------------------------------------------------------------
// 
//	Units: A compile-time c++14 unit conversion library with no dependencies
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
// http://stackoverflow.com/questions/28253399/check-traits-for-all-variadic-template-arguments/28253503
// http://stackoverflow.com/questions/36321295/rational-approximation-of-square-root-of-stdratio-at-compile-time?noredirect=1#comment60266601_36321295
//
//--------------------------------------------------------------------------------------------------
// 
// Copyright (c) 2016 Nic Holthaus
// 
//--------------------------------------------------------------------------------------------------

#ifndef units_h__
#define units_h__

#if _MCS_VER < 1800
#	pragma warning(push)
#	pragma warning(disable : 4520)
#endif

//--------------------
//	INCLUDES
//--------------------

#include <ratio>
#include <type_traits>
#include <cstdint>
#include <cmath>
#include <iostream>

//--------------------
//	UNITS NAMESPACE
//--------------------

/**
 * @namespace units
 * @brief Unit Conversion Library namespace
 */
namespace units
{
	//----------------------------------
	//	DOXYGEN
	//----------------------------------

	/**
	 * @defgroup	UnitContainers Unit Containers
	 * @brief		Defines a series of classes which contain dimensioned values. Unit containers
	 *				store a value, and support various arithmetic operations.
	 */

	/**
	 * @defgroup	UnitTypes Unit Types
	 * @brief		Defines a series of classes which represent units. These types are tags used by
	 *				the conversion function, to create compound units, or to create `unit_t` types.
	 *				By themselves, they are not containers and have no stored value.
	 */

	/**
	 * @defgroup	UnitManipulators Unit Manipulators
	 * @brief		Defines a series of classes used to manipulate unit types, such as `inverse<>`, `squared<>`, and metric prefixes. Unit
	 *				manipulators can be chained together, e.g. `inverse<squared<pico<time::seconds>>>` to
	 *				represent picoseconds^-2.
	 */

	 /**
	 * @defgroup	UnitMath Unit Math
	 * @brief		Defines a collection of unit-enabled, strongly-typed versions of `<cmath>` functions.
	 * @details		Includes most c++11 extensions.
	 */

	/**
	 * @defgroup	Conversion Explicit Conversion
	 * @brief		Functions used to convert values of one logical type to another.
	 */

	/**
	 * @defgroup	TypeTraits Type Traits
	 * @brief		Defines a series of classes to obtain unit type information at compile-time.
	 */

	//------------------------------
	//	FORWARD DECLARATIONS
	//------------------------------

	/** @cond */	// DOXYGEN IGNORE
	namespace constants
	{
		static const double PI = 3.14159265358979323846264338327950288419716939937510;
	}
	/** @endcond */	// END DOXYGEN IGNORE

	//------------------------------
	//	RATIO TRAITS
	//------------------------------

	/**
	 * @ingroup TypeTraits
	 * @{
	 */

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/// has_num implementation.
		template<class T>
		struct has_num_impl
		{
			template<class U>
			static auto test(U*)->std::is_integral<decltype(U::num)>;
			template<typename>
			static std::false_type test(...);

			using type = decltype(test<T>(0));
		};
	}

	/**
	 * @brief		Trait which checks for the existence of a static numerator.
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `has_num<T>::value` to test
	 *				whether `class T` has a numerator static member.
	 */
	template<class T>
	struct has_num : detail::has_num_impl<T>::type {};

	namespace detail
	{
		/// has_den implementation.
		template<class T>
		struct has_den_impl
		{
			template<class U>
			static auto test(U*)->std::is_integral<decltype(U::den)>;
			template<typename>
			static std::false_type test(...);

			using type = decltype(test<T>(0));
		};
	}

	/**
	 * @brief		Trait which checks for the existence of a static denominator.
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `has_den<T>::value` to test
	 *				whether `class T` has a denominator static member.
	 */
	template<class T>
	struct has_den : detail::has_den_impl<T>::type {};

	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @brief		Trait that tests whether a type represents a std::ratio.
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_ratio<T>::value` to test
	 *				whether `class T` implements a std::ratio.
	 */
	template<class T>
	struct is_ratio : std::integral_constant<bool,
		has_num<T>::value &&
		has_den<T>::value>
	{};

	//------------------------------
	//	UNIT TRAITS
	//------------------------------

	/** @cond */	// DOXYGEN IGNORE
	/**
	 * @brief		void type.
	 * @details		Helper class for creating type traits.
	 */
	template<class ...>
	struct void_t { typedef void type; };

	template<bool... Args>
	struct all_true_impl;

	template<bool First, bool... Args>
	struct all_true_impl<First, Args...>
	{
		using type = typename std::integral_constant<bool, First && all_true_impl<Args...>::type::value>::type;
	};

	template<bool Last>
	struct all_true_impl<Last>
	{
		using type = typename std::integral_constant<bool, Last>::type;
	};

	template<bool... Args>
	using all_true = typename all_true_impl<Args...>::type;

	/**
	 * @brief		unit traits implementation for classes which are not units.
	 */
	template<class T, typename = void>
	struct unit_traits
	{
		typedef void base_unit_type;
		typedef void conversion_ratio;
		typedef void pi_exponent_ratio;
		typedef void translation_ratio;
	};
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @ingroup		TypeTraits
	 * @brief		Traits class defining the properties of units.
	 * @details
	 */
	template<class T>
	struct unit_traits
		<T, typename void_t<
		typename T::base_unit_type,
		typename T::conversion_ratio,
		typename T::pi_exponent_ratio,
		typename T::translation_ratio>::type>
	{
		typedef typename T::base_unit_type base_unit_type;											///< Unit type that the unit was derived from. May be a `base_unit` or another `unit`. Use the `base_unit_of` trait to find the SI base unit type. 
		typedef typename T::conversion_ratio conversion_ratio;										///< std::ratio representing the conversion factor to the `base_unit_type`.
		typedef typename T::pi_exponent_ratio pi_exponent_ratio;									///< std::ratio representing the exponent of pi to be used in the conversion.
		typedef typename T::translation_ratio translation_ratio;									///< std::ratio representing a datum translation to the base unit (i.e. degrees C to degrees F conversion).
	};

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		helper type to identify base units.
		 * @details		A non-templated base class for `base_unit` which enables RTTI testing.
		 */
		struct _base_unit_t {};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @ingroup		TypeTraits
	 * @brief		Traits which tests if a class is a `base_unit` type.
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_base_unit<T>::value` to test
	 *				whether `class T` implements a `base_unit`.
	 */
	template<class T>
	struct is_base_unit : std::is_base_of<detail::_base_unit_t, T> {};

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		helper type to identify units.
		 * @details		A non-templated base class for `unit` which enables RTTI testing.
		 */
		struct _unit {};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @ingroup		TypeTraits
	 * @brief		Traits which tests if a class is a `unit`
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_unit<T>::value` to test
	 *				whether `class T` implements a `unit`.
	 */
	template<class T>
	struct is_unit : std::is_base_of<detail::_unit, T>::type {};

	/** @} */ // end of TypeTraits

	//------------------------------
	//	BASE UNIT CLASS
	//------------------------------

	/**
	 * @brief		Class representing SI base unit types.
	 * @details		Base units are represented by a combination of `std::ratio` template parameters, each
	 *				describing the exponent of the type of unit they represent. Example: meters per second
	 *				would be described by a +1 exponent for meters, and a -1 exponent for seconds, thus:
	 *				`base_unit<std::ratio<1>, std::ratio<0>, std::ratio<-1>>`
	 * @tparam		Meter		`std::ratio` representing the exponent value for meters.
	 * @tparam		Kilogram	`std::ratio` representing the exponent value for kilograms.
	 * @tparam		Second		`std::ratio` representing the exponent value for seconds.
	 * @tparam		Radian		`std::ratio` representing the exponent value for radians. Although radians are not SI base units, they are included because radians are described by the SI as m * m^-1, which would make them indistinguishable from scalars.
	 * @tparam		Ampere		`std::ratio` representing the exponent value for amperes.
	 * @tparam		Kelvin		`std::ratio` representing the exponent value for Kelvin.
	 * @tparam		Mole		`std::ratio` representing the exponent value for moles.
	 * @tparam		Candela		`std::ratio` representing the exponent value for candelas.
	 * @ingroup		UnitTypes
	 * @sa			category	 for type aliases for SI base_unit types.
	 */
	template<class Meter = std::ratio<0>,
	class Kilogram = std::ratio<0>,
	class Second = std::ratio<0>,
	class Radian = std::ratio<0>,
	class Ampere = std::ratio<0>,
	class Kelvin = std::ratio<0>,
	class Mole = std::ratio<0>,
	class Candela = std::ratio < 0 >>
	struct base_unit : detail::_base_unit_t
	{
		static_assert(is_ratio<Meter>::value, "Template parameter `Meter` must be a `std::ratio` representing the exponent of meters the unit has");
		static_assert(is_ratio<Kilogram>::value, "Template parameter `Kilogram` must be a `std::ratio` representing the exponent of kilograms the unit has");
		static_assert(is_ratio<Second>::value, "Template parameter `Second` must be a `std::ratio` representing the exponent of seconds the unit has");
		static_assert(is_ratio<Ampere>::value, "Template parameter `Ampere` must be a `std::ratio` representing the exponent of amperes the unit has");
		static_assert(is_ratio<Kelvin>::value, "Template parameter `Kelvin` must be a `std::ratio` representing the exponent of kelvin the unit has");
		static_assert(is_ratio<Candela>::value, "Template parameter `Candela` must be a `std::ratio` representing the exponent of candelas the unit has");
		static_assert(is_ratio<Mole>::value, "Template parameter `Mole` must be a `std::ratio` representing the exponent of moles the unit has");
		static_assert(is_ratio<Radian>::value, "Template parameter `Radian` must be a `std::ratio` representing the exponent of radians the unit has");
	};

	//------------------------------
	//	UNIT CATEGORIES
	//------------------------------

	/**
	 * @brief		namespace representing the implemented base and derived unit types. These will not generally be needed by library users.
	 */
	namespace category
	{
		// SCALAR (DIMENSIONLESS) TYPES
		using scalar_unit = base_unit<>;
		using dimensionless_unit = base_unit<>;

		// SI BASE UNIT TYPES	--------------------		METERS			KILOGRAMS		SECONDS			RADIANS			AMPERES			KELVIN			MOLE			CANDELA			
		using length_unit = base_unit<std::ratio<1>>;
		using mass_unit = base_unit<std::ratio<0>, std::ratio<1>>;
		using time_unit = base_unit<std::ratio<0>, std::ratio<0>, std::ratio<1>>;
		using angle_unit = base_unit<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>>;
		using current_unit = base_unit<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>>;
		using temperature_unit = base_unit<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>>;
		using substance_unit = base_unit<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>>;
		using luminous_intensity_unit = base_unit<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>>;

		// SI DERIVED UNIT TYPES	---------------			METERS			KILOGRAMS		SECONDS			RADIANS			AMPERES			KELVIN			MOLE			CANDELA			
		using solid_angle_unit = base_unit<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<2>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>>;
		using frequency_unit = base_unit<std::ratio<0>, std::ratio<0>, std::ratio<-1>>;
		using velocity_unit = base_unit<std::ratio<1>, std::ratio<0>, std::ratio<-1>>;
		using angular_velocity_unit = base_unit<std::ratio<0>, std::ratio<0>, std::ratio<-1>, std::ratio<1>>;
		using acceleration_unit = base_unit<std::ratio<1>, std::ratio<0>, std::ratio<-2>>;
		using force_unit = base_unit<std::ratio<1>, std::ratio<1>, std::ratio<-2>>;
		using pressure_unit = base_unit<std::ratio<-1>, std::ratio<1>, std::ratio<-2>>;
		using charge_unit = base_unit<std::ratio<0>, std::ratio<0>, std::ratio<1>, std::ratio<0>, std::ratio<1>>;
		using energy_unit = base_unit<std::ratio<2>, std::ratio<1>, std::ratio<-2>>;
		using power_unit = base_unit<std::ratio<2>, std::ratio<1>, std::ratio<-3>>;
		using voltage_unit = base_unit<std::ratio<2>, std::ratio<1>, std::ratio<-3>, std::ratio<0>, std::ratio<-1>>;
		using capacitance_unit = base_unit<std::ratio<-2>, std::ratio<-1>, std::ratio<4>, std::ratio<0>, std::ratio<2>>;
		using impedance_unit = base_unit<std::ratio<2>, std::ratio<1>, std::ratio<-3>, std::ratio<0>, std::ratio<-2>>;
		using conductance_unit = base_unit<std::ratio<-2>, std::ratio<-1>, std::ratio<3>, std::ratio<0>, std::ratio<2>>;
		using magnetic_flux_unit = base_unit<std::ratio<2>, std::ratio<1>, std::ratio<-2>, std::ratio<0>, std::ratio<-1>>;
		using magnetic_field_strength_unit = base_unit<std::ratio<0>, std::ratio<1>, std::ratio<-2>, std::ratio<0>, std::ratio<-1>>;
		using inductance_unit = base_unit<std::ratio<2>, std::ratio<1>, std::ratio<-2>, std::ratio<0>, std::ratio<-2>>;
		using luminous_flux_unit = base_unit<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<2>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>>;
		using illuminance_unit = base_unit<std::ratio<-2>, std::ratio<0>, std::ratio<0>, std::ratio<2>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>>;
		using radioactivity_unit = base_unit<std::ratio<0>, std::ratio<0>, std::ratio<-1>>;

		// OTHER UNIT TYPES			---------------			METERS			KILOGRAMS		SECONDS			RADIANS			AMPERES			KELVIN			MOLE			CANDELA			
		using	torque_unit = base_unit<std::ratio<2>, std::ratio<1>, std::ratio<-2>>;
		using	area_unit = base_unit<std::ratio<2>>;
		using	volume_unit = base_unit<std::ratio<3>>;
		using	density_unit = base_unit<std::ratio<-3>, std::ratio<1>>;
	}

	//------------------------------
	//	UNIT CLASSES
	//------------------------------

	/** @cond */	// DOXYGEN IGNORE
	/**
	 * @brief		unit type template specialization for units derived from base units.
	 */
	template <class, class, class, class> struct unit;
	template<class Conversion, class... Exponents, class PiExponent, class Translation>
	struct unit<Conversion, base_unit<Exponents...>, PiExponent, Translation> : detail::_unit
	{
		static_assert(is_ratio<Conversion>::value, "Template parameter `Conversion` must be a `std::ratio` representing the conversion factor to `BaseUnit`.");
		static_assert(is_ratio<PiExponent>::value, "Template parameter `PiExponent` must be a `std::ratio` representing the exponents of Pi the unit has.");
		static_assert(is_ratio<Translation>::value, "Template parameter `Translation` must be a `std::ratio` representing an additive translation required by the unit conversion.");

		typedef typename units::base_unit<Exponents...> base_unit_type;
		typedef Conversion conversion_ratio;
		typedef Translation translation_ratio;
		typedef PiExponent pi_exponent_ratio;
	};
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @brief		Type representing an arbitrary unit.
	 * @ingroup		UnitTypes
	 * @details		`unit` types are used as tags for the `conversion` function. They are *not* containers
	 *				(see `unit_t` for a  container class). Each unit is defined by:
	 *
	 *				- A `std::ratio` defining the conversion factor to the base unit type. (e.g. `std::ratio<1,12>` for inches to feet)
	 *				- A base unit that the unit is derived from (or a unit category. Must be of type `unit` or `base_unit`)
	 *				- An exponent representing factors of PI required by the conversion. (e.g. `std::ratio<-1>` for a radians to degrees conversion)
	 *				- a ratio representing a datum translation required for the conversion (e.g. `std::ratio<32>` for a farenheit to celsius conversion)
	 *
	 *				Typically, a specific unit, like `meters`, would be implemented as a type alias
	 *				of `unit`, i.e. `using meters = unit<std::ratio<1>, category::length_unit`, or
	 *				`using inches = unit<std::ratio<1,12>, feet>`.
	 * @tparam		Conversion	std::ratio representing scalar multiplication factor.
	 * @tparam		BaseUnit	Unit type which this unit is derived from. May be a `base_unit`, or another `unit`.
	 * @tparam		PiExponent	std::ratio representing the exponent of pi required by the conversion.
	 * @tparam		Translation	std::ratio representing any datum translation required by the conversion.
	 */
	template<class Conversion, class BaseUnit, class PiExponent = std::ratio<0>, class Translation = std::ratio<0>>
	struct unit : detail::_unit
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

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		base_unit_of trait implementation
		 * @details		recursively seeks base_unit type that a unit is derived from. Since units can be
		 *				derived from other units, the `base_unit_type` typedef may not represent this value.
		 */
		template<class> struct base_unit_of_impl;
		template<class Conversion, class BaseUnit, class PiExponent, class Translation>
		struct base_unit_of_impl<unit<Conversion, BaseUnit, PiExponent, Translation>> : base_unit_of_impl<BaseUnit> {};
		template<class... Exponents>
		struct base_unit_of_impl<base_unit<Exponents...>>
		{
			typedef base_unit<Exponents...> type;
		};
		template<>
		struct base_unit_of_impl<void>
		{
			typedef void type;
		};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @brief		Trait which returns the `base_unit` type that a unit is originally derived from.
	 * @details		Since units can be derived from other `unit` types in addition to `base_unit` types,
	 *				the `base_unit_type` typedef will not always be a `base_unit` (or unit category).
	 *				Since compatible
	 */
	template<class U>
	using base_unit_of = typename detail::base_unit_of_impl<U>::type;

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		implementation of base_unit_multiply
		 * @details		'multiples' (adds exponent ratios of) two base unit types. Base units can be found
		 *				using `base_unit_of`.
		 */
		template<class, class> struct base_unit_multiply_impl;
		template<class... Exponents1, class... Exponents2>
		struct base_unit_multiply_impl<base_unit<Exponents1...>, base_unit<Exponents2...>> {
			using type = base_unit<std::ratio_add<Exponents1, Exponents2>...>;
		};

		/**
		 * @brief		represents type of two base units multiplied together
		 */
		template<class U1, class U2>
		using base_unit_multiply = typename base_unit_multiply_impl<U1, U2>::type;

		/**
		 * @brief		implementation of base_unit_divide
		 * @details		'dived' (subtracts exponent ratios of) two base unit types. Base units can be found
		 *				using `base_unit_of`.
		 */
		template<class, class> struct base_unit_divide_impl;
		template<class... Exponents1, class... Exponents2>
		struct base_unit_divide_impl<base_unit<Exponents1...>, base_unit<Exponents2...>> {
			using type = base_unit<std::ratio_subtract<Exponents1, Exponents2>...>;
		};

		/**
		 * @brief		represents the resulting type of `base_unit` U1 divided by U2.
		 */
		template<class U1, class U2>
		using base_unit_divide = typename base_unit_divide_impl<U1, U2>::type;

		/**
		 * @brief		implementation of inverse_base
		 * @details		multiplies all `base_unit` exponent ratios by -1. The resulting type represents
		 *				the inverse base unit of the given `base_unit` type.
		 */
		template<class> struct inverse_base_impl;

		template<class... Exponents>
		struct inverse_base_impl<base_unit<Exponents...>> {
			using type = base_unit<std::ratio_multiply<Exponents, std::ratio<-1>>...>;
		};

		/**
		 * @brief		represent the inverse type of `class U`
		 * @details		E.g. if `U` is `length_unit`, then `inverse<U>` will represent `length_unit^-1`.
		 */
		template<class U> using inverse_base = typename inverse_base_impl<U>::type;

		/**
		 * @brief		implementation of `squared_base`
		 * @details		multiplies all the exponent ratios of the given class by 2. The resulting type is
		 *				equivalent to the given type squared.
		 */
		template<class U> struct squared_base_impl;
		template<class... Exponents>
		struct squared_base_impl<base_unit<Exponents...>> {
			using type = base_unit<std::ratio_multiply<Exponents, std::ratio<2>>...>;
		};

		/**
		 * @brief		represents the type of a `base_unit` squared.
		 * @details		E.g. `squared<length_unit>` will represent `length_unit^2`.
		 */
		template<class U> using squared_base = typename squared_base_impl<U>::type;

		/**
		 * @brief		implementation of `cubed_base`
		 * @details		multiplies all the exponent ratios of the given class by 3. The resulting type is
		 *				equivalent to the given type cubed.
		 */
		template<class U> struct cubed_base_impl;
		template<class... Exponents>
		struct cubed_base_impl<base_unit<Exponents...>> {
			using type = base_unit<std::ratio_multiply<Exponents, std::ratio<3>>...>;
		};

		/**
		 * @brief		represents the type of a `base_unit` cubed.
		 * @details		E.g. `cubed<length_unit>` will represent `length_unit^3`.
		 */
		template<class U> using cubed_base = typename cubed_base_impl<U>::type;

		/**
		 * @brief		implementation of `sqrt_base`
		 * @details		divides all the exponent ratios of the given class by 2. The resulting type is
		 *				equivalent to the square root of the given type.
		 */
		template<class U> struct sqrt_base_impl;
		template<class... Exponents>
		struct sqrt_base_impl<base_unit<Exponents...>> {
			using type = base_unit<std::ratio_divide<Exponents, std::ratio<2>>...>;
		};

		/**
		 * @brief		represents the square-root type of a `base_unit`.
		 * @details		E.g. `sqrt<length_unit>` will represent `length_unit^(1/2)`.
		 */
		template<class U> using sqrt_base = typename sqrt_base_impl<U>::type;

		/**
		 * @brief		implementation of `cbrt_base`
		 * @details		divides all the exponent ratios of the given class by 3. The resulting type is
		 *				equivalent to the given type's cube-root.
		 */
		template<class U> struct cbrt_base_impl;
		template<class... Exponents>
		struct cbrt_base_impl<base_unit<Exponents...>> {
			using type = base_unit<std::ratio_multiply<Exponents, std::ratio<3>>...>;
		};

		/**
		 * @brief		represents the cube-root type of a `base_unit` .
		 * @details		E.g. `cbrt<length_unit>` will represent `length_unit^(1/3)`.
		 */
		template<class U> using cbrt_base = typename cbrt_base_impl<U>::type;
	}
	/** @endcond */	// END DOXYGEN IGNORE

	//------------------------------
	//	UNIT MANIPULATORS
	//------------------------------

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		implementation of `unit_multiply`.
		 * @details		multiplies two units. The base unit becomes the base units of each with their exponents
		 *				added together. The conversion factors of each are multiplied by each other. Pi exponent ratios
		 *				are added, and datum translations are removed.
		 */
		template<class Unit1, class Unit2>
		struct unit_multiply_impl
		{
			using type = unit < std::ratio_multiply<typename Unit1::conversion_ratio, typename Unit2::conversion_ratio>,
				base_unit_multiply <base_unit_of<typename Unit1::base_unit_type>, base_unit_of<typename Unit2::base_unit_type>>,
				std::ratio_add<typename Unit1::pi_exponent_ratio, typename Unit2::pi_exponent_ratio>,
				std::ratio < 0 >> ;
		};

		/**
		 * @brief		represents the type of two units multiplied together.
		 * @details		recalculates conversion and exponent ratios at compile-time.
		 */
		template<class U1, class U2>
		using unit_multiply = typename unit_multiply_impl<U1, U2>::type;

		/**
		 * @brief		implementation of `unit_divide`.
		 * @details		divides two units. The base unit becomes the base units of each with their exponents
		 *				subtracted from each other. The conversion factors of each are divided by each other. Pi exponent ratios
		 *				are subtracted, and datum translations are removed.
		 */
		template<class Unit1, class Unit2>
		struct unit_divide_impl
		{
			using type = unit < std::ratio_divide<typename Unit1::conversion_ratio, typename Unit2::conversion_ratio>,
				base_unit_divide<base_unit_of<typename Unit1::base_unit_type>, base_unit_of<typename Unit2::base_unit_type>>,
				std::ratio_subtract<typename Unit1::pi_exponent_ratio, typename Unit2::pi_exponent_ratio>,
				std::ratio < 0 >> ;
		};

		/**
		 * @brief		represents the type of two units divided by each other.
		 * @details		recalculates conversion and exponent ratios at compile-time.
		 */
		template<class U1, class U2>
		using unit_divide = typename unit_divide_impl<U1, U2>::type;

		/**
		 * @brief		implementation of `inverse`
		 * @details		inverts a unit (equivalent to 1/unit). The `base_unit` and pi exponents are all multiplied by
		 *				-1. The conversion ratio numerator and denominator are swapped. Datum translation
		 *				ratios are removed.
		 */
		template<class Unit>
		struct inverse_impl
		{
			using type = unit < std::ratio<Unit::conversion_ratio::den, Unit::conversion_ratio::num>,
				inverse_base<base_unit_of<typename unit_traits<Unit>::base_unit_type>>,
				std::ratio_multiply<typename unit_traits<Unit>::pi_exponent_ratio, std::ratio<-1>>,
				std::ratio < 0 >> ;	// inverses are rates or change, the translation factor goes away.
		};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @brief		represents the inverse unit type of `class U`.
	 * @ingroup		UnitManipulators
	 * @tparam		U	`unit` type to invert.
	 * @details		E.g. `inverse<meters>` will represent meters^-1 (i.e. 1/meters).
	 */
	template<class U> using inverse = typename detail::inverse_impl<U>::type;

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		implementation of `squared`
		 * @details		Squares the conversion ratio, `base_unit` exponents, pi exponents, and removes
		 *				datum translation ratios.
		 */
		template<class Unit>
		struct squared_impl
		{
			static_assert(is_unit<Unit>::value, "Template parameter `Unit` must be a `unit` type.");
			using Conversion = typename Unit::conversion_ratio;
			using type = unit < std::ratio_multiply<Conversion, Conversion>,
				squared_base<base_unit_of<typename Unit::base_unit_type>>,
				std::ratio_multiply<typename Unit::pi_exponent_ratio, std::ratio<2>>,
				std::ratio < 0 >> ;
		};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @brief		represents the unit type of `class U` squared
	 * @ingroup		UnitManipulators
	 * @tparam		U	`unit` type to square.
	 * @details		E.g. `square<meters>` will represent meters^2.
	 */
	template<class U>
	using squared = typename detail::squared_impl<U>::type;

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/**
			 * @brief		implementation of `cubed`
			 * @details		Cubes the conversion ratio, `base_unit` exponents, pi exponents, and removes
			 *				datum translation ratios.
			 */
		template<class Unit>
		struct cubed_impl
		{
			static_assert(is_unit<Unit>::value, "Template parameter `Unit` must be a `unit` type.");
			using Conversion = typename Unit::conversion_ratio;
			using type = unit < std::ratio_multiply<Conversion, std::ratio_multiply<Conversion, Conversion>>,
				cubed_base<base_unit_of<typename Unit::base_unit_type>>,
				std::ratio_multiply<typename Unit::pi_exponent_ratio, std::ratio<3>>,
				std::ratio < 0 >> ;
		};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @brief		represents the type of `class U` cubed.
	 * @ingroup		UnitManipulators
	 * @tparam		U	`unit` type to cube.
	 * @details		E.g. `cubed<meters>` will represent meters^3.
	 */
	template<class U>
	using cubed = typename detail::cubed_impl<U>::type;

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		//----------------------------------
		//	RATIO_SQRT IMPLEMENTATION
		//----------------------------------

		using Zero = std::ratio<0>;
		using One = std::ratio<1>;
		template <typename R> using Square = std::ratio_multiply<R, R>;

		// Find the largest std::integer N such that Predicate<N>::value is true.
		template <template <std::intmax_t N> class Predicate, typename enabled = void>
		struct BinarySearch {
			template <std::intmax_t N>
			struct SafeDouble_ {
				static const std::intmax_t value = 2 * N;
				static_assert(value > 0, "Overflows when computing 2 * N");
			};

			template <intmax_t Lower, intmax_t Upper, typename Condition1 = void, typename Condition2 = void>
			struct DoubleSidedSearch_ : DoubleSidedSearch_<Lower, Upper,
				typename std::conditional<(Upper - Lower == 1), std::true_type, std::false_type>::type,
				typename std::conditional<((Upper - Lower>1 && Predicate<Lower + (Upper - Lower) / 2>::value)), std::true_type, std::false_type>::type> {};

			template <intmax_t Lower, intmax_t Upper>
			struct DoubleSidedSearch_<Lower, Upper, std::false_type, std::false_type> : DoubleSidedSearch_<Lower, Lower + (Upper - Lower) / 2> {};

			template <intmax_t Lower, intmax_t Upper, typename Condition2>
			struct DoubleSidedSearch_<Lower, Upper, std::true_type, Condition2> : std::integral_constant<intmax_t, Lower>{};

			template <intmax_t Lower, intmax_t Upper, typename Condition1>
			struct DoubleSidedSearch_<Lower, Upper, Condition1, std::true_type> : DoubleSidedSearch_<Lower + (Upper - Lower) / 2, Upper>{};

			template <std::intmax_t Lower, class enabled1 = void>
			struct SingleSidedSearch_ : SingleSidedSearch_<Lower, typename std::conditional<Predicate<SafeDouble_<Lower>::value>::value, std::true_type, std::false_type>::type>{};

			template <std::intmax_t Lower>
			struct SingleSidedSearch_<Lower, std::false_type> : DoubleSidedSearch_<Lower, SafeDouble_<Lower>::value> {};

			template <std::intmax_t Lower>
			struct SingleSidedSearch_<Lower, std::true_type> : SingleSidedSearch_<SafeDouble_<Lower>::value>{};

			const static std::intmax_t value = SingleSidedSearch_<1>::value;
 		};

		template <template <std::intmax_t N> class Predicate>
		struct BinarySearch<Predicate, typename std::enable_if<!Predicate<1>::value>::type> : std::integral_constant<std::intmax_t, 0>{};

		// Find largest std::integer N such that N<=sqrt(R)
		template <typename R>
		struct Integer {
			template <std::intmax_t N> using Predicate_ = std::ratio_less_equal<std::ratio<N>, std::ratio_divide<R, std::ratio<N>>>;
			const static std::intmax_t value = BinarySearch<Predicate_>::value;
		};

		template <typename R>
		struct IsPerfectSquare {
			const static std::intmax_t DenSqrt_ = Integer<std::ratio<R::den>>::value;
			const static std::intmax_t NumSqrt_ = Integer<std::ratio<R::num>>::value;
			const static bool value =( DenSqrt_ * DenSqrt_ == R::den && NumSqrt_ * NumSqrt_ == R::num);
			using Sqrt = std::ratio<NumSqrt_, DenSqrt_>;
		};

		// Represents sqrt(P)-Q.
		template <typename Tp, typename Tq>
		struct Remainder {
			using P = Tp;
			using Q = Tq;
		};

		// Represents 1/R = I + Rem where R is a Remainder.
		template <typename R>
		struct Reciprocal {
			using P_ = typename R::P;
			using Q_ = typename R::Q;
			using Den_ = std::ratio_subtract<P_, Square<Q_>>;
			using A_ = std::ratio_divide<Q_, Den_>;
			using B_ = std::ratio_divide<P_, Square<Den_>>;
			const static std::intmax_t I_ = (A_::num + Integer<std::ratio_multiply<B_, Square<std::ratio<A_::den>>>>::value) / A_::den;
			using I = std::ratio<I_>;
			using Rem = Remainder<B_, std::ratio_subtract<I, A_>>;
		};

		// Expands sqrt(R) to continued fraction:
		// f(x)=C1+1/(C2+1/(C3+1/(...+1/(Cn+x)))) = (U*x+V)/(W*x+1) and sqrt(R)=f(Rem).
		// The error |f(Rem)-V| = |(U-W*V)x/(W*x+1)| <= |U-W*V|*Rem <= |U-W*V|/I' where
		// I' is the std::integer part of reciprocal of Rem.
		template <typename Tr, std::intmax_t N>
		struct ContinuedFraction {
			template <typename T>
			using Abs_ = typename std::conditional<std::ratio_less<T, Zero>::value, std::ratio_subtract<Zero, T>, T>::type;

			using R = Tr;
			using Last_ = ContinuedFraction<R, N - 1>;
			using Reciprocal_ = Reciprocal<typename Last_::Rem>;
			using Rem = typename Reciprocal_::Rem;
			using I_ = typename Reciprocal_::I;
			using Den_ = std::ratio_add<typename Last_::W, I_>;
			using U = std::ratio_divide<typename Last_::V, Den_>;
			using V = std::ratio_divide<std::ratio_add<typename Last_::U, std::ratio_multiply<typename Last_::V, I_>>, Den_>;
			using W = std::ratio_divide<One, Den_>;
			using Error = Abs_<std::ratio_divide<std::ratio_subtract<U, std::ratio_multiply<V, W>>, typename Reciprocal<Rem>::I>>;
		};

		template <typename Tr>
		struct ContinuedFraction<Tr, 1> {
			using R = Tr;
			using U = One;
			using V = std::ratio<Integer<R>::value>;
			using W = Zero;
			using Rem = Remainder<R, V>;
			using Error = std::ratio_divide<One, typename Reciprocal<Rem>::I>;
		};

		template <typename R, typename Eps, std::intmax_t N = 1, typename enabled = void>
		struct Sqrt_ : Sqrt_<R, Eps, N + 1> {};

		template <typename R, typename Eps, std::intmax_t N>
		struct Sqrt_<R, Eps, N, typename std::enable_if<std::ratio_less_equal<typename ContinuedFraction<R, N>::Error, Eps>::value>::type> {
			using type = typename ContinuedFraction<R, N>::V;
		};

		template <typename R, typename Eps, typename enabled = void>
		struct Sqrt {
			static_assert(std::ratio_greater_equal<R, Zero>::value, "R can't be negative");
		};

		template <typename R, typename Eps>
		struct Sqrt<R, Eps, typename std::enable_if<std::ratio_greater_equal<R, Zero>::value && IsPerfectSquare<R>::value>::type> {
			using type = typename IsPerfectSquare<R>::Sqrt;
		};

		template <typename R, typename Eps>
		struct Sqrt<R, Eps, typename std::enable_if<(std::ratio_greater_equal<R, Zero>::value && !IsPerfectSquare<R>::value)>::type> : Sqrt_<R, Eps>{};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @ingroup		TypeTraits
	 * @brief		Calculate square root of a ratio at compile-time
	 * @details		Calculates a rational approximation of the square root of the ratio. The error
	 *				in the calculation is bounded by 1/epsilon (Eps). E.g. for the default value
	 *				of 10000000000, the maximum error will be a/10000000000, or 1e-8, or said another way,
	 *				the error will be on the order of 10^-9. Since these calculations are done at 
	 *				compile time, it is advisable to set epsilon to the highest value that does not
	 *				cause an integer overflow in the calculation. If you can't compile `ratio_sqrt` 
	 *				due to overflow errors, reducing the value of epsilon sufficiently will correct
	 *				the problem.\n\n
	 *				`ratio_sqrt` is guaranteed to converge for all values of `Ratio` which do not
	 *				overflow. 
	 * @note		This function provides a rational approximation, _NOT_ an exact value.
	 * @tparam		Ratio	ratio to take the square root of. This can represent any rational value,
	 *						_not_ just integers or values with integer roots.
	 * @tparam		Eps		Value of epsilon, which represents the inverse of the maximum allowable
	 *						error. This value should be chosen to be as high as possible before
	 *						integer overflow errors occur in the compiler.
	 */
	template<typename Ratio, std::intmax_t Eps = 10000000000>
	using ratio_sqrt = typename  detail::Sqrt<Ratio, std::ratio<1, Eps>>::type;

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		implementation of `sqrt`
		 * @details		square roots the conversion ratio, `base_unit` exponents, pi exponents, and removes
		 *				datum translation ratios.
		 */
		template<class Unit, std::intmax_t Eps>
		struct sqrt_impl
		{
			static_assert(is_unit<Unit>::value, "Template parameter `Unit` must be a `unit` type.");
			using Conversion = typename Unit::conversion_ratio;
			using type = unit <ratio_sqrt<Conversion, Eps>,
				sqrt_base<base_unit_of<typename Unit::base_unit_type>>,
				std::ratio_divide<typename Unit::pi_exponent_ratio, std::ratio<2>>,
				std::ratio < 0 >>;
		};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	/**	 
	 * @ingroup		UnitManipulators
	 * @brief		represents the square root of type `class U`.
	 * @details	Calculates a rational approximation of the square root of the unit. The error
	 *				in the calculation is bounded by 1/epsilon (Eps). E.g. for the default value
	 *				of 10000000000, the maximum error will be a/10000000000, or 1e-8, or said another way,
	 *				the error will be on the order of 10^-9. Since these calculations are done at
	 *				compile time, it is advisable to set epsilon to the highest value that does not
	 *				cause an integer overflow in the calculation. If you can't compile `ratio_sqrt`
	 *				due to overflow errors, reducing the value of epsilon sufficiently will correct
	 *				the problem.\n\n
	 *				`ratio_sqrt` is guaranteed to converge for all values of `Ratio` which do not
	 *				overflow.
	 * @tparam		U	`unit` type to take the square root of.
	 * @tparam		Eps	Value of epsilon, which represents the inverse of the maximum allowable
	 *					error. This value should be chosen to be as high as possible before
	 *					integer overflow errors occur in the compiler. 
	 * @note		USE WITH CAUTION. The is an approximate value. In general, squared<sqrt<meter>> != meter,
	 *				i.e. the operation is not reversible, and it will result in propogated approximations.
	 *				Use only when absolutely necessary.
	 */
	template<class U, std::intmax_t Eps = 10000000000>
	using square_root = typename detail::sqrt_impl<U, Eps>::type;

	//------------------------------
	//	COMPOUND UNITS
	//------------------------------

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/**
			 * @brief		implementation of compound_unit
			 * @details		multiplies a variadic list of units together, and is inherited from the resulting
			 *				type.
			 */
		template<class U, class... Us> struct compound_impl;
		template<class U> struct compound_impl<U> { using type = U; };
		template<class U1, class U2, class...Us>
		struct compound_impl<U1, U2, Us...>
			: compound_impl<unit_multiply<U1, U2>, Us...> {};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @brief		Represents a unit type made up from other units.
	 * @details		Compound units are formed by multiplying the units of all the types provided in
	 *				the template argument. Types provided must inherit from `unit`. A compound unit can
	 *				be formed from any number of other units, and unit manipulators like `inverse` and
	 *				`squared` are supported. E.g. to specify acceleration, on could create
	 *				`using acceleration = compound_unit<length::meters, inverse<squared<seconds>>;`
	 * @tparam		U...	units which, when multiplied together, form the desired compound unit.
	 * @ingroup		UnitTypes
	 */
	template<class U, class... Us>
	using compound_unit = typename detail::compound_impl<U, Us...>::type;

	//------------------------------
	//	PREFIXES
	//------------------------------

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/**
			 * @brief		prefix applicator.
			 * @details		creates a unit type from a prefix and a unit
			 */
		template<class Ratio, class Unit>
		struct prefix
		{
			static_assert(is_ratio<Ratio>::value, "Template parameter `Ratio` must be a `std::ratio`.");
			static_assert(is_unit<Unit>::value, "Template parameter `Unit` must be a `unit` type.");
			typedef typename units::unit<Ratio, Unit> type;
		};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @ingroup UnitManipulators
	 * @{
	 * @ingroup Prefixes
	 * @{
	 */
	template<class U> using atto = typename detail::prefix<std::atto, U>::type;								///< Represents the type of `class U` with the metric 'atto' prefix appended.  @details E.g. atto<meters> represents meters*10^-21	@tparam U unit type to apply the prefix to.
	template<class U> using femto = typename detail::prefix<std::femto, U>::type;							///< Represents the type of `class U` with the metric 'atto' prefix appended.  @details E.g. femto<meters> represents meters*10^-18	@tparam U unit type to apply the prefix to.
	template<class U> using pico = typename detail::prefix<std::pico, U>::type;								///< Represents the type of `class U` with the metric 'femto' prefix appended. @details E.g. pico<meters> represents meters*10^-15	@tparam U unit type to apply the prefix to.
	template<class U> using nano = typename detail::prefix<std::nano, U>::type;								///< Represents the type of `class U` with the metric 'pico' prefix appended.  @details E.g. nano<meters> represents meters*10^-12	@tparam U unit type to apply the prefix to.
	template<class U> using micro = typename detail::prefix<std::micro, U>::type;							///< Represents the type of `class U` with the metric 'nano' prefix appended.  @details E.g. micro<meters> represents meters*10^-9	@tparam U unit type to apply the prefix to.
	template<class U> using milli = typename detail::prefix<std::milli, U>::type;							///< Represents the type of `class U` with the metric 'micro' prefix appended. @details E.g. milli<meters> represents meters*10^-6	@tparam U unit type to apply the prefix to.
	template<class U> using centi = typename detail::prefix<std::centi, U>::type;							///< Represents the type of `class U` with the metric 'milli' prefix appended. @details E.g. centi<meters> represents meters*10^-3	@tparam U unit type to apply the prefix to.
	template<class U> using deci = typename detail::prefix<std::deci, U>::type;								///< Represents the type of `class U` with the metric 'centi' prefix appended. @details E.g. deci<meters> represents meters*10^-2	@tparam U unit type to apply the prefix to.
	template<class U> using deca = typename detail::prefix<std::deca, U>::type;								///< Represents the type of `class U` with the metric 'deci' prefix appended.  @details E.g. deca<meters> represents meters*10^-1	@tparam U unit type to apply the prefix to.
	template<class U> using hecto = typename detail::prefix<std::hecto, U>::type;							///< Represents the type of `class U` with the metric 'deca' prefix appended.  @details E.g. hecto<meters> represents meters*10^1	@tparam U unit type to apply the prefix to.
	template<class U> using kilo = typename detail::prefix<std::kilo, U>::type;								///< Represents the type of `class U` with the metric 'hecto' prefix appended. @details E.g. kilo<meters> represents meters*10^2	@tparam U unit type to apply the prefix to.
	template<class U> using mega = typename detail::prefix<std::mega, U>::type;								///< Represents the type of `class U` with the metric 'kilo' prefix appended.  @details E.g. mega<meters> represents meters*10^3	@tparam U unit type to apply the prefix to.
	template<class U> using giga = typename detail::prefix<std::giga, U>::type;								///< Represents the type of `class U` with the metric 'mega' prefix appended.  @details E.g. giga<meters> represents meters*10^6	@tparam U unit type to apply the prefix to.
	template<class U> using tera = typename detail::prefix<std::tera, U>::type;								///< Represents the type of `class U` with the metric 'giga' prefix appended.  @details E.g. tera<meters> represents meters*10^9	@tparam U unit type to apply the prefix to.
	template<class U> using peta = typename detail::prefix<std::peta, U>::type;								///< Represents the type of `class U` with the metric 'tera' prefix appended.  @details E.g. peta<meters> represents meters*10^12	@tparam U unit type to apply the prefix to.
	template<class U> using exa = typename detail::prefix<std::exa, U>::type;								///< Represents the type of `class U` with the metric 'peta' prefix appended.  @details E.g. exa<meters> represents meters*10^15	@tparam U unit type to apply the prefix to.
	/** @} @} */

	//------------------------------
	//	CONVERSION TRAITS
	//------------------------------

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which checks whether two units can be converted to each other
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_convertible_unit<U1, U2>::value` to test
	 *				whether `class U1` is convertible to `class U2`. Note: convertible has both the semantic meaning,
	 *				(i.e. meters can be converted to feet), and the c++ meaning of conversion (type meters can be
	 *				converted to type feet). Conversion is always symmetric, so if U1 is convertible to U2, then
	 *				U2 will be convertible to U1.
	 * @tparam		U1 Unit to convert from.
	 * @tparam		U2 Unit to convert to.
	 * @sa			is_convertible_unit_t
	 */
	template<class U1, class U2>
	struct is_convertible_unit : std::is_same <base_unit_of<typename unit_traits<U1>::base_unit_type>,
		base_unit_of<typename unit_traits<U2>::base_unit_type >> {};

	//------------------------------
	//	CONVERSION FUNCTION
	//------------------------------

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/// convert dispatch for units which are both the same
		template<class, class, typename T>
		static inline T convert(const T& value, std::true_type, std::false_type, std::false_type)
		{
			return value;
		}

		/// convert dispatch for units which are both the same
		template<class, class, typename T>
		static inline T convert(const T& value, std::true_type, std::false_type, std::true_type)
		{
			return value;
		}

		/// convert dispatch for units which are both the same
		template<class, class, typename T>
		static inline T convert(const T& value, std::true_type, std::true_type, std::false_type)
		{
			return value;
		}

		/// convert dispatch for units which are both the same
		template<class, class, typename T>
		static inline T convert(const T& value, std::true_type, std::true_type, std::true_type)
		{
			return value;
		}

		/// convert dispatch for units of different types w/ no translation and no PI
		template<class UnitFrom, class UnitTo, typename T>
		static inline T convert(const T& value, std::false_type, std::false_type, std::false_type)
		{
			using Ratio = std::ratio_divide<typename UnitFrom::conversion_ratio, typename UnitTo::conversion_ratio>;
			return (double(Ratio::num) * value / Ratio::den);
		}

		/// convert dispatch for units of different types w/ no translation, but has PI
		template<class UnitFrom, class UnitTo, typename T>
		static inline T convert(const T& value, std::false_type, std::true_type, std::false_type)
		{
			using Ratio = std::ratio_divide<typename UnitFrom::conversion_ratio, typename UnitTo::conversion_ratio>;
			using PiRatio = std::ratio_subtract<typename UnitFrom::pi_exponent_ratio, typename UnitTo::pi_exponent_ratio>;
			return ((double(Ratio::num) * value / Ratio::den) * std::pow(constants::PI, (double(PiRatio::num) / PiRatio::den)));
		}

		/// convert dispatch for units of different types with a translation, but no PI
		template<class UnitFrom, class UnitTo, typename T>
		static inline T convert(const T& value, std::false_type, std::false_type, std::true_type)
		{
			using Ratio = std::ratio_divide<typename UnitFrom::conversion_ratio, typename UnitTo::conversion_ratio>;
			using Translation = std::ratio_divide<std::ratio_subtract<typename UnitFrom::translation_ratio, typename UnitTo::translation_ratio>, typename UnitTo::conversion_ratio>;
			return ((double(Ratio::num) * value / Ratio::den) + (double(Translation::num) / Translation::den));
		}

		/// convert dispatch for units of different types with a translation AND PI
		template<class UnitFrom, class UnitTo, typename T>
		static inline T convert(const T& value, std::false_type, std::true_type, std::true_type)
		{
			using Ratio = std::ratio_divide<typename UnitFrom::conversion_ratio, typename UnitTo::conversion_ratio>;
			using Translation = std::ratio_divide<std::ratio_subtract<typename UnitFrom::translation_ratio, typename UnitTo::translation_ratio>, typename UnitTo::conversion_ratio>;
			using PiRatio = std::ratio_subtract<typename UnitFrom::pi_exponent_ratio, typename UnitTo::pi_exponent_ratio>;
			return ((double(Ratio::num) * value / Ratio::den) * std::pow(constants::PI, (double(PiRatio::num) / PiRatio::den)) + (double(Translation::num) / Translation::den));
		}
	}
	/** @endcond */	// END DOXYGEN IGNORE


	/**
	 * @ingroup		Conversion
	 * @brief		converts a <i>value</i> from one type to another.
	 * @details		Converts a <i>value</i> of a built-in arithmetic type to another unit. This does not change
	 *				the type of <i>value</i>, only what it contains. E.g. @code double result = convert<length::meters, length::feet>(1.0);	// result == 3.28084 @endcode
	 * @sa			unit_t	for implicit conversion of unit containers.
	 * @tparam		UnitFrom unit tag to convert <i>value</i> from. Must be a `unit` type (i.e. is_unit<UnitFrom>::value == true),
	 *				and must be convertible to `UnitTo` (i.e. is_converitble_unit<UnitFrom, UnitTo>::value == true).
	 * @tparam		UnitTo unit tag to convert <i>value</i> to. Must be a `unit` type (i.e. is_unit<UnitTo>::value == true),
	 *				and must be convertible from `UnitFrom` (i.e. is_converitble_unit<UnitFrom, UnitTo>::value == true).
	 * @tparam		T type of <i>value</i>. It is infered from <i>value</i>, and is expected to be a built-in arethmetic type.
	 * @param[in]	value Arithmetic value to convert from `UnitFrom` to `UnitTo`. The value should represent
	 *				a quantity in units of `UnitFrom`.
	 * @returns		value, converted from units of `UnitFrom` to `UnitTo`.
	 */
	template<class UnitFrom, class UnitTo, typename T = double>
	static inline T convert(const T& value)
	{
		static_assert(is_unit<UnitFrom>::value, "Template parameter `UnitFrom` must be a `unit` type.");
		static_assert(is_unit<UnitTo>::value, "Template parameter `UnitTo` must be a `unit` type.");
		static_assert(is_convertible_unit<UnitFrom, UnitTo>::value, "Units are not compatible.");

		using isSame = typename std::is_same<typename std::decay<UnitFrom>::type, typename std::decay<UnitTo>::type>::type;
		using piRequired = std::integral_constant<bool, !(std::is_same<std::ratio<0>, typename UnitFrom::pi_exponent_ratio>::value &&
			std::is_same<std::ratio<0>, typename UnitTo::pi_exponent_ratio>::value)>;
		using translationRequired = std::integral_constant<bool, !(std::is_same<std::ratio<0>, typename UnitFrom::translation_ratio>::value &&
			std::is_same<std::ratio<0>, typename UnitTo::translation_ratio>::value)>;

		return detail::convert<UnitFrom, UnitTo, T>(value, isSame{}, piRequired{}, translationRequired{});
	};

	//----------------------------------
	//	NON-LINEAR SCALE TRAITS
	//----------------------------------

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		/**
			 * @brief		implementation of has_operator_parenthesis
			 * @details		checks that operator() returns the same type as `Ret`
			 */
		template<class T, class Ret>
		struct has_operator_parenthesis_impl
		{
			template<class U>
			static auto test(U*) -> decltype(std::declval<U>()());
			template<typename>
			static std::false_type test(...);

			using type = typename std::is_same<Ret, decltype(test<T>(0))>::type;
		};
	}

	/**
	 * @brief		checks that `class T` has an `operator()` member which returns `Ret`
	 * @details		used as part of the linear_scale concept.
	 */
	template<class T, class Ret>
	struct has_operator_parenthesis : detail::has_operator_parenthesis_impl<T, Ret>::type {};

	namespace detail
	{
		/**
			* @brief		implementation of has_value_member
			* @details		checks for a member named `m_member` with type `Ret`
			*/
		template<class T, class Ret>
		struct has_value_member_impl
		{
			template<class U>
			static auto test(U* p) -> decltype(p->m_value);
			template<typename>
			static auto test(...)->std::false_type;

			using type = typename std::is_same<typename std::decay<Ret>::type, typename std::decay<decltype(test<T>(0))>::type>::type;
		};
	}

	/**
	 * @brief		checks for a member named `m_member` with type `Ret`
	 * @details		used as part of the linear_scale concept checker.
	 */
	template<class T, class Ret>
	struct has_value_member : detail::has_value_member_impl<T, Ret>::type {};
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @ingroup		TypeTraits
	 * @brief		Tests that `class T` meets the requirements for a non-linear scale
	 * @details		A non-linear scale must:
	 *				- be default constructible
	 *				- have an `operator()` member which returns the non-linear value stored in the scale
	 *				- have an accessible `m_value` member type which stores the linearized value in the scale.
	 *
	 *				Linear/nonlinear scales are used by `units::unit` to store values and scale them
	 *				if they represent things like dB.
	 */
	template<class T, class Ret>
	struct is_nonlinear_scale : std::integral_constant<bool,
		std::is_default_constructible<T>::value &&
		has_operator_parenthesis<T, Ret>::value &&
		has_value_member<T, Ret>::value>
	{};

	//------------------------------
	//	UNIT_T TYPE TRAITS
	//------------------------------

	/** @cond */	// DOXYGEN IGNORE
	/**
	 * @brief		unit_t_traits specialization for things which are not unit_t
	 * @details
	 */
	template<typename T, typename = void>
	struct unit_t_traits
	{
		typedef void non_linear_scale_type;
		typedef void underlying_type;
		typedef void unit_type;
	};
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait for accessing the publically defined types of `units::unit_t`
	 * @details
	 */
	template<typename T>
	struct unit_t_traits <T, typename void_t<
		typename T::non_linear_scale_type,
		typename T::underlying_type,
		typename T::unit_type>::type>
	{
		typedef typename T::non_linear_scale_type non_linear_scale_type;
		typedef typename T::underlying_type underlying_type;
		typedef typename T::unit_type unit_type;
	};

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether two container types derived from `unit_t` are convertible to each other
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_convertible_unit_t<U1, U2>::value` to test
	 *				whether `class U1` is convertible to `class U2`. Note: convertible has both the semantic meaning,
	 *				(i.e. meters can be converted to feet), and the c++ meaning of conversion (type meters can be
	 *				converted to type feet). Conversion is always symmetric, so if U1 is convertible to U2, then
	 *				U2 will be convertible to U1.
	 * @tparam		U1 Unit to convert from.
	 * @tparam		U2 Unit to convert to.
	 * @sa			is_convertible_unit
	 */
	template<class U1, class U2>
	struct is_convertible_unit_t : std::integral_constant<bool,
		is_convertible_unit<typename unit_t_traits<U1>::unit_type, typename unit_t_traits<U2>::unit_type>::value>
	{};

	//---------------------------------- 
	//	UNIT TYPE
	//----------------------------------

	/** @cond */	// DOXYGEN IGNORE
	// forward declaration
	template<typename T> struct linear_scale;

	namespace detail
	{
		/**
		* @brief		helper type to identify units.
		* @details		A non-templated base class for `unit` which enables RTTI testing.
		*/
		struct _unit_t {};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @ingroup		TypeTraits
	 * @brief		Traits which tests if a class is a `unit`
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_unit<T>::value` to test
	 *				whether `class T` implements a `unit`.
	 */
	template<class T>
	struct is_unit_t : std::is_base_of<detail::_unit_t, T>::type {};

	/**
	 * @ingroup		UnitContainers
	 * @brief		Container for values which represent quantities of a given unit.
	 * @details		Stores a value which represents a quantity in the given units. Unit containers
	 *				(except scalar values) are *not* convertible to built-in c++ types, in order to
	 *				provide type safety in dimensional analysis. Unit containers *are* implicitely
	 *				convertible to other compatible unit container types. Unit containers support
	 *				various types of arithmetic operations, depending on their scale type.
	 *
	 *				The value of a `unit_t` can only be changed on construction, or by assignment
	 *				from another `unit_t` type. If necessary, the underlying value can be accessed
	 *				using `operator()`: @code
	 *				meter_t m(5.0);
	 *				double val = m(); // val == 5.0	@endcode.
	 * @tparam		Units unit tag for which type of units the `unit_t` represents (e.g. meters)
	 * @tparam		T underlying type of the storage. Defaults to double.
	 * @tparam		NonLinearScale optional scale class for the units. Defaults to linear (i.e. does
	 *				not scale the unit value). Examples of non-linear scales could be logarithmic,
	 *				decibel, or richter scales. Non-linear scales must adhere to the non-linear-scale
	 *				concept, i.e. `is_nonlinear_scale<...>::value` must be `true`.
	 * @sa
	 *				- \ref lengthContainers "length unit containers"
	 *				- \ref massContainers "mass unit containers"
	 *				- \ref timeContainers "time unit containers"
	 *				- \ref angleContainers "angle unit containers"
	 *				- \ref currentContainers "current unit containers"
	 *				- \ref temperatureContainers "temperature unit containers"
	 *				- \ref substanceContainers "substance unit containers"
	 *				- \ref luminousIntensityContainers "luminous intensity unit containers"
	 *				- \ref solidAngleContainers "solid angle unit containers"
	 *				- \ref frequencyContainers "frequency unit containers"
	 *				- \ref velocityContainers "velocity unit containers"
	 *				- \ref angularVelocityContainers "angular velocity unit containers"
	 *				- \ref accelerationContainers "acceleration unit containers"
	 *				- \ref forceContainers "force unit containers"
	 *				- \ref pressureContainers "pressure unit containers"
	 *				- \ref chargeContainers "charge unit containers"
	 *				- \ref energyContainers "energy unit containers"
	 *				- \ref powerContainers "power unit containers"
	 *				- \ref voltageContainers "voltage unit containers"
	 *				- \ref capacitanceContainers "capacitance unit containers"
	 *				- \ref impedanceContainers "impedance unit containers"
	 *				- \ref magneticFluxContainers "magnetic flux unit containers"
	 *				- \ref magneticFieldStrengthContainers "magnetic field strength unit containers"
	 *				- \ref inductanceContainers "inductance unit containers"
	 *				- \ref luminousFluxContainers "luminous flux unit containers"
	 *				- \ref illuminanceContainers "illuminance unit containers"
	 *				- \ref radiationContainers "radiation unit containers"
	 *				- \ref torqueContainers "torque unit containers"
	 *				- \ref areaContainers "area unit containers"
	 *				- \ref volumeContainers "volume unit containers"
	 *				- \ref densityContainers "density unit containers"
	 *				- \ref concentrationContainers "concentration unit containers"
	 *				- \ref constantContainers "constant unit containers"
	 */
	template<class Units, typename T = double, template<typename> class NonLinearScale = linear_scale>
	class unit_t : public NonLinearScale<T>, detail::_unit_t
	{
		static_assert(units::is_nonlinear_scale<NonLinearScale<T>, T>::value, "Template parameter `NonLinearScale` does not conform to the `is_nonlinear_scale` concept.");

	protected:

		using nls = NonLinearScale<T>;
		using nls::m_value;

	public:

		typedef NonLinearScale<T> non_linear_scale_type;											///< Type of the non-linear scale of the unit_t (e.g. linear_scale)
		typedef T underlying_type;																	///< Type of the underlying storage of the unit_t (e.g. double)
		typedef Units unit_type;																	///< Type of `unit` the `unit_t` represents (e.g. meters)

		/**
		 * @ingroup		Constructors
		 * @brief		default constructor.
		 */
		inline unit_t() : NonLinearScale<T>(0) {};


		/**
		 * @brief		constructor
		 * @details		constructs a new unit_t using the non-linear scale's constructor.
		 * @param[in]	args	constructor arguments are forwarded to the non-linear scale constructor. Which
		 *				args are required depends on which scale is used. For the default (linear) scale,
		 *				a single double-type value should be given.
		 */
		template<class... Args>
		inline explicit unit_t(const Args&... args) : nls(args...) {};

		/**
		 * @brief		constructor
		 * @details		enable implicit conversions from T types ONLY for linear scalar units
		 * @param[in]	value value of the unit_t
		 */
		template<class Ty, class = typename std::enable_if<std::is_same<base_unit_of<Units>, category::scalar_unit>::value && std::is_arithmetic<Ty>::value>::type>
		inline unit_t(Ty value) : nls(value) {};

		/**
		 * @brief		copy constructor
		 * @details		performs implicit unit conversions if required.
		 * @param[in]	rhs unit to copy.
		 */
		template<class UnitsRhs, typename Ty, template<typename> class NlsRhs>
		inline unit_t(const unit_t<UnitsRhs, Ty, NlsRhs>& rhs)
		{
			nls::m_value = units::convert<UnitsRhs, Units, T>(rhs.m_value);
		};

		/**
		 * @brief		assignment
		 * @details		performs implicit unit conversions if required
		 * @param[in]	rhs unit to copy.
		 */
		template<class UnitsRhs, typename Ty, template<typename> class NlsRhs>
		inline unit_t& operator=(const unit_t<UnitsRhs, Ty, NlsRhs>& rhs)
		{
			nls::m_value = units::convert<UnitsRhs, Units, T>(rhs.m_value);
			return *this;
		}

		/**
		* @brief		assignment
		* @details		performs implicit conversions from built-in types ONLY for scalar units
		* @param[in]	rhs value to copy.
		*/
		template<class Ty, class = typename std::enable_if<std::is_same<base_unit_of<Units>, category::scalar_unit>::value && std::is_arithmetic<Ty>::value>::type>
		inline unit_t& operator=(Ty rhs)
		{
			nls::m_value = rhs;
			return *this;
		}

		/**
		 * @brief		less-than
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` is less than the value of `rhs`
		 */
		template<class UnitsRhs, typename Ty, template<typename> class NlsRhs>
		inline bool operator<(const unit_t<UnitsRhs, Ty, NlsRhs>& rhs) const
		{
			return (nls::m_value < units::convert<UnitsRhs, Units>(rhs.m_value));
		}

		/**
		 * @brief		less-than or equal
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` is less than or equal to the value of `rhs`
		 */
		template<class UnitsRhs, typename Ty, template<typename> class NlsRhs>
		inline bool operator<=(const unit_t<UnitsRhs, Ty, NlsRhs>& rhs) const
		{
			return (nls::m_value <= units::convert<UnitsRhs, Units>(rhs.m_value));
		}

		/**
		 * @brief		greater-than
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` is greater than the value of `rhs`
		 */
		template<class UnitsRhs, typename Ty, template<typename> class NlsRhs>
		inline bool operator>(const unit_t<UnitsRhs, Ty, NlsRhs>& rhs) const
		{
			return (nls::m_value > units::convert<UnitsRhs, Units>(rhs.m_value));
		}

		/**
		 * @brief		greater-than or equal
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` is greater than or equal to the value of `rhs`
		 */
		template<class UnitsRhs, typename Ty, template<typename> class NlsRhs>
		inline bool operator>=(const unit_t<UnitsRhs, Ty, NlsRhs>& rhs) const
		{
			return (nls::m_value >= units::convert<UnitsRhs, Units>(rhs.m_value));
		}

		/**
		 * @brief		equality
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` exactly equal to the value of rhs.
		 * @note		This may not be suitable for all applications when the underlying_type of unit_t is a double.
		 */
		template<class UnitsRhs, typename Ty, template<typename> class NlsRhs>
		inline bool operator==(const unit_t<UnitsRhs, Ty, NlsRhs>& rhs) const
		{
			return (nls::m_value == units::convert<UnitsRhs, Units>(rhs.m_value));
		}

		/**
		 * @brief		inequality
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` is not equal to the value of rhs.
		 * @note		This may not be suitable for all applications when the underlying_type of unit_t is a double.
		 */
		template<class UnitsRhs, typename Ty, template<typename> class NlsRhs>
		inline bool operator!=(const unit_t<UnitsRhs, Ty, NlsRhs>& rhs) const
		{
			return (nls::m_value != units::convert<UnitsRhs, Units>(rhs.m_value));
		}

		/**
		 * @brief		unit value
		 * @returns		value of the unit in it's underlying, non-safe type.
		 */
		inline T toDouble() const
		{
			return (*this)();
		}

		/**
		 * @brief		linearized unit value
		 * @returns		linearized value of unit which has a non-linear scale. For `unit_t` types with
		 *				linear scales, this is equivalent to `value`.
		 */
		inline T toLinearizedDouble() const
		{
			return m_value;
		}

		/**
		 * @brief		conversion
		 * @details		Converts to a different unit container. Units can be converted to other containers
		 *				implicitly, but this can be used in cases where explicit notation of a conversion
		 *				is beneficial, or where an r-value container is needed.
		 * @tparam		U unit (not unit_t) to convert to
		 * @returns		a unit container with the specified units containing the equivalent value to
		 *				*this.
		 */
		template<class U>
		inline auto convert() const -> unit_t<U>
		{
			static_assert(units::is_unit<U>::value, "Template parameter `U` must be a unit type.");
			return unit_t<U>(*this);
		}

		/**
		 * @brief		implicit type conversion.
		 * @details		only enabled for scalar unit types.
		 */
		template<class Ty, class = typename std::enable_if<std::is_same<base_unit_of<Units>, category::scalar_unit>::value && std::is_arithmetic<Ty>::value>::type>
		operator Ty() const { return  units::convert<Units, unit<std::ratio<1>, category::scalar_unit>>(nls::m_value); }

	public:

		template<class U, typename Ty, template<typename> class Nlt>
		friend class unit_t;

	};

	template<class Units, typename T, template<typename> class NonLinearScale>
	std::ostream& operator<<(std::ostream& os, const unit_t<Units, T, NonLinearScale>& obj)
	{
		os << obj.toDouble();
		return os;
	}

	//------------------------------
	//	NON-LINEAR SCALE TRAITS
	//------------------------------

	// forward declaration
	template<typename T> struct decibel_scale;

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type is inherited from a linear scale.
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `has_linear_scale<U1 [, U2, ...]>::value` to test
	 *				one or more types to see if they represent unit_t's whose scale is linear.
	 * @tparam		T	one or more types to test.
	 */
	template<typename... T>
	struct has_linear_scale : std::integral_constant<bool,
		all_true<std::is_base_of<linear_scale<typename unit_t_traits<T>::underlying_type>, T>::value...>::value >
	{};

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type is inherited from a decibel scale.
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `has_decibel_scale<U1 [, U2, ...]>::value` to test
	 *				one or more types to see if they represent unit_t's whose scale is in decibels.
	 * @tparam		T	one or more types to test.
	 */
	template<typename... T>
	struct has_decibel_scale : std::integral_constant<bool,
		all_true<std::is_base_of<decibel_scale<typename unit_t_traits<T>::underlying_type>, T>::value...>::value>
	{};

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether two types has the same non-linear scale.
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_same_scale<U1 , U2>::value` to test
	 *				whether two types have the same non-linear scale.
	 * @tparam		T1	left hand type.
	 * @tparam		T2	right hand type
	 */
	template<typename T1, typename T2>
	struct is_same_scale : std::integral_constant<bool,
		std::is_same<typename unit_t_traits<T1>::non_linear_scale_type, typename unit_t_traits<T2>::non_linear_scale_type>::value>
	{};

	//----------------------------------
	//	NON-LINEAR SCALES
	//----------------------------------

	// Non-linear transforms are used to pre and post scale units which are defined in terms of non-
	// linear functions of their current value. A good example of a non-linear scale would be a 
	// logarithmic or decibel scale

	//------------------------------
	//	LINEAR SCALE
	//------------------------------

	/**
	 * @brief		unit_t scale which is linear
	 * @details		Represents units on a linear scale. This is the appropriate unit_t scale for almost
	 *				all units almost all of the time.
	 * @tparam		T	underlying storage type
	 * @sa			unit_t
	 */
	template<typename T>
	struct linear_scale
	{
		inline linear_scale() : m_value(0) {}														///< default constructor.
		inline linear_scale(T value) : m_value(value) {}											///< constructor.
		inline T operator()() const { return m_value; }												///< returns value.

		T m_value;																					///< linearized value.	
	};

	//----------------------------------
	//	SCALAR (LINEAR) UNITS
	//----------------------------------

	// Scalar units are the *ONLY* units implicitly convertible to/from built-in types.
	namespace dimensionless
	{
		using scalar = unit<std::ratio<1>, category::scalar_unit>;
		using dimensionless = unit<std::ratio<1>, category::dimensionless_unit>;

		using scalar_t = unit_t<scalar>;
		using dimensionless_t = scalar_t;
	}

	namespace detail
	{
		template<class T> struct is_scalar_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_scalar_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::scalar_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_scalar_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::scalar_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether one or more types derived from `unit_t` represent scalar values.
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_scalar_unit<U1 [, U2, ...]>::value` to test
	 *				one or more types to see if they represent scalar value containers. A scalar unit is one which has no
	 *				dimensions (e.g. PI).
	 * @tparam		T	one or more types to test.
	 */
	template<class... T>
	struct is_scalar_unit : std::integral_constant<bool, all_true<detail::is_scalar_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	LINEAR ARITHMETIC
	//------------------------------

	template<class UnitTypeLhs, class UnitTypeRhs, typename std::enable_if<!is_same_scale<UnitTypeLhs, UnitTypeRhs>::value, int>::type = 0>
	inline int operator+(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs)
	{
		static_assert(is_same_scale<UnitTypeLhs, UnitTypeRhs>::value, "Cannot add units with different linear/non-linear scales.");
		return 0;
	}

	/// Addition operator for unit_t types with a linear_scale.
	template<class UnitTypeLhs, class UnitTypeRhs, typename std::enable_if<has_linear_scale<UnitTypeLhs, UnitTypeRhs>::value, int>::type = 0>
	inline UnitTypeLhs operator+(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs)
	{
		return UnitTypeLhs(lhs.toDouble() + convert<typename unit_t_traits<UnitTypeRhs>::unit_type, typename unit_t_traits<UnitTypeLhs>::unit_type>(rhs.toDouble()));
	}

	/// Addition operator for scalar unit_t types with a linear_scale. Scalar types can be implicitly converted to built-in types.
	template<typename T, typename std::enable_if<std::is_arithmetic<T>::value, int>::type = 0>
	inline dimensionless::scalar_t operator+(const dimensionless::scalar_t& lhs, T rhs)
	{
		return dimensionless::scalar_t(lhs.toDouble() + rhs);
	}

	/// Addition operator for scalar unit_t types with a linear_scale. Scalar types can be implicitly converted to built-in types.
	template<typename T, typename std::enable_if<std::is_arithmetic<T>::value, int>::type = 0>
	inline dimensionless::scalar_t operator+(T lhs, const dimensionless::scalar_t& rhs)
	{
		return dimensionless::scalar_t(lhs + rhs.toDouble());
	}

	/// Subtraction operator for unit_t types with a linear_scale.
	template<class UnitTypeLhs, class UnitTypeRhs, typename std::enable_if<has_linear_scale<UnitTypeLhs, UnitTypeRhs>::value, int>::type = 0>
	inline UnitTypeLhs operator-(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs)
	{
		return UnitTypeLhs(lhs.toDouble() - convert<typename unit_t_traits<UnitTypeRhs>::unit_type, typename unit_t_traits<UnitTypeLhs>::unit_type>(rhs.toDouble()));
	}

	/// Subtraction operator for scalar unit_t types with a linear_scale. Scalar types can be implicitly converted to built-in types.
	template<typename T, typename std::enable_if<std::is_arithmetic<T>::value, int>::type = 0>
	inline dimensionless::scalar_t operator-(const dimensionless::scalar_t& lhs, T rhs)
	{
		return dimensionless::scalar_t(lhs.toDouble() - rhs);
	}

	/// Subtraction operator for scalar unit_t types with a linear_scale. Scalar types can be implicitly converted to built-in types.
	template<typename T, typename std::enable_if<std::is_arithmetic<T>::value, int>::type = 0>
	inline dimensionless::scalar_t operator-(T lhs, const dimensionless::scalar_t& rhs)
	{
		return dimensionless::scalar_t(lhs - rhs.toDouble());
	}

	/// Multiplication type for convertible unit_t types with a linear scale. @returns the multiplied value, with the same type as left-hand side unit.
	template<class UnitTypeLhs, class UnitTypeRhs,
		typename std::enable_if<is_convertible_unit_t<UnitTypeLhs, UnitTypeRhs>::value && has_linear_scale<UnitTypeLhs, UnitTypeRhs>::value, int>::type = 0>
		inline auto operator*(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) -> unit_t<compound_unit<squared<typename unit_t_traits<UnitTypeLhs>::unit_type>>>
	{
		return  unit_t<compound_unit<squared<typename unit_t_traits<UnitTypeLhs>::unit_type>>>
			(lhs.toDouble() * convert<typename unit_t_traits<UnitTypeRhs>::unit_type, typename unit_t_traits<UnitTypeLhs>::unit_type>(rhs.toDouble()));
	}

	/// Multiplication type for convertible unit_t types with a linear scale. @returns the multiplied value, whose type is a compound unit of the left and right hand side values.
	template<class UnitTypeLhs, class UnitTypeRhs,
		typename std::enable_if<!is_convertible_unit_t<UnitTypeLhs, UnitTypeRhs>::value && has_linear_scale<UnitTypeLhs, UnitTypeRhs>::value, int>::type = 0>
		inline auto operator*(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) -> unit_t<compound_unit<typename unit_t_traits<UnitTypeLhs>::unit_type, typename unit_t_traits<UnitTypeRhs>::unit_type>>
	{
		return unit_t<compound_unit<typename unit_t_traits<UnitTypeLhs>::unit_type, typename unit_t_traits<UnitTypeRhs>::unit_type>>
			(lhs.toDouble() * rhs.toDouble());
	}

	/// Multiplication by a scalar for unit_t types with a linear scale.
	template<class UnitTypeLhs, typename T,
		typename std::enable_if<std::is_arithmetic<T>::value && has_linear_scale<UnitTypeLhs>::value, int>::type = 0>
		inline UnitTypeLhs operator*(const UnitTypeLhs& lhs, T rhs)
	{
		return UnitTypeLhs(lhs.toDouble() * rhs);
	}

	/// Multiplication by a scalar for unit_t types with a linear scale.
	template<class UnitTypeRhs, typename T,
		typename std::enable_if<std::is_arithmetic<T>::value && has_linear_scale<UnitTypeRhs>::value, int>::type = 0>
		inline UnitTypeRhs operator*(T lhs, const UnitTypeRhs& rhs)
	{
		return UnitTypeRhs(lhs * rhs.toDouble());
	}

	/// Division for convertible unit_t types with a linear scale. @returns the lhs divided by rhs value, whose type is a scalar
	template<class UnitTypeLhs, class UnitTypeRhs,
		typename std::enable_if<is_convertible_unit_t<UnitTypeLhs, UnitTypeRhs>::value && has_linear_scale<UnitTypeLhs, UnitTypeRhs>::value, int>::type = 0>
		inline dimensionless::scalar_t operator/(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs)
	{
		return dimensionless::scalar_t(lhs.toDouble() / convert<typename unit_t_traits<UnitTypeRhs>::unit_type, typename unit_t_traits<UnitTypeLhs>::unit_type>(rhs.toDouble()));
	}

	/// Division for non-convertible unit_t types with a linear scale. @returns the lhs divided by the rhs, with a compound unit type of lhs/rhs 
	template<class UnitTypeLhs, class UnitTypeRhs,
		typename std::enable_if<!is_convertible_unit_t<UnitTypeLhs, UnitTypeRhs>::value && has_linear_scale<UnitTypeLhs, UnitTypeRhs>::value, int>::type = 0>
		inline auto operator/(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) ->  unit_t<compound_unit<typename unit_t_traits<UnitTypeLhs>::unit_type, inverse<typename unit_t_traits<UnitTypeRhs>::unit_type>>>
	{
		return unit_t<compound_unit<typename unit_t_traits<UnitTypeLhs>::unit_type, inverse<typename unit_t_traits<UnitTypeRhs>::unit_type>>>
			(lhs.toDouble() / rhs.toDouble());
	}

	/// Division by a scalar for unit_t types with a linear scale
	template<class UnitTypeLhs, typename T,
		typename std::enable_if<std::is_arithmetic<T>::value && has_linear_scale<UnitTypeLhs>::value, int>::type = 0>
		inline UnitTypeLhs operator/(const UnitTypeLhs& lhs, T rhs)
	{
		return UnitTypeLhs(lhs.toDouble() / rhs);
	}

	/// Division of a scalar  by a unit_t type with a linear scale
	template<class UnitTypeRhs, typename T,
		typename std::enable_if<std::is_arithmetic<T>::value && has_linear_scale<UnitTypeRhs>::value, int>::type = 0>
		inline auto operator/(T lhs, const UnitTypeRhs& rhs) -> unit_t<inverse<typename unit_t_traits<UnitTypeRhs>::unit_type>>
	{
		return unit_t<inverse<typename unit_t_traits<UnitTypeRhs>::unit_type>>
			(lhs / rhs.toDouble());
	}

	//----------------------------------
	//	SCALAR COMPARISONS
	//----------------------------------

	bool operator==(double lhs, const dimensionless::scalar_t& rhs)
	{
		return lhs == rhs.toDouble();
	}

	bool operator==(const dimensionless::scalar_t& lhs, double rhs)
	{
		return lhs.toDouble() == rhs;
	}

	bool operator!=(double lhs, const dimensionless::scalar_t& rhs)
	{
		return lhs != rhs.toDouble();
	}

	bool operator!=(const dimensionless::scalar_t& lhs, double rhs)
	{
		return lhs.toDouble() != rhs;
	}

	bool operator>=(double lhs, const dimensionless::scalar_t& rhs)
	{
		return lhs >= rhs.toDouble();
	}

	bool operator>=(const dimensionless::scalar_t& lhs, double rhs)
	{
		return lhs.toDouble() >= rhs;
	}

	bool operator>(double lhs, const dimensionless::scalar_t& rhs)
	{
		return lhs > rhs.toDouble();
	}

	bool operator>(const dimensionless::scalar_t& lhs, double rhs)
	{
		return lhs.toDouble() > rhs;
	}

	bool operator<=(double lhs, const dimensionless::scalar_t& rhs)
	{
		return lhs <= rhs.toDouble();
	}

	bool operator<=(const dimensionless::scalar_t& lhs, double rhs)
	{
		return lhs.toDouble() <= rhs;
	}

	bool operator<(double lhs, const dimensionless::scalar_t& rhs)
	{
		return lhs < rhs.toDouble();
	}

	bool operator<(const dimensionless::scalar_t& lhs, double rhs)
	{
		return lhs.toDouble() < rhs;
	}

	/** @cond */	// DOXYGEN IGNORE

	//----------------------------------
	//	POW
	//----------------------------------

	namespace detail
	{
		/// recursive exponential implementation
		template <int N, class U> 
		struct power_of_ratio
		{
			typedef std::ratio_multiply<U, typename power_of_ratio<N - 1, U>::type> type;
		};

		/// End recursion
		template <class U>
		struct power_of_ratio<1, U>
		{
			typedef U type;
		};

		/// recursive exponential implementation
		template <int N, class U> struct power_of_unit
		{
			typedef typename units::detail::unit_multiply<U, typename power_of_unit<N - 1, U>::type> type;
		};

		/// End recursion
		template <class U> struct power_of_unit<1, U>
		{
			typedef U type;
		};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	namespace math
	{
		/**
		 * @brief		computes the value of <i>value</i> raised to the <i>power</i>
		 * @details		Only implemented for linear_scale units. <i>Power</i> must be known at compile time, so the resulting unit type can be deduced.
		 * @tparam		power exponential power to raise <i>value</i> by.
		 * @param[in]	value `unit_t` derived type to raise to the given <i>power</i>
		 * @returns		new unit_t, raised to the given exponent
		 */
		template<int power, class UnitType, class = typename std::enable_if<units::has_linear_scale<UnitType>::value, int>>
		inline auto pow(const UnitType& value) -> unit_t<typename detail::power_of_unit<power, typename unit_t_traits<UnitType>::unit_type>::type, typename unit_t_traits<UnitType>::underlying_type, linear_scale>
		{
			return unit_t<typename detail::power_of_unit<power, typename unit_t_traits<UnitType>::unit_type>::type, typename unit_t_traits<UnitType>::underlying_type, linear_scale>
				(std::pow(value(), power));
		}
	}

	//------------------------------
	//	DECIBEL SCALE
	//------------------------------

	/**
	* @brief		unit_t scale for representing decibel values.
	* @details		internally stores linearized values. `operator()` returns the value in dB.
	* @tparam		T	underlying storage type
	* @sa			unit_t
	*/
	template<typename T>
	struct decibel_scale
	{
		inline decibel_scale() : m_value(1) {}
		inline decibel_scale(T value) { m_value = std::pow(10, value / 10); }
		inline T operator()() const { return 10 * std::log10(m_value); }

		T m_value;	///< linearized value	
	};

	//------------------------------
	//	SCALAR (DECIBEL) UNITS
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers for units that have no dimension (scalar units)
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace dimensionless
	{
		using dB_t = unit_t<scalar, double, decibel_scale>;
		using dBi_t = dB_t;
	}

	//------------------------------
	//	DECIBEL ARITHMETIC
	//------------------------------

	/// Addition for convertible unit_t types with a decibel_scale
	template<class UnitTypeLhs, class UnitTypeRhs,
		typename std::enable_if<has_decibel_scale<UnitTypeLhs, UnitTypeRhs>::value, int>::type = 0>
		inline auto operator+(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) -> unit_t<compound_unit<squared<typename unit_t_traits<UnitTypeLhs>::unit_type>>, typename unit_t_traits<UnitTypeLhs>::underlying_type, decibel_scale>
	{
		using LhsUnits = typename unit_t_traits<UnitTypeLhs>::unit_type;
		using RhsUnits = typename unit_t_traits<UnitTypeRhs>::unit_type;
		using underlying_type = typename unit_t_traits<UnitTypeLhs>::underlying_type;

		unit_t<compound_unit<squared<LhsUnits>>, underlying_type, decibel_scale> ret;
		reinterpret_cast<decibel_scale<underlying_type>&>(ret).m_value = lhs.toLinearizedDouble() * convert<RhsUnits, LhsUnits>(rhs.toLinearizedDouble());
		return ret;
	}

	/// Addition between unit_t types with a decibel_scale and dimensionless dB units
	template<class UnitTypeLhs, typename std::enable_if<has_decibel_scale<UnitTypeLhs>::value && !is_scalar_unit<UnitTypeLhs>::value, int>::type = 0>
	inline UnitTypeLhs operator+(const UnitTypeLhs& lhs, const dimensionless::dB_t& rhs)
	{
		using underlying_type = typename unit_t_traits<UnitTypeLhs>::underlying_type;

		UnitTypeLhs ret;
		reinterpret_cast<decibel_scale<underlying_type>&>(ret).m_value = lhs.toLinearizedDouble() * rhs.toLinearizedDouble();
		return ret;
	}

	/// Addition between unit_t types with a decibel_scale and dimensionless dB units
	template<class UnitTypeRhs, typename std::enable_if<has_decibel_scale<UnitTypeRhs>::value && !is_scalar_unit<UnitTypeRhs>::value, int>::type = 0>
	inline UnitTypeRhs operator+(const dimensionless::dB_t& lhs, const UnitTypeRhs& rhs)
	{
		using underlying_type = typename unit_t_traits<UnitTypeRhs>::underlying_type;

		UnitTypeRhs ret;
		reinterpret_cast<decibel_scale<underlying_type>&>(ret).m_value = lhs.toLinearizedDouble() * rhs.toLinearizedDouble();
		return ret;
	}

	/// Subtraction for convertible unit_t types with a decibel_scale
	template<class UnitTypeLhs, class UnitTypeRhs, typename std::enable_if<has_decibel_scale<UnitTypeLhs, UnitTypeRhs>::value, int>::type = 0>
	inline auto operator-(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) -> unit_t<compound_unit<typename unit_t_traits<UnitTypeLhs>::unit_type, inverse<typename unit_t_traits<UnitTypeRhs>::unit_type>>, typename unit_t_traits<UnitTypeLhs>::underlying_type, decibel_scale>
	{
		using LhsUnits = typename unit_t_traits<UnitTypeLhs>::unit_type;
		using RhsUnits = typename unit_t_traits<UnitTypeRhs>::unit_type;
		using underlying_type = typename unit_t_traits<UnitTypeLhs>::underlying_type;

		unit_t<compound_unit<LhsUnits, inverse<RhsUnits>>, underlying_type, decibel_scale> ret;
		reinterpret_cast<decibel_scale<underlying_type>&>(ret).m_value = lhs.toLinearizedDouble() / convert<RhsUnits, LhsUnits>(rhs.toLinearizedDouble());
		return ret;
	}

	/// Subtraction between unit_t types with a decibel_scale and dimensionless dB units
	template<class UnitTypeLhs, typename std::enable_if<has_decibel_scale<UnitTypeLhs>::value && !is_scalar_unit<UnitTypeLhs>::value, int>::type = 0>
	inline UnitTypeLhs operator-(const UnitTypeLhs& lhs, const dimensionless::dB_t& rhs)
	{
		using underlying_type = typename unit_t_traits<UnitTypeLhs>::underlying_type;

		UnitTypeLhs ret;
		reinterpret_cast<decibel_scale<underlying_type>&>(ret).m_value = lhs.toLinearizedDouble() / rhs.toLinearizedDouble();
		return ret;
	}

	/// Subtraction between unit_t types with a decibel_scale and dimensionless dB units
	template<class UnitTypeRhs, typename std::enable_if<has_decibel_scale<UnitTypeRhs>::value && !is_scalar_unit<UnitTypeRhs>::value, int>::type = 0>
	inline auto operator-(const dimensionless::dB_t& lhs, const UnitTypeRhs& rhs) -> unit_t<inverse<typename unit_t_traits<UnitTypeRhs>::unit_type>, typename unit_t_traits<UnitTypeRhs>::underlying_type, decibel_scale>
	{
		using RhsUnits = typename unit_t_traits<UnitTypeRhs>::unit_type;
		using underlying_type = typename unit_t_traits<RhsUnits>::underlying_type;

		unit_t<inverse<RhsUnits>, underlying_type, decibel_scale> ret;
		reinterpret_cast<decibel_scale<underlying_type>&>(ret).m_value = lhs.toLinearizedDouble() / rhs.linearizedValue();
		return ret;
	}

	//----------------------------------
	//	UNIT RATIO CLASS
	//----------------------------------

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		template<class Units>
		struct _unit_value_t {};
	}

	/**
	 * @brief		unit_value_t_traits specialization for things which are not unit_t
	 * @details
	 */
	template<typename T, typename = void>
	struct unit_value_t_traits
	{
		typedef void unit_type;
		typedef void ratio;
	};
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait for accessing the publically defined types of `units::unit_value_t_traits`
	 * @details
	 */
	template<typename T>
	struct unit_value_t_traits <T, typename void_t<
		typename T::unit_type,
		typename T::ratio>::type>
	{
		typedef typename T::unit_type unit_type;
		typedef typename T::ratio ratio;
	};

	//------------------------------------------------------------------------------
	//	COMPILE-TIME UNIT VALUES AND ARITHMETIC
	//------------------------------------------------------------------------------

	/**
	 * @ingroup		UnitContainers
	 * @brief		Stores a rational unit value as a compile-time constant
	 * @details		unit_value_t is useful for performing compile-time arithmetic on known 
	 *				unit quantities.
	 * @tparam		Units	units represented by the `unit_value_t`
	 * @tparam		Num		numerator of the represented value.
	 * @tparam		Denom	denominator of the represented value.
	 * @note		This is intentionally identical in concept to a `std::ratio`.
	 *
	 */
	template<typename Units, std::uintmax_t Num, std::uintmax_t Denom = 1>
	struct unit_value_t : detail::_unit_value_t<Units>
	{
		typedef Units unit_type;
		typedef std::ratio<Num, Denom> ratio;

		static_assert(is_unit<Units>::value, "Template parameter `Units` must be a unit type.");
		static const unit_t<Units> value() { return unit_t<Units>((double)ratio::num / ratio::den); }
	};

	/**
	 * @ingroup		TypeTraits
	 * @brief		tests whether a type is a unit_value_t representing the given unit type.
	 * @details		e.g. is_unit_value_t<meters, myType>::value` would test that `myType` is a 
	 *				`unit_value_t<meters>`.
	 * @tparam		Units	units that the `unit_value_t` is supposed to have.
	 * @tparam		T		type to test.
	 */
	template<typename T, typename Units = typename unit_value_t_traits<T>::unit_type>
	struct is_unit_value_t : std::integral_constant<bool, 
		std::is_base_of<detail::_unit_value_t<Units>, T>::value>
	{};

	/**
	 * @ingroup		TypeTraits
	 * @brief		type trait that tests whether type T is a unit_value_t with a unit type in the given category.
	 * @details		e.g. `is_unit_value_t_category<units::category::length, unit_value_t<feet>>::value` would be true
	 */
	template<typename Category, typename T>
	struct is_unit_value_t_category : std::integral_constant<bool,
		std::is_same<base_unit_of<typename unit_value_t_traits<T>::unit_type>, Category>::value>
	{
		static_assert(is_base_unit<Category>::value, "Template parameter `Category` must be a `base_unit` type.");
	};

	/** @cond */	// DOXYGEN IGNORE
	namespace detail
	{
		// base class for common arithmetic
		template<class U1, class U2>
		struct unit_value_arithmetic
		{
			static_assert(units::is_unit_value_t<U1>::value, "Template parameter `U1` must be a `unit_value_t` type.");
			static_assert(units::is_unit_value_t<U2>::value, "Template parameter `U1` must be a `unit_value_t` type.");

			using _UNIT1 = typename unit_value_t_traits<U1>::unit_type;
			using _UNIT2 = typename unit_value_t_traits<U2>::unit_type;
			using _CONV1 = typename unit_traits<_UNIT1>::conversion_ratio;
			using _CONV2 = typename unit_traits<_UNIT2>::conversion_ratio;
			using _RATIO1 = typename unit_value_t_traits<U1>::ratio;
			using _RATIO2 = typename unit_value_t_traits<U2>::ratio;
			using _RATIO2CONV = typename std::ratio_divide<std::ratio_multiply<_RATIO2, _CONV2>, _CONV1>;
			using _PI_EXP = std::ratio_subtract<typename unit_traits<_UNIT2>::pi_exponent_ratio, typename unit_traits<_UNIT1>::pi_exponent_ratio>;
		};
	}
	/** @endcond */	// END DOXYGEN IGNORE

	/**
	 * @brief		adds two unit_value_t types at compile-time
	 * @details		The resulting unit will the the `unit_type` of `U1`
	 * @tparam		U1	left-hand `unit_value_t`
	 * @patarm		U2	right-hand `unit_value_t`
	 * @note		very similar in concept to `std::ratio_add`
	 */
	template<class U1, class U2>
	struct unit_value_add : detail::unit_value_arithmetic<U1, U2>, detail::_unit_value_t<typename unit_value_t_traits<U1>::unit_type>
	{
		using Base = detail::unit_value_arithmetic<U1, U2>;
		typedef typename Base::_UNIT1 unit_type;
		using ratio = std::ratio_add<typename Base::_RATIO1, typename Base::_RATIO2CONV>;

		static_assert(units::is_convertible_unit<typename Base::_UNIT1, typename Base::_UNIT2>::value, "Unit types are not compatible.");

		// dispatch value based on pi exponent
		static const unit_t<unit_type> value()
		{
			using UsePi = typename std::conditional<Base::_PI_EXP::num != 0, std::true_type, std::false_type>::type;
			return value(UsePi());
		}

		// value if PI isn't involved
		static const unit_t<unit_type> value(std::false_type) 
		{ 
			return unit_t<unit_type>((double)ratio::num / ratio::den); 
		}

		// value if PI *is* involved
		static const unit_t<unit_type> value(std::true_type)
		{
			return unit_t<unit_type>(((double)Base::_RATIO1::num / Base::_RATIO1::den) + 
			((double)Base::_RATIO2CONV::num / Base::_RATIO2CONV::den) * std::pow(units::constants::PI, ((double)Base::_PI_EXP::num / Base::_PI_EXP::den)));
		}
	};

	/**
	 * @brief		subtracts two unit_value_t types at compile-time
	 * @details		The resulting unit will the the `unit_type` of `U1`
	 * @tparam		U1	left-hand `unit_value_t`
	 * @patarm		U2	right-hand `unit_value_t`
	 * @note		very similar in concept to `std::ratio_subtract`
	 */
	template<class U1, class U2>
	struct unit_value_subtract : detail::unit_value_arithmetic<U1, U2>, detail::_unit_value_t<typename unit_value_t_traits<U1>::unit_type>
	{
		using Base = detail::unit_value_arithmetic<U1, U2>;
		
		typedef typename Base::_UNIT1 unit_type;
		using ratio = std::ratio_subtract<typename Base::_RATIO1, typename Base::_RATIO2CONV>;

		static_assert(units::is_convertible_unit<typename Base::_UNIT1, typename Base::_UNIT2>::value, "Unit types are not compatible.");

		// dispatch value based on pi exponent
		static const unit_t<unit_type> value()
		{
			using UsePi = typename std::conditional<Base::_PI_EXP::num != 0, std::true_type, std::false_type>::type;
			return value(UsePi());
		}

		// value if PI isn't involved
		static const unit_t<unit_type> value(std::false_type)
		{
			return unit_t<unit_type>((double)ratio::num / ratio::den);
		}

		// value if PI *is* involved
		static const unit_t<unit_type> value(std::true_type)
		{
			return unit_t<unit_type>(((double)Base::_RATIO1::num / Base::_RATIO1::den) - ((double)Base::_RATIO2CONV::num / Base::_RATIO2CONV::den) 
			* std::pow(units::constants::PI, ((double)Base::_PI_EXP::num / Base::_PI_EXP::den)));
		}
	};

	/**
	 * @brief		multiplies two unit_value_t types at compile-time
	 * @details		The resulting unit will the the `unit_type` of `U1 * U2`
	 * @tparam		U1	left-hand `unit_value_t`
	 * @patarm		U2	right-hand `unit_value_t`
	 * @note		very similar in concept to `std::ratio_multiply`
	 */
	template<class U1, class U2>
	struct unit_value_multiply : detail::unit_value_arithmetic<U1, U2>, detail::_unit_value_t<compound_unit<typename unit_value_t_traits<U1>::unit_type, typename unit_value_t_traits<U2>::unit_type>>
	{
		using Base = detail::unit_value_arithmetic<U1, U2>;
		
		using unit_type = typename std::conditional<is_convertible_unit<typename Base::_UNIT1, typename Base::_UNIT2>::value, compound_unit<squared<typename Base::_UNIT1>>, compound_unit<typename Base::_UNIT1, typename Base::_UNIT2>>::type;
		using ratio = typename std::conditional<is_convertible_unit<typename Base::_UNIT1, typename Base::_UNIT2>::value, std::ratio_multiply<typename Base::_RATIO1, typename Base::_RATIO2CONV>, std::ratio_multiply<typename Base::_RATIO1, typename Base::_RATIO2>>::type;

		// dispatch value based on pi exponent
		static const unit_t<unit_type> value()
		{
			using UsePi = typename std::conditional<Base::_PI_EXP::num != 0, std::true_type, std::false_type>::type;
			return value(UsePi());
		}

		// value if PI isn't involved
		static const unit_t<unit_type> value(std::false_type)
		{
			return unit_t<unit_type>((double)ratio::num / ratio::den);
		}

		// value if PI *is* involved
		static const unit_t<unit_type> value(std::true_type)
		{
			return unit_t<unit_type>(((double)ratio::num / ratio::den) * std::pow(units::constants::PI, ((double)Base::_PI_EXP::num / Base::_PI_EXP::den)));
		}
	};

	/**
	 * @brief		divides two unit_value_t types at compile-time
	 * @details		The resulting unit will the the `unit_type` of `U1`
	 * @tparam		U1	left-hand `unit_value_t`
	 * @patarm		U2	right-hand `unit_value_t`
	 * @note		very similar in concept to `std::ratio_divide`
	 */
	template<class U1, class U2>
	struct unit_value_divide : detail::unit_value_arithmetic<U1, U2>, detail::_unit_value_t<compound_unit<typename unit_value_t_traits<U1>::unit_type, inverse<typename unit_value_t_traits<U1>::unit_type>>>
	{
		using Base = detail::unit_value_arithmetic<U1, U2>;
		
		using unit_type = typename std::conditional<is_convertible_unit<typename Base::_UNIT1, typename Base::_UNIT2>::value, dimensionless::scalar, compound_unit<typename Base::_UNIT1, inverse<typename Base::_UNIT2>>>::type;
		using ratio = typename std::conditional<is_convertible_unit<typename Base::_UNIT1, typename Base::_UNIT2>::value, std::ratio_divide<typename Base::_RATIO1, typename Base::_RATIO2CONV>, std::ratio_divide<typename Base::_RATIO1, typename Base::_RATIO2>>::type;

		// dispatch value based on pi exponent
		static const unit_t<unit_type> value()
		{
			using UsePi = typename std::conditional<Base::_PI_EXP::num != 0, std::true_type, std::false_type>::type;
			return value(UsePi());
		}

		// value if PI isn't involved
		static const unit_t<unit_type> value(std::false_type)
		{
			return unit_t<unit_type>((double)ratio::num / ratio::den);
		}

		// value if PI *is* involved
		static const unit_t<unit_type> value(std::true_type)
		{
			return unit_t<unit_type>(((double)ratio::num / ratio::den) * std::pow(units::constants::PI, ((double)Base::_PI_EXP::num / Base::_PI_EXP::den)));
		}
	};

	/**
	 * @brief		raises unit_value_to a power at compile-time
	 * @details		The resulting unit will the `unit_type` of `U1` squared
	 * @tparam		U1	`unit_value_t` to take the square root of
	 * @note		very similar in concept to `units::math::pow`
	 */
	template<class U1, int power>
	struct unit_value_power : detail::unit_value_arithmetic<U1, U1>, detail::_unit_value_t<detail::power_of_unit<2, typename unit_value_t_traits<U1>::unit_type>>
	{
		using Base = detail::unit_value_arithmetic<U1, U1>;
		
		using unit_type = typename detail::power_of_unit<power, typename Base::_UNIT1>::type;
		using ratio = typename detail::power_of_ratio<power, typename Base::_RATIO1>::type;
		using pi_exponent = std::ratio_multiply<std::ratio<power>, typename Base::_UNIT1::pi_exponent_ratio>;

		// dispatch value based on pi exponent
		static const unit_t<unit_type> value()
		{
			using UsePi = typename std::conditional<Base::_PI_EXP::num != 0, std::true_type, std::false_type>::type;
			return value(UsePi());
		}

		// value if PI isn't involved
		static const unit_t<unit_type> value(std::false_type)
		{
			return unit_t<unit_type>((double)ratio::num / ratio::den);
		}

		// value if PI *is* involved
		static const unit_t<unit_type> value(std::true_type)
		{
			return unit_t<unit_type>(((double)ratio::num / ratio::den) * std::pow(units::constants::PI, ((double)pi_exponent::num / pi_exponent::den)));
		}
	};

	/**
	* @brief		calculates square root of unit_value_t at compile-time
	* @details		The resulting unit will the square root `unit_type` of `U1`	 
	* @tparam		U1	`unit_value_t` to take the square root of
	* @note			very similar in concept to `units::ratio_sqrt`
	*/
	template<class U1, std::intmax_t Eps = 10000000000>
	struct unit_value_sqrt : detail::unit_value_arithmetic<U1, U1>, detail::_unit_value_t<square_root<typename unit_value_t_traits<U1>::unit_type, Eps>>
	{
		using Base = detail::unit_value_arithmetic<U1, U1>;

		using unit_type = square_root<typename Base::_UNIT1, Eps>;
		using ratio = ratio_sqrt<typename Base::_RATIO1, Eps>;
		using pi_exponent = ratio_sqrt<typename Base::_UNIT1::pi_exponent_ratio, Eps>;

		// dispatch value based on pi exponent
		static const unit_t<unit_type> value()
		{
			using UsePi = typename std::conditional<Base::_PI_EXP::num != 0, std::true_type, std::false_type>::type;
			return value(UsePi());
		}

		// value if PI isn't involved
		static const unit_t<unit_type> value(std::false_type)
		{
			return unit_t<unit_type>((double)ratio::num / ratio::den);
		}

		// value if PI *is* involved
		static const unit_t<unit_type> value(std::true_type)
		{
			return unit_t<unit_type>(((double)ratio::num / ratio::den) * std::pow(units::constants::PI, ((double)pi_exponent::num / pi_exponent::den)));
		}
	};

	//------------------------------
	//	LENGTH UNITS
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing length values
	 * @details		The SI unit for length is `meters`, and the corresponding `base_unit` category is
	 *				`length_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
 	namespace length
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
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
		using angstroms = unit<std::ratio<1, 10>, nanometers>;
		using cubits = unit<std::ratio<18>, inches>;
		using fathoms = unit<std::ratio<6>, feet>;
		using chains = unit<std::ratio<66>, feet>;
		using furlongs = unit<std::ratio<10>, chains>;
		using hands = unit<std::ratio<4>, inches>;
		using leagues = unit<std::ratio<3>, miles>;
		using nauticalLeagues = unit<std::ratio<3>, nauticalMiles>;
		using yards = unit<std::ratio<3>, feet>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
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
		using angstrom = angstroms;
		using cubit = cubits;
		using fathom = fathoms;
		using chain = chains;
		using furlong = furlongs;
		using hand = hands;
		using league = leagues;
		using nauticalLeague = nauticalLeagues;
		using yard = yards;
		/** @} */

		/**
		* @name Units (abbreviated names)
		* @{
		*/
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
		using ftm = fathoms;
		using ch = chains;
		using fur = furlongs;
		using lea = leagues;
		using nl = nauticalLeagues;
		using yd = yards;
		/** @} */

		/**
		 * @anchor		lengthContainers
		 * @name		Unit Containers
		 * @{
		 */
		using meter_t = unit_t<meter>;
		using nanometer_t = unit_t<nanometer>;
		using micrometer_t = unit_t<micrometer>;
		using millimeter_t = unit_t<millimeter>;
		using centimeter_t = unit_t<centimeter>;
		using kilometer_t = unit_t<kilometer>;
		using foot_t = unit_t<foot>;
		using inch_t = unit_t<inch>;
		using mile_t = unit_t<mile>;
		using nauticalMile_t = unit_t<nauticalMile>;
		using astronicalUnit_t = unit_t<astronicalUnit>;
		using lightyear_t = unit_t<lightyear>;
		using parsec_t = unit_t<parsec>;
		using angstrom_t = unit_t<angstrom>;
		using cubit_t = unit_t<cubit>;
		using fathom_t = unit_t<fathom>;
		using chain_t = unit_t<chain>;
		using furlong_t = unit_t<furlong>;
		using hand_t = unit_t<hand>;
		using league_t = unit_t<league>;
		using nauticalLeague_t = unit_t<nauticalLeague>;
		using yard_t = unit_t<yard>;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_length_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_length_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::length_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_length_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::length_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of length
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_length_unit<T>::value` to test
	 *				the unit represents a length quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_length_unit : std::integral_constant<bool, all_true<detail::is_length_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	MASS UNITS
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing mass values
	 * @details		The SI unit for mass is `kilograms`, and the corresponding `base_unit` category is
	 *				`mass_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace mass
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
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
		using slugs = unit<std::ratio<145939029, 10000000>, kilograms>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
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
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
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
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor massContainers
		 * @{
		 */
		using gram_t = unit_t<gram>;
		using microgram_t = unit_t<microgram>;
		using milligram_t = unit_t<milligram>;
		using kilogram_t = unit_t<kilogram>;
		using metric_ton_t = unit_t<metric_ton>;
		using pound_t = unit_t<pound>;
		using imperial_ton_t = unit_t<imperial_ton>;
		using us_ton_t = unit_t<us_ton>;
		using stone_t = unit_t<stone>;
		using ounce_t = unit_t<ounce>;
		using carat_t = unit_t<carat>;
		using slug_t = unit_t<slug>;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_mass_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_mass_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::mass_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_mass_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::mass_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of mass
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_mass_unit<T>::value` to test
	 *				the unit represents a mass quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_mass_unit : std::integral_constant<bool, all_true<detail::is_mass_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	TIME UNITS
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing time values
	 * @details		The SI unit for time is `seconds`, and the corresponding `base_unit` category is
	 *				`time_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace time
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using seconds = unit<std::ratio<1>, category::time_unit>;
		using nanoseconds = nano<seconds>;
		using microseconds = micro<seconds>;
		using millseconds = milli<seconds>;
		using minutes = unit<std::ratio<60>, seconds>;
		using hours = unit<std::ratio<60>, minutes>;
		using days = unit<std::ratio<24>, hours>;
		using weeks = unit<std::ratio<7>, days>;
		using years = unit<std::ratio<365>, days>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using second = seconds;
		using nanosecond = nanoseconds;
		using microsecond = microseconds;
		using millsecond = millseconds;
		using minute = minutes;
		using hour = hours;
		using day = days;
		using week = weeks;
		using year = years;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using s = seconds;
		using ns = nanoseconds;
		using us = microseconds;
		using ms = millseconds;
		using m = minutes;
		using hr = hours;
		using d = days;
		using wk = weeks;
		using yr = years;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor timeContainers
		 * @{
		 */
		using second_t = unit_t<second>;
		using nanosecond_t = unit_t<nanosecond>;
		using microsecond_t = unit_t<microsecond>;
		using millsecond_t = unit_t<millsecond>;
		using minute_t = unit_t<minute>;
		using hour_t = unit_t<hour>;
		using day_t = unit_t<day>;
		using week_t = unit_t<week>;
		using year_t = unit_t<year>;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_time_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_time_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::time_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_time_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::time_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of time
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_time_unit<T>::value` to test
	 *				the unit represents a time quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_time_unit : std::integral_constant<bool, all_true<detail::is_time_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	ANGLE UNITS
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing angle values
	 * @details		The SI unit for angle is `radians`, and the corresponding `base_unit` category is
	 *				`angle_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace angle
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using radians = unit<std::ratio<1>, category::angle_unit>;
		using milliradians = milli<radians>;
		using degrees = unit<std::ratio<1, 180>, radians, std::ratio<1>>;
		using arcminutes = unit<std::ratio<1, 60>, degrees>;
		using arcseconds = unit<std::ratio<1, 60>, arcminutes>;
		using milliarcseconds = milli<arcseconds>;
		using turns = unit<std::ratio<2>, radians, std::ratio<1>>;
		using mils = unit<std::ratio<1, 6400>, radians>;	// 1/6400 of a circle
		using gradians = unit<std::ratio<1, 400>, turns>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using radian = radians;
		using milliradian = milliradians;
		using degree = degrees;
		using arcminute = arcminutes;
		using arcsecond = arcseconds;
		using milliarcsecond = milliarcseconds;
		using turn = turns;
		using mil = mils;
		using gradian = gradians;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using rad = radians;
		using mrad = milliradians;
		using deg = degrees;
		using min = arcminutes;
		using sec = arcseconds;
		using mas = milliarcseconds;
		using tr = turn;
		using gon = gradians;
		using grad = gradians;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor angleContainers
		 * @{
		 */
		using radian_t = unit_t<radian>;
		using milliradian_t = unit_t<milliradian>;
		using degree_t = unit_t<degree>;
		using minute_t = unit_t<arcminute>;
		using second_t = unit_t<arcsecond>;
		using turn_t = unit_t<turn>;
		using mil_t = unit_t<mil>;
		using gradian_t = unit_t<gradian>;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_angle_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_angle_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::angle_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_angle_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::angle_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of angle
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_angle_unit<T>::value` to test
	 *				the unit represents a angle quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_angle_unit : std::integral_constant<bool, all_true<detail::is_angle_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF CURRENT
	//------------------------------
	/**
	 * @brief		namespace for unit types and containers representing current values
	 * @details		The SI unit for current is `amperes`, and the corresponding `base_unit` category is
	 *				`current_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace current
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using amperes = unit<std::ratio<1>, category::current_unit>;
		using milliamps = milli<amperes>;
		using microamps = micro<amperes>;
		using nanoamps = nano<amperes>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using ampere = amperes;
		using amps = amperes;
		using amp = amperes;
		using milliamp = milliamps;
		using microamp = microamps;
		using nanoamp = nanoamps;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using A = amperes;
		using mA = milliamps;
		using uA = microamps;
		using nA = nanoamps;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor currentContainers
		 * @{
		 */
		using ampere_t = unit_t<ampere>;
		using amps_t = unit_t<amps>;
		using amp_t = unit_t<amp>;
		using milliamp_t = unit_t<milliamp>;
		using microamp_t = unit_t<microamp>;
		using nanoamp_t = unit_t<nanoamp>;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_current_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_current_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::current_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_current_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::current_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of current
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_current_unit<T>::value` to test
	 *				the unit represents a current quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_current_unit : std::integral_constant<bool, all_true<detail::is_current_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF TEMPERATURE
	//------------------------------

	// NOTE: temperature units have special conversion overloads, since they
	// require translations and aren't a reversible transform.

	/**
	 * @brief		namespace for unit types and containers representing temperature values
	 * @details		The SI unit for temperature is `kelvin`, and the corresponding `base_unit` category is
	 *				`temperature_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace temperature
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using kelvin = unit<std::ratio<1>, category::temperature_unit>;
		using celsius = unit<std::ratio<1>, kelvin, std::ratio<0>, std::ratio<27315, 100>>;
		using fahrenheit = unit<std::ratio<5, 9>, celsius, std::ratio<0>, std::ratio<-160, 9>>;
		using reaumur = unit<std::ratio<10, 8>, celsius>;
		using rankine = unit<std::ratio<5, 9>, kelvin>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using centigrade = celsius;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using K = kelvin;
		using F = fahrenheit;
		using C = celsius;
		using Ra = rankine;
		using Re = reaumur;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor temperatureContainers
		 * @{
		 */
		using kelvin_t = unit_t<kelvin>;
		using celsius_t = unit_t<celsius>;
		using fahrenheit_t = unit_t<fahrenheit>;
		using reaumur_t = unit_t<reaumur>;
		using rankine_t = unit_t<rankine>;
		using centigrade_t = unit_t<centigrade>;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_temperature_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_temperature_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::temperature_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_temperature_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::temperature_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of temperature
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_temperature_unit<T>::value` to test
	 *				the unit represents a temperature quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_temperature_unit : std::integral_constant<bool, all_true<detail::is_temperature_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF AMOUNT OF SUBSTANCE
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing substance values
	 * @details		The SI unit for substance is `moles`, and the corresponding `base_unit` category is
	 *				`substance_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace substance
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using moles = unit<std::ratio<1>, category::substance_unit>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using mole = moles;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using mol = mole;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor substanceContainers
		 * @{
		 */
		using mole_t = unit_t<mole>;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_substance_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_substance_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::substance_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_substance_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::substance_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of substance
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_substance_unit<T>::value` to test
	 *				the unit represents a substance quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_substance_unit : std::integral_constant<bool, all_true<detail::is_substance_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF LUMINOUS INTENSITY
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing luminous_intensity values
	 * @details		The SI unit for luminous_intensity is `candelas`, and the corresponding `base_unit` category is
	 *				`luminous_intensity_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace luminous_intensity
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using candelas = unit<std::ratio<1>, category::luminous_intensity_unit>;
		using millicandelas = milli<candelas>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using candela = candelas;
		using millicandela = millicandelas;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using cd = candela;
		using mcd = millicandela;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor luminousIntensityContainers
		 * @{
		 */
		using candela_t = unit_t<candela>;
		using millicandela_t = unit_t<millicandela>;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_luminous_intensity_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_luminous_intensity_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::luminous_intensity_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_luminous_intensity_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::luminous_intensity_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of luminous_intensity
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_luminous_intensity_unit<T>::value` to test
	 *				the unit represents a luminous_intensity quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_luminous_intensity_unit : std::integral_constant<bool, all_true<detail::is_luminous_intensity_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF SOLID ANGLE
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing solid_angle values
	 * @details		The SI unit for solid_angle is `steradians`, and the corresponding `base_unit` category is
	 *				`solid_angle_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace solid_angle
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using steradians = unit<std::ratio<1>, category::solid_angle_unit>;
		using degrees_squared = squared<angle::degrees>;
		using spats = unit<std::ratio<4>, steradians, std::ratio<1>>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using steradian = steradians;
		using degree_squared = degrees_squared;
		using spat = spats;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using sr = steradians;
		using sq_deg = degrees_squared;
		using sp = spat;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor solidAngleContainers
		 * @{
		 */
		using steradian_t = unit_t<steradian>;
		using degree_squared_t = unit_t<degree_squared>;
		using spat_t = unit_t<spat>;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_solid_angle_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_solid_angle_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::solid_angle_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_solid_angle_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::solid_angle_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of solid_angle
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_solid_angle_unit<T>::value` to test
	 *				the unit represents a solid_angle quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_solid_angle_unit : std::integral_constant<bool, all_true<detail::is_solid_angle_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	FREQUENCY UNITS
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing frequency values
	 * @details		The SI unit for frequency is `hertz`, and the corresponding `base_unit` category is
	 *				`frequency_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace frequency
	{
		/**
		 * @name Units (full names)
		 * @{
		 */
		using hertz = unit<std::ratio<1>, category::frequency_unit>;
		using kilohertz = kilo<hertz>;
		using megahertz = mega<hertz>;
		using gigahertz = giga<hertz>;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using Hz = hertz;
		using kHz = kilohertz;
		using MHz = megahertz;
		using GHz = gigahertz;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor frequencyContainers
		 * @{
		 */
		using hertz_t = unit_t<hertz>;
		using kilohertz_t = unit_t<kilohertz>;
		using megahertz_t = unit_t<megahertz>;
		using gigahertz_t = unit_t<gigahertz>;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_frequency_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_frequency_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::frequency_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_frequency_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::frequency_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of frequency
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_frequency_unit<T>::value` to test
	 *				the unit represents a frequency quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_frequency_unit : std::integral_constant<bool, all_true<detail::is_frequency_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	VELOCITY UNITS
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing velocity values
	 * @details		The SI unit for velocity is `meters_per_second`, and the corresponding `base_unit` category is
	 *				`velocity_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace velocity
	{
		/**
		 * @name  Units (full names plural)
		 * @{
		 */
		using meters_per_second = unit<std::ratio<1>, category::velocity_unit>;
		using feet_per_second = compound_unit<length::feet, inverse<time::seconds>>;
		using miles_per_hour = compound_unit<length::miles, inverse<time::hour>>;
		using kilometers_per_hour = compound_unit<length::kilometers, inverse<time::hour>>;
		using knots = compound_unit<length::nauticalMiles, inverse<time::hour>>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using knot = knots;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using mps = meters_per_second;
		using mph = miles_per_hour;
		using fps = feet_per_second;
		using kmph = kilometers_per_hour;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor velocityContainers
		 * @{
		 */
		using meters_per_second_t = unit_t<meters_per_second>;
		using feet_per_second_t = unit_t<feet_per_second>;
		using miles_per_hour_t = unit_t<miles_per_hour>;
		using kilometers_per_hour_t = unit_t<kilometers_per_hour>;
		using knot_t = unit_t<knot>;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_velocity_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_velocity_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::velocity_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_velocity_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::velocity_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of velocity
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_velocity_unit<T>::value` to test
	 *				the unit represents a velocity quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_velocity_unit : std::integral_constant<bool, all_true<detail::is_velocity_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	ANGULAR VELOCITY UNITS
	//------------------------------

	/**
	* @brief		namespace for unit types and containers representing angular velocity values
	* @details		The SI unit for angular velocity is `radians_per_second`, and the corresponding `base_unit` category is
	*				`angular_velocity_unit`.
	* @sa			See unit_t for more information on unit type containers.
	*/
	namespace angular_velocity
	{
		/**
		* @name  Units (full names plural)
		* @{
		*/
		using radians_per_second = unit<std::ratio<1>, category::angular_velocity_unit>;
		using degrees_per_second = compound_unit<angle::degrees, inverse<time::seconds>>;
		using revolutions_per_minute = unit<std::ratio<2, 60>, radians_per_second, std::ratio<1>>;
		using milliarcseconds_per_year = compound_unit<angle::milliarcseconds, inverse<time::year>>;
		/** @} */

		/**
		* @name Units (full names singular)
		* @{
		*/
		using radian_per_second = radians_per_second;
		using degree_per_second = degrees_per_second;
		using revolution_per_minute = revolutions_per_minute;
		using milliarcsecond_per_year = milliarcseconds_per_year;
		/** @} */

		/**
		* @name Units (abbreviated names)
		* @{
		*/
		using rpm = revolutions_per_minute;
		/** @} */

		/**
		* @name Unit Containers
		* @anchor angular_velocityContainers
		* @{
		*/
		using radians_per_second_t = unit_t<radians_per_second>;
		using degrees_per_second_t = unit_t<degrees_per_second>;
		using revolutions_per_minute_t = unit_t<revolutions_per_minute>;
		using milliarcseconds_per_year_t = unit_t<milliarcseconds_per_year>;
		/** @} */
	}

	/**
	* @ingroup		TypeTraits
	* @brief		Trait which tests whether a type represents a unit of angular_velocity
	* @details		Inherits from `std::true_type` or `std::false_type`. Use `is_angular_velocity_unit<T>::value` to test
	*				the unit represents a angular_velocity quantity.
	* @tparam		T	one or more types to test
	*/
	template<typename T> struct is_angular_velocity_unit : std::false_type {};
	template<typename C, typename U, typename P, typename T>
	struct is_angular_velocity_unit<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::angular_velocity_unit>::type {};
	template<typename U, typename S, template<typename> class N>
	struct is_angular_velocity_unit<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::angular_velocity_unit>::type {};

	//------------------------------
	//	UNITS OF ACCELERATION
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing acceleration values
	 * @details		The SI unit for acceleration is `meters_per_second_squared`, and the corresponding `base_unit` category is
	 *				`acceleration_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace acceleration
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using meters_per_second_squared = unit<std::ratio<1>, category::acceleration_unit>;
		using feet_per_second_squared = compound_unit<length::feet, inverse<squared<time::seconds>>>;
		using standard_gravity = unit<std::ratio<980665, 100000>, meters_per_second_squared>;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor accelerationContainers
		 * @{
		 */
		using meters_per_second_squared_t = unit_t<meters_per_second_squared>;
		using feet_per_second_squared_t = unit_t<feet_per_second_squared>;
		using standard_gravity_t = unit_t<standard_gravity>;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_acceleration_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_acceleration_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::acceleration_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_acceleration_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::acceleration_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of acceleration
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_acceleration_unit<T>::value` to test
	 *				the unit represents a acceleration quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_acceleration_unit : std::integral_constant<bool, all_true<detail::is_acceleration_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF FORCE
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing force values
	 * @details		The SI unit for force is `newtons`, and the corresponding `base_unit` category is
	 *				`force_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace force
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using newtons = unit<std::ratio<1>, category::force_unit>;
		using pounds = compound_unit<mass::slug, length::foot, inverse<squared<time::seconds>>>;
		using dynes = unit<std::ratio<1, 100000>, newtons>;
		using kiloponds = compound_unit<acceleration::standard_gravity, mass::kilograms>;
		using poundals = compound_unit<mass::pound, length::foot, inverse<squared<time::seconds>>>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using newton = newtons;
		using pound = pounds;
		using dyne = dynes;
		using kilopond = kiloponds;
		using poundal = poundals;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using N = newtons;
		using lbf = pounds;
		using dyn = dynes;
		using kp = kiloponds;
		using pdl = poundals;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor forceContainers
		 * @{
		 */
		using newton_t = unit_t<newton>;
		using pound_t = unit_t<pound>;
		using dyne_t = unit_t<dyne>;
		using kilopond_t = unit_t<kilopond>;
		using poundal_t = unit_t<poundal>;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_force_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_force_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::force_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_force_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::force_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of force
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_force_unit<T>::value` to test
	 *				the unit represents a force quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_force_unit : std::integral_constant<bool, all_true<detail::is_force_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF PRESSURE
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing pressure values
	 * @details		The SI unit for pressure is `pascals`, and the corresponding `base_unit` category is
	 *				`pressure_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace pressure
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using pascals = unit<std::ratio<1>, category::pressure_unit>;
		using bars = unit<std::ratio<100>, kilo<pascals>>;
		using atmospheres = unit<std::ratio<101325>, pascals>;
		using pounds_per_square_inch = compound_unit<force::pounds, inverse<squared<length::inch>>>;
		using torrs = unit<std::ratio<1, 760>, atmospheres>;
		/** @} */

		/**
		 * @name  Units (full names singular)
		 * @{
		 */
		using pascal = pascals;
		using bar = bars;
		using atmosphere = atmospheres;
		using pound_per_square_inch = pounds_per_square_inch;
		using torr = torrs;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using Pa = pascals;
		using atm = atmospheres;
		using psi = pound_per_square_inch;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor pressureContainers
		 * @{
		 */
		using pascal_t = unit_t<pascal>;
		using bar_t = unit_t<bar>;
		using atmosphere_t = unit_t<atmosphere>;
		using pound_per_square_inch_t = unit_t<pound_per_square_inch>;
		using torr_t = unit_t<torr>;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_pressure_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_pressure_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::pressure_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_pressure_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::pressure_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of pressure
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_pressure_unit<T>::value` to test
	 *				the unit represents a pressure quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_pressure_unit : std::integral_constant<bool, all_true<detail::is_pressure_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF CHARGE
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing charge values
	 * @details		The SI unit for charge is `coulombs`, and the corresponding `base_unit` category is
	 *				`charge_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace charge
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using coulombs = unit<std::ratio<1>, category::charge_unit>;
		using ampere_hours = compound_unit<current::ampere, time::hours>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using coulomb = coulombs;
		using ampere_hour = ampere_hours;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using C = coulombs;
		using Ah = ampere_hours;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor chargeContainers
		 * @{
		 */
		using coulomb_t = unit_t<coulomb>;
		using ampere_hour_t = unit_t<ampere_hour>;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_charge_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_charge_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::charge_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_charge_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::charge_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of charge
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_charge_unit<T>::value` to test
	 *				the unit represents a charge quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_charge_unit : std::integral_constant<bool, all_true<detail::is_charge_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF ENERGY
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing energy values
	 * @details		The SI unit for energy is `joules`, and the corresponding `base_unit` category is
	 *				`energy_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace energy
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using joules = unit<std::ratio<1>, category::energy_unit>;
		using megajoules = mega<joules>;
		using kilojoules = kilo<joules>;
		using calories = unit<std::ratio<4184, 1000>, joules>;
		using kilocalories = kilo<calories>;
		using kilowatt_hours = unit<std::ratio<36, 10>, megajoules>;
		using watt_hours = unit<std::ratio<1, 1000>, kilowatt_hours>;
		using british_thermal_units = unit<std::ratio<105505585262, 100000000>, joules>;
		using british_thermal_units_iso = unit<std::ratio<1055056, 1000>, joules>;
		using british_thermal_units_59 = unit<std::ratio<1054804, 1000>, joules>;
		using therms = unit<std::ratio<100000>, british_thermal_units_59>;
		using foot_pounds = unit<std::ratio<13558179483314004, 10000000000000000>, joules>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using joule = joules;
		using megajoule = megajoules;
		using kilojoule = kilojoules;
		using calorie = calories;
		using kilocalorie = kilocalories;
		using watt_hour = watt_hours;
		using kilowatt_hour = kilowatt_hours;
		using british_thermal_unit = british_thermal_units;
		using therm = therms;
		using foot_pound = foot_pounds;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using J = joules;
		using MJ = megajoules;
		using kJ = kilojoules;
		using cal = calories;
		using kcal = kilocalories;
		using Wh = watt_hours;
		using kWh = kilowatt_hours;
		using BTU = british_thermal_units;
		using thm = therms;
		using ftlbf = foot_pounds;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor energyContainers
		 * @{
		 */
		using joule_t = unit_t<joule>;
		using megajoule_t = unit_t<megajoule>;
		using kilojoule_t = unit_t<kilojoule>;
		using calorie_t = unit_t<calorie>;
		using kilocalorie_t = unit_t<kilocalorie>;
		using watt_hour_t = unit_t<watt_hour>;
		using kilowatt_hour_t = unit_t<kilowatt_hour>;
		using british_thermal_unit_t = unit_t<british_thermal_unit>;
		using therm_t = unit_t<therm>;
		using foot_pound_t = unit_t<foot_pound>;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_energy_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_energy_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::energy_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_energy_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::energy_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of energy
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_energy_unit<T>::value` to test
	 *				the unit represents a energy quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_energy_unit : std::integral_constant<bool, all_true<detail::is_energy_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF POWER
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing power values
	 * @details		The SI unit for power is `watts`, and the corresponding `base_unit` category is
	 *				`power_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace powerNum
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using watts = unit<std::ratio<1>, category::power_unit>;
		using nanowatts = nano<watts>;
		using microwatts = micro<watts>;
		using milliwatts = milli<watts>;
		using kilowatts = kilo<watts>;
		using megawatts = mega<watts>;
		using gigawatts = giga<watts>;
		using horsepower = unit<std::ratio<7457, 10>, watts>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using watt = watts;
		using nanowatt = nanowatts;
		using microwatt = microwatts;
		using milliwatt = milliwatts;
		using kilwatt = kilowatts;
		using megawatt = megawatts;
		using gigawatt = gigawatts;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using W = watts;
		using nW = nanowatts;
		using uW = microwatts;
		using mW = milliwatts;
		using kW = kilowatts;
		using MW = megawatts;
		using GW = gigawatts;
		using hp = horsepower;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor powerContainers
		 * @{
		 */
		using watt_t = unit_t<watt>;
		using nanowatt_t = unit_t<nanowatt>;
		using microwatt_t = unit_t<microwatt>;
		using milliwatt_t = unit_t<milliwatt>;
		using kilwatt_t = unit_t<kilwatt>;
		using megawatt_t = unit_t<megawatt>;
		using gigawatt_t = unit_t<gigawatt>;

		using dBW_t = unit_t<watt, double, decibel_scale>;
		using dBm_t = unit_t<milliwatt, double, decibel_scale>;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_power_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_power_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::power_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_power_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::power_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of power
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_power_unit<T>::value` to test
	 *				the unit represents a power quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_power_unit : std::integral_constant<bool, all_true<detail::is_power_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF VOLTAGE
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing voltage values
	 * @details		The SI unit for voltage is `volts`, and the corresponding `base_unit` category is
	 *				`voltage_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace voltage
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using volts = unit<std::ratio<1>, category::voltage_unit>;
		using picovolts = pico<volts>;
		using nanovolts = nano<volts>;
		using microvolts = micro<volts>;
		using millivolts = milli<volts>;
		using kilovolts = kilo<volts>;
		using megavolts = mega<volts>;
		using gigavolts = giga<volts>;
		using statvolts = unit<std::ratio<1000000, 299792458>, volts>;
		using abvolts = unit<std::ratio<1, 100000000>, volts>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using volt = volts;
		using picovolt = picovolts;
		using nanovolt = nanovolts;
		using microvolt = microvolts;
		using millivolt = millivolts;
		using kilovolt = kilovolts;
		using megavolt = megavolts;
		using gigavolt = gigavolts;
		using statvolt = statvolts;
		using abvolt = abvolts;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using volt_t = unit_t<volt>;
		using picovolt_t = unit_t<picovolt>;
		using nanovolt_t = unit_t<nanovolt>;
		using microvolt_t = unit_t<microvolt>;
		using millivolt_t = unit_t<millivolt>;
		using kilovolt_t = unit_t<kilovolt>;
		using megavolt_t = unit_t<megavolt>;
		using gigavolt_t = unit_t<gigavolt>;
		using statvolt_t = unit_t<statvolt>;
		using abvolt_t = unit_t<abvolt>;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor voltageContainers
		 * @{
		 */
		using V = volts;
		using pV = picovolts;
		using nV = nanovolts;
		using uV = microvolts;
		using mV = millivolts;
		using kV = kilovolts;
		using MV = megavolts;
		using GV = gigavolts;
		using statV = statvolts;
		using abV = abvolts;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_voltage_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_voltage_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::voltage_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_voltage_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::voltage_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of voltage
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_voltage_unit<T>::value` to test
	 *				the unit represents a voltage quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_voltage_unit : std::integral_constant<bool, all_true<detail::is_voltage_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF CAPACITANCE
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing capacitance values
	 * @details		The SI unit for capacitance is `farads`, and the corresponding `base_unit` category is
	 *				`capacitance_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace capacitance
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using farads = unit<std::ratio<1>, category::capacitance_unit>;
		using picofarads = pico<farads>;
		using nanofarads = nano<farads>;
		using microfarads = micro<farads>;
		using millifarads = milli<farads>;
		using kilofarads = kilo<farads>;
		using megafarads = mega<farads>;
		using gigafarads = giga<farads>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using farad = farads;
		using picofarad = picofarads;
		using nanofarad = nanofarads;
		using microfarad = microfarads;
		using millifarad = millifarads;
		using kilofarad = kilofarads;
		using megafarad = megafarads;
		using gigafarad = gigafarads;
		/** @} */

		/**
		 * @name  Units (abbreviated names)
		 * @{
		 */
		using F = farads;
		using pF = picofarads;
		using nF = nanofarads;
		using uF = microfarads;
		using mF = millifarads;
		using kF = kilofarads;
		using MF = megafarads;
		using GF = gigafarads;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor capacitanceContainers
		 * @{
		 */
		using farad_t = unit_t<farad>;
		using picofarad_t = unit_t<picofarad>;
		using nanofarad_t = unit_t<nanofarad>;
		using microfarad_t = unit_t<microfarad>;
		using millifarad_t = unit_t<millifarad>;
		using kilofarad_t = unit_t<kilofarad>;
		using megafarad_t = unit_t<megafarad>;
		using gigafarad_t = unit_t<gigafarad>;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_capacitance_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_capacitance_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::capacitance_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_capacitance_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::capacitance_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of capacitance
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_capacitance_unit<T>::value` to test
	 *				the unit represents a capacitance quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_capacitance_unit : std::integral_constant<bool, all_true<detail::is_capacitance_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF IMPEDANCE
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing impedance values
	 * @details		The SI unit for impedance is `ohms`, and the corresponding `base_unit` category is
	 *				`impedance_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace impedance
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using ohms = unit<std::ratio<1>, category::impedance_unit>;
		using picoohms = pico<ohms>;
		using nanoohms = nano<ohms>;
		using microohms = micro<ohms>;
		using milliohms = milli<ohms>;
		using kiloohms = kilo<ohms>;
		using megaohms = mega<ohms>;
		using gigaohms = giga<ohms>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using ohm = ohms;
		using picoohm = picoohms;
		using nanoohm = nanoohms;
		using microohm = microohms;
		using milliohm = milliohms;
		using kiloohm = kiloohms;
		using megaohm = megaohms;
		using gigaohm = gigaohms;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor impedanceContainers
		 * @{
		 */
		using ohm_t = unit_t<ohm>;
		using picoohm_t = unit_t<picoohm>;
		using nanoohm_t = unit_t<nanoohm>;
		using microohm_t = unit_t<microohm>;
		using milliohm_t = unit_t<milliohm>;
		using kiloohm_t = unit_t<kiloohm>;
		using megaohm_t = unit_t<megaohm>;
		using gigaohm_t = unit_t<gigaohm>;
		/** @} */

		/**
		 * @name  Units (abbreviated names)
		 * @{
		 */
		using Ohm = ohms;
		using pOhm = picoohms;
		using nOhm = nanoohms;
		using uOhm = microohms;
		using mOhm = milliohms;
		using kOhm = kiloohms;
		using MOhm = megaohms;
		using GOhm = gigaohms;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_impedance_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_impedance_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::impedance_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_impedance_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::impedance_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of impedance
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_impedance_unit<T>::value` to test
	 *				the unit represents a impedance quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_impedance_unit : std::integral_constant<bool, all_true<detail::is_impedance_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF CONDUCTANCE
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing conductance values
	 * @details		The SI unit for conductance is `siemens`, and the corresponding `base_unit` category is
	 *				`conductance_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace conductance
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using siemens = unit<std::ratio<1>, category::conductance_unit>;
		using picosiemens = pico<siemens>;
		using nanosiemens = nano<siemens>;
		using microsiemens = micro<siemens>;
		using millisiemens = milli<siemens>;
		using kilosiemens = kilo<siemens>;
		using megasiemens = mega<siemens>;
		using gigasiemens = giga<siemens>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using siemen = siemens;
		using picosiemen = picosiemens;
		using nanosiemen = nanosiemens;
		using microsiemen = microsiemens;
		using millisiemen = millisiemens;
		using kilosiemen = kilosiemens;
		using megasiemen = megasiemens;
		using gigasiemen = gigasiemens;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor conductanceContainers
		 * @{
		 */
		using siemen_t = unit_t<siemen>;
		using picosiemen_t = unit_t<picosiemen>;
		using nanosiemen_t = unit_t<nanosiemen>;
		using microsiemen_t = unit_t<microsiemen>;
		using millisiemen_t = unit_t<millisiemen>;
		using kilosiemen_t = unit_t<kilosiemen>;
		using megasiemen_t = unit_t<megasiemen>;
		using gigasiemen_t = unit_t<gigasiemen>;
		/** @} */

		/**
		 * @name  Units (abbreviated names)
		 * @{
		 */
		using S = siemens;
		using pS = picosiemens;
		using nS = nanosiemens;
		using uS = microsiemens;
		using mS = millisiemens;
		using kS = kilosiemens;
		using MS = megasiemens;
		using GS = gigasiemens;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_conductance_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_conductance_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::conductance_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_conductance_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::conductance_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of conductance
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_conductance_unit<T>::value` to test
	 *				the unit represents a conductance quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_conductance_unit : std::integral_constant<bool, all_true<detail::is_conductance_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF MAGNETIC FLUX
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing magnetic_flux values
	 * @details		The SI unit for magnetic_flux is `webers`, and the corresponding `base_unit` category is
	 *				`magnetic_flux_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace magnetic_flux
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using webers = unit<std::ratio<1>, category::magnetic_flux_unit>;
		using picowebers = pico<webers>;
		using nanowebers = nano<webers>;
		using microwebers = micro<webers>;
		using milliwebers = milli<webers>;
		using kilowebers = kilo<webers>;
		using megawebers = mega<webers>;
		using gigawebers = giga<webers>;
		using maxwells = unit<std::ratio<1, 100000000>, webers>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using weber = webers;
		using picoweber = picowebers;
		using nanoweber = nanowebers;
		using microweber = microwebers;
		using milliweber = milliwebers;
		using kiloweber = kilowebers;
		using megaweber = megawebers;
		using gigaweber = gigawebers;
		using maxwell = maxwells;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor magneticFluxContainers
		 * @{
		 */
		using weber_t = unit_t<weber>;
		using picoweber_t = unit_t<picoweber>;
		using nanoweber_t = unit_t<nanoweber>;
		using microweber_t = unit_t<microweber>;
		using milliweber_t = unit_t<milliweber>;
		using kiloweber_t = unit_t<kiloweber>;
		using megaweber_t = unit_t<megaweber>;
		using gigaweber_t = unit_t<gigaweber>;
		using maxwell_t = unit_t<maxwell>;
		/** @} */

		/**
		 * @name  Units (abbreviated names)
		 * @{
		 */
		using Wb = webers;
		using pWb = picowebers;
		using nWb = nanowebers;
		using uWb = microwebers;
		using mWb = milliwebers;
		using kWb = kilowebers;
		using MWb = megawebers;
		using GWb = gigawebers;
		using Mx = maxwells;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_magnetic_flux_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_magnetic_flux_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::magnetic_flux_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_magnetic_flux_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::magnetic_flux_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of magnetic_flux
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_magnetic_flux_unit<T>::value` to test
	 *				the unit represents a magnetic_flux quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_magnetic_flux_unit : std::integral_constant<bool, all_true<detail::is_magnetic_flux_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//----------------------------------------
	//	UNITS OF MAGNETIC FIELD STRENGTH
	//----------------------------------------

	/**
	 * @brief		namespace for unit types and containers representing magnetic_field_strength values
	 * @details		The SI unit for magnetic_field_strength is `teslas`, and the corresponding `base_unit` category is
	 *				`magnetic_field_strength_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace magnetic_field_strength
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using teslas = unit<std::ratio<1>, category::magnetic_field_strength_unit>;
		using picoteslas = pico<teslas>;
		using nanoteslas = nano<teslas>;
		using microteslas = micro<teslas>;
		using milliteslas = milli<teslas>;
		using kiloteslas = kilo<teslas>;
		using megateslas = mega<teslas>;
		using gigateslas = giga<teslas>;
		using gauss = compound_unit<magnetic_flux::maxwell, inverse<squared<length::centimeter>>>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using tesla = teslas;
		using picotesla = picoteslas;
		using nanotesla = nanoteslas;
		using microtesla = microteslas;
		using millitesla = milliteslas;
		using kilotesla = kiloteslas;
		using megatesla = megateslas;
		using gigatesla = gigateslas;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor magneticFieldStrengthContainers
		 * @{
		 */
		using tesla_t = unit_t<tesla>;
		using picotesla_t = unit_t<picotesla>;
		using nanotesla_t = unit_t<nanotesla>;
		using microtesla_t = unit_t<microtesla>;
		using millitesla_t = unit_t<millitesla>;
		using kilotesla_t = unit_t<kilotesla>;
		using megatesla_t = unit_t<megatesla>;
		using gigatesla_t = unit_t<gigatesla>;
		using gauss_t = unit_t<gauss>;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using T = teslas;
		using pT = picoteslas;
		using nT = nanoteslas;
		using uT = microteslas;
		using mT = milliteslas;
		using kT = kiloteslas;
		using MT = megateslas;
		using GT = gigateslas;
		using G = gauss;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_magnetic_field_strength_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_magnetic_field_strength_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::magnetic_field_strength_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_magnetic_field_strength_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::magnetic_field_strength_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of magnetic_field_strength
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_magnetic_field_strength_unit<T>::value` to test
	 *				the unit represents a magnetic_field_strength quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_magnetic_field_strength_unit : std::integral_constant<bool, all_true<detail::is_magnetic_field_strength_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF INDUCTANCE
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing inductance values
	 * @details		The SI unit for inductance is `henrys`, and the corresponding `base_unit` category is
	 *				`inductance_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace inductance
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using henrys = unit<std::ratio<1>, category::inductance_unit>;
		using picohenrys = pico<henrys>;
		using nanohenrys = nano<henrys>;
		using microhenrys = micro<henrys>;
		using millihenrys = milli<henrys>;
		using kilohenrys = kilo<henrys>;
		using megahenrys = mega<henrys>;
		using gigahenrys = giga<henrys>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using henry = henrys;
		using picohenry = picohenrys;
		using nanohenry = nanohenrys;
		using microhenry = microhenrys;
		using millihenry = millihenrys;
		using kilohenry = kilohenrys;
		using megahenry = megahenrys;
		using gigahenry = gigahenrys;
		/** @} */

		/**
		 * @name Units (alternate spellings)
		 * @{
		 */
		using henries = henrys;
		using picohenries = picohenrys;
		using nanohenries = nanohenrys;
		using microhenries = microhenrys;
		using millihenries = millihenrys;
		using kilohenries = kilohenrys;
		using megahenries = megahenrys;
		using gigahenries = gigahenrys;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor inductanceContainers
		 * @{
		 */
		using henry_t = unit_t<henry>;
		using picohenry_t = unit_t<picohenry>;
		using nanohenry_t = unit_t<nanohenry>;
		using microhenry_t = unit_t<microhenry>;
		using millihenry_t = unit_t<millihenry>;
		using kilohenry_t = unit_t<kilohenry>;
		using megahenry_t = unit_t<megahenry>;
		using gigahenry_t = unit_t<gigahenry>;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using H = henrys;
		using pH = picohenrys;
		using nH = nanohenrys;
		using uH = microhenrys;
		using mH = millihenrys;
		using kH = kilohenrys;
		using MH = megahenrys;
		using GH = gigahenrys;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_inductance_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_inductance_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::inductance_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_inductance_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::inductance_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of inductance
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_inductance_unit<T>::value` to test
	 *				the unit represents a inductance quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_inductance_unit : std::integral_constant<bool, all_true<detail::is_inductance_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF LUMINOUS FLUX
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing luminous_flux values
	 * @details		The SI unit for luminous_flux is `lumens`, and the corresponding `base_unit` category is
	 *				`luminous_flux_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace luminous_flux
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using lumens = unit<std::ratio<1>, category::luminous_flux_unit>;
		using picolumens = pico<lumens>;
		using nanolumens = nano<lumens>;
		using microlumens = micro<lumens>;
		using millilumens = milli<lumens>;
		using kilolumens = kilo<lumens>;
		using megalumens = mega<lumens>;
		using gigalumens = giga<lumens>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using lumen = lumens;
		using picolumen = picolumens;
		using nanolumen = nanolumens;
		using microlumen = microlumens;
		using millilumen = millilumens;
		using kilolumen = kilolumens;
		using megalumen = megalumens;
		using gigalumen = gigalumens;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor luminousFluxContainers
		 * @{
		 */
		using lumen_t = unit_t<lumen>;
		using picolumen_t = unit_t<picolumen>;
		using nanolumen_t = unit_t<nanolumen>;
		using microlumen_t = unit_t<microlumen>;
		using millilumen_t = unit_t<millilumen>;
		using kilolumen_t = unit_t<kilolumen>;
		using megalumen_t = unit_t<megalumen>;
		using gigalumen_t = unit_t<gigalumen>;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using lm = lumens;
		using plm = picolumens;
		using nlm = nanolumens;
		using ulm = microlumens;
		using mlm = millilumens;
		using klm = kilolumens;
		using Mlm = megalumens;
		using Glm = gigalumens;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_luminous_flux_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_luminous_flux_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::luminous_flux_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_luminous_flux_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::luminous_flux_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of luminous_flux
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_luminous_flux_unit<T>::value` to test
	 *				the unit represents a luminous_flux quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_luminous_flux_unit : std::integral_constant<bool, all_true<detail::is_luminous_flux_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF ILLUMINANCE
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing illuminance values
	 * @details		The SI unit for illuminance is `luxes`, and the corresponding `base_unit` category is
	 *				`illuminance_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace illuminance
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using luxes = unit<std::ratio<1>, category::illuminance_unit>;
		using picoluxes = pico<luxes>;
		using nanoluxes = nano<luxes>;
		using microluxes = micro<luxes>;
		using milliluxes = milli<luxes>;
		using kiloluxes = kilo<luxes>;
		using megaluxes = mega<luxes>;
		using gigaluxes = giga<luxes>;
		using footcandles = compound_unit<luminous_flux::lumen, inverse<squared<length::foot>>>;
		using lumens_per_square_inch = compound_unit<luminous_flux::lumen, inverse<squared<length::inch>>>;
		using phots = compound_unit<luminous_flux::lumens, inverse<squared<length::centimeter>>>;
		/** @} */

		/**
		 * @name  Units (full names singular)
		 * @{
		 */
		using lux = luxes;
		using picolux = picoluxes;
		using nanolux = nanoluxes;
		using microlux = microluxes;
		using millilux = milliluxes;
		using kilolux = kiloluxes;
		using megalux = megaluxes;
		using gigalux = gigaluxes;
		using footcandle = footcandles;
		using phot = phots;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor illuminanceContainers
		 * @{
		 */
		using lux_t = unit_t<lux>;
		using picolux_t = unit_t<picolux>;
		using nanolux_t = unit_t<nanolux>;
		using microlux_t = unit_t<microlux>;
		using millilux_t = unit_t<millilux>;
		using kilolux_t = unit_t<kilolux>;
		using megalux_t = unit_t<megalux>;
		using gigalux_t = unit_t<gigalux>;
		using footcandle_t = unit_t<footcandle>;
		using lumens_per_square_inch_t = unit_t<lumens_per_square_inch>;
		using phot_t = unit_t<phot>;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using lx = luxes;
		using plx = picoluxes;
		using nlx = nanoluxes;
		using ulx = microluxes;
		using mlx = milliluxes;
		using klx = kiloluxes;
		using Mlx = megaluxes;
		using Glx = gigaluxes;
		using fc = footcandles;
		using ph = phots;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_illuminance_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_illuminance_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::illuminance_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_illuminance_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::illuminance_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of illuminance
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_illuminance_unit<T>::value` to test
	 *				the unit represents a illuminance quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_illuminance_unit : std::integral_constant<bool, all_true<detail::is_illuminance_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF RADIATION
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing radiation values
	 * @details		The SI units for radiation are:
	 *				- source activity:	becquerel
	 *				- absorbed dose:	gray
	 *				- equivalent dose:	sievert
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace radiation
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using becquerels = inverse<time::seconds>;
		using picobecquerels = pico<becquerels>;
		using nanobecquerels = nano<becquerels>;
		using microbecquerels = micro<becquerels>;
		using millibecquerels = milli<becquerels>;
		using kilobecquerels = kilo<becquerels>;
		using megabecquerels = mega<becquerels>;
		using gigabecquerels = giga<becquerels>;
		using grays = compound_unit<energy::joules, inverse<mass::kilogram>>;
		using picograys = pico<grays>;
		using nanograys = nano<grays>;
		using micrograys = micro<grays>;
		using milligrays = milli<grays>;
		using kilograys = kilo<grays>;
		using megagrays = mega<grays>;
		using gigagrays = giga<grays>;
		using sieverts = compound_unit<energy::joules, inverse<mass::kilogram>>;
		using picosieverts = pico<sieverts>;
		using nanosieverts = nano<sieverts>;
		using microsieverts = micro<sieverts>;
		using millisieverts = milli<sieverts>;
		using kilosieverts = kilo<sieverts>;
		using megasieverts = mega<sieverts>;
		using gigasieverts = giga<sieverts>;
		using curies = unit<std::ratio<37>, gigabecquerels>;
		using rutherfords = megabecquerels;
		using rads = unit<std::ratio<1, 100>, grays>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using becquerel = becquerels;
		using picobecquerel = picobecquerels;
		using nanobecquerel = nanobecquerels;
		using microbecquerel = microbecquerels;
		using millibecquerel = millibecquerels;
		using kilobecquerel = kilobecquerels;
		using megabecquerel = megabecquerels;
		using gigabecquerel = gigabecquerels;
		using gray = grays;
		using picogray = picograys;
		using nanogray = nanograys;
		using microgray = micrograys;
		using milligray = milligrays;
		using kilogray = kilograys;
		using megagray = megagrays;
		using gigagray = gigagrays;
		using sievert = sieverts;
		using picosievert = picosieverts;
		using nanosievert = nanosieverts;
		using microsievert = microsieverts;
		using millisievert = millisieverts;
		using kilosievert = kilosieverts;
		using megasievert = megasieverts;
		using gigasievert = gigasieverts;
		using curie = curies;
		using rutherford = rutherfords;
		using rad = rads;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor radiationContainers
		 * @{
		 */
		using becquerel_t = unit_t<becquerel>;
		using picobecquerel_t = unit_t<picobecquerel>;
		using nanobecquerel_t = unit_t<nanobecquerel>;
		using microbecquerel_t = unit_t<microbecquerel>;
		using millibecquerel_t = unit_t<millibecquerel>;
		using kilobecquerel_t = unit_t<kilobecquerel>;
		using megabecquerel_t = unit_t<megabecquerel>;
		using gigabecquerel_t = unit_t<gigabecquerel>;
		using gray_t = unit_t<gray>;
		using picogray_t = unit_t<picogray>;
		using nanogray_t = unit_t<nanogray>;
		using microgray_t = unit_t<microgray>;
		using milligray_t = unit_t<milligray>;
		using kilogray_t = unit_t<kilogray>;
		using megagray_t = unit_t<megagray>;
		using gigagray_t = unit_t<gigagray>;
		using sievert_t = unit_t<sievert>;
		using picosievert_t = unit_t<picosievert>;
		using nanosievert_t = unit_t<nanosievert>;
		using microsievert_t = unit_t<microsievert>;
		using millisievert_t = unit_t<millisievert>;
		using kilosievert_t = unit_t<kilosievert>;
		using megasievert_t = unit_t<megasievert>;
		using gigasievert_t = unit_t<gigasievert>;
		using curie_t = unit_t<curie>;
		using rutherford_t = unit_t<rutherford>;
		using rad_t = unit_t<rad>;
		/** @} */

		/**
		 * @name  Units (abbreviated names)
		 * @{
		 */
		using Bq = becquerels;
		using pBq = picobecquerels;
		using nBq = nanobecquerels;
		using uBq = microbecquerels;
		using mBq = millibecquerels;
		using kBq = kilobecquerels;
		using MBq = megabecquerels;
		using GBq = gigabecquerels;
		using Gy = grays;
		using pGy = picograys;
		using nGy = nanograys;
		using uGy = micrograys;
		using mGy = milligrays;
		using kGy = kilograys;
		using MGy = megagrays;
		using GGy = gigagrays;
		using Sv = sieverts;
		using pSv = picosieverts;
		using nSv = nanosieverts;
		using uSv = microsieverts;
		using mSv = millisieverts;
		using kSv = kilosieverts;
		using MSv = megasieverts;
		using GSv = gigasieverts;
		using Ci = curies;
		using rd = rutherfords;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_radioactivity_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_radioactivity_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::radioactivity_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_radioactivity_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::radioactivity_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of radiation
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_radioactivity_unit<T>::value` to test
	 *				the unit represents a radiation quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_radioactivity_unit : std::integral_constant<bool, all_true<detail::is_radioactivity_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF TORQUE
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing torque values
	 * @details		The SI unit for torque is `newton_meters`, and the corresponding `base_unit` category is
	 *				`torque_units`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace torque
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using newton_meters = unit<std::ratio<1>, category::torque_unit>;
		using foot_pounds = compound_unit<length::foot, force::pounds>;
		using foot_poundals = compound_unit<length::foot, force::poundal>;
		using inch_pounds = compound_unit<length::inch, force::pounds>;
		using meter_kilograms = compound_unit<length::meter, force::kiloponds>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using newton_meter = newton_meters;
		using foot_pound = foot_pounds;
		using foot_poundal = foot_poundals;
		using inch_pound = inch_pounds;
		using meter_kilogram = meter_kilograms;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor torqueContainers
		 * @{
		 */
		using newton_meter_t = unit_t<newton_meter>;
		using foot_pound_t = unit_t<foot_pound>;
		using foot_poundal_t = unit_t<foot_poundal>;
		using inch_pound_t = unit_t<inch_pound>;
		using meter_kilogram_t = unit_t<meter_kilogram>;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using Nm = newton_meters;
		using ftlbf = foot_pounds;
		using ftpdl = foot_poundals;
		using inlbf = inch_pounds;
		using mkgf = meter_kilograms;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_torque_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_torque_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::torque_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_torque_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::torque_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of torque
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_torque_unit<T>::value` to test
	 *				the unit represents a torque quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_torque_unit : std::integral_constant<bool, all_true<detail::is_torque_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	AREA UNITS
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing area values
	 * @details		The SI unit for area is `square_meters`, and the corresponding `base_unit` category is
	 *				`area_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace area
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using square_meters = unit<std::ratio<1>, category::area_unit>;
		using square_feet = squared<length::feet>;
		using square_inches = squared<length::inch>;
		using square_miles = squared<length::miles>;
		using square_kilometers = squared<length::kilometers>;
		using hectares = unit<std::ratio<10000>, square_meters>;
		using acres = unit<std::ratio<43560>, square_feet>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using square_meter = square_meters;
		using square_foot = square_feet;
		using square_inch = square_inches;
		using square_mile = square_miles;
		using square_kilometer = square_kilometers;
		using hectare = hectares;
		using acre = acres;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using ha = hectares;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor areaContainers
		 * @{
		 */
		using square_meter_t = unit_t<square_meter>;
		using square_foot_t = unit_t<square_foot>;
		using square_inch_t = unit_t<square_inch>;
		using square_mile_t = unit_t<square_mile>;
		using square_kilometer_t = unit_t<square_kilometer>;
		using hectare_t = unit_t<hectare>;
		using acre_t = unit_t<acre>;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_area_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_area_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::area_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_area_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::area_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of area
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_area_unit<T>::value` to test
	 *				the unit represents a area quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_area_unit : std::integral_constant<bool, all_true<detail::is_area_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF VOLUME
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing volume values
	 * @details		The SI unit for volume is `cubic_meters`, and the corresponding `base_unit` category is
	 *				`volume_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace volume
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using cubic_meters = unit<std::ratio<1>, category::volume_unit>;
		using cubic_millimeters = cubed<length::millimeter>;
		using cubic_kilometers = cubed<length::kilometer>;
		using liters = cubed<deci<length::meter>>;
		using milliliters = milli<liters>;
		using cubic_inches = cubed<length::inches>;
		using cubic_feet = cubed<length::feet>;
		using cubic_yards = cubed<length::yards>;
		using cubic_miles = cubed<length::miles>;
		using gallons = unit<std::ratio<231>, cubic_inches>;
		using quarts = unit<std::ratio<1, 4>, gallons>;
		using pints = unit<std::ratio<1, 2>, quarts>;
		using cups = unit<std::ratio<1, 2>, pints>;
		using ounces = unit<std::ratio<1, 8>, cups>;
		using barrels = unit<std::ratio<42>, gallons>;
		using bushels = unit<std::ratio<215042, 100>, cubic_inches>;
		using cords = unit<std::ratio<128>, cubic_feet>;
		using cubic_fathoms = cubed<length::fathom>;
		using tablespoons = unit<std::ratio<1, 2>, ounces>;
		using teaspoons = unit<std::ratio<1, 6>, ounces>;
		using pinches = unit<std::ratio<1, 8>, teaspoons>;
		using dashes = unit<std::ratio<1, 2>, pinches>;
		using drops = unit<std::ratio<1, 360>, ounces>;
		using fifths = unit<std::ratio<1, 5>, gallons>;
		using drams = unit<std::ratio<1, 8>, ounces>;
		using gills = unit<std::ratio<4>, ounces>;
		using pecks = unit<std::ratio<1, 4>, bushels>;
		using sacks = unit<std::ratio<3>, bushels>;
		using shots = unit<std::ratio<3, 2>, ounces>;
		using strikes = unit<std::ratio<2>, bushels>;
		/** @} */

		/**
		 * @name Units (alternate names)
		 * @{
		 */
		using fluidOunces = ounces;
		/** @} */

		/**
		 * @name  Units (full names singular)
		 * @{
		 */
		using cubic_meter = cubic_meters;
		using cubic_millimeter = cubic_millimeters;
		using cubic_kilometer = cubic_kilometers;
		using liter = liters;
		using milliliter = milliliters;
		using cubic_inch = cubic_inches;
		using cubic_foot = cubic_feet;
		using cubic_yard = cubic_yards;
		using cubic_mile = cubic_miles;
		using gallon = gallons;
		using quart = quarts;
		using pint = pints;
		using cup = cups;
		using ounce = ounces;
		using barrel = barrels;
		using bushel = bushels;
		using cord = cords;
		using cubic_fathom = cubic_fathoms;
		using tablespoon = tablespoons;
		using teaspoon = teaspoons;
		using pinch = pinches;
		using dash = dashes;
		using drop = drops;
		using fifth = fifths;
		using dram = drams;
		using gill = gills;
		using peck = pecks;
		using sack = sacks;
		using shot = shots;
		using strike = strikes;
		using fluidOunce = fluidOunces;
		/** @} */

		/**
		 * @name  Unit Containers
		 * @anchor volumeContainers
		 * @{
		 */
		using cubic_meter_t = unit_t<cubic_meter>;
		using cubic_millimeter_t = unit_t<cubic_millimeter>;
		using cubic_kilometer_t = unit_t<cubic_kilometer>;
		using liter_t = unit_t<liter>;
		using milliliter_t = unit_t<milliliter>;
		using cubic_inch_t = unit_t<cubic_inch>;
		using cubic_foot_t = unit_t<cubic_foot>;
		using cubic_yard_t = unit_t<cubic_yard>;
		using cubic_mile_t = unit_t<cubic_mile>;
		using gallon_t = unit_t<gallon>;
		using quart_t = unit_t<quart>;
		using pint_t = unit_t<pint>;
		using cup_t = unit_t<cup>;
		using ounce_t = unit_t<ounce>;
		using barrel_t = unit_t<barrel>;
		using bushel_t = unit_t<bushel>;
		using cord_t = unit_t<cord>;
		using cubic_fathom_t = unit_t<cubic_fathom>;
		using tablespoon_t = unit_t<tablespoon>;
		using teaspoon_t = unit_t<teaspoon>;
		using pinch_t = unit_t<pinch>;
		using dash_t = unit_t<dash>;
		using drop_t = unit_t<drop>;
		using fifth_t = unit_t<fifth>;
		using dram_t = unit_t<dram>;
		using gill_t = unit_t<gill>;
		using peck_t = unit_t<peck>;
		using sack_t = unit_t<sack>;
		using shot_t = unit_t<shot>;
		using strike_t = unit_t<strike>;
		/** @} */

		/**
		 * @name Units (abbreviated names)
		 * @{
		 */
		using m3 = cubic_meters;
		using mm3 = cubic_millimeters;
		using km3 = cubic_kilometers;
		using L = liters;
		using mL = milliliters;
		using cu_in = cubic_inches;
		using cu_ft = cubic_feet;
		using cu_yd = cubic_yards;
		using cu_mi = cubic_miles;
		using gal = gallons;
		using qt = quarts;
		using pt = pints;
		using c = cups;
		using oz = ounces;
		using bl = barrels;
		using bu = bushels;
		using cu_fm = cubic_fathoms;
		using tbsp = tablespoons;
		using tsp = teaspoons;
		using dr = drams;
		using gi = gills;
		using pk = pecks;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_volume_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_volume_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::volume_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_volume_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::volume_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of volume
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_volume_unit<T>::value` to test
	 *				the unit represents a volume quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_volume_unit : std::integral_constant<bool, all_true<detail::is_volume_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF DENSITY
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing density values
	 * @details		The SI unit for density is `kilograms_per_cubic_meter`, and the corresponding `base_unit` category is
	 *				`density_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace density
	{
		/**
		 * @name Units (full names plural)
		 * @{
		 */
		using kilograms_per_cubic_meter = unit<std::ratio<1>, category::density_unit>;
		using grams_per_milliliter = compound_unit<mass::grams, inverse<volume::milliliter>>;
		using kilograms_per_liter = compound_unit<mass::kilograms, inverse<volume::liter>>;
		using ounces_per_cubic_foot = compound_unit<mass::ounces, inverse<volume::cubic_foot>>;
		using ounces_per_cubic_inch = compound_unit<mass::ounces, inverse<volume::cubic_inch>>;
		using ounces_per_gallon = compound_unit<mass::ounces, inverse<volume::gallon>>;
		using pounds_per_cubic_foot = compound_unit<mass::pounds, inverse<volume::cubic_foot>>;
		using pounds_per_cubic_inch = compound_unit<mass::pounds, inverse<volume::cubic_inch>>;
		using pounds_per_gallon = compound_unit<mass::pounds, inverse<volume::gallon>>;
		using slugs_per_cubic_foot = compound_unit<mass::slugs, inverse<volume::cubic_foot>>;
		/** @} */

		/**
		 * @name Units (full names singular)
		 * @{
		 */
		using kilogram_per_cubic_meter = kilograms_per_cubic_meter;
		using gram_per_milliliter = grams_per_milliliter;
		using kilogram_per_liter = kilograms_per_liter;
		using ounce_per_cubic_foot = ounces_per_cubic_foot;
		using ounce_per_cubic_inch = ounces_per_cubic_inch;
		using ounce_per_gallon = ounces_per_gallon;
		using pound_per_cubic_foot = pounds_per_cubic_foot;
		using pound_per_cubic_inch = pounds_per_cubic_inch;
		using pound_per_gallon = pounds_per_gallon;
		using slug_per_cubic_foot = slugs_per_cubic_foot;
		/** @} */

		/**
		 * @name  Unit Containers
		 * @anchor densityContainers
		 * @{
		 */
		using kilogram_per_cubic_meter_t = unit_t<kilogram_per_cubic_meter>;
		using gram_per_milliliter_t = unit_t<gram_per_milliliter>;
		using kilogram_per_liter_t = unit_t<kilogram_per_liter>;
		using ounce_per_cubic_foot_t = unit_t<ounce_per_cubic_foot>;
		using ounce_per_cubic_inch_t = unit_t<ounce_per_cubic_inch>;
		using ounce_per_gallon_t = unit_t<ounce_per_gallon>;
		using pound_per_cubic_foot_t = unit_t<pound_per_cubic_foot>;
		using pound_per_cubic_inch_t = unit_t<pound_per_cubic_inch>;
		using pound_per_gallon_t = unit_t<pound_per_gallon>;
		using slug_per_cubic_foot_t = unit_t<slug_per_cubic_foot>;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_density_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_density_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::density_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_density_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::density_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of density
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_density_unit<T>::value` to test
	 *				the unit represents a density quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename... T> struct is_density_unit : std::integral_constant<bool, all_true<detail::is_density_unit_impl<typename std::decay<T>::type>::value...>::value> {};

	//------------------------------
	//	UNITS OF CONCENTRATION
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers representing concentration values
	 * @details		The SI unit for concentration is `parts_per_million`, and the corresponding `base_unit` category is
	 *				`scalar_unit`.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace concentration
	{
		/**
		 * @name  Units (full names plural)
		 * @{
		 */
		using parts_per_million = unit<std::ratio<1, 1000000>, category::scalar_unit>;
		using parts_per_billion = unit<std::ratio<1, 1000>, parts_per_million>;
		using parts_per_trillion = unit<std::ratio<1, 1000>, parts_per_billion>;
		using percent = unit<std::ratio<1, 100>, category::scalar_unit>;
		/** @} */

		/**
		 * @name  Units (full names singular)
		 * @{
		 */
		using ppm = parts_per_million;
		using ppb = parts_per_billion;
		using ppt = parts_per_trillion;
		/** @} */

		/**
		 * @name Unit Containers
		 * @anchor concentrationContainers
		 * @{
		 */
		using ppm_t = unit_t<ppm>;
		using ppb_t = unit_t<ppb>;
		using ppt_t = unit_t<ppt>;
		using percent_t = unit_t<percent>;
		/** @} */
	}

	namespace detail
	{
		template<typename T> struct is_concentration_unit_impl : std::false_type {};
		template<typename C, typename U, typename P, typename T>
		struct is_concentration_unit_impl<unit<C, U, P, T>> : std::is_same<base_unit_of<typename unit_traits<unit<C, U, P, T>>::base_unit_type>, category::scalar_unit>::type {};
		template<typename U, typename S, template<typename> class N>
		struct is_concentration_unit_impl<unit_t<U, S, N>> : std::is_same<base_unit_of<typename unit_t_traits<unit_t<U, S, N>>::unit_type>, category::scalar_unit>::type {};
	}

	/**
	 * @ingroup		TypeTraits
	 * @brief		Trait which tests whether a type represents a unit of concentration
	 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_concentration_unit<T>::value` to test
	 *				the unit represents a concentration quantity.
	 * @tparam		T	one or more types to test
	 */
	template<typename T> struct is_concentration_unit : detail::is_concentration_unit_impl <typename std::decay<T>::type> {};

	//------------------------------
	//	CONSTANTS
	//------------------------------

	/**
	 * @brief		namespace for physical constants like PI and Avogadro's Number.
	 * @sa			See unit_t for more information on unit type containers.
	 */
	namespace constants
	{
		/**
		 * @name Unit Containers
		 * @anchor constantContainers
		 * @{
		 */
		static const unit_t<unit<std::ratio<1>, dimensionless::scalar, std::ratio<1>>>														pi(1.0);										///< Ratio of a circle's circumference to its diameter.
		static const velocity::meters_per_second_t																							c(299792458.0);									///< Speed of light in vacuum.
		static const unit_t<compound_unit<cubed<length::meters>, inverse<mass::kilogram>, inverse<squared<time::seconds>>>>					G(6.67408e-11);									///< Newtonian constant of gravitation.
		static const unit_t<compound_unit<energy::joule, time::seconds>>																	h(6.626070040e-34);								///< Planck constant.
		static const unit_t<compound_unit<force::newtons, inverse<squared<current::ampere>>>>												mu0(4.0e-7 * PI);								///< vacuum permeability.
		static const unit_t<compound_unit<capacitance::farad, inverse<length::meter>>>														epsilon0(1.0 / (mu0 * math::pow<2>(c)));		///< vacuum permitivity.
		static const impedance::ohm_t																										Z0(mu0 * c);									///< characteristic impedance of vacuum.
		static const unit_t<compound_unit<force::newtons, area::square_meter, inverse<squared<charge::coulomb>>>>							k_e(1.0 / (4 * pi * epsilon0));					///< Coulomb's constant.
		static const charge::coulomb_t																										e(1.602176565e-19);								///< elementary charge.
		static const mass::kilogram_t																										m_e(9.10938291e-31);							///< electron mass.
		static const mass::kilogram_t																										m_p(1.672621777e-27);							///< proton mass.
		static const unit_t<compound_unit<energy::joules, inverse<magnetic_field_strength::tesla>>>											mu_B(e * h / (4 * pi *m_e));					///< Bohr magneton.
		static const unit_t<inverse<substance::mol>>																						N_A(6.02214129e23);								///< Avagadro's Number.
		static const unit_t<compound_unit<energy::joules, inverse<temperature::kelvin>, inverse<substance::moles>>>							R(8.3144621);									///< Gas constant.
		static const unit_t<compound_unit<energy::joules, inverse<temperature::kelvin>>>													k_B(R / N_A);									///< Boltzmann constant.
		static const unit_t<compound_unit<charge::coulomb, inverse<substance::mol>>>														F(N_A * e);										///< Faraday constnat.
		static const unit_t<compound_unit<powerNum::watts, inverse<area::square_meters>, inverse<squared<squared<temperature::kelvin>>>>>	sigma((2 * math::pow<5>(pi) * math::pow<4>(R)) / (15 * math::pow<3>(h) * math::pow<2>(c) * math::pow<4>(N_A)));	///< Stefan-Boltzmann constant.
		/** @} */
	}

	//----------------------------------
	//	UNIT-ENABLED CMATH FUNCTIONS
	//----------------------------------

	/**
	 * @brief		namespace for unit-enabled versions of the `<cmath>` library
	 * @details		Includes trigonometric functions, exponential/log functions, rounding functions, etc.
	 * @sa			See `unit_t` for more information on unit type containers.
	 */
	namespace math
	{

		//----------------------------------
		//	TRIGONOMETRIC FUNCTIONS
		//----------------------------------

		/**
		 * @ingroup		UnitMath
		 * @brief		Compute cosine
		 * @details		The input value can be in any unit of angle, including radians or degrees.
		 * @tparam		AngleUnit	any `unit_t` type of `catgeory::angle_unit`. 
		 * @param[in]	angle		angle to compute the cosine of
		 * @returns		Returns the cosine of <i>angle</i>
		 */
		template<class AngleUnit>
		dimensionless::scalar_t cos(AngleUnit angle)
		{
			static_assert(units::is_angle_unit<AngleUnit>::value, "Type `AngleUnit` must be a unit of angle derived from `unit_t`.");
			return dimensionless::scalar_t(std::cos(angle.convert<angle::radian>().toDouble()));
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Compute sine
		 * @details		The input value can be in any unit of angle, including radians or degrees.
		 * @tparam		AngleUnit	any `unit_t` type of `catgeory::angle_unit`.
		 * @param[in]	angle		angle to compute the since of
		 * @returns		Returns the sine of <i>angle</i>
		 */
		template<class AngleUnit>
		dimensionless::scalar_t sin(AngleUnit angle)
		{
			static_assert(units::is_angle_unit<AngleUnit>::value, "Type `AngleUnit` must be a unit of angle derived from `unit_t`.");
			return dimensionless::scalar_t(std::sin(angle.convert<angle::radian>().toDouble()));
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Compute tangent
		 * @details		The input value can be in any unit of angle, including radians or degrees.
		 * @tparam		AngleUnit	any `unit_t` type of `catgeory::angle_unit`.
		 * @param[in]	angle		angle to compute the tangent of
		 * @returns		Returns the tangent of <i>angle</i>
		 */
		template<class AngleUnit>
		dimensionless::scalar_t tan(AngleUnit angle)
		{
			static_assert(units::is_angle_unit<AngleUnit>::value, "Type `AngleUnit` must be a unit of angle derived from `unit_t`.");
			return dimensionless::scalar_t(std::tan(angle.convert<angle::radian>().toDouble()));
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Compute arc cosine
		 * @details		Returns the principal value of the arc cosine of x, expressed in radians.
		 * @param[in]	x		Value whose arc cosine is computed, in the interval [-1,+1].
		 * @returns		Principal arc cosine of x, in the interval [0,pi] radians.
		 */
		angle::radian_t acos(dimensionless::scalar_t x)
		{
			return angle::radian_t(std::acos(x.toDouble()));
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Compute arc sine
		 * @details		Returns the principal value of the arc sine of x, expressed in radians.
		 * @param[in]	x		Value whose arc sine is computed, in the interval [-1,+1].
		 * @returns		Principal arc sine of x, in the interval [-pi/2,+pi/2] radians.
		 */
		angle::radian_t asin(dimensionless::scalar_t x)
		{
			return angle::radian_t(std::asin(x.toDouble()));
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Compute arc tangent
		 * @details		Returns the principal value of the arc tangent of x, expressed in radians. 
		 *				Notice that because of the sign ambiguity, the function cannot determine with 
		 *				certainty in which quadrant the angle falls only by its tangent value. See 
		 *				atan2 for an alternative that takes a fractional argument instead.
		 * @tparam		AngleUnit	any `unit_t` type of `catgeory::angle_unit`.
		 * @param[in]	x		Value whose arc tangent is computed, in the interval [-1,+1].
		 * @returns		Principal arc tangent of x, in the interval [-pi/2,+pi/2] radians.
		 */
		angle::radian_t atan(dimensionless::scalar_t x)
		{
			return angle::radian_t(std::atan(x.toDouble()));
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Compute arc tangent with two parameters
		 * @details		To compute the value, the function takes into account the sign of both arguments in order to determine the quadrant.
		 * @param[in]	y		y-component of the triangle expressed.
		 * @param[in]	x		x-component of the triangle expressed.
		 * @returns		Returns the principal value of the arc tangent of <i>y/x</i>, expressed in radians.
		 */
		template<class Y, class X>
		angle::radian_t atan2(Y y, X x)
		{
			static_assert(is_scalar_unit<decltype(y/x)>::value, "The quantity y/x must yield a dimensionless ratio.");

			// X and Y could be different length units, so normalize them
			return angle::radian_t(std::atan2(y.convert<typename unit_t_traits<X>::unit_type>().toDouble(), x.toDouble()));
		}

		//----------------------------------
		//	HYPERBOLIC TRIG FUNCTIONS
		//----------------------------------

		/**
		 * @ingroup		UnitMath
		 * @brief		Compute hyperbolic cosine
		 * @details		The input value can be in any unit of angle, including radians or degrees.
		 * @tparam		AngleUnit	any `unit_t` type of `catgeory::angle_unit`.
		 * @param[in]	angle		angle to compute the hyperbolic cosine of
		 * @returns		Returns the hyperbolic cosine of <i>angle</i>
		 */
		template<class AngleUnit>
		dimensionless::scalar_t cosh(AngleUnit angle)
		{
			static_assert(units::is_angle_unit<AngleUnit>::value, "Type `AngleUnit` must be a unit of angle derived from `unit_t`.");
			return dimensionless::scalar_t(std::cosh(angle.convert<angle::radian>().toDouble()));
		}

		/**
		* @ingroup		UnitMath
		* @brief		Compute hyperbolic sine
		* @details		The input value can be in any unit of angle, including radians or degrees.
		* @tparam		AngleUnit	any `unit_t` type of `catgeory::angle_unit`.
		* @param[in]	angle		angle to compute the hyperbolic sine of
		* @returns		Returns the hyperbolic sine of <i>angle</i>
		*/
		template<class AngleUnit>
		dimensionless::scalar_t sinh(AngleUnit angle)
		{
			static_assert(units::is_angle_unit<AngleUnit>::value, "Type `AngleUnit` must be a unit of angle derived from `unit_t`.");
			return dimensionless::scalar_t(std::sinh(angle.convert<angle::radian>().toDouble()));
		}

		/**
		* @ingroup		UnitMath
		* @brief		Compute hyperbolic tangent
		* @details		The input value can be in any unit of angle, including radians or degrees.
		* @tparam		AngleUnit	any `unit_t` type of `catgeory::angle_unit`.
		* @param[in]	angle		angle to compute the hyperbolic tangent of
		* @returns		Returns the hyperbolic tangent of <i>angle</i>
		*/
		template<class AngleUnit>
		dimensionless::scalar_t tanh(AngleUnit angle)
		{
			static_assert(units::is_angle_unit<AngleUnit>::value, "Type `AngleUnit` must be a unit of angle derived from `unit_t`.");
			return dimensionless::scalar_t(std::tanh(angle.convert<angle::radian>().toDouble()));
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Compute arc hyperbolic cosine
		 * @details		Returns the nonnegative arc hyperbolic cosine of x, expressed in radians.
		 * @param[in]	x	Value whose arc hyperbolic cosine is computed. If the argument is less
		 *					than 1, a domain error occurs.
		 * @returns		Nonnegative arc hyperbolic cosine of x, in the interval [0,+INFINITY] radians.
		 */
		angle::radian_t acosh(dimensionless::scalar_t x)
		{
			return angle::radian_t(std::acosh(x.toDouble()));
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Compute arc hyperbolic sine
		 * @details		Returns the arc hyperbolic sine of x, expressed in radians.
		 * @param[in]	x	Value whose arc hyperbolic sine is computed.
		 * @returns		Arc hyperbolic sine of x, in radians.
		 */
		angle::radian_t asinh(dimensionless::scalar_t x)
		{
			return angle::radian_t(std::asinh(x.toDouble()));
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Compute arc hyperbolic tangent
		 * @details		Returns the arc hyperbolic tangent of x, expressed in radians.
		 * @param[in]	x	Value whose arc hyperbolic tangent is computed, in the interval [-1,+1]. 
		 *					If the argument is out of this interval, a domain error occurs. For 
		 *					values of -1 and +1, a pole error may occur.
		 * @returns		units::angle::radian_t
		 */
		angle::radian_t atanh(dimensionless::scalar_t x)
		{
			return angle::radian_t(std::atanh(x.toDouble()));
		}

		//----------------------------------
		//	TRANSCENDENTAL FUNCTIONS
		//----------------------------------

		// it makes NO SENSE to put dimensioned units into a transcendental function, and if you think it does you are
		// demonstrably wrong. https://en.wikipedia.org/wiki/Transcendental_function#Dimensional_analysis
		
		/**
		 * @ingroup		UnitMath
		 * @brief		Compute exponential function
		 * @details		Returns the base-e exponential function of x, which is e raised to the power x: ex.
		 * @param[in]	x	scalar value of the exponent.
		 * @returns		Exponential value of x.
		 *				If the magnitude of the result is too large to be represented by a value of the return type, the
		 *				function returns HUGE_VAL (or HUGE_VALF or HUGE_VALL) with the proper sign, and an overflow range error occurs
		 */
		dimensionless::scalar_t exp(dimensionless::scalar_t x)
		{
			return dimensionless::scalar_t(std::exp(x.toDouble()));
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Compute natural logarithm
		 * @details		Returns the natural logarithm of x.
		 * @param[in]	x	scalar value whose logarithm is calculated. If the argument is negative, a 
		 *					domain error occurs.
		 * @sa			log10 for more common base-10 logarithms
		 * @returns		Natural logarithm of x.
		 */
		dimensionless::scalar_t log(dimensionless::scalar_t x)
		{
			return dimensionless::scalar_t(std::log(x.toDouble()));
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Compute common logarithm
		 * @details		Returns the common (base-10) logarithm of x.
		 * @param[in]	x	Value whose logarithm is calculated. If the argument is negative, a 
		 *					domain error occurs.
		 * @returns		Common logarithm of x.
		 */
		dimensionless::scalar_t log10(dimensionless::scalar_t x)
		{
			return dimensionless::scalar_t(std::log10(x.toDouble()));
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Break into fractional and integral parts.
		 * @details		The integer part is stored in the object pointed by intpart, and the 
		 *				fractional part is returned by the function. Both parts have the same sign 
		 *				as x.
		 * @param[in]	x		scalar value to break into parts.
		 * @param[in]	intpart Pointer to an object (of the same type as x) where the integral part
		 *				is stored with the same sign as x.
		 * @returns		The fractional part of x, with the same sign.
		 */
		dimensionless::scalar_t modf(dimensionless::scalar_t x, dimensionless::scalar_t* intpart)
		{
			double intp;
			dimensionless::scalar_t fracpart = dimensionless::scalar_t(std::modf(x.toDouble(), &intp));
			*intpart = intp;
			return fracpart;
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Compute binary exponential function
		 * @details		Returns the base-2 exponential function of x, which is 2 raised to the power x: 2^x.
		 * 2param[in]	x	Value of the exponent.
		 * @returns		2 raised to the power of x.
		 */
		dimensionless::scalar_t exp2(dimensionless::scalar_t x)
		{
			return dimensionless::scalar_t(std::exp2(x.toDouble()));
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Compute exponential minus one
		 * @details		Returns e raised to the power x minus one: e^x-1. For small magnitude values 
		 *				of x, expm1 may be more accurate than exp(x)-1.
		 * @param[in]	x	Value of the exponent.
		 * @returns		e raised to the power of x, minus one.
		 */
		dimensionless::scalar_t expm1(dimensionless::scalar_t x)
		{
			return dimensionless::scalar_t(std::expm1(x.toDouble()));
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Compute logarithm plus one
		 * @details		Returns the natural logarithm of one plus x. For small magnitude values of 
		 *				x, logp1 may be more accurate than log(1+x).
		 * @param[in]	x	Value whose logarithm is calculated. If the argument is less than -1, a 
		 *					domain error occurs.
		 * @returns		The natural logarithm of (1+x).
		 */
		dimensionless::scalar_t log1p(dimensionless::scalar_t x)
		{
			return dimensionless::scalar_t(std::log1p(x.toDouble()));
		}
		
		/**
		 * @ingroup		UnitMath
		 * @brief		Compute binary logarithm
		 * @details		Returns the binary (base-2) logarithm of x.
		 * @param[in]	x	Value whose logarithm is calculated. If the argument is negative, a 
		 *					domain error occurs.
		 * @returns		The binary logarithm of x: log2x.
		 */
		dimensionless::scalar_t log2(dimensionless::scalar_t x)
		{
			return dimensionless::scalar_t(std::log2(x.toDouble()));
		}

		//----------------------------------
		//	POWER FUNCTIONS
		//----------------------------------
		
		/* pow is implemented earlier in the library since a lot of the unit definitions depend on it */

		/**
		 * @ingroup		UnitMath
		 * @brief		computes the value of <i>value</i> raised to the <i>power</i>
		 * @details		Only implemented for linear_scale units. <i>Power</i> must be known at compile time, so the resulting unit type can be deduced.
		 * @tparam		power exponential power to raise <i>value</i> by.
		 * @param[in]	value `unit_t` derived type to raise to the given <i>power</i>
		 * @returns		new unit_t, raised to the given exponent
		 */
		template<class UnitType, typename std::enable_if<units::has_linear_scale<UnitType>::value, int>::type = 0>
		inline auto sqrt(const UnitType& value) -> unit_t<square_root<typename unit_t_traits<UnitType>::unit_type>, typename unit_t_traits<UnitType>::underlying_type, linear_scale>
		{
			return unit_t<square_root<typename unit_t_traits<UnitType>::unit_type>, typename unit_t_traits<UnitType>::underlying_type, linear_scale>
				(std::sqrt(value.toDouble()));
		}

		//----------------------------------
		//	ROUNDING FUNCTIONS
		//----------------------------------

		/**
		 * @ingroup		UnitMath
		 * @brief		Round up value
		 * @details		Rounds x upward, returning the smallest integral value that is not less than x.
		 * @param[in]	x	Unit value to round up.
		 * @returns		The smallest integral value that is not less than x.
		 */
		template<class UnitType, class = typename std::enable_if<is_unit_t<UnitType>::value>::type>
		UnitType ceil(UnitType x)
		{
			return UnitType(std::ceil(x.toDouble()));
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Round down value
		 * @details		Rounds x downward, returning the largest integral value that is not greater than x.
		 * @param[in]	x	Unit value to round down.
		 * @returns		The value of x rounded downward.
		 */
		template<class UnitType, class = typename std::enable_if<is_unit_t<UnitType>::value>::type>
		UnitType floor(UnitType x)
		{
			return UnitType(std::floor(x.toDouble()));
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Compute remainder of division
		 * @details		Returns the floating-point remainder of numer/denom (rounded towards zero).
		 * @param[in]	numer	Value of the quotient numerator.
		 * @param[in]	denom	Value of the quotient denominator.
		 * @returns		The remainder of dividing the arguments.
		 */
		template<class UnitType, class = typename std::enable_if<is_unit_t<UnitType>::value>::type>
		UnitType fmod(UnitType numer, UnitType denom)
		{
			return UnitType(std::fmod(numer.toDouble(), denom.toDouble()));
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Truncate value
		 * @details		Rounds x toward zero, returning the nearest integral value that is not 
		 *				larger in magnitude than x. Effectively rounds towards 0.
		 * @param[in]	x	Value to truncate
		 * @returns		The nearest integral value that is not larger in magnitude than x.
		 */
		template<class UnitType, class = typename std::enable_if<is_unit_t<UnitType>::value>::type>
		UnitType trunc(UnitType x)
		{
			return UnitType(std::trunc(x.toDouble()));
		}


		/**
		 * @ingroup		UnitMath
		 * @brief		Round to nearest
		 * @details		Returns the integral value that is nearest to x, with halfway cases rounded
		 *				away from zero.
		 * @param[in]	x	value to round.
		 * @returns		The value of x rounded to the nearest integral.
		 */
		template<class UnitType, class = typename std::enable_if<is_unit_t<UnitType>::value>::type>
		UnitType round(UnitType x)
		{
			return UnitType(std::round(x.toDouble()));
		}

		//----------------------------------
		//	FLOATING POINT MANIPULATION 
		//----------------------------------

		/**
		 * @ingroup		UnitMath
		 * @brief		Copy sign
		 * @details		Returns a value with the magnitude and dimension of x, and the sign of y. 
		 *				Values x and y do not have to be compatible units.
		 * @param[in]	x	Value with the magnitude of the resulting value.
		 * @param[in]	y	Value with the sign of the resulting value.
		 * @returns		value with the magnitude and dimension of x, and the sign of y.
		 */
		template<class UnitTypeLhs, class UnitTypeRhs, class = typename std::enable_if<is_unit_t<UnitTypeLhs>::value && is_unit_t<UnitTypeRhs>::value>::type>
		UnitTypeLhs copysign(UnitTypeLhs x, UnitTypeRhs y)
		{
			return UnitTypeLhs(std::copysign(x.toDouble(), y.toDouble()));
		}

		/// Overload to copy the sign from a raw double
		template<class UnitTypeLhs, class = typename std::enable_if<is_unit_t<UnitTypeLhs>::value>::type>
		UnitTypeLhs copysign(UnitTypeLhs x, double y)
		{
			return UnitTypeLhs(std::copysign(x.toDouble(), y));
		}

		//----------------------------------
		//	MIN / MAX / DIFFERENCE 
		//----------------------------------
		
		/**
		 * @ingroup		UnitMath
		 * @brief		Positive difference
		 * @details		The function returns x-y if x>y, and zero otherwise, in the same units as x.
		 *				Values x and y do not have to be the same type of units, but they do have to
		 *				be compatible.
		 * @param[in]	x	Values whose difference is calculated.
		 * @param[in]	y	Values whose difference is calculated.
		 * @returns		The positive difference between x and y.
		 */
		template<class UnitTypeLhs, class UnitTypeRhs, class = typename std::enable_if<is_unit_t<UnitTypeLhs>::value && is_unit_t<UnitTypeRhs>::value>::type>
		UnitTypeLhs fdim(UnitTypeLhs x, UnitTypeRhs y)
		{
			static_assert(is_convertible_unit_t<UnitTypeLhs, UnitTypeRhs>::value, "Unit types are not compatible.");
			return UnitTypeLhs(std::fdim(x.toDouble(), y.convert<typename unit_t_traits<UnitTypeLhs>::unit_type>().toDouble()));
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Maximum value
		 * @details		Returns the larger of its arguments: either x or y, in the same units as x.
		 *				Values x and y do not have to be the same type of units, but they do have to
		 *				be compatible.
		 * @param[in]	x	Values among which the function selects a maximum.
		 * @param[in]	y	Values among which the function selects a maximum.
		 * @returns		The maximum numeric value of its arguments.
		 */
		template<class UnitTypeLhs, class UnitTypeRhs, class = typename std::enable_if<is_unit_t<UnitTypeLhs>::value && is_unit_t<UnitTypeRhs>::value>::type>
		UnitTypeLhs fmax(UnitTypeLhs x, UnitTypeRhs y)
		{
			static_assert(is_convertible_unit_t<UnitTypeLhs, UnitTypeRhs>::value, "Unit types are not compatible.");
			return UnitTypeLhs(std::fmax(x.toDouble(), y.convert<typename unit_t_traits<UnitTypeLhs>::unit_type>().toDouble()));
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Minimum value
		 * @details		Returns the smaller of its arguments: either x or y, in the same units as x.
		 *				If one of the arguments in a NaN, the other is returned.
		 *				Values x and y do not have to be the same type of units, but they do have to
		 *				be compatible.
		 * @param[in]	x	Values among which the function selects a minimum.
		 * @param[in]	y	Values among which the function selects a minimum.
		 * @returns		The minimum numeric value of its arguments.
		 */
		template<class UnitTypeLhs, class UnitTypeRhs, class = typename std::enable_if<is_unit_t<UnitTypeLhs>::value && is_unit_t<UnitTypeRhs>::value>::type>
		UnitTypeLhs fmin(UnitTypeLhs x, UnitTypeRhs y)
		{
			static_assert(is_convertible_unit_t<UnitTypeLhs, UnitTypeRhs>::value, "Unit types are not compatible.");
			return UnitTypeLhs(std::fmin(x.toDouble(), y.convert<typename unit_t_traits<UnitTypeLhs>::unit_type>().toDouble()));
		}

		//----------------------------------
		//	OTHER FUNCTIONS
		//----------------------------------
		
		/**
		 * @ingroup		UnitMath
		 * @brief		Compute absolute value
		 * @details		Returns the absolute value of x, i.e. |x|.
		 * @param[in]	x	Value whose absolute value is returned.
		 * @returns		The absolute value of x.
		 */
		template<class UnitType, class = typename std::enable_if<is_unit_t<UnitType>::value>::type>
		UnitType fabs(UnitType x)
		{
			return UnitType(std::fabs(x.toDouble()));
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Compute absolute value
		 * @details		Returns the absolute value of x, i.e. |x|.
		 * @param[in]	x	Value whose absolute value is returned.
		 * @returns		The absolute value of x.
		 */
		template<class UnitType, class = typename std::enable_if<is_unit_t<UnitType>::value>::type>
		UnitType abs(UnitType x)
		{
			return UnitType(std::fabs(x.toDouble()));
		}

		/**
		 * @ingroup		UnitMath
		 * @brief		Multiply-add
		 * @details		Returns x*y+z. The function computes the result without losing precision in 
		 *				any intermediate result. The resulting unit type is a compound unit of x* y.
		 * @param[in]	x	Values to be multiplied.
		 * @param[in]	y	Values to be multiplied.
		 * @param[in]	z	Value to be added.
		 * @returns		The result of x*y+z
		 */
		template<class UnitTypeLhs, class UnitMultiply, class UnitAdd, class = typename std::enable_if<is_unit_t<UnitTypeLhs>::value && is_unit_t<UnitMultiply>::value && is_unit_t<UnitAdd>::value>::type>
		auto fma(UnitTypeLhs x, UnitMultiply y, UnitAdd z) -> decltype(x * y)
		{
			using resultType = decltype(x * y);
			static_assert(is_convertible_unit_t<compound_unit<typename unit_t_traits<UnitTypeLhs>::unit_type, typename unit_t_traits<UnitMultiply>::unit_type>, typename unit_t_traits<UnitAdd>::unit_type>::value, "Unit types are not compatible.");
			return resultType(std::fma(x.toDouble(), y.toDouble(), resultType(z).toDouble()));
		}

	}	// end namespace math

};	// end namespace units

#if _MCS_VER < 1800
#	pragma warning(pop)
#endif

#endif // units_h__

// For Emacs
// Local Variables:
// Mode: C++
// c-basic-offset: 2
// fill-column: 116
// tab-width: 4
// End: