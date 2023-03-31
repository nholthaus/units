//--------------------------------------------------------------------------------------------------
//
//	UnitConversion: A compile-time c++14 unit conversion library with no dependencies
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
// Copyright (c) 2016 Nic Holthaus
//
//--------------------------------------------------------------------------------------------------
//
// ATTRIBUTION:
// Parts of this work have been adapted from:
// http://stackoverflow.com/questions/35069778/create-comparison-trait-for-template-classes-whose-parameters-are-in-a-different
// http://stackoverflow.com/questions/28253399/check-traits-for-all-variadic-template-arguments/28253503
// http://stackoverflow.com/questions/36321295/rational-approximation-of-square-root-of-stdratio-at-compile-time?noredirect=1#comment60266601_36321295
// https://github.com/swatanabe/cppnow17-units
//
//--------------------------------------------------------------------------------------------------
//
/// @file	units/core.h
/// @brief	`unit`, dimensional analisys, generic cmath functions, traits (not dimension-specific),
///			and what they're implemented with (`conversion_factor`, unit manipulators, etc.)
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_core_h__
#define units_core_h__

#ifndef UNIT_LIB_DEFAULT_TYPE
#define UNIT_LIB_DEFAULT_TYPE double
#endif

//--------------------
//	INCLUDES
//--------------------

#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <numeric>
#include <ratio>
#include <type_traits>
#include <utility>

#if !defined(UNIT_LIB_DISABLE_IOSTREAM)
#include <iostream>
#include <locale>
#include <string>

//------------------------------
//	STRING FORMATTER
//------------------------------

namespace units
{
	namespace detail
	{
		template<typename T>
		std::string to_string(const T& t)
		{
			std::string str{std::to_string(t)};
			int offset{1};

			// remove trailing decimal points for integer value units. Locale aware!
			struct lconv* lc;
			lc                = localeconv();
			char decimalPoint = *lc->decimal_point;
			if (str.find_last_not_of('0') == str.find(decimalPoint))
			{
				offset = 0;
			}
			str.erase(str.find_last_not_of('0') + offset, std::string::npos);
			return str;
		}
	} // namespace detail
} // namespace units
#endif

//------------------------------
//	FORWARD DECLARATIONS
//------------------------------

namespace units
{
	template<class Unit>
	struct unit_name;
	template<class Unit>
	struct unit_abbreviation;

	template<class Unit>
	inline constexpr const char* unit_name_v = unit_name<Unit>::value;
	template<class Unit>
	inline constexpr const char* unit_abbreviation_v = unit_abbreviation<Unit>::value;
} // namespace units

//------------------------------
//	MACROS
//------------------------------

/**
 * @def			UNIT_ADD_UNIT_TAGS(namespaceName,nameSingular, namePlural, abbreviation, definition)
 * @brief		Helper macro for generating the boiler-plate code generating the tags of a new unit.
 * @details		The macro generates singular, plural, and abbreviated forms
 *				of the unit definition (e.g. `meter`, `meters`, and `m`), as aliases for the
 *				unit tag.
 * @param		namespaceName namespace in which the new units will be encapsulated.
 * @param		nameSingular singular version of the unit name, e.g. 'meter'
 * @param		namePlural - plural version of the unit name, e.g. 'meters'
 * @param		abbreviation - abbreviated unit name, e.g. 'm'
 * @param		definition - the variadic parameter is used for the definition of the unit
 *				(e.g. `conversion_factor<std::ratio<1>, units::dimension::length>`)
 * @note		a variadic template is used for the definition to allow templates with
 *				commas to be easily expanded. All the variadic 'arguments' should together
 *				comprise the unit definition.
 */
#define UNIT_ADD_UNIT_TAGS(namespaceName, nameSingular, namePlural, abbreviation, /*definition*/...) \
	inline namespace namespaceName \
	{ \
		/** @name UnitConversion (full names plural) */ /** @{ */ struct namePlural : __VA_ARGS__ \
		{ \
		};                                                                                           /** @} */ \
		/** @name UnitConversion (full names singular) */ /** @{ */ using nameSingular = namePlural; /** @} */ \
		/** @name UnitConversion (abbreviated) */ /** @{ */ using abbreviation         = namePlural; /** @} */ \
	} \
	namespace traits \
	{ \
		template<> \
		struct strong<__VA_ARGS__> \
		{ \
			using type = namespaceName::namePlural; \
		}; \
	}

/**
 * @def			UNIT_ADD_UNIT_DEFINITION(namespaceName,nameSingular)
 * @brief		Macro for generating the boiler-plate code for the unit type definition.
 * @details		The macro generates the definition of the unit container types, e.g. `meter_t`
 * @param		namespaceName namespace in which the new units will be encapsulated.
 * @param		nameSingular singular version of the unit name, e.g. 'meter'
 */
#define UNIT_ADD_UNIT_DEFINITION(namespaceName, nameSingular) \
	inline namespace namespaceName \
	{ \
		/** @name Unit Containers */ /** @{ */ template<class Underlying> \
		using nameSingular##_t = unit<nameSingular, Underlying>; /** @} */ \
	}

/**
 * @def			UNIT_ADD_IO(namespaceName,nameSingular, abbreviation)
 * @brief		Macro for generating the boiler-plate code needed for I/O for a new unit.
 * @details		The macro generates the code to insert units into an ostream. It
 *				prints both the value and abbreviation of the unit when invoked.
 * @param		namespaceName namespace in which the new units will be encapsulated.
 * @param		nameSingular singular version of the unit name, e.g. 'meter'
 * @param		abbrev - abbreviated unit name, e.g. 'm'
 * @note		When UNIT_LIB_DISABLE_IOSTREAM is defined, the macro does not generate any code
 */
#if defined(UNIT_LIB_DISABLE_IOSTREAM)
#define UNIT_ADD_IO(namespaceName, nameSingular, abbrev)
#else
#define UNIT_ADD_IO(namespaceName, nameSingular, abbrev) \
	inline namespace namespaceName \
	{ \
		template<class Underlying> \
		inline std::ostream& operator<<(std::ostream& os, const nameSingular##_t<Underlying>& obj) \
		{ \
			os << obj() << " " #abbrev; \
			return os; \
		} \
		template<class Underlying> \
		inline std::string to_string(const nameSingular##_t<Underlying>& obj) \
		{ \
			return units::detail::to_string(obj()) + std::string(" " #abbrev); \
		} \
	}
#endif

/**
 * @def		UNIT_ADD_NAME(namespaceName,nameSingular,abbreviation)
 * @brief		Macro for generating constexpr names/abbreviations for units.
 * @details	The macro generates names for units. E.g. name() of 1_m would be "meter", and
 *				abbreviation would be "m".
 * @param		namespaceName namespace in which the new units will be encapsulated. All literal values
 *				are placed in the `units::literals` namespace.
 * @param		nameSingular singular version of the unit name, e.g. 'meter'
 * @param		abbreviation - abbreviated unit name, e.g. 'm'
 */
#define UNIT_ADD_NAME(namespaceName, nameSingular, abbrev) \
	template<class Underlying> \
	struct unit_name<namespaceName::nameSingular##_t<Underlying>> \
	{ \
		static constexpr const char* value = #nameSingular; \
	}; \
\
	template<class Underlying> \
	struct unit_abbreviation<namespaceName::nameSingular##_t<Underlying>> \
	{ \
		static constexpr const char* value = #abbrev; \
	};

/**
 * @def			UNIT_ADD_LITERALS(namespaceName,nameSingular,abbreviation)
 * @brief		Macro for generating user-defined literals for units.
 * @details		The macro generates user-defined literals for units. A literal suffix is created
 *				using the abbreviation (e.g. `10.0_m`).
 * @param		namespaceName namespace in which the new units will be encapsulated. All literal values
 *				are placed in the `units::literals` namespace.
 * @param		nameSingular singular version of the unit name, e.g. 'meter'
 * @param		abbreviation - abbreviated unit name, e.g. 'm'
 * @note		When UNIT_HAS_LITERAL_SUPPORT is not defined, the macro does not generate any code
 */
#define UNIT_ADD_LITERALS(namespaceName, nameSingular, abbreviation) \
	namespace literals \
	{ \
		constexpr namespaceName::nameSingular##_t<double> operator""_##abbreviation(long double d) noexcept \
		{ \
			return namespaceName::nameSingular##_t<double>(static_cast<double>(d)); \
		} \
		constexpr namespaceName::nameSingular##_t<int> operator""_##abbreviation(unsigned long long d) noexcept \
		{ \
			return namespaceName::nameSingular##_t<int>(static_cast<int>(d)); \
		} \
	}

/**
 * @def			UNIT_ADD(namespaceName,nameSingular, namePlural, abbreviation, definition)
 * @brief		Macro for generating the boiler-plate code needed for a new unit.
 * @details		The macro generates singular, plural, and abbreviated forms
 *				of the unit definition (e.g. `meter`, `meters`, and `m`), as well as the
 *				appropriately named unit container (e.g. `meter_t`). A literal suffix is created
 *				using the abbreviation (e.g. `10.0_m`). It also defines a class-specific
 *				cout function which prints both the value and abbreviation of the unit when invoked.
 * @param		namespaceName namespace in which the new units will be encapsulated. All literal values
 *				are placed in the `units::literals` namespace.
 * @param		nameSingular singular version of the unit name, e.g. 'meter'
 * @param		namePlural - plural version of the unit name, e.g. 'meters'
 * @param		abbreviation - abbreviated unit name, e.g. 'm'
 * @param		definition - the variadic parameter is used for the definition of the unit
 *				(e.g. `conversion_factor<std::ratio<1>, units::dimension::length>`)
 * @note		a variadic template is used for the definition to allow templates with
 *				commas to be easily expanded. All the variadic 'arguments' should together
 *				comprise the unit definition.
 */
#define UNIT_ADD(namespaceName, nameSingular, namePlural, abbreviation, /*definition*/...) \
	UNIT_ADD_UNIT_TAGS(namespaceName, nameSingular, namePlural, abbreviation, __VA_ARGS__) \
	UNIT_ADD_UNIT_DEFINITION(namespaceName, nameSingular) \
	UNIT_ADD_NAME(namespaceName, nameSingular, abbreviation) \
	UNIT_ADD_IO(namespaceName, nameSingular, abbreviation) \
	UNIT_ADD_LITERALS(namespaceName, nameSingular, abbreviation)

/**
 * @def			UNIT_ADD_WITH_CUSTOM_TYPE(namespaceName,nameSingular, namePlural, abbreviation, underlyingType,
 *				definition)
 * @brief		Macro for generating the boiler-plate code needed for a new unit with a non-default underlying type.
 * @details		The macro generates singular, plural, and abbreviated forms
 *				of the unit definition (e.g. `meter`, `meters`, and `m`), as well as the
 *				appropriately named unit container (e.g. `meter_t`). A literal suffix is created
 *				using the abbreviation (e.g. `10.0_m`). It also defines a class-specific
 *				cout function which prints both the value and abbreviation of the unit when invoked.
 * @param		namespaceName namespace in which the new units will be encapsulated. All literal values
 *				are placed in the `units::literals` namespace.
 * @param		nameSingular singular version of the unit name, e.g. 'meter'
 * @param		namePlural - plural version of the unit name, e.g. 'meters'
 * @param		abbreviation - abbreviated unit name, e.g. 'm'
 * @param		underlyingType - the underlying type, e.g. 'int' or 'float'
 * @param		definition - the variadic parameter is used for the definition of the unit
 *				(e.g. `conversion_factor<std::ratio<1>, units::dimension::length>`)
 * @note		a variadic template is used for the definition to allow templates with
 *				commas to be easily expanded. All the variadic 'arguments' should together
 *				comprise the unit definition.
 */
#define UNIT_ADD_WITH_CUSTOM_TYPE( \
	namespaceName, nameSingular, namePlural, abbreviation, underlyingType, /*definition*/...) \
	UNIT_ADD_UNIT_TAGS(namespaceName, nameSingular, namePlural, abbreviation, __VA_ARGS__) \
	UNIT_ADD_CUSTOM_TYPE_UNIT_DEFINITION(namespaceName, nameSingular, underlyingType) \
	UNIT_ADD_IO(namespaceName, nameSingular, abbreviation) \
	UNIT_ADD_LITERALS(namespaceName, nameSingular, abbreviation)

/**
 * @def			UNIT_ADD_DECIBEL(namespaceName, nameSingular, abbreviation)
 * @brief		Macro to create decibel container and literals for an existing unit type.
 * @details		This macro generates the decibel unit container, cout overload, and literal definitions.
 * @param		namespaceName namespace in which the new units will be encapsulated. All literal values
 *				are placed in the `units::literals` namespace.
 * @param		nameSingular singular version of the dimension name, e.g. 'watt'
 * @param		abbreviation - abbreviated decibel unit name, e.g. 'dBW'
 */
#define UNIT_ADD_DECIBEL(namespaceName, nameSingular, abbreviation) \
	inline namespace namespaceName \
	{ \
		/** @name Unit Containers */ /** @{ */ template<class Underlying> \
		using abbreviation##_t = unit<nameSingular, Underlying, units::decibel_scale>; /** @} */ \
	} \
	UNIT_ADD_IO(namespaceName, abbreviation, abbreviation) \
	UNIT_ADD_LITERALS(namespaceName, abbreviation, abbreviation)

/**
 * @def			UNIT_ADD_DIMENSION_TRAIT(unitdimension)
 * @brief		Macro to create the `is_dimension_unit` type trait.
 * @details		This trait allows users to test whether a given type matches
 *				an intended dimension. This macro comprises all the boiler-plate
 *				code necessary to do so.
 * @param		unitdimension The name of the dimension of unit, e.g. length or mass.
 */

#define UNIT_ADD_DIMENSION_TRAIT(unitdimension) \
	/** @ingroup	TypeTraits*/ \
	/** @brief		Trait which tests whether a type represents a unit of unitdimension*/ \
	/** @details	Inherits from `std::true_type` or `std::false_type`. Use `is_ ## unitdimension ## _unit_v<T>` to \
	 ** 			test the unit represents a unitdimension quantity.*/ \
	/** @tparam		T	one or more types to test*/ \
	namespace traits \
	{ \
		template<typename... T> \
		struct is_##unitdimension##_unit \
		  : std::conjunction<::units::detail::has_dimension_of<std::decay_t<T>, units::dimension::unitdimension>...> \
		{ \
		}; \
		template<typename... T> \
		inline constexpr bool is_##unitdimension##_unit_v = is_##unitdimension##_unit<T...>::value; \
	}

/**
 * @def			UNIT_ADD_WITH_METRIC_PREFIXES(nameSingular, namePlural, abbreviation, definition)
 * @brief		Macro for generating the boiler-plate code needed for a new unit, including its metric
 *				prefixes from femto to peta.
 * @details		See UNIT_ADD. In addition to generating the unit definition and containers '(e.g. `meters` and
 *				'meter_t', it also creates corresponding units with metric suffixes such as `millimeters`, and
 *				`millimeter_t`), as well as the literal suffixes (e.g. `10.0_mm`).
 * @param		namespaceName namespace in which the new units will be encapsulated. All literal values
 *				are placed in the `units::literals` namespace.
 * @param		nameSingular singular version of the unit name, e.g. 'meter'
 * @param		namePlural - plural version of the unit name, e.g. 'meters'
 * @param		abbreviation - abbreviated unit name, e.g. 'm'
 * @param		definition - the variadic parameter is used for the definition of the unit
 *				(e.g. `conversion_factor<std::ratio<1>, units::dimension::length>`)
 * @note		a variadic template is used for the definition to allow templates with
 *				commas to be easily expanded. All the variadic 'arguments' should together
 *				comprise the unit definition.
 */
#define UNIT_ADD_WITH_METRIC_PREFIXES(namespaceName, nameSingular, namePlural, abbreviation, /*definition*/...) \
	UNIT_ADD(namespaceName, nameSingular, namePlural, abbreviation, __VA_ARGS__) \
	UNIT_ADD(namespaceName, femto##nameSingular, femto##namePlural, f##abbreviation, femto<namePlural>) \
	UNIT_ADD(namespaceName, pico##nameSingular, pico##namePlural, p##abbreviation, pico<namePlural>) \
	UNIT_ADD(namespaceName, nano##nameSingular, nano##namePlural, n##abbreviation, nano<namePlural>) \
	UNIT_ADD(namespaceName, micro##nameSingular, micro##namePlural, u##abbreviation, micro<namePlural>) \
	UNIT_ADD(namespaceName, milli##nameSingular, milli##namePlural, m##abbreviation, milli<namePlural>) \
	UNIT_ADD(namespaceName, centi##nameSingular, centi##namePlural, c##abbreviation, centi<namePlural>) \
	UNIT_ADD(namespaceName, deci##nameSingular, deci##namePlural, d##abbreviation, deci<namePlural>) \
	UNIT_ADD(namespaceName, deca##nameSingular, deca##namePlural, da##abbreviation, deca<namePlural>) \
	UNIT_ADD(namespaceName, hecto##nameSingular, hecto##namePlural, h##abbreviation, hecto<namePlural>) \
	UNIT_ADD(namespaceName, kilo##nameSingular, kilo##namePlural, k##abbreviation, kilo<namePlural>) \
	UNIT_ADD(namespaceName, mega##nameSingular, mega##namePlural, M##abbreviation, mega<namePlural>) \
	UNIT_ADD(namespaceName, giga##nameSingular, giga##namePlural, G##abbreviation, giga<namePlural>) \
	UNIT_ADD(namespaceName, tera##nameSingular, tera##namePlural, T##abbreviation, tera<namePlural>) \
	UNIT_ADD(namespaceName, peta##nameSingular, peta##namePlural, P##abbreviation, peta<namePlural>)

/**
 * @def		UNIT_ADD_WITH_METRIC_AND_BINARY_PREFIXES(nameSingular, namePlural, abbreviation, definition)
 * @brief		Macro for generating the boiler-plate code needed for a new unit, including its metric
 *				prefixes from femto to peta, and binary prefixes from kibi to exbi.
 * @details	See UNIT_ADD. In addition to generating the unit definition and containers '(e.g. `bytes` and 'byte_t',
 *				it also creates corresponding units with metric suffixes such as `millimeters`, and `millimeter_t`), as
 *				well as the literal suffixes (e.g. `10.0_B`).
 * @param		namespaceName namespace in which the new units will be encapsulated. All literal values
 *				are placed in the `units::literals` namespace.
 * @param		nameSingular singular version of the unit name, e.g. 'byte'
 * @param		namePlural - plural version of the unit name, e.g. 'bytes'
 * @param		abbreviation - abbreviated unit name, e.g. 'B'
 * @param		definition - the variadic parameter is used for the definition of the unit
 *				(e.g. `conversion_factor<std::ratio<1>, units::dimension::data>`)
 * @note		a variadic template is used for the definition to allow templates with
 *				commas to be easily expanded. All the variadic 'arguments' should together
 *				comprise the unit definition.
 */
#define UNIT_ADD_WITH_METRIC_AND_BINARY_PREFIXES( \
	namespaceName, nameSingular, namePlural, abbreviation, /*definition*/...) \
	UNIT_ADD_WITH_METRIC_PREFIXES(namespaceName, nameSingular, namePlural, abbreviation, __VA_ARGS__) \
	UNIT_ADD(namespaceName, kibi##nameSingular, kibi##namePlural, Ki##abbreviation, kibi<namePlural>) \
	UNIT_ADD(namespaceName, mebi##nameSingular, mebi##namePlural, Mi##abbreviation, mebi<namePlural>) \
	UNIT_ADD(namespaceName, gibi##nameSingular, gibi##namePlural, Gi##abbreviation, gibi<namePlural>) \
	UNIT_ADD(namespaceName, tebi##nameSingular, tebi##namePlural, Ti##abbreviation, tebi<namePlural>) \
	UNIT_ADD(namespaceName, pebi##nameSingular, pebi##namePlural, Pi##abbreviation, pebi<namePlural>) \
	UNIT_ADD(namespaceName, exbi##nameSingular, exbi##namePlural, Ei##abbreviation, exbi<namePlural>)

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
	 *				the conversion function, to create compound units, or to create `unit` types.
	 *				By themselves, they are not containers and have no stored value.
	 */

	/**
	 * @defgroup	UnitManipulators Unit Manipulators
	 * @brief		Defines a series of classes used to manipulate unit types, such as `inverse<>`, `squared<>`, and
	 *				metric prefixes. Unit manipulators can be chained together, e.g.
	 *				`inverse<squared<pico<time::seconds>>>` to represent picoseconds^-2.
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

	/**
	 * @defgroup	STDTypeTraits Standard Type Traits Specializations
	 * @brief		Specialization of `std::common_type` for unit types.
	 */

	//------------------------------
	//	FORWARD DECLARATIONS
	//------------------------------

	/** @cond */ // DOXYGEN IGNORE
	namespace constants
	{
		namespace detail
		{
			inline constexpr UNIT_LIB_DEFAULT_TYPE PI_VAL = 3.14159265358979323846264338327950288419716939937510;
		}
	}               // namespace constants
	/** @endcond */ // END DOXYGEN IGNORE

	//------------------------------
	//	RATIO TRAITS
	//------------------------------

	/**
	 * @ingroup TypeTraits
	 * @{
	 */

	namespace traits
	{
		/** @cond */ // DOXYGEN IGNORE
		namespace detail
		{
			template<class T>
			struct is_ratio_impl : std::false_type
			{
			};

			template<std::intmax_t N, std::intmax_t D>
			struct is_ratio_impl<std::ratio<N, D>> : std::true_type
			{
			};
		}               // namespace detail
		/** @endcond */ // END DOXYGEN IGNORE

		/**
		 * @brief		Trait that tests whether a type represents a std::ratio.
		 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_ratio_v<T>` to test
		 *				whether `class T` implements a std::ratio.
		 */
		template<class T>
		using is_ratio = detail::is_ratio_impl<T>;

		template<class T>
		inline constexpr bool is_ratio_v = is_ratio<T>::value;
	} // namespace traits

	//------------------------------
	//	UNIT TRAITS
	//------------------------------

	/**
	 * @brief namespace representing type traits which can access the properties of types provided by the units library.
	 */
	namespace traits
	{
#ifdef FOR_DOXYGEN_PURPOSES_ONLY
		/**
		 * @ingroup		TypeTraits
		 * @brief		Traits class defining the properties of units.
		 * @details		The units library determines certain properties of the units passed to
		 *				them and what they represent by using the members of the corresponding
		 *				unit_traits instantiation.
		 */
		template<class T>
		struct conversion_factor_traits
		{
			typedef typename T::dimension_type
				dimension_type; ///< Unit type that the unit was derived from. May be a `dimension` or another
								///< `conversion_factor`. Use the `dimension_of_t` trait to find the SI dimension type.
								///< This will be `void` if type `T` is not a unit.
			typedef typename T::conversion_ratio
				conversion_ratio; ///< `std::ratio` representing the conversion factor to the `dimension_type`. This
								  ///< will be `void` if type `T` is not a unit.
			typedef typename T::pi_exponent_ratio
				pi_exponent_ratio; ///< `std::ratio` representing the exponent of pi to be used in the conversion. This
								   ///< will be `void` if type `T` is not a unit.
			typedef typename T::translation_ratio
				translation_ratio; ///< `std::ratio` representing a datum translation to the dimension (i.e. degrees C
								   ///< to degrees F conversion). This will be `void` if type `T` is not a unit.
		};
#endif
		/** @cond */ // DOXYGEN IGNORE
		/**
		 * @brief		unit traits implementation for classes which are not units.
		 */
		template<class T, typename = void>
		struct conversion_factor_traits
		{
			using dimension_type    = void;
			using conversion_ratio  = void;
			using pi_exponent_ratio = void;
			using translation_ratio = void;
		};

		template<class T>
		struct conversion_factor_traits<T,
			std::void_t<typename T::dimension_type, typename T::conversion_ratio, typename T::pi_exponent_ratio,
				typename T::translation_ratio>>
		{
			using dimension_type =
				typename T::dimension_type; ///< Unit type that the unit was derived from. May be a `dimension` or
											///< another `conversion_factor`. Use the `dimension_of_t` trait to find the
											///< SI dimension type. This will be `void` if type `T` is not a unit.
			using conversion_ratio =
				typename T::conversion_ratio; ///< `std::ratio` representing the conversion factor to the
											  ///< `dimension_type`. This will be `void` if type `T` is not a unit.
			using pi_exponent_ratio =
				typename T::pi_exponent_ratio; ///< `std::ratio` representing the exponent of pi to be used in the
											   ///< conversion. This will be `void` if type `T` is not a unit.
			using translation_ratio =
				typename T::translation_ratio; ///< `std::ratio` representing a datum translation to the dimension (i.e.
											   ///< degrees C to degrees F conversion). This will be `void` if type `T`
											   ///< is not a unit.
		};
		/** @endcond */ // END DOXYGEN IGNORE
	}                   // namespace traits

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		helper type to identify units.
		 * @details		A non-templated base class for `unit` which enables compile-time testing.
		 */
		struct _conversion_factor
		{
		};
	} // namespace detail

	/** @endcond */ // END DOXYGEN IGNORE

	namespace traits
	{
		/**
		 * @ingroup		TypeTraits
		 * @brief		Traits which tests if a class is a `unit`
		 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_unit_v<T>` to test
		 *				whether `class T` implements a `unit`.
		 */
		template<class T>
		using is_conversion_factor = typename std::is_base_of<units::detail::_conversion_factor, T>::type;

		template<class T>
		inline constexpr bool is_conversion_factor_v = is_conversion_factor<T>::value;

		/**
		 * @ingroup			TypeTraits
		 * @brief			SFINAE-able trait that maps a `conversion_factor` to its strengthened type.
		 * @details			If `T` is a cv-unqualified `conversion_factor`, the member `type` alias names the strong
		 *					type alias of `T`, if any, and `T` otherwise. Otherwise, there is no `type` member. This may
		 *					be specialized only if `T` depends on a program-defined type.
		 */
		template<class T>
		struct strong : std::enable_if<is_conversion_factor_v<T> && std::is_same_v<T, std::remove_cv_t<T>>, T>
		{
		};

		template<class T>
		using strong_t = typename strong<T>::type;
	} // namespace traits

	/** @} */ // end of TypeTraits

	//------------------------------
	//	DIMENSIONS
	//------------------------------
	// see: https://github.com/swatanabe/cppnow17-units
	// license for this code: https://github.com/swatanabe/cppnow17-units/blob/master/LICENSE_1_0.txt
	//------------------------------

	template<class D, class E>
	struct dim
	{
		using dimension = D;
		using exponent  = E;
	};

	template<class... D>
	struct dimension_t;

	template<>
	struct dimension_t<>
	{
		static const constexpr bool empty = true;
	};

	template<class D0, class... D>
	struct dimension_t<D0, D...>
	{
		static const constexpr bool empty = false;
		using front                       = D0;
		using pop_front                   = dimension_t<D...>;
	};

	template<class T, class U>
	using combine_dims = dim<typename T::dimension, std::ratio_add<typename T::exponent, typename U::exponent>>;

	template<int Test>
	struct merge_dimensions_impl;

	constexpr int const_strcmp(const char* lhs, const char* rhs)
	{
		return (*lhs && *rhs) ? (*lhs == *rhs ? const_strcmp(lhs + 1, rhs + 1) : (*lhs < *rhs ? -1 : 1))
							  : ((!*lhs && !*rhs) ? 0 : (!*lhs ? -1 : 1));
	}

	template<bool HasT, bool HasU>
	struct merge_dimensions_recurse_impl;

	template<>
	struct merge_dimensions_recurse_impl<true, true>
	{
		template<class T, class U, class... R>
		using apply = typename merge_dimensions_impl<const_strcmp(
			T::front::dimension::name, U::front::dimension::name)>::template apply<T, U, R...>;
	};

	template<class T, class U>
	struct append;

	template<class... T, class... U>
	struct append<dimension_t<T...>, dimension_t<U...>>
	{
		using type = dimension_t<T..., U...>;
	};

	template<>
	struct merge_dimensions_recurse_impl<true, false>
	{
		template<class T, class U, class... R>
		using apply = typename append<dimension_t<R...>, T>::type;
	};

	template<>
	struct merge_dimensions_recurse_impl<false, true>
	{
		template<class T, class U, class... R>
		using apply = typename append<dimension_t<R...>, U>::type;
	};

	template<>
	struct merge_dimensions_recurse_impl<false, false>
	{
		template<class T, class U, class... R>
		using apply = dimension_t<R...>;
	};

	template<class T, class U, class... R>
	using merge_dimensions_recurse =
		typename merge_dimensions_recurse_impl<!T::empty, !U::empty>::template apply<T, U, R...>;

	template<>
	struct merge_dimensions_impl<1>
	{
		template<class T, class U, class... R>
		using apply = merge_dimensions_recurse<T, typename U::pop_front, R..., typename U::front>;
	};

	template<>
	struct merge_dimensions_impl<-1>
	{
		template<class T, class U, class... R>
		using apply = merge_dimensions_recurse<typename T::pop_front, U, R..., typename T::front>;
	};

	template<bool Cancels>
	struct merge_dimensions_combine_impl;

	template<>
	struct merge_dimensions_combine_impl<true>
	{
		template<class T, class U, class X, class... R>
		using apply = merge_dimensions_recurse<T, U, R...>;
	};

	template<>
	struct merge_dimensions_combine_impl<false>
	{
		template<class T, class U, class X, class... R>
		using apply = merge_dimensions_recurse<T, U, R..., X>;
	};

	template<>
	struct merge_dimensions_impl<0>
	{
		template<class T, class U, class... R>
		using apply = typename merge_dimensions_combine_impl<
			std::ratio_add<typename T::front::exponent, typename U::front::exponent>::num ==
			0>::template apply<typename T::pop_front, typename U::pop_front,
			dim<typename T::front::dimension, std::ratio_add<typename T::front::exponent, typename U::front::exponent>>,
			R...>;
	};

	template<class T, class U>
	using merge_dimensions = merge_dimensions_recurse<T, U>;

	template<class T, class E>
	struct dimension_pow_impl;

	template<class... T, class... E, class R>
	struct dimension_pow_impl<dimension_t<dim<T, E>...>, R>
	{
		using type = dimension_t<dim<T, std::ratio_multiply<E, R>>...>;
	};

	template<class T, class E>
	using dimension_pow = typename dimension_pow_impl<T, E>::type;

	template<class T, class E>
	using dimension_root = dimension_pow<T, std::ratio_divide<std::ratio<1>, E>>;

	template<class T, class U>
	using dimension_multiply = merge_dimensions<T, U>;

	template<class T, class U>
	using dimension_divide = merge_dimensions<T, dimension_pow<U, std::ratio<-1>>>;

	template<class T0 = void, class N0 = std::ratio<1>, class... Rest>
	struct make_dimension_list
	{
		using type = dimension_multiply<dimension_t<dim<T0, N0>>, typename make_dimension_list<Rest...>::type>;
	};

	template<class... T, class N0, class... Rest>
	struct make_dimension_list<dimension_t<T...>, N0, Rest...>
	{
		using type =
			dimension_multiply<dimension_pow<dimension_t<T...>, N0>, typename make_dimension_list<Rest...>::type>;
	};

	template<>
	struct make_dimension_list<>
	{
		using type = dimension_t<>;
	};

	template<class... T>
	using make_dimension = typename make_dimension_list<T...>::type;

	//------------------------------
	//	UNIT DIMENSIONS
	//------------------------------

	/**
	 * @brief		namespace representing the implemented base and derived unit types. These will not generally be
	 *				needed by library users.
	 * @sa			dimension for the definition of the dimension parameters.
	 */
	namespace dimension
	{
		// DIMENSION TAGS
		struct length_tag
		{
			static constexpr const char* const name         = "length";
			static constexpr const char* const abbreviation = "m";
		};

		struct mass_tag
		{
			static constexpr const char* const name         = "mass";
			static constexpr const char* const abbreviation = "kg";
		};

		struct time_tag
		{
			static constexpr const char* const name         = "time";
			static constexpr const char* const abbreviation = "s";
		};

		struct current_tag
		{
			static constexpr const char* const name         = "current";
			static constexpr const char* const abbreviation = "A";
		};

		struct temperature_tag
		{
			static constexpr const char* const name         = "temperature";
			static constexpr const char* const abbreviation = "K";
		};

		struct substance_tag
		{
			static constexpr const char* const name         = "amount of substance";
			static constexpr const char* const abbreviation = "mol";
		};

		struct luminous_intensity_tag
		{
			static constexpr const char* const name         = "luminous intensity";
			static constexpr const char* const abbreviation = "cd";
		};

		struct angle_tag
		{
			static constexpr const char* const name         = "angle";
			static constexpr const char* const abbreviation = "rad";
		};

		struct data_tag
		{
			static constexpr const char* const name         = "data";
			static constexpr const char* const abbreviation = "byte";
		};

		// SI BASE UNITS
		using length             = make_dimension<length_tag>;
		using mass               = make_dimension<mass_tag>;
		using time               = make_dimension<time_tag>;
		using current            = make_dimension<current_tag>;
		using temperature        = make_dimension<temperature_tag>;
		using substance          = make_dimension<substance_tag>;
		using luminous_intensity = make_dimension<luminous_intensity_tag>;

		// dimensionless (DIMENSIONLESS) TYPES
		using dimensionless = dimension_t<>;             ///< Represents a quantity with no dimension.
		using angle         = make_dimension<angle_tag>; ///< Represents a quantity of angle

		// SI DERIVED UNIT TYPES
		using solid_angle      = dimension_pow<angle, std::ratio<2>>;  ///< Represents an SI derived unit of solid angle
		using frequency        = make_dimension<time, std::ratio<-1>>; ///< Represents an SI derived unit of frequency
		using velocity         = dimension_divide<length, time>;       ///< Represents an SI derived unit of velocity
		using angular_velocity = dimension_divide<angle, time>; ///< Represents an SI derived unit of angular velocity
		using acceleration     = dimension_divide<velocity, time>; ///< Represents an SI derived unit of acceleration
		using force            = dimension_multiply<mass, acceleration>; ///< Represents an SI derived unit of force
		using area             = dimension_pow<length, std::ratio<2>>;   ///< Represents an SI derived unit of area
		using pressure         = dimension_divide<force, area>;          ///< Represents an SI derived unit of pressure
		using charge           = dimension_multiply<time, current>;      ///< Represents an SI derived unit of charge
		using energy           = dimension_multiply<force, length>;      ///< Represents an SI derived unit of energy
		using power            = dimension_divide<energy, time>;         ///< Represents an SI derived unit of power
		using voltage          = dimension_divide<power, current>;       ///< Represents an SI derived unit of voltage
		using capacitance      = dimension_divide<charge, voltage>;  ///< Represents an SI derived unit of capacitance
		using impedance        = dimension_divide<voltage, current>; ///< Represents an SI derived unit of impedance
		using conductance      = dimension_divide<current, voltage>; ///< Represents an SI derived unit of conductance
		using magnetic_flux    = dimension_divide<energy, current>;  ///< Represents an SI derived unit of magnetic flux
		using magnetic_field_strength = make_dimension<mass, std::ratio<1>, time, std::ratio<-2>, current,
			std::ratio<-1>>; ///< Represents an SI derived unit of magnetic field strength
		using inductance = dimension_multiply<impedance, time>; ///< Represents an SI derived unit of inductance
		using luminous_flux =
			dimension_multiply<solid_angle, luminous_intensity>; ///< Represents an SI derived unit of luminous flux
		using illuminance   = make_dimension<luminous_flux, std::ratio<1>, length,
            std::ratio<-2>>; ///< Represents an SI derived unit of illuminance
		using radioactivity = make_dimension<length, std::ratio<2>, time,
			std::ratio<-2>>; ///< Represents an SI derived unit of radioactivity

		// OTHER UNIT TYPES
		using torque             = dimension_multiply<force, length>;      ///< Represents an SI derived unit of torque
		using volume             = dimension_pow<length, std::ratio<3>>;   ///< Represents an SI derived unit of volume
		using density            = dimension_divide<mass, volume>;         ///< Represents an SI derived unit of density
		using concentration      = make_dimension<volume, std::ratio<-1>>; ///< Represents a unit of concentration
		using data               = make_dimension<data_tag>;               ///< Represents a unit of data size
		using data_transfer_rate = make_dimension<data, std::ratio<-1>>;   ///< Represents a unit of data transfer rate
	}                                                                      // namespace dimension

	//------------------------------
	//	UNIT CLASSES
	//------------------------------

	/** @cond */ // DOXYGEN IGNORE
	/**
	 * @brief		unit type template specialization for units derived from dimensions.
	 */
	template<class, class, class, class>
	struct conversion_factor;
	template<class Conversion, class... Exponents, class PiExponent, class Translation>
	struct conversion_factor<Conversion, dimension_t<Exponents...>, PiExponent, Translation>
	  : units::detail::_conversion_factor
	{
		static_assert(traits::is_ratio_v<Conversion>,
			"Template parameter `Conversion` must be a `std::ratio` representing the conversion factor to "
			"`Dimension`.");
		static_assert(traits::is_ratio_v<PiExponent>,
			"Template parameter `PiExponent` must be a `std::ratio` representing the exponents of Pi the unit has.");
		static_assert(traits::is_ratio_v<Translation>,
			"Template parameter `Translation` must be a `std::ratio` representing an additive translation required by "
			"the unit conversion.");

		using dimension_type    = dimension_t<Exponents...>;
		using conversion_ratio  = Conversion;
		using translation_ratio = Translation;
		using pi_exponent_ratio = PiExponent;
	};
	/** @endcond */ // END DOXYGEN IGNORE

	/** @cond */ // DOXYGEN IGNORE
	namespace traits
	{
		template<typename C, typename U, typename P, typename T>
		struct strong<conversion_factor<C, U, P, T>>
		{
			using type = conversion_factor<C, U, P, T>;
		};
	} // namespace traits

	namespace detail
	{
		template<typename C, typename U, typename P, typename T>
		conversion_factor<C, U, P, T> conversion_factor_base_t_impl(conversion_factor<C, U, P, T>*);

		template<typename T>
		using conversion_factor_base_t = decltype(conversion_factor_base_t_impl(std::declval<T*>()));

		/**
		 * @brief		dimension_of_t trait implementation
		 * @details		recursively seeks dimension type that a unit is derived from. Since units can be
		 *				derived from other units, the `dimension_type` typedef may not represent this type.
		 */
		template<class UnitConversion>
		struct dimension_of_impl : dimension_of_impl<conversion_factor_base_t<UnitConversion>>
		{
		};

		template<class Conversion, class BaseUnit, class PiExponent, class Translation>
		struct dimension_of_impl<conversion_factor<Conversion, BaseUnit, PiExponent, Translation>>
		  : dimension_of_impl<BaseUnit>
		{
		};

		template<class... Exponents>
		struct dimension_of_impl<dimension_t<Exponents...>>
		{
			using type = dimension_t<Exponents...>;
		};

		template<>
		struct dimension_of_impl<void>
		{
			using type = void;
		};
	}               // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	namespace traits
	{
		/**
		 * @brief		Trait which returns the `dimension_t` type that a unit is originally derived from.
		 * @details		Since units can be derived from other `conversion_factor` types in addition to `dimension_t`
		 *				types, the `dimension_type` typedef will not always be a `dimension_t` (or unit dimension).
		 */
		template<class U>
		using dimension_of_t = typename units::detail::dimension_of_impl<U>::type;
	} // namespace traits

	/** @cond */ // DOXYGEN IGNORE
	template<class UnitType, typename T, template<typename> class NonLinearScale>
	class unit;

	namespace detail
	{
		template<typename T, class Dim, bool IsConv = false>
		struct has_dimension_of_impl : std::false_type
		{
		};

		template<typename T, class Dim>
		struct has_dimension_of_impl<T, Dim, true> : has_dimension_of_impl<conversion_factor_base_t<T>, Dim, true>::type
		{
		};

		template<typename C, typename U, typename P, typename T, class Dim>
		struct has_dimension_of_impl<conversion_factor<C, U, P, T>, Dim, true>
		  : std::is_same<typename conversion_factor<C, U, P, T>::dimension_type, Dim>::type
		{
		};

		template<typename U, typename S, template<typename> class N, class Dim>
		struct has_dimension_of_impl<unit<U, S, N>, Dim> : std::is_same<traits::dimension_of_t<U>, Dim>::type
		{
		};

		template<typename T, class Dim>
		using has_dimension_of = typename has_dimension_of_impl<T, Dim, traits::is_conversion_factor_v<T>>::type;
	}               // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	/**
	 * @brief		Type representing an arbitrary unit.
	 * @ingroup		UnitTypes
	 * @details		`conversion_factor` types are used as tags for the `conversion` function. They are *not* containers
	 *				(see `unit` for a  container class). Each unit is defined by:
	 *
	 *				- A `std::ratio` defining the conversion factor to the dimension type. (e.g. `std::ratio<1,12>` for
	 *					inches to feet)
	 *				- A dimension that the unit is derived from (or a unit dimension. Must be of type
	 *				`conversion_factor` or `dimension`)
	 *				- An exponent representing factors of PI required by the conversion. (e.g. `std::ratio<-1>` for a
	 *					radians to degrees conversion)
	 *				- a ratio representing a datum translation required for the conversion (e.g. `std::ratio<32>` for a
	 *					farenheit to celsius conversion)
	 *
	 *				Typically, a specific unit, like `meters`, would be implemented as a type alias
	 *				of `conversion_factor`, i.e. `using meters = conversion_factor<std::ratio<1>,
	 *				units::dimension::length>`, or `using inches = conversion_factor<std::ratio<1,12>, feet>`.
	 * @tparam		Conversion	std::ratio representing dimensionless multiplication factor.
	 * @tparam		BaseUnit	Unit type which this unit is derived from. May be a `dimension`, or another
	 *				`conversion_factor`.
	 * @tparam		PiExponent	std::ratio representing the exponent of pi required by the conversion.
	 * @tparam		Translation	std::ratio representing any datum translation required by the conversion.
	 */
	template<class Conversion, class BaseUnit, class PiExponent = std::ratio<0>, class Translation = std::ratio<0>>
	struct conversion_factor : units::detail::_conversion_factor
	{
		static_assert(traits::is_conversion_factor_v<BaseUnit>,
			"Template parameter `BaseUnit` must be a `conversion_factor` type.");
		static_assert(traits::is_ratio_v<Conversion>,
			"Template parameter `Conversion` must be a `std::ratio` representing the conversion factor to `BaseUnit`.");
		static_assert(traits::is_ratio_v<PiExponent>,
			"Template parameter `PiExponent` must be a `std::ratio` representing the exponents of Pi the unit has.");

		using dimension_type    = units::traits::dimension_of_t<BaseUnit>;
		using conversion_ratio  = typename std::ratio_multiply<typename BaseUnit::conversion_ratio, Conversion>;
		using pi_exponent_ratio = typename std::ratio_add<typename BaseUnit::pi_exponent_ratio, PiExponent>;
		using translation_ratio =
			typename std::ratio_add<std::ratio_multiply<typename BaseUnit::conversion_ratio, Translation>,
				typename BaseUnit::translation_ratio>;
	};

	//------------------------------
	//	UNIT MANIPULATORS
	//------------------------------

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		implementation of `unit_multiply`.
		 * @details		multiplies two units. The dimension becomes the dimensions of each with their exponents
		 *				added together. The conversion factors of each are multiplied by each other. Pi exponent ratios
		 *				are added, and datum translations are removed.
		 */
		template<class Unit1, class Unit2>
		struct unit_multiply_impl
		{
			using type = conversion_factor<
				std::ratio_multiply<typename Unit1::conversion_ratio, typename Unit2::conversion_ratio>,
				dimension_multiply<traits::dimension_of_t<typename Unit1::dimension_type>,
					traits::dimension_of_t<typename Unit2::dimension_type>>,
				std::ratio_add<typename Unit1::pi_exponent_ratio, typename Unit2::pi_exponent_ratio>, std::ratio<0>>;
		};

		/**
		 * @brief		represents the type of two units multiplied together.
		 * @details		recalculates conversion and exponent ratios at compile-time.
		 */
		template<class U1, class U2>
		using unit_multiply = typename unit_multiply_impl<U1, U2>::type;

		/**
		 * @brief		implementation of `unit_divide`.
		 * @details		divides two units. The dimension becomes the dimensions of each with their exponents
		 *				subtracted from each other. The conversion factors of each are divided by each other. Pi
		 *				exponent ratios are subtracted, and datum translations are removed.
		 */
		template<class Unit1, class Unit2>
		struct unit_divide_impl
		{
			using type =
				conversion_factor<std::ratio_divide<typename Unit1::conversion_ratio, typename Unit2::conversion_ratio>,
					dimension_divide<traits::dimension_of_t<typename Unit1::dimension_type>,
						traits::dimension_of_t<typename Unit2::dimension_type>>,
					std::ratio_subtract<typename Unit1::pi_exponent_ratio, typename Unit2::pi_exponent_ratio>,
					std::ratio<0>>;
		};

		/**
		 * @brief		represents the type of two units divided by each other.
		 * @details		recalculates conversion and exponent ratios at compile-time.
		 */
		template<class U1, class U2>
		using unit_divide = typename unit_divide_impl<U1, U2>::type;

		/**
		 * @brief		implementation of `inverse`
		 * @details		inverts a unit (equivalent to 1/unit). The `dimension` and pi exponents are all multiplied by
		 *				-1. The conversion ratio numerator and denominator are swapped. Datum translation
		 *				ratios are removed.
		 */
		template<class Unit>
		struct inverse_impl
		{
			using type = conversion_factor<std::ratio<Unit::conversion_ratio::den, Unit::conversion_ratio::num>,
				dimension_pow<
					traits::dimension_of_t<typename units::traits::conversion_factor_traits<Unit>::dimension_type>,
					std::ratio<-1>>,
				std::ratio_multiply<typename units::traits::conversion_factor_traits<Unit>::pi_exponent_ratio,
					std::ratio<-1>>,
				std::ratio<0>>; // inverses are rates or change, the translation factor goes away.
		};
	}               // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	/**
	 * @brief		represents the inverse unit type of `class U`.
	 * @ingroup		UnitManipulators
	 * @tparam		U	`unit` type to invert.
	 * @details		E.g. `inverse<meters>` will represent meters^-1 (i.e. 1/meters).
	 */
	template<class U>
	using inverse = typename units::detail::inverse_impl<U>::type;

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		implementation of `squared`
		 * @details		Squares the conversion ratio, `dimension` exponents, pi exponents, and removes
		 *				datum translation ratios.
		 */
		template<class Unit>
		struct squared_impl
		{
			static_assert(traits::is_conversion_factor_v<Unit>, "Template parameter `Unit` must be a `unit` type.");
			using Conversion = typename Unit::conversion_ratio;
			using type       = conversion_factor<std::ratio_multiply<Conversion, Conversion>,
                dimension_pow<traits::dimension_of_t<typename Unit::dimension_type>, std::ratio<2>>,
                std::ratio_multiply<typename Unit::pi_exponent_ratio, std::ratio<2>>, typename Unit::translation_ratio>;
		};
	}               // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	/**
	 * @brief		represents the unit type of `class U` squared
	 * @ingroup		UnitManipulators
	 * @tparam		U	`unit` type to square.
	 * @details		E.g. `square<meters>` will represent meters^2.
	 */
	template<class U>
	using squared = typename units::detail::squared_impl<U>::type;

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		implementation of `cubed`
		 * @details		Cubes the conversion ratio, `dimension` exponents, pi exponents, and removes
		 *				datum translation ratios.
		 */
		template<class Unit>
		struct cubed_impl
		{
			static_assert(traits::is_conversion_factor_v<Unit>, "Template parameter `Unit` must be a `unit` type.");
			using Conversion = typename Unit::conversion_ratio;
			using type = conversion_factor<std::ratio_multiply<Conversion, std::ratio_multiply<Conversion, Conversion>>,
				dimension_pow<traits::dimension_of_t<typename Unit::dimension_type>, std::ratio<3>>,
				std::ratio_multiply<typename Unit::pi_exponent_ratio, std::ratio<3>>, typename Unit::translation_ratio>;
		};
	}               // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	/**
	 * @brief		represents the type of `class U` cubed.
	 * @ingroup		UnitManipulators
	 * @tparam		U	`unit` type to cube.
	 * @details		E.g. `cubed<meters>` will represent meters^3.
	 */
	template<class U>
	using cubed = typename units::detail::cubed_impl<U>::type;

	/** @cond */ // DOXYGEN IGNORE
	// clang-format off
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
				static constexpr const std::intmax_t value = 2 * N;
				static_assert(value > 0, "Overflows when computing 2 * N");
			};

			template <std::intmax_t Lower, std::intmax_t Upper, typename Condition1 = void, typename Condition2 = void>
			struct DoubleSidedSearch_ : DoubleSidedSearch_<Lower, Upper,
				std::integral_constant<bool, (Upper - Lower == 1)>,
				std::integral_constant<bool, ((Upper - Lower>1 && Predicate<Lower + (Upper - Lower) / 2>::value))>> {};

			template <std::intmax_t Lower, std::intmax_t Upper>
			struct DoubleSidedSearch_<Lower, Upper, std::false_type, std::false_type> : DoubleSidedSearch_<Lower, Lower + (Upper - Lower) / 2> {};

			template <std::intmax_t Lower, std::intmax_t Upper, typename Condition2>
			struct DoubleSidedSearch_<Lower, Upper, std::true_type, Condition2> : std::integral_constant<std::intmax_t, Lower>{};

			template <std::intmax_t Lower, std::intmax_t Upper, typename Condition1>
			struct DoubleSidedSearch_<Lower, Upper, Condition1, std::true_type> : DoubleSidedSearch_<Lower + (Upper - Lower) / 2, Upper>{};

			template <std::intmax_t Lower, class enabled1 = void>
			struct SingleSidedSearch_ : SingleSidedSearch_<Lower, std::integral_constant<bool, Predicate<SafeDouble_<Lower>::value>::value>>{};

			template <std::intmax_t Lower>
			struct SingleSidedSearch_<Lower, std::false_type> : DoubleSidedSearch_<Lower, SafeDouble_<Lower>::value> {};

			template <std::intmax_t Lower>
			struct SingleSidedSearch_<Lower, std::true_type> : SingleSidedSearch_<SafeDouble_<Lower>::value>{};

			static constexpr const std::intmax_t value = SingleSidedSearch_<1>::value;
 		};

		template <template <std::intmax_t N> class Predicate>
		struct BinarySearch<Predicate, std::enable_if_t<!Predicate<1>::value>> : std::integral_constant<std::intmax_t, 0>{};

		// Find largest std::integer N such that N<=sqrt(R)
		template <typename R>
		struct Integer {
			template <std::intmax_t N> using Predicate_ = std::ratio_less_equal<std::ratio<N>, std::ratio_divide<R, std::ratio<N>>>;
			static constexpr const std::intmax_t value = BinarySearch<Predicate_>::value;
		};

		template <typename R>
		struct IsPerfectSquare {
			static constexpr const std::intmax_t DenSqrt_ = Integer<std::ratio<R::den>>::value;
			static constexpr const std::intmax_t NumSqrt_ = Integer<std::ratio<R::num>>::value;
			static constexpr const bool value =( DenSqrt_ * DenSqrt_ == R::den && NumSqrt_ * NumSqrt_ == R::num);
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
			static constexpr const std::intmax_t I_ = (A_::num + Integer<std::ratio_multiply<B_, Square<std::ratio<A_::den>>>>::value) / A_::den;
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
			using Abs_ = std::conditional_t<std::ratio_less<T, Zero>::value, std::ratio_subtract<Zero, T>, T>;

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
		struct Sqrt_<R, Eps, N, std::enable_if_t<std::ratio_less_equal<typename ContinuedFraction<R, N>::Error, Eps>::value>> {
			using type = typename ContinuedFraction<R, N>::V;
		};

		template <typename R, typename Eps, typename enabled = void>
		struct Sqrt {
			static_assert(std::ratio_greater_equal<R, Zero>::value, "R can't be negative");
		};

		template <typename R, typename Eps>
		struct Sqrt<R, Eps, std::enable_if_t<std::ratio_greater_equal<R, Zero>::value && IsPerfectSquare<R>::value>> {
			using type = typename IsPerfectSquare<R>::Sqrt;
		};

		template <typename R, typename Eps>
		struct Sqrt<R, Eps, std::enable_if_t<(std::ratio_greater_equal<R, Zero>::value && !IsPerfectSquare<R>::value)>> : Sqrt_<R, Eps>{};
	}
	// clang-format on
	/** @endcond */ // END DOXYGEN IGNORE

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
	using ratio_sqrt = typename units::detail::Sqrt<Ratio, std::ratio<1, Eps>>::type;

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		implementation of `sqrt`
		 * @details		square roots the conversion ratio, `dimension` exponents, pi exponents, and removes
		 *				datum translation ratios.
		 */
		template<class Unit, std::intmax_t Eps>
		struct sqrt_impl
		{
			static_assert(traits::is_conversion_factor_v<Unit>, "Template parameter `Unit` must be a `unit` type.");
			using Conversion = typename Unit::conversion_ratio;
			using type       = conversion_factor<ratio_sqrt<Conversion, Eps>,
                dimension_root<traits::dimension_of_t<typename Unit::dimension_type>, std::ratio<2>>,
                std::ratio_divide<typename Unit::pi_exponent_ratio, std::ratio<2>>, typename Unit::translation_ratio>;
		};
	}               // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	/**
	 * @ingroup		UnitManipulators
	 * @brief		represents the square root of type `class U`.
	 * @details		Calculates a rational approximation of the square root of the unit. The error
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
	 * @note		USE WITH CAUTION. The is an approximate value. In general, square<square_root<meter>> != meter,
	 *				i.e. the operation is not reversible, and it will result in propogated approximations.
	 *				Use only when absolutely necessary.
	 */
	template<class U, std::intmax_t Eps = 10000000000>
	using square_root = typename units::detail::sqrt_impl<U, Eps>::type;

	//------------------------------
	//	COMPOUND UNITS
	//------------------------------

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		implementation of compound_unit
		 * @details		multiplies a variadic list of units together, and is inherited from the resulting
		 *				type.
		 */
		template<class U, class... Us>
		struct compound_impl;
		template<class U>
		struct compound_impl<U>
		{
			using type = U;
		};
		template<class U1, class U2, class... Us>
		struct compound_impl<U1, U2, Us...> : compound_impl<unit_multiply<U1, U2>, Us...>
		{
		};
	}               // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

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
	using compound_conversion_factor = typename units::detail::compound_impl<U, Us...>::type;

	//------------------------------
	//	PREFIXES
	//------------------------------

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		prefix applicator.
		 * @details		creates a unit type from a prefix and a unit
		 */
		template<class Ratio, class Unit>
		struct prefix
		{
			static_assert(traits::is_ratio_v<Ratio>, "Template parameter `Ratio` must be a `std::ratio`.");
			static_assert(
				traits::is_conversion_factor_v<Unit>, "Template parameter `Unit` must be a `conversion_factor` type.");
			using type = typename units::conversion_factor<Ratio, Unit>;
		};

		/// recursive exponential implementation
		template<int N, class U>
		struct power_of_ratio
		{
			using type = std::ratio_multiply<U, typename power_of_ratio<N - 1, U>::type>;
		};

		/// End recursion
		template<class U>
		struct power_of_ratio<1, U>
		{
			using type = U;
		};
	}               // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	// clang-format off
	/**
	 * @ingroup UnitManipulators
	 * @{
	 * @ingroup Decimal Prefixes
	 * @{
	 */
	template<class U> using atto	= typename units::detail::prefix<std::atto,	U>::type;			///< Represents the type of `class U` with the metric 'atto' prefix appended.	@details E.g. atto<meters> represents meters*10^-18		@tparam U unit type to apply the prefix to.
	template<class U> using femto	= typename units::detail::prefix<std::femto,U>::type;			///< Represents the type of `class U` with the metric 'femto' prefix appended.  @details E.g. femto<meters> represents meters*10^-15	@tparam U unit type to apply the prefix to.
	template<class U> using pico	= typename units::detail::prefix<std::pico,	U>::type;			///< Represents the type of `class U` with the metric 'pico' prefix appended.	@details E.g. pico<meters> represents meters*10^-12		@tparam U unit type to apply the prefix to.
	template<class U> using nano	= typename units::detail::prefix<std::nano,	U>::type;			///< Represents the type of `class U` with the metric 'nano' prefix appended.	@details E.g. nano<meters> represents meters*10^-9		@tparam U unit type to apply the prefix to.
	template<class U> using micro	= typename units::detail::prefix<std::micro,U>::type;			///< Represents the type of `class U` with the metric 'micro' prefix appended.	@details E.g. micro<meters> represents meters*10^-6		@tparam U unit type to apply the prefix to.
	template<class U> using milli	= typename units::detail::prefix<std::milli,U>::type;			///< Represents the type of `class U` with the metric 'milli' prefix appended.	@details E.g. milli<meters> represents meters*10^-3		@tparam U unit type to apply the prefix to.
	template<class U> using centi	= typename units::detail::prefix<std::centi,U>::type;			///< Represents the type of `class U` with the metric 'centi' prefix appended.	@details E.g. centi<meters> represents meters*10^-2		@tparam U unit type to apply the prefix to.
	template<class U> using deci	= typename units::detail::prefix<std::deci,	U>::type;			///< Represents the type of `class U` with the metric 'deci' prefix appended.	@details E.g. deci<meters> represents meters*10^-1		@tparam U unit type to apply the prefix to.
	template<class U> using deca	= typename units::detail::prefix<std::deca,	U>::type;			///< Represents the type of `class U` with the metric 'deca' prefix appended.	@details E.g. deca<meters> represents meters*10^1		@tparam U unit type to apply the prefix to.
	template<class U> using hecto	= typename units::detail::prefix<std::hecto,U>::type;			///< Represents the type of `class U` with the metric 'hecto' prefix appended.	@details E.g. hecto<meters> represents meters*10^2		@tparam U unit type to apply the prefix to.
	template<class U> using kilo	= typename units::detail::prefix<std::kilo,	U>::type;			///< Represents the type of `class U` with the metric 'kilo' prefix appended.	@details E.g. kilo<meters> represents meters*10^3		@tparam U unit type to apply the prefix to.
	template<class U> using mega	= typename units::detail::prefix<std::mega,	U>::type;			///< Represents the type of `class U` with the metric 'mega' prefix appended.	@details E.g. mega<meters> represents meters*10^6		@tparam U unit type to apply the prefix to.
	template<class U> using giga	= typename units::detail::prefix<std::giga,	U>::type;			///< Represents the type of `class U` with the metric 'giga' prefix appended.	@details E.g. giga<meters> represents meters*10^9		@tparam U unit type to apply the prefix to.
	template<class U> using tera	= typename units::detail::prefix<std::tera,	U>::type;			///< Represents the type of `class U` with the metric 'tera' prefix appended.	@details E.g. tera<meters> represents meters*10^12		@tparam U unit type to apply the prefix to.
	template<class U> using peta	= typename units::detail::prefix<std::peta,	U>::type;			///< Represents the type of `class U` with the metric 'peta' prefix appended.	@details E.g. peta<meters> represents meters*10^15		@tparam U unit type to apply the prefix to.
	template<class U> using exa		= typename units::detail::prefix<std::exa,	U>::type;			///< Represents the type of `class U` with the metric 'exa' prefix appended.	@details E.g. exa<meters> represents meters*10^18		@tparam U unit type to apply the prefix to.
	/** @} @} */

	/**
	 * @ingroup UnitManipulators
	 * @{
	 * @ingroup Binary Prefixes
	 * @{
	 */
	template<class U> using kibi	= typename units::detail::prefix<std::ratio<1024>,					U>::type;	///< Represents the type of `class U` with the binary 'kibi' prefix appended.	@details E.g. kibi<bytes> represents bytes*2^10	@tparam U unit type to apply the prefix to.
	template<class U> using mebi	= typename units::detail::prefix<std::ratio<1048576>,				U>::type;	///< Represents the type of `class U` with the binary 'mibi' prefix appended.	@details E.g. mebi<bytes> represents bytes*2^20	@tparam U unit type to apply the prefix to.
	template<class U> using gibi	= typename units::detail::prefix<std::ratio<1073741824>,			U>::type;	///< Represents the type of `class U` with the binary 'gibi' prefix appended.	@details E.g. gibi<bytes> represents bytes*2^30	@tparam U unit type to apply the prefix to.
	template<class U> using tebi	= typename units::detail::prefix<std::ratio<1099511627776>,			U>::type;	///< Represents the type of `class U` with the binary 'tebi' prefix appended.	@details E.g. tebi<bytes> represents bytes*2^40	@tparam U unit type to apply the prefix to.
	template<class U> using pebi	= typename units::detail::prefix<std::ratio<1125899906842624>,		U>::type;	///< Represents the type of `class U` with the binary 'pebi' prefix appended.	@details E.g. pebi<bytes> represents bytes*2^50	@tparam U unit type to apply the prefix to.
	template<class U> using exbi	= typename units::detail::prefix<std::ratio<1152921504606846976>,	U>::type;	///< Represents the type of `class U` with the binary 'exbi' prefix appended.	@details E.g. exbi<bytes> represents bytes*2^60	@tparam U unit type to apply the prefix to.
	/** @} @} */
	// clang-format on

	//------------------------------
	//	CONVERSION TRAITS
	//------------------------------

	namespace traits
	{
		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait which checks whether two units can be converted to each other
		 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_convertible_unit_v<U1, U2>` to test
		 *				whether `class U1` is convertible to `class U2`. Note: convertible has both the semantic
		 *				meaning, (i.e. meters can be converted to feet), and the c++ meaning of conversion (type meters
		 *				can be converted to type feet). Conversion is always symmetric, so if U1 is convertible to U2,
		 *				then U2 will be convertible to U1.
		 * @tparam		U1 Unit to convert from.
		 * @tparam		U2 Unit to convert to.
		 * @sa			is_convertible_unit
		 */
		template<class U1, class U2>
		struct is_convertible_conversion_factor
		  : std::is_same<traits::dimension_of_t<typename units::traits::conversion_factor_traits<U1>::dimension_type>,
				traits::dimension_of_t<typename units::traits::conversion_factor_traits<U2>::dimension_type>>::type
		{
		};

		template<class U1, class U2>
		inline constexpr bool is_convertible_conversion_factor_v = is_convertible_conversion_factor<U1, U2>::value;
	} // namespace traits

	//------------------------------
	//	CONSTEXPR MATH FUNCTIONS
	//------------------------------

	/** @cond */ // DOXYGEN IGNORE
	namespace traits
	{
		// forward declaration
		template<class T>
		struct is_unit;
	} // namespace traits

	namespace detail
	{
		/**
		 * @brief		Helper trait to promote integers or integral units to `double` (units).
		 * @details		Simulates the promotion undergone by integers when calling the standard cmath functions
		 *				overloaded on `float`, `double` and `long double`. Works for both arithmetic types and
		 *				unit types.
		 */
		template<typename T, bool IsUnit = false>
		struct floating_point_promotion : std::conditional<std::is_floating_point_v<T>, T, double>
		{
		};

		template<typename T>
		using floating_point_promotion_t = typename floating_point_promotion<T, traits::is_unit<T>::value>::type;

		template<template<class, typename, template<typename> class> class Unit, class UnitConversion, typename T,
			template<typename> class NonLinearScale>
		struct floating_point_promotion<Unit<UnitConversion, T, NonLinearScale>, true>
		{
			using type = Unit<UnitConversion, floating_point_promotion_t<T>, NonLinearScale>;
		};
	} // namespace detail

	namespace Detail
	{
		template<typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
		constexpr T sqrtNewtonRaphson(T x, T curr, T prev)
		{
			return curr == prev ? curr : sqrtNewtonRaphson(x, 0.5 * (curr + x / curr), curr);
		}
	}               // namespace Detail
	/** @endcond */ // END DOXYGEN IGNORE

	template<typename T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
	constexpr detail::floating_point_promotion_t<T> sqrt(T x_)
	{
		using FloatingPoint = detail::floating_point_promotion_t<T>;

		const FloatingPoint x(x_);

		return x >= 0 && x < std::numeric_limits<FloatingPoint>::infinity()
			? Detail::sqrtNewtonRaphson(x, x, FloatingPoint(0))
			: std::numeric_limits<FloatingPoint>::quiet_NaN();
	}

	template<typename T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
	constexpr detail::floating_point_promotion_t<T> pow(T x, unsigned long long y)
	{
		return y == 0 ? 1.0 : x * pow(x, y - 1);
	}

	template<typename T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
	constexpr T abs(T x)
	{
		return x < 0 ? -x : x;
	}

	//------------------------------
	//	CONVERSION FUNCTIONS
	//------------------------------

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		SFINAE helper to test that types are convertible `conversion_factor`s.
		 */
		template<class From, class To>
		inline constexpr bool is_convertible_conversion_factor = traits::is_conversion_factor_v<From>&&
			traits::is_conversion_factor_v<To>&& traits::is_convertible_conversion_factor_v<From, To>;
	}               // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	/**
	 * @ingroup		Conversion
	 * @brief		converts a <i>value</i> from one type to another.
	 * @details		Converts a <i>value</i> of a built-in arithmetic type to another unit. E.g. @code double result =
	 *				convert<length::meters, length::feet>(1.0);	// result == 3.28084 @endcode Intermediate computations
	 *				are carried in the widest representation before being converted to `To`.
	 * @sa			unit	for implicit conversion of unit containers.
	 * @tparam		UnitFrom unit tag to convert <i>value</i> from. Must be a `conversion_factor` type (i.e.
	 *				is_conversion_factor_v<UnitFrom> == true), and must be convertible to `UnitTo` (i.e.
	 *				is_convertible_conversion_factor_v<UnitFrom, UnitTo> == true).
	 * @tparam		UnitTo unit tag to convert <i>value</i> to. Must be a `conversion_factor` type (i.e.
	 *				is_conversion_factor_v<UnitTo> == true), and must be convertible from `UnitFrom` (i.e.
	 *				is_convertible_conversion_factor_v<UnitFrom, UnitTo> == true).
	 * @tparam		From type of <i>value</i>. It is inferred from <i>value</i>, and is expected to be a built-in
	 *				arithmetic type.
	 * @param[in]	value Arithmetic value to convert from `UnitFrom` to `UnitTo`. The value should represent
	 *				a quantity in units of `UnitFrom`.
	 * @tparam		To arithmetic type of the converted unit value. The value represents a quantity in units of
	 *				`UnitTo`.
	 * @returns		value, converted from units of `UnitFrom` to `UnitTo`.
	 */
	template<class UnitFrom, class UnitTo, typename To = UNIT_LIB_DEFAULT_TYPE, typename From,
		class = std::enable_if_t<detail::is_convertible_conversion_factor<UnitFrom, UnitTo>>>
	constexpr To convert(const From& value) noexcept
	{
		using Ratio   = std::ratio_divide<typename UnitFrom::conversion_ratio, typename UnitTo::conversion_ratio>;
		using PiRatio = std::ratio_subtract<typename UnitFrom::pi_exponent_ratio, typename UnitTo::pi_exponent_ratio>;
		using Translation = std::ratio_divide<
			std::ratio_subtract<typename UnitFrom::translation_ratio, typename UnitTo::translation_ratio>,
			typename UnitTo::conversion_ratio>;

		[[maybe_unused]] constexpr auto normal_convert = [](const auto& value) {
			using ResolvedUnitFrom =
				conversion_factor<typename UnitFrom::conversion_ratio, typename UnitFrom::dimension_type>;
			using ResolvedUnitTo =
				conversion_factor<typename UnitTo::conversion_ratio, typename UnitTo::dimension_type>;
			return convert<ResolvedUnitFrom, ResolvedUnitTo, std::decay_t<decltype(value)>>(value);
		};

		[[maybe_unused]] constexpr auto pi_convert = [](const auto& value) {
			using ResolvedUnitFrom = conversion_factor<typename UnitFrom::conversion_ratio,
				typename UnitFrom::dimension_type, typename UnitFrom::pi_exponent_ratio>;
			using ResolvedUnitTo = conversion_factor<typename UnitTo::conversion_ratio, typename UnitTo::dimension_type,
				typename UnitTo::pi_exponent_ratio>;
			return convert<ResolvedUnitFrom, ResolvedUnitTo, std::decay_t<decltype(value)>>(value);
		};

		// same exact unit on both sides
		if constexpr (std::is_same_v<UnitFrom, UnitTo>)
		{
			return static_cast<To>(value);
		}
		// PI REQUIRED, no translation
		else if constexpr (!std::is_same_v<std::ratio<0>, PiRatio> && !!std::is_same_v<std::ratio<0>, Translation>)
		{
			using CommonUnderlying = std::common_type_t<To, From, UNIT_LIB_DEFAULT_TYPE>;

			// constexpr pi in numerator
			if constexpr (PiRatio::num / PiRatio::den >= 1 && PiRatio::num % PiRatio::den == 0)
			{
				return static_cast<To>(normal_convert(static_cast<CommonUnderlying>(value) *
					static_cast<CommonUnderlying>(pow(constants::detail::PI_VAL, PiRatio::num / PiRatio::den))));
			}
			// constexpr pi in denominator
			else if constexpr (PiRatio::num / PiRatio::den <= -1 && PiRatio::num % PiRatio::den == 0)
			{
				return static_cast<To>(normal_convert(static_cast<CommonUnderlying>(value) /
					static_cast<CommonUnderlying>(pow(constants::detail::PI_VAL, -PiRatio::num / PiRatio::den))));
			}
			// non-constexpr pi in numerator. This case (only) isn't actually constexpr.
			else if constexpr (PiRatio::num / PiRatio::den < 1 && PiRatio::num / PiRatio::den > -1)
			{
				return static_cast<To>(normal_convert(static_cast<CommonUnderlying>(value) *
					static_cast<CommonUnderlying>(std::pow(constants::detail::PI_VAL, PiRatio::num / PiRatio::den))));
			}
		}
		// Translation required, no pi variable
		else if constexpr (!!std::is_same_v<std::ratio<0>, PiRatio> && !std::is_same_v<std::ratio<0>, Translation>)
		{
			using CommonUnderlying = std::common_type_t<To, From, UNIT_LIB_DEFAULT_TYPE>;

			return static_cast<To>(normal_convert(static_cast<CommonUnderlying>(value)) +
				(static_cast<CommonUnderlying>(Translation::num) / static_cast<CommonUnderlying>(Translation::den)));
		}
		// pi and translation needed
		else if constexpr (!std::is_same_v<std::ratio<0>, PiRatio> && !std::is_same_v<std::ratio<0>, Translation>)
		{
			using CommonUnderlying = std::common_type_t<To, From, UNIT_LIB_DEFAULT_TYPE>;

			return static_cast<To>(pi_convert(static_cast<CommonUnderlying>(value)) +
				(static_cast<CommonUnderlying>(Translation::num) / static_cast<CommonUnderlying>(Translation::den)));
		}
		// normal conversion between two different units
		else
		{
			using CommonUnderlying = std::common_type_t<To, From, std::intmax_t>;

			if constexpr (Ratio::num == 1 && Ratio::den == 1)
				return static_cast<To>(value);
			if constexpr (Ratio::num != 1 && Ratio::den == 1)
				return static_cast<To>(
					static_cast<CommonUnderlying>(value) * static_cast<CommonUnderlying>(Ratio::num));
			if constexpr (Ratio::num == 1 && Ratio::den != 1)
				return static_cast<To>(
					static_cast<CommonUnderlying>(value) / static_cast<CommonUnderlying>(Ratio::den));
			if constexpr (Ratio::num != 1 && Ratio::den != 1)
				return static_cast<To>(
					(static_cast<CommonUnderlying>(value) * static_cast<CommonUnderlying>(Ratio::num)) /
					static_cast<CommonUnderlying>(Ratio::den));
		}
	}

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		SFINAE helper to prevent warnings in Clang 6 when `From` or `To` is a `conversion_factor`.
		 * @details		`typename T::conversion_factor` is interpreted as a constructor when `T` is a
		 *				`conversion_factor` (-Winjected-class-name).
		 */
		template<class UnitFrom, class UnitTo>
		struct delayed_is_convertible_conversion_factor : std::false_type
		{
			static constexpr bool value =
				traits::is_convertible_conversion_factor_v<typename UnitFrom::conversion_factor,
					typename UnitTo::conversion_factor>;
		};

		/**
		 * @brief		SFINAE helper to test that types are convertible `unit`s.
		 */
		template<class From, class To>
		inline constexpr bool is_convertible_unit = std::conjunction_v<traits::is_unit<From>, traits::is_unit<To>,
			delayed_is_convertible_conversion_factor<From, To>>;
	}               // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	/**
	 * @ingroup		Conversion
	 * @brief		converts a unit to another unit.
	 * @details		Converts the value of a unit to another unit. E.g. @code length::meter_t result =
	 *				convert<length::meters>(length::feet(1.0));	// result == 3.28084_m @endcode Intermediate
	 *				computations are carried in the widest representation before being converted to `UnitTo`.
	 * @sa			unit	for implicit conversion of unit containers.
	 * @tparam		UnitFrom unit to convert to `UnitTo`. Must be a `unit` type (i.e. is_unit<UnitFrom>::value == true),
	 *				and must be convertible to `UnitTo` (i.e. is_convertible_unit<UnitFrom, UnitTo>::value == true).
	 * @tparam		UnitTo unit to convert `from` to. Must be a `unit` type (i.e. is_unit<UnitTo>::value == true),
	 *				and must be convertible from `UnitFrom` (i.e. is_convertible_unit<UnitFrom, UnitTo>::value == true).
	 * @returns		from, converted from units of `UnitFrom` to `UnitTo`.
	 */
	template<class UnitTo, class UnitFrom, class = std::enable_if_t<detail::is_convertible_unit<UnitFrom, UnitTo>>>
	constexpr UnitTo convert(const UnitFrom& from) noexcept
	{
		return UnitTo(
			convert<typename UnitFrom::conversion_factor, typename UnitTo::conversion_factor,
				typename UnitTo::underlying_type>(from.template toLinearized<typename UnitFrom::underlying_type>()),
			std::true_type() /*store linear value*/);
	}

	//----------------------------------
	//	NON-LINEAR SCALE TRAITS
	//----------------------------------

	namespace traits
	{
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
				static constexpr auto test(U* p) -> decltype(p->m_value);
				template<typename>
				static constexpr auto test(...) -> std::false_type;

				using type = typename std::is_same<std::decay_t<Ret>, std::decay_t<decltype(test<T>(0))>>::type;
			};
		} // namespace detail

		/**
		 * @brief		checks for a member named `m_member` with type `Ret`
		 * @details		used as part of the linear_scale concept checker.
		 */
		template<class T, class Ret>
		using has_value_member = typename traits::detail::has_value_member_impl<T, Ret>::type;

		template<class T, class Ret>
		inline constexpr bool has_value_member_v = has_value_member<T, Ret>::value;
	}               // namespace traits
	/** @endcond */ // END DOXYGEN IGNORE

	namespace traits
	{
		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait which tests that `class T` meets the requirements for a non-linear scale
		 * @details		A non-linear scale must:
		 *				- be default constructible
		 *				- have an `operator()` member which returns the non-linear value stored in the scale
		 *				- have an accessible `m_value` data member which stores the linearized value in the scale.
		 *
		 *				Linear/nonlinear scales are used by `units::unit` to store values and scale them
		 *				if they represent things like dB.
		 */
		template<class T, class Ret>
		using is_nonlinear_scale = std::conjunction<std::is_default_constructible<T>, std::is_invocable_r<Ret, T>,
			has_value_member<T, Ret>, std::is_trivial<T>>;

		template<class T, class Ret>
		inline constexpr bool is_nonlinear_scale_v = is_nonlinear_scale<T, Ret>::value;
	} // namespace traits

	//------------------------------
	//	UNIT_T TYPE TRAITS
	//------------------------------

	namespace traits
	{
#ifdef FOR_DOXYGEN_PURPOSOES_ONLY
		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait for accessing the publically defined types of `units::unit`
		 * @details		The units library determines certain properties of the unit types passed to them
		 *				and what they represent by using the members of the corresponding unit_traits instantiation.
		 */
		template<typename T>
		struct unit_traits
		{
			typedef typename T::non_linear_scale_type
				non_linear_scale_type; ///< Type of the unit non_linear_scale (e.g. linear_scale, decibel_scale). This
									   ///< property is used to enable the proper linear or logarithmic arithmetic
									   ///< functions.
			typedef
				typename T::underlying_type underlying_type; ///< Underlying storage type of the `unit`, e.g. `double`.
			typedef typename T::value_type
				value_type; ///< Synonym for underlying type. May be removed in future versions. Prefer underlying_type.
			typedef
				typename T::conversion_factor conversion_factor; ///< Type of unit the `unit` represents, e.g. `meters`
		};
#endif

		/** @cond */ // DOXYGEN IGNORE
		/**
		 * @brief		unit_traits specialization for things which are not unit
		 * @details
		 */
		template<typename T, typename = void>
		struct unit_traits
		{
			using non_linear_scale_type = void;
			using underlying_type       = void;
			using value_type            = void;
			using conversion_factor     = void;
		};

		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait for accessing the publically defined types of `units::unit`
		 * @details
		 */
		template<typename T>
		struct unit_traits<T,
			std::void_t<typename T::non_linear_scale_type, typename T::underlying_type, typename T::value_type,
				typename T::conversion_factor>>
		{
			using non_linear_scale_type = typename T::non_linear_scale_type;
			using underlying_type       = typename T::underlying_type;
			using value_type            = typename T::value_type;
			using conversion_factor     = typename T::conversion_factor;
		};
		/** @endcond */ // END DOXYGEN IGNORE
	}                   // namespace traits

	namespace traits
	{
		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait which tests whether two container types derived from `unit` are convertible to each other
		 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_convertible_unit_v<U1, U2>` to test
		 *				whether `class U1` is convertible to `class U2`. Note: convertible has both the semantic
		 *				meaning, (i.e. meters can be converted to feet), and the c++ meaning of conversion (type meters
		 *				can be converted to type feet). Conversion is always symmetric, so if U1 is convertible to U2,
		 *				then U2 will be convertible to U1.
		 * @tparam		U1 Unit to convert from.
		 * @tparam		U2 Unit to convert to.
		 * @sa			is_convertible_conversion_factor
		 */
		template<class U1, class U2>
		using is_convertible_unit =
			is_convertible_conversion_factor<typename units::traits::unit_traits<U1>::conversion_factor,
				typename units::traits::unit_traits<U2>::conversion_factor>;

		template<class U1, class U2>
		inline constexpr bool is_convertible_unit_v = is_convertible_unit<U1, U2>::value;

	} // namespace traits

	//----------------------------------
	//	UNIT TYPE
	//----------------------------------

	/** @cond */ // DOXYGEN IGNORE
	// forward declaration
	template<typename T>
	struct linear_scale;
	template<typename T>
	struct decibel_scale;

	namespace detail
	{
		/**
		 * @brief		SFINAE helper to test if an arithmetic conversion is non lossy.
		 */
		template<class From, class To>
		inline constexpr bool is_non_lossy_convertible = std::is_arithmetic_v<From> &&
			(std::is_floating_point_v<To> || !std::is_floating_point_v<From>);

		/**
		 * @brief		Trait which tests if a unit type can be converted to another unit type without truncation error.
		 * @details		Valid only when the involved units have integral underlying types.
		 */
		template<class UnitConversionFrom, class UnitConversionTo>
		struct is_non_truncated_convertible_unit : std::false_type
		{
			static constexpr bool value = std::ratio_divide<typename UnitConversionFrom::conversion_ratio,
											  typename UnitConversionTo::conversion_ratio>::den == 1;
		};

		/**
		 * @brief		SFINAE helper to test if a conversion of units is non lossy.
		 */
		template<class UnitFrom, class UnitTo>
		inline constexpr bool is_non_lossy_convertible_unit = traits::is_convertible_unit_v<UnitFrom, UnitTo> &&
			(std::is_floating_point_v<typename UnitTo::underlying_type> ||
				std::conjunction_v<std::negation<std::is_floating_point<typename UnitFrom::underlying_type>>,
					detail::is_non_truncated_convertible_unit<typename UnitFrom::conversion_factor,
						typename UnitTo::conversion_factor>>);

		/**
		 * @brief		SFINAE helper to test if a `conversion_factor` is of the time dimension.
		 */
		template<class UnitConversion>
		inline constexpr bool is_time_conversion_factor = traits::is_convertible_conversion_factor_v<UnitConversion,
			conversion_factor<std::ratio<1>, dimension::time>>;

		/**
		 * @brief		helper type to identify units.
		 * @details		A non-templated base class for `unit` which enables compile-time testing.
		 */
		struct _unit
		{
		};
	}               // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	namespace traits
	{
		// forward declaration
		template<typename... T>
		struct is_dimensionless_unit;

		/**
		 * @ingroup		TypeTraits
		 * @brief		Traits which tests if a class is a `unit`
		 * @details		Inherits from `std::true_type` or `std::false_type`. Use `is_unit_v<T>` to test
		 *				whether `class T` implements a `unit`.
		 */
		template<class T>
		struct is_unit : std::is_base_of<units::detail::_unit, T>::type
		{
		};

		template<class T>
		inline constexpr bool is_unit_v = is_unit<T>::value;
	} // namespace traits

	/**
	 * @ingroup		UnitContainers
	 * @brief		Container for values which represent quantities of a given unit.
	 * @details		Stores a value which represents a quantity in the given units. Unit containers
	 *				(except dimensionless values) are *not* convertible to built-in c++ types, in order to
	 *				provide type safety in dimensional analysis. Unit containers *are* implicitly
	 *				convertible to other compatible unit container types. Unit containers support
	 *				various types of arithmetic operations, depending on their scale type.
	 *
	 *				The value of a `unit` can only be changed on construction, or by assignment
	 *				from another `unit` type. If necessary, the underlying value can be accessed
	 *				using `operator()`: @code
	 *				meter_t m(5.0);
	 *				double val = m(); // val == 5.0	@endcode.
	 * @tparam		UnitConversion unit tag for which type of units the `conversion_factor` represents (e.g. meters)
	 * @tparam		T underlying type of the storage. Defaults to double.
	 * @tparam		NonLinearScale optional scale class for the units. Defaults to linear (i.e. does
	 *				not scale the unit value). Examples of non-linear scales could be logarithmic,
	 *				decibel, or richter scales. Non-linear scales must adhere to the non-linear-scale
	 *				concept, i.e. `is_nonlinear_scale_v<...>` must be `true`.
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
	template<class UnitType, typename T = UNIT_LIB_DEFAULT_TYPE, template<typename> class NonLinearScale = linear_scale>
	class unit : public NonLinearScale<T>, units::detail::_unit
	{
		static_assert(traits::is_conversion_factor_v<UnitType>,
			"Template parameter `UnitType` must be a unit tag. Check that you aren't using a unit type (_t).");
		static_assert(traits::is_nonlinear_scale_v<NonLinearScale<T>, T>,
			"Template parameter `NonLinearScale` does not conform to the `is_nonlinear_scale` concept.");

	protected:
		using nls = NonLinearScale<T>;
		using nls::m_value;

	public:
		using non_linear_scale_type =
			NonLinearScale<T>;     ///< Type of the non-linear scale of the unit (e.g. linear_scale)
		using underlying_type = T; ///< Type of the underlying storage of the unit (e.g. double)
		using value_type =
			T; ///< Synonym for underlying type. May be removed in future versions. Prefer underlying_type.
		using conversion_factor = UnitType; ///< Type of `unit` the `unit` represents (e.g. meters)

		/**
		 * @ingroup		Constructors
		 * @brief		default constructor.
		 */
		constexpr unit() = default;

		/**
		 * @ingroup		Constructors
		 * @brief		default copy constructor.
		 */
		constexpr unit(const unit&) = default;

		/**
		 * @brief		constructor
		 * @details		constructs a new unit using the non-linear scale's constructor.
		 * @param[in]	value	unit value magnitude.
		 * @param[in]	args	additional constructor arguments are forwarded to the non-linear scale constructor.
		 *				Which args are required depends on which scale is used. For the default (linear) scale, no
		 *				additional args are necessary.
		 */
		template<class Ty, class... Args, class = std::enable_if_t<detail::is_non_lossy_convertible<Ty, T>>>
		explicit constexpr unit(const Ty value, const Args&... args) noexcept : nls(value, args...)
		{
		}

		/**
		 * @brief		constructor
		 * @details		enable implicit conversions from T types ONLY for linear dimensionless units
		 * @param[in]	value value of the unit
		 */
		template<class Ty,
			class = std::enable_if_t<traits::is_dimensionless_unit<UnitType>::value &&
				detail::is_non_lossy_convertible<Ty, T>>>
		constexpr unit(const Ty value) noexcept : nls(value)
		{
		}

		/**
		 * @brief		chrono constructor
		 * @details		enable implicit conversions from std::chrono::duration types ONLY for time units
		 * @param[in]	value value of the unit
		 */
		template<class Rep, class Period, typename U = UnitType,
			class = std::enable_if_t<detail::is_time_conversion_factor<U> && detail::is_non_lossy_convertible<Rep, T>>>
		constexpr unit(const std::chrono::duration<Rep, Period>& value) noexcept
		  : nls(units::convert<unit>(
				units::unit<units::conversion_factor<Period, dimension::time>, Rep>(value.count()))())
		{
		}

		/**
		 * @brief		converting constructor
		 * @details		performs implicit unit conversions if required.
		 * @param[in]	rhs unit to copy.
		 */
		template<class UnitTypeRhs, typename Ty, template<typename> class NlsRhs,
			class = std::enable_if_t<detail::is_non_lossy_convertible_unit<unit<UnitTypeRhs, Ty, NlsRhs>, unit>>>
		constexpr unit(const unit<UnitTypeRhs, Ty, NlsRhs>& rhs) noexcept
		  : nls(units::convert<unit>(rhs).m_value, std::true_type() /*store linear value*/)
		{
		}

		/**
		 * @brief		default assignment
		 * @details		performs implicit unit conversions if required.
		 * @param[in]	rhs unit to copy.
		 */
		constexpr unit& operator=(const unit& rhs) noexcept = default;

		/**
		 * @brief		assignment
		 * @details		performs implicit conversions from built-in types ONLY for dimensionless units
		 * @param[in]	rhs value to copy.
		 */
		template<class Ty,
			class = std::enable_if_t<traits::is_dimensionless_unit<UnitType>::value &&
				detail::is_non_lossy_convertible<Ty, T>>>
		constexpr unit& operator=(const Ty& rhs) noexcept
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
		template<class UnitTypeRhs, typename Ty, template<typename> class NlsRhs>
		constexpr bool operator<(const unit<UnitTypeRhs, Ty, NlsRhs>& rhs) const noexcept
		{
			using CommonUnit = std::common_type_t<unit, unit<UnitTypeRhs, Ty, NlsRhs>>;
			return (CommonUnit(*this).m_value < CommonUnit(rhs).m_value);
		}

		/**
		 * @brief		less-than or equal
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` is less than or equal to the value of `rhs`
		 */
		template<class UnitTypeRhs, typename Ty, template<typename> class NlsRhs>
		constexpr bool operator<=(const unit<UnitTypeRhs, Ty, NlsRhs>& rhs) const noexcept
		{
			using CommonUnit = std::common_type_t<unit, unit<UnitTypeRhs, Ty, NlsRhs>>;
			return (CommonUnit(*this).m_value <= CommonUnit(rhs).m_value);
		}

		/**
		 * @brief		greater-than
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` is greater than the value of `rhs`
		 */
		template<class UnitTypeRhs, typename Ty, template<typename> class NlsRhs>
		constexpr bool operator>(const unit<UnitTypeRhs, Ty, NlsRhs>& rhs) const noexcept
		{
			using CommonUnit = std::common_type_t<unit, unit<UnitTypeRhs, Ty, NlsRhs>>;
			return (CommonUnit(*this).m_value > CommonUnit(rhs).m_value);
		}

		/**
		 * @brief		greater-than or equal
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` is greater than or equal to the value of `rhs`
		 */
		template<class UnitTypeRhs, typename Ty, template<typename> class NlsRhs>
		constexpr bool operator>=(const unit<UnitTypeRhs, Ty, NlsRhs>& rhs) const noexcept
		{
			using CommonUnit = std::common_type_t<unit, unit<UnitTypeRhs, Ty, NlsRhs>>;
			return (CommonUnit(*this).m_value >= CommonUnit(rhs).m_value);
		}

		/**
		 * @brief		equality
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` exactly equal to the value of rhs.
		 * @note		This may not be suitable for all applications when the underlying_type of unit is a double.
		 */
		template<class UnitTypeRhs, typename Ty, template<typename> class NlsRhs>
		constexpr std::enable_if_t<std::is_floating_point_v<T> || std::is_floating_point_v<Ty>, bool> operator==(
			const unit<UnitTypeRhs, Ty, NlsRhs>& rhs) const noexcept
		{
			using CommonUnit       = std::common_type_t<unit, unit<UnitTypeRhs, Ty, NlsRhs>>;
			using CommonUnderlying = typename CommonUnit::underlying_type;

			const auto common_lhs(CommonUnit(*this).m_value);
			const auto common_rhs(CommonUnit(rhs).m_value);

			return abs(common_lhs - common_rhs) <
				std::numeric_limits<CommonUnderlying>::epsilon() * abs(common_lhs + common_rhs) ||
				abs(common_lhs - common_rhs) < std::numeric_limits<CommonUnderlying>::min();
		}

		template<class UnitTypeRhs, typename Ty, template<typename> class NlsRhs>
		constexpr std::enable_if_t<std::is_integral<T>::value && std::is_integral<Ty>::value, bool> operator==(
			const unit<UnitTypeRhs, Ty, NlsRhs>& rhs) const noexcept
		{
			using CommonUnit = std::common_type_t<unit, unit<UnitTypeRhs, Ty, NlsRhs>>;
			return CommonUnit(*this).m_value == CommonUnit(rhs).m_value;
		}

		/**
		 * @brief		inequality
		 * @details		compares the linearized value of two units. Performs unit conversions if necessary.
		 * @param[in]	rhs right-hand side unit for the comparison
		 * @returns		true IFF the value of `this` is not equal to the value of rhs.
		 * @note		This may not be suitable for all applications when the underlying_type of unit is a double.
		 */
		template<class UnitTypeRhs, typename Ty, template<typename> class NlsRhs>
		constexpr bool operator!=(const unit<UnitTypeRhs, Ty, NlsRhs>& rhs) const noexcept
		{
			return !(*this == rhs);
		}

		/**
		 * @brief		unit value
		 * @returns		value of the unit in it's underlying, non-safe type.
		 */
		constexpr underlying_type value() const noexcept
		{
			return static_cast<underlying_type>(*this);
		}

		/**
		 * @brief		unit value
		 * @returns		value of the unit converted to an arithmetic, non-safe type.
		 */
		template<typename Ty, class = std::enable_if_t<std::is_arithmetic_v<Ty>>>
		constexpr Ty to() const noexcept
		{
			return static_cast<Ty>(*this);
		}

		/**
		 * @brief		linearized unit value
		 * @returns		linearized value of unit which has a non-linear scale. For `unit` types with
		 *				linear scales, this is equivalent to `value`.
		 */
		template<typename Ty, class = std::enable_if_t<std::is_arithmetic_v<Ty>>>
		constexpr Ty toLinearized() const noexcept
		{
			return static_cast<Ty>(m_value);
		}

		/**
		 * @brief		conversion
		 * @details		Converts to a different unit container. UnitType can be converted to other containers
		 *				implicitly, but this can be used in cases where explicit notation of a conversion
		 *				is beneficial, or where an r-value container is needed.
		 * @tparam		U unit (not unit) to convert to
		 * @tparam		Ty underlying type to convert to
		 * @returns		a unit container with the specified units containing the equivalent value to
		 *				*this.
		 */
		template<class U, typename Ty = T>
		constexpr unit<U, Ty> convert() const noexcept
		{
			static_assert(traits::is_conversion_factor_v<U>, "Template parameter `U` must be a unit tag type.");
			return unit<U, Ty>(*this);
		}

		/**
		 * @brief		implicit type conversion.
		 * @details		only enabled for dimensionless unit types.
		 */
		template<class Ty,
			std::enable_if_t<traits::is_dimensionless_unit<UnitType>::value && std::is_arithmetic<Ty>::value, int> = 0>
		constexpr operator Ty() const noexcept
		{
			// this conversion also resolves any PI exponents, by converting from a non-zero PI ratio to a zero-pi
			// ratio.
			return units::convert<units::unit<units::conversion_factor<std::ratio<1>, units::dimension::dimensionless>,
				Ty, NonLinearScale>>(*this)();
		}

		/**
		 * @brief		explicit type conversion.
		 * @details		only enabled for non-dimensionless unit types.
		 */
		template<class Ty,
			std::enable_if_t<!traits::is_dimensionless_unit<UnitType>::value && std::is_arithmetic<Ty>::value, int> = 0>
		constexpr explicit operator Ty() const noexcept
		{
			return static_cast<Ty>((*this)());
		}

		/**
		 * @brief		chrono implicit type conversion.
		 * @details		only enabled for time unit types.
		 */
		template<class Rep, class Period, typename U = UnitType,
			std::enable_if_t<detail::is_time_conversion_factor<U> && detail::is_non_lossy_convertible<T, Rep>, int> = 0>
		constexpr operator std::chrono::duration<Rep, Period>() const noexcept
		{
			return std::chrono::duration<Rep, Period>(
				units::unit<units::conversion_factor<Period, dimension::time>, Rep>(*this)());
		}

		/**
		 * @brief		returns the unit name
		 */
		template<class Unit = unit>
		constexpr const char* name() const noexcept
		{
			return unit_name_v<Unit>;
		}

		/**
		 * @brief		returns the unit abbreviation
		 */
		template<class Unit = unit>
		constexpr const char* abbreviation() const noexcept
		{
			return unit_abbreviation_v<Unit>;
		}

	private:
		template<class U, typename Ty, template<typename> class Nlt>
		friend class unit;
	};

	//------------------------------
	//	UNIT_T NON-MEMBER FUNCTIONS
	//------------------------------

	/**
	 * @ingroup		UnitContainers
	 * @brief		Constructs a unit container from an arithmetic type.
	 * @details		make_unit can be used to construct a unit container from an arithmetic type, as an alternative to
	 *				using the explicit constructor. Unlike the explicit constructor it forces the user to explicitly
	 *				specify the units.
	 * @tparam		UnitType Type to construct.
	 * @tparam		T		Arithmetic type.
	 * @param[in]	value	Arithmetic value that represents a quantity in units of `UnitType`.
	 */
	template<class UnitType, typename T,
		class = std::enable_if_t<detail::is_non_lossy_convertible<T, typename UnitType::underlying_type>>>
	constexpr UnitType make_unit(const T value) noexcept
	{
		static_assert(traits::is_unit_v<UnitType>, "Template parameter `UnitType` must be a unit type.");
		return UnitType(value);
	}

#if !defined(UNIT_LIB_DISABLE_IOSTREAM)

	//-----------------------------------------
	//	OSTREAM OPERATOR FOR EPHEMERAL UNITS
	//-----------------------------------------

	template<class D, class E>
	std::ostream& operator<<(std::ostream& os, const dim<D, E>&)
	{
		if constexpr (E::num != 0)
			os << ' ' << D::abbreviation;
		if constexpr (E::num != 0 && E::num != 1)
		{
			os << "^" << E::num;
		}
		if constexpr (E::den != 1)
		{
			os << "/" << E::den;
		}
		return os;
	}

	inline std::ostream& operator<<(std::ostream& os, const dimension_t<>&)
	{
		return os;
	}

	template<class Dim, class... Dims>
	std::ostream& operator<<(std::ostream& os, const dimension_t<Dim, Dims...>&)
	{
		os << Dim{};
		os << dimension_t<Dims...>{};
		return os;
	}

	template<class UnitConversion, typename T, template<typename> class NonLinearScale>
	std::ostream& operator<<(std::ostream& os, const unit<UnitConversion, T, NonLinearScale>& obj)
	{
		using BaseConversion   = conversion_factor<std::ratio<1>, typename UnitConversion::dimension_type>;
		using BaseUnit         = unit<BaseConversion, T, NonLinearScale>;
		using PromotedBaseUnit = unit<BaseConversion, detail::floating_point_promotion_t<T>, NonLinearScale>;

		os << std::conditional_t<detail::is_non_lossy_convertible_unit<std::decay_t<decltype(obj)>, BaseUnit>, BaseUnit,
			PromotedBaseUnit>(obj)();

		using DimType = typename traits::dimension_of_t<UnitConversion>;
		if constexpr (!DimType::empty)
		{
			os << DimType{};
		}

		return os;
	}
#endif

	//------------------------------
	//	std::common_type
	//------------------------------

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		greatest common divisor of two ratios.
		 */
		template<class Ratio1, class Ratio2>
		using ratio_gcd = std::ratio<std::gcd(Ratio1::num, Ratio2::num), std::lcm(Ratio1::den, Ratio2::den)>;
	}               // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE
} // end namespace units

namespace std
{
	/**
	 * @ingroup		STDTypeTraits
	 * @brief		common type of units
	 * @details		The `type` alias of the `std::common_type` of two `unit`s of the same dimension is the least precise
	 *				`unit` to which both `unit` arguments can be converted to without requiring a division operation or
	 *				truncating any value of these conversions, although floating-point units may have round-off errors.
	 *				If the units have mixed scales, preference is given to `linear_scale` for their common type.
	 */
	template<class UnitConversionLhs, class Tx, class UnitConversionRhs, class Ty,
		template<typename> class NonLinearScale>
	struct common_type<units::unit<UnitConversionLhs, Tx, NonLinearScale>,
		units::unit<UnitConversionRhs, Ty, NonLinearScale>>
	  : std::enable_if<units::traits::is_convertible_conversion_factor_v<UnitConversionLhs, UnitConversionRhs>,
			units::unit<units::traits::strong_t<units::conversion_factor<
							units::detail::ratio_gcd<typename UnitConversionLhs::conversion_ratio,
								typename UnitConversionRhs::conversion_ratio>,
							units::traits::dimension_of_t<UnitConversionLhs>,
							units::detail::ratio_gcd<typename UnitConversionLhs::pi_exponent_ratio,
								typename UnitConversionRhs::pi_exponent_ratio>,
							units::detail::ratio_gcd<typename UnitConversionLhs::translation_ratio,
								typename UnitConversionRhs::translation_ratio>>>,
				common_type_t<Tx, Ty>, NonLinearScale>>
	{
	};

	// In the case the two units are the same type, just use that type as common type
	// instead of some dummy conversion_factor of 1.
	template<class UnitConversionT, class T,
		template<typename> class NonLinearScale>
	struct common_type<units::unit<UnitConversionT, T, NonLinearScale>,
		units::unit<UnitConversionT, T, NonLinearScale>>
	{
		using type = units::unit<UnitConversionT, T, NonLinearScale>;
	};

	/** @cond */ // DOXYGEN IGNORE
	/**
	 * @brief		`linear_scale` preferring specializations.
	 */
	template<class UnitConversionLhs, class Tx, class UnitConversionRhs, class Ty>
	struct common_type<units::unit<UnitConversionLhs, Tx, units::linear_scale>,
		units::unit<UnitConversionRhs, Ty, units::decibel_scale>>
	  : common_type<units::unit<UnitConversionLhs, Tx, units::linear_scale>,
			units::unit<UnitConversionRhs, Ty, units::linear_scale>>
	{
	};

	template<class UnitConversionLhs, class Tx, class UnitConversionRhs, class Ty>
	struct common_type<units::unit<UnitConversionLhs, Tx, units::decibel_scale>,
		units::unit<UnitConversionRhs, Ty, units::linear_scale>>
	  : common_type<units::unit<UnitConversionLhs, Tx, units::linear_scale>,
			units::unit<UnitConversionRhs, Ty, units::linear_scale>>
	{
	};
	/** @endcond */ // END DOXYGEN IGNORE
} // namespace std

namespace units
{
	//----------------------------------------
	//	UNIT_T COMPOUND ASSIGNMENT OPERATORS
	//----------------------------------------

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/**
		 * @brief		Helper to make the use of a template parameter a non-deduced context.
		 */
		template<class T>
		struct type_identity
		{
			using type = T;
		};

		template<class T>
		using type_identity_t = typename type_identity<T>::type;
	}               // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	template<class UnitConversion, typename T, template<typename> class NonLinearScale>
	constexpr unit<UnitConversion, T, NonLinearScale>& operator+=(unit<UnitConversion, T, NonLinearScale>& lhs,
		const detail::type_identity_t<unit<UnitConversion, T, NonLinearScale>>& rhs) noexcept
	{
		lhs = lhs + rhs;
		return lhs;
	}

	template<class UnitConversion, typename T, template<typename> class NonLinearScale>
	constexpr unit<UnitConversion, T, NonLinearScale>& operator-=(unit<UnitConversion, T, NonLinearScale>& lhs,
		const detail::type_identity_t<unit<UnitConversion, T, NonLinearScale>>& rhs) noexcept
	{
		lhs = lhs - rhs;
		return lhs;
	}

	template<class UnitConversion, typename T, template<typename> class NonLinearScale>
	constexpr unit<UnitConversion, T, NonLinearScale>& operator*=(
		unit<UnitConversion, T, NonLinearScale>& lhs, const detail::type_identity_t<T>& rhs) noexcept
	{
		lhs = lhs * rhs;
		return lhs;
	}

	template<class UnitConversion, typename T, template<typename> class NonLinearScale>
	constexpr unit<UnitConversion, T, NonLinearScale>& operator/=(
		unit<UnitConversion, T, NonLinearScale>& lhs, const detail::type_identity_t<T>& rhs) noexcept
	{
		lhs = lhs / rhs;
		return lhs;
	}

	template<class UnitConversion, typename T, template<typename> class NonLinearScale>
	constexpr unit<UnitConversion, T, NonLinearScale>& operator%=(unit<UnitConversion, T, NonLinearScale>& lhs,
		const detail::type_identity_t<unit<UnitConversion, T, NonLinearScale>>& rhs) noexcept
	{
		lhs = lhs % rhs;
		return lhs;
	}

	template<class UnitConversionLhs, typename T, template<typename> class NonLinearScaleLhs, class UnitConversionRhs,
		template<typename> class NonLinearScaleRhs,
		class = std::enable_if_t<traits::is_dimensionless_unit<UnitConversionRhs>::value>>
	constexpr unit<UnitConversionLhs, T, NonLinearScaleLhs>& operator%=(
		unit<UnitConversionLhs, T, NonLinearScaleLhs>& lhs,
		const unit<UnitConversionRhs, detail::type_identity_t<T>, NonLinearScaleRhs>& rhs) noexcept
	{
		lhs = lhs % rhs;
		return lhs;
	}

	template<class UnitConversion, typename T, template<typename> class NonLinearScale>
	constexpr unit<UnitConversion, T, NonLinearScale>& operator%=(
		unit<UnitConversion, T, NonLinearScale>& lhs, const detail::type_identity_t<T>& rhs) noexcept
	{
		lhs = lhs % rhs;
		return lhs;
	}

	//------------------------------
	//	UNIT_T UNARY OPERATORS
	//------------------------------

	// unary addition: +T
	template<class UnitConversion, typename T, template<typename> class NonLinearScale>
	constexpr unit<UnitConversion, T, NonLinearScale> operator+(
		const unit<UnitConversion, T, NonLinearScale>& u) noexcept
	{
		return u;
	}

	// prefix increment: ++T
	template<class UnitConversion, typename T, template<typename> class NonLinearScale>
	constexpr unit<UnitConversion, T, NonLinearScale>& operator++(unit<UnitConversion, T, NonLinearScale>& u) noexcept
	{
		u = unit<UnitConversion, T, NonLinearScale>(u() + 1);
		return u;
	}

	// postfix increment: T++
	template<class UnitConversion, typename T, template<typename> class NonLinearScale>
	constexpr unit<UnitConversion, T, NonLinearScale> operator++(
		unit<UnitConversion, T, NonLinearScale>& u, int) noexcept
	{
		auto ret = u;
		u        = unit<UnitConversion, T, NonLinearScale>(u() + 1);
		return ret;
	}

	// unary addition: -T
	template<class UnitConversion, typename T, template<typename> class NonLinearScale>
	constexpr unit<UnitConversion, T, NonLinearScale> operator-(
		const unit<UnitConversion, T, NonLinearScale>& u) noexcept
	{
		return unit<UnitConversion, T, NonLinearScale>(-u());
	}

	// prefix increment: --T
	template<class UnitConversion, typename T, template<typename> class NonLinearScale>
	constexpr unit<UnitConversion, T, NonLinearScale>& operator--(unit<UnitConversion, T, NonLinearScale>& u) noexcept
	{
		u = unit<UnitConversion, T, NonLinearScale>(u() - 1);
		return u;
	}

	// postfix increment: T--
	template<class UnitConversion, typename T, template<typename> class NonLinearScale>
	constexpr unit<UnitConversion, T, NonLinearScale> operator--(
		unit<UnitConversion, T, NonLinearScale>& u, int) noexcept
	{
		auto ret = u;
		u        = unit<UnitConversion, T, NonLinearScale>(u() - 1);
		return ret;
	}

	//------------------------------
	//	UNIT_CAST
	//------------------------------

	/**
	 * @ingroup		Conversion
	 * @brief		Casts a unit container to an arithmetic type.
	 * @details		unit_cast can be used to remove the strong typing from a unit class, and convert it
	 *				to a built-in arithmetic type. This may be useful for compatibility with libraries
	 *				and legacy code that don't support `unit` types. E.g
	 * @code		meter_t unitVal(5);
	 *				double value = units::unit_cast<double>(unitVal);	// value == 5.0
	 * @endcode
	 * @tparam		T		Type to cast the unit type to. Must be a built-in arithmetic type.
	 * @param		value	Unit value to cast.
	 * @sa			unit::to
	 */
	template<typename T, typename UnitConversion>
	constexpr std::enable_if_t<std::is_arithmetic_v<T> && traits::is_unit_v<UnitConversion>, T> unit_cast(
		const UnitConversion& value) noexcept
	{
		return static_cast<T>(value);
	}

	//------------------------------
	//	NON-LINEAR SCALE TRAITS
	//------------------------------

	// forward declaration
	template<typename T>
	struct decibel_scale;

	namespace traits
	{
		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait which tests whether a type is inherited from a linear scale.
		 * @details		Inherits from `std::true_type` or `std::false_type`. Use `has_linear_scale_v<U1 [, U2, ...]>` to
		 *				test one or more types to see if they represent units whose scale is linear.
		 * @tparam		T	one or more types to test.
		 */
		template<typename... T>
		struct has_linear_scale
		  : std::conjunction<
				std::is_base_of<units::linear_scale<typename units::traits::unit_traits<T>::underlying_type>, T>...>
		{
		};

		template<typename... T>
		inline constexpr bool has_linear_scale_v = has_linear_scale<T...>::value;

		/**
		 * @ingroup		TypeTraits
		 * @brief		Trait which tests whether a type is inherited from a decibel scale.
		 * @details		Inherits from `std::true_type` or `std::false_type`. Use `has_decibel_scale_v<U1 [, U2, ...]>`
		 *				to test one or more types to see if they represent units whose scale is in decibels.
		 * @tparam		T	one or more types to test.
		 */
		template<typename... T>
		struct has_decibel_scale
		  : std::conjunction<
				std::is_base_of<units::decibel_scale<typename units::traits::unit_traits<T>::underlying_type>, T>...>
		{
		};

		template<typename... T>
		inline constexpr bool has_decibel_scale_v = has_decibel_scale<T...>::value;

	} // namespace traits

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
	 * @brief		unit scale which is linear
	 * @details		Represents units on a linear scale. This is the appropriate unit scale for almost
	 *				all units almost all of the time.
	 * @tparam		T	underlying storage type
	 * @sa			unit
	 */
	template<typename T>
	struct linear_scale
	{
		constexpr linear_scale()                    = default; ///< default constructor.
		constexpr linear_scale(const linear_scale&) = default;
		~linear_scale()                             = default;
		linear_scale& operator=(const linear_scale&) = default;
		constexpr linear_scale(linear_scale&&)       = default;
		linear_scale& operator=(linear_scale&&) = default;

		template<class... Args>
		constexpr linear_scale(const T& value, Args&&...) noexcept : m_value(value)
		{
		} ///< constructor.
		constexpr T operator()() const noexcept
		{
			return m_value;
		} ///< returns value.

		T m_value; ///< linearized value.
	};

	//----------------------------------
	//	dimensionless (LINEAR) UNITS
	//----------------------------------

	// dimensionless units are the *ONLY* units implicitly convertible to/from built-in types.
	struct dimensionless_unit : conversion_factor<std::ratio<1>, units::dimension::dimensionless>
	{
	};

	template<class Underlying>
	using dimensionless = unit<dimensionless_unit, Underlying>;

	namespace traits
	{
		template<>
		struct strong<units::detail::conversion_factor_base_t<dimensionless_unit>>
		{
			using type = dimensionless_unit;
		};
	} // namespace traits

// ignore the redeclaration of the default template parameters
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4348)
#endif
	UNIT_ADD_DIMENSION_TRAIT(dimensionless)
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

	//------------------------------
	//	LINEAR ARITHMETIC
	//------------------------------

	/// Addition operator for unit types with a linear_scale.
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::is_convertible_unit_v<UnitTypeLhs, UnitTypeRhs> &&
				traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>,
			int> = 0>
	constexpr std::common_type_t<UnitTypeLhs, UnitTypeRhs> operator+(
		const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = std::common_type_t<UnitTypeLhs, UnitTypeRhs>;
		return CommonUnit(CommonUnit(lhs)() + CommonUnit(rhs)());
	}

	/// Addition operator for dimensionless unit types with a linear_scale. dimensionless types can be implicitly
	/// converted to built-in types.
	template<class UnitTypeLhs, typename T,
		std::enable_if_t<std::is_arithmetic_v<T> && traits::has_linear_scale_v<UnitTypeLhs> &&
				traits::is_dimensionless_unit_v<UnitTypeLhs>,
			int> = 0>
	constexpr unit<dimensionless_unit, std::common_type_t<typename UnitTypeLhs::underlying_type, T>> operator+(
		const UnitTypeLhs& lhs, T rhs) noexcept
	{
		using CommonUnit = unit<dimensionless_unit, std::common_type_t<typename UnitTypeLhs::underlying_type, T>>;
		return CommonUnit(CommonUnit(lhs)() + rhs);
	}

	/// Addition operator for dimensionless unit types with a linear_scale. dimensionless types can be implicitly
	/// converted to built-in types.
	template<class UnitTypeRhs, typename T,
		std::enable_if_t<std::is_arithmetic_v<T> && traits::has_linear_scale_v<UnitTypeRhs> &&
				traits::is_dimensionless_unit_v<UnitTypeRhs>,
			int> = 0>
	constexpr unit<dimensionless_unit, std::common_type_t<T, typename UnitTypeRhs::underlying_type>> operator+(
		T lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = unit<dimensionless_unit, std::common_type_t<T, typename UnitTypeRhs::underlying_type>>;
		return CommonUnit(lhs + CommonUnit(rhs)());
	}

	/// Subtraction operator for unit types with a linear_scale.
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::is_convertible_unit_v<UnitTypeLhs, UnitTypeRhs> &&
				traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>,
			int> = 0>
	constexpr std::common_type_t<UnitTypeLhs, UnitTypeRhs> operator-(
		const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = std::common_type_t<UnitTypeLhs, UnitTypeRhs>;
		return CommonUnit(CommonUnit(lhs)() - CommonUnit(rhs)());
	}

	/// Subtraction operator for dimensionless unit types with a linear_scale. dimensionless types can be implicitly
	/// converted to built-in types.
	template<class UnitTypeLhs, typename T,
		std::enable_if_t<std::is_arithmetic_v<T> && traits::has_linear_scale_v<UnitTypeLhs> &&
				traits::is_dimensionless_unit_v<UnitTypeLhs>,
			int> = 0>
	constexpr unit<dimensionless_unit, std::common_type_t<typename UnitTypeLhs::underlying_type, T>> operator-(
		const UnitTypeLhs& lhs, T rhs) noexcept
	{
		using CommonUnit = unit<dimensionless_unit, std::common_type_t<typename UnitTypeLhs::underlying_type, T>>;
		return CommonUnit(CommonUnit(lhs)() - rhs);
	}

	/// Subtraction operator for dimensionless unit types with a linear_scale. dimensionless types can be implicitly
	/// converted to built-in types.
	template<class UnitTypeRhs, typename T,
		std::enable_if_t<std::is_arithmetic_v<T> && traits::has_linear_scale_v<UnitTypeRhs> &&
				traits::is_dimensionless_unit_v<UnitTypeRhs>,
			int> = 0>
	constexpr unit<dimensionless_unit, std::common_type_t<T, typename UnitTypeRhs::underlying_type>> operator-(
		T lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = unit<dimensionless_unit, std::common_type_t<T, typename UnitTypeRhs::underlying_type>>;
		return CommonUnit(lhs - CommonUnit(rhs)());
	}

	/// Multiplication type for convertible unit types with a linear scale. @returns the multiplied value, with the same
	/// type as left-hand side unit.
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::is_convertible_unit_v<UnitTypeLhs, UnitTypeRhs> &&
				traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>,
			int> = 0>
	constexpr auto operator*(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
		-> unit<traits::strong_t<squared<typename units::traits::unit_traits<
					std::common_type_t<UnitTypeLhs, UnitTypeRhs>>::conversion_factor>>,
			typename std::common_type_t<UnitTypeLhs, UnitTypeRhs>::underlying_type>
	{
		using CommonUnit = std::common_type_t<UnitTypeLhs, UnitTypeRhs>;
		return unit<traits::strong_t<squared<typename units::traits::unit_traits<CommonUnit>::conversion_factor>>,
			typename CommonUnit::underlying_type>(CommonUnit(lhs)() * CommonUnit(rhs)());
	}

	/// Multiplication type for non-convertible unit types with a linear scale. @returns the multiplied value, whose
	/// type is a compound unit of the left and right hand side values.
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<!traits::is_convertible_unit_v<UnitTypeLhs, UnitTypeRhs> &&
				traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs> && !traits::is_dimensionless_unit_v<UnitTypeLhs> &&
				!traits::is_dimensionless_unit_v<UnitTypeRhs>,
			int> = 0>
	constexpr auto operator*(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept -> unit<
		traits::strong_t<compound_conversion_factor<typename units::traits::unit_traits<UnitTypeLhs>::conversion_factor,
			typename units::traits::unit_traits<UnitTypeRhs>::conversion_factor>>,
		std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>>
	{
		using UnitConversionLhs = typename units::traits::unit_traits<UnitTypeLhs>::conversion_factor;
		using UnitConversionRhs = typename units::traits::unit_traits<UnitTypeRhs>::conversion_factor;
		using CommonUnderlying =
			std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>;
		return unit<traits::strong_t<compound_conversion_factor<UnitConversionLhs, UnitConversionRhs>>,
			CommonUnderlying>(static_cast<CommonUnderlying>(lhs) * static_cast<CommonUnderlying>(rhs));
	}

	/// Multiplication by a dimensionless unit for unit types with a linear scale.
	template<class UnitTypeLhs, typename UnitTypeRhs,
		std::enable_if_t<traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs> &&
				!traits::is_dimensionless_unit_v<UnitTypeLhs> && traits::is_dimensionless_unit_v<UnitTypeRhs>,
			int> = 0>
	constexpr unit<typename UnitTypeLhs::conversion_factor,
		std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>>
	operator*(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnderlying =
			std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>;
		using CommonUnit = unit<typename UnitTypeLhs::conversion_factor, CommonUnderlying>;
		// the cast makes sure factors of PI are handled as expected
		return CommonUnit(CommonUnit(lhs)() * static_cast<CommonUnderlying>(rhs));
	}

	/// Multiplication by a dimensionless unit for unit types with a linear scale.
	template<class UnitTypeLhs, typename UnitTypeRhs,
		std::enable_if_t<traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs> &&
				traits::is_dimensionless_unit_v<UnitTypeLhs> && !traits::is_dimensionless_unit_v<UnitTypeRhs>,
			int> = 0>
	constexpr unit<typename UnitTypeRhs::conversion_factor,
		std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>>
	operator*(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnderlying =
			std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>;
		using CommonUnit = unit<typename UnitTypeRhs::conversion_factor, CommonUnderlying>;
		// the cast makes sure factors of PI are handled as expected
		return CommonUnit(static_cast<CommonUnderlying>(lhs) * CommonUnit(rhs)());
	}

	/// Multiplication by a dimensionless for unit types with a linear scale.
	template<class UnitTypeLhs, typename T,
		std::enable_if_t<std::is_arithmetic_v<T> && traits::has_linear_scale_v<UnitTypeLhs>, int> = 0>
	constexpr unit<typename UnitTypeLhs::conversion_factor,
		std::common_type_t<typename UnitTypeLhs::underlying_type, T>>
	operator*(const UnitTypeLhs& lhs, T rhs) noexcept
	{
		using CommonUnit =
			unit<typename UnitTypeLhs::conversion_factor, std::common_type_t<typename UnitTypeLhs::underlying_type, T>>;
		return CommonUnit(CommonUnit(lhs)() * rhs);
	}

	/// Multiplication by a dimensionless for unit types with a linear scale.
	template<class UnitTypeRhs, typename T,
		std::enable_if_t<std::is_arithmetic_v<T> && traits::has_linear_scale_v<UnitTypeRhs>, int> = 0>
	constexpr unit<typename UnitTypeRhs::conversion_factor,
		std::common_type_t<T, typename UnitTypeRhs::underlying_type>>
	operator*(T lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit =
			unit<typename UnitTypeRhs::conversion_factor, std::common_type_t<T, typename UnitTypeRhs::underlying_type>>;
		return CommonUnit(lhs * CommonUnit(rhs)());
	}

	/// Division for convertible unit types with a linear scale. @returns the lhs divided by rhs value, whose type is a
	/// dimensionless
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::is_convertible_unit_v<UnitTypeLhs, UnitTypeRhs> &&
				traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>,
			int> = 0>
	constexpr dimensionless<
		std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>>
	operator/(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = std::common_type_t<UnitTypeLhs, UnitTypeRhs>;
		return unit<dimensionless_unit, typename CommonUnit::underlying_type>(CommonUnit(lhs)() / CommonUnit(rhs)());
	}

	/// Division for non-convertible unit types with a linear scale. @returns the lhs divided by the rhs, with a
	/// compound unit type of lhs/rhs
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<!traits::is_convertible_unit_v<UnitTypeLhs, UnitTypeRhs> &&
				traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs> && !traits::is_dimensionless_unit_v<UnitTypeLhs> &&
				!traits::is_dimensionless_unit_v<UnitTypeRhs>,
			int> = 0>
	constexpr auto operator/(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept -> unit<
		traits::strong_t<compound_conversion_factor<typename units::traits::unit_traits<UnitTypeLhs>::conversion_factor,
			inverse<typename units::traits::unit_traits<UnitTypeRhs>::conversion_factor>>>,
		std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>>
	{
		using UnitConversionLhs = typename units::traits::unit_traits<UnitTypeLhs>::conversion_factor;
		using UnitConversionRhs = typename units::traits::unit_traits<UnitTypeRhs>::conversion_factor;
		using CommonUnderlying =
			std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>;
		return unit<traits::strong_t<compound_conversion_factor<UnitConversionLhs, inverse<UnitConversionRhs>>>,
			CommonUnderlying>(static_cast<CommonUnderlying>(lhs) / static_cast<CommonUnderlying>(rhs));
	}

	/// Division by a dimensionless unit for unit types with a linear scale
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs> &&
				!traits::is_dimensionless_unit_v<UnitTypeLhs> && traits::is_dimensionless_unit_v<UnitTypeRhs>,
			int> = 0>
	constexpr unit<typename UnitTypeLhs::conversion_factor,
		std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>>
	operator/(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnderlying =
			std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>;
		using CommonUnit = unit<typename UnitTypeLhs::conversion_factor, CommonUnderlying>;
		return CommonUnit(CommonUnit(lhs)() / static_cast<CommonUnderlying>(rhs));
	}

	/// Division of a dimensionless unit  by a unit type with a linear scale
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs> &&
				traits::is_dimensionless_unit_v<UnitTypeLhs> && !traits::is_dimensionless_unit_v<UnitTypeRhs>,
			int> = 0>
	constexpr auto operator/(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
		-> unit<traits::strong_t<inverse<typename units::traits::unit_traits<UnitTypeRhs>::conversion_factor>>,
			std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>>
	{
		using CommonUnderlying =
			std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>;
		return unit<traits::strong_t<inverse<typename units::traits::unit_traits<UnitTypeRhs>::conversion_factor>>,
			CommonUnderlying>(static_cast<CommonUnderlying>(lhs) / static_cast<CommonUnderlying>(rhs));
	}

	/// Division by a dimensionless for unit types with a linear scale
	template<class UnitTypeLhs, typename T,
		std::enable_if_t<std::is_arithmetic_v<T> && traits::has_linear_scale_v<UnitTypeLhs>, int> = 0>
	constexpr unit<typename UnitTypeLhs::conversion_factor,
		std::common_type_t<typename UnitTypeLhs::underlying_type, T>>
	operator/(const UnitTypeLhs& lhs, T rhs) noexcept
	{
		using CommonUnit =
			unit<typename UnitTypeLhs::conversion_factor, std::common_type_t<typename UnitTypeLhs::underlying_type, T>>;
		return CommonUnit(CommonUnit(lhs)() / rhs);
	}

	/// Division of a dimensionless  by a unit type with a linear scale
	template<class UnitTypeRhs, typename T,
		std::enable_if_t<std::is_arithmetic_v<T> && traits::has_linear_scale_v<UnitTypeRhs>, int> = 0>
	constexpr auto operator/(T lhs, const UnitTypeRhs& rhs) noexcept
		-> unit<traits::strong_t<inverse<typename units::traits::unit_traits<UnitTypeRhs>::conversion_factor>>,
			std::common_type_t<T, typename UnitTypeRhs::underlying_type>>
	{
		using UnitConversionRhs = typename units::traits::unit_traits<UnitTypeRhs>::conversion_factor;
		using CommonUnderlying  = std::common_type_t<T, typename UnitTypeRhs::underlying_type>;
		using CommonUnitRhs     = unit<UnitConversionRhs, CommonUnderlying>;
		return unit<traits::strong_t<inverse<UnitConversionRhs>>, CommonUnderlying>(lhs / CommonUnitRhs(rhs)());
	}

	/// Modulo for convertible unit types with a linear scale. @returns the lhs value modulo the rhs value, whose type
	/// is their common type
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::is_convertible_unit_v<UnitTypeLhs, UnitTypeRhs> &&
				traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>,
			int> = 0>
	constexpr std::common_type_t<UnitTypeLhs, UnitTypeRhs> operator%(
		const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnit = std::common_type_t<UnitTypeLhs, UnitTypeRhs>;
		return CommonUnit(CommonUnit(lhs)() % CommonUnit(rhs)());
	}

	/// Modulo by a dimensionless for unit types with a linear scale
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs> &&
				!traits::is_dimensionless_unit_v<UnitTypeLhs> && traits::is_dimensionless_unit_v<UnitTypeRhs>,
			int> = 0>
	constexpr unit<typename UnitTypeLhs::conversion_factor,
		std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>>
	operator%(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnderlying =
			std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>;
		using CommonUnit = unit<typename UnitTypeLhs::conversion_factor, CommonUnderlying>;
		return CommonUnit(CommonUnit(lhs)() % static_cast<CommonUnderlying>(rhs));
	}

	/// Modulo by a dimensionless for unit types with a linear scale
	template<class UnitTypeLhs, typename T,
		std::enable_if_t<std::is_arithmetic_v<T> && traits::has_linear_scale_v<UnitTypeLhs>, int> = 0>
	constexpr unit<typename UnitTypeLhs::conversion_factor,
		std::common_type_t<typename UnitTypeLhs::underlying_type, T>>
	operator%(const UnitTypeLhs& lhs, const T& rhs) noexcept
	{
		using CommonUnit =
			unit<typename UnitTypeLhs::conversion_factor, std::common_type_t<typename UnitTypeLhs::underlying_type, T>>;
		return CommonUnit(CommonUnit(lhs)() % rhs);
	}

	//----------------------------------
	//	DIMENSIONLESS COMPARISONS
	//----------------------------------

	template<typename UnitConversion, typename T>
	constexpr std::enable_if_t<units::traits::is_dimensionless_unit_v<UnitConversion> && std::is_arithmetic_v<T>, bool>
	operator==(const T& lhs, const UnitConversion& rhs) noexcept
	{
		using CommonUnderlying = std::common_type_t<T, typename UnitConversion::underlying_type>;

		const auto common_lhs = static_cast<CommonUnderlying>(lhs);
		const auto common_rhs = static_cast<CommonUnderlying>(rhs);

		if constexpr (std::is_integral_v<CommonUnderlying>)
		{
			return common_lhs == common_rhs;
		}
		else
		{
			return abs(common_lhs - common_rhs) <
				std::numeric_limits<CommonUnderlying>::epsilon() * abs(common_lhs + common_rhs) ||
				abs(common_lhs - common_rhs) < std::numeric_limits<CommonUnderlying>::min();
		}
	}

	template<typename UnitConversion, typename T>
	constexpr std::enable_if_t<units::traits::is_dimensionless_unit_v<UnitConversion> && std::is_arithmetic_v<T>, bool>
	operator==(const UnitConversion& lhs, const T& rhs) noexcept
	{
		return rhs == lhs;
	}

	template<typename UnitConversion, typename T>
	constexpr std::enable_if_t<units::traits::is_dimensionless_unit_v<UnitConversion> && std::is_arithmetic_v<T>, bool>
	operator!=(const T& lhs, const UnitConversion& rhs) noexcept
	{
		return !(lhs == rhs);
	}

	template<typename UnitConversion, typename T>
	constexpr std::enable_if_t<units::traits::is_dimensionless_unit_v<UnitConversion> && std::is_arithmetic_v<T>, bool>
	operator!=(const UnitConversion& lhs, const T& rhs) noexcept
	{
		return !(lhs == rhs);
	}

	template<typename UnitConversion, typename T>
	constexpr std::enable_if_t<units::traits::is_dimensionless_unit_v<UnitConversion> && std::is_arithmetic_v<T>, bool>
	operator>=(const T& lhs, const UnitConversion& rhs) noexcept
	{
		using CommonUnderlying = std::common_type_t<T, typename UnitConversion::underlying_type>;
		return lhs >= static_cast<CommonUnderlying>(rhs);
	}

	template<typename UnitConversion, typename T>
	constexpr std::enable_if_t<units::traits::is_dimensionless_unit_v<UnitConversion> && std::is_arithmetic_v<T>, bool>
	operator>=(const UnitConversion& lhs, const T& rhs) noexcept
	{
		using CommonUnderlying = std::common_type_t<typename UnitConversion::underlying_type, T>;
		return static_cast<CommonUnderlying>(lhs) >= rhs;
	}

	template<typename UnitConversion, typename T>
	constexpr std::enable_if_t<units::traits::is_dimensionless_unit_v<UnitConversion> && std::is_arithmetic_v<T>, bool>
	operator>(const T& lhs, const UnitConversion& rhs) noexcept
	{
		using CommonUnderlying = std::common_type_t<T, typename UnitConversion::underlying_type>;
		return lhs > static_cast<CommonUnderlying>(rhs);
	}

	template<typename UnitConversion, typename T>
	constexpr std::enable_if_t<units::traits::is_dimensionless_unit_v<UnitConversion> && std::is_arithmetic_v<T>, bool>
	operator>(const UnitConversion& lhs, const T& rhs) noexcept
	{
		using CommonUnderlying = std::common_type_t<typename UnitConversion::underlying_type, T>;
		return static_cast<CommonUnderlying>(lhs) > rhs;
	}

	template<typename UnitConversion, typename T>
	constexpr std::enable_if_t<units::traits::is_dimensionless_unit_v<UnitConversion> && std::is_arithmetic_v<T>, bool>
	operator<=(const T& lhs, const UnitConversion& rhs) noexcept
	{
		using CommonUnderlying = std::common_type_t<T, typename UnitConversion::underlying_type>;
		return lhs <= static_cast<CommonUnderlying>(rhs);
	}

	template<typename UnitConversion, typename T>
	constexpr std::enable_if_t<units::traits::is_dimensionless_unit_v<UnitConversion> && std::is_arithmetic_v<T>, bool>
	operator<=(const UnitConversion& lhs, const T& rhs) noexcept
	{
		using CommonUnderlying = std::common_type_t<typename UnitConversion::underlying_type, T>;
		return static_cast<CommonUnderlying>(lhs) <= rhs;
	}

	template<typename UnitConversion, typename T>
	constexpr std::enable_if_t<units::traits::is_dimensionless_unit_v<UnitConversion> && std::is_arithmetic_v<T>, bool>
	operator<(const T& lhs, const UnitConversion& rhs) noexcept
	{
		using CommonUnderlying = std::common_type_t<T, typename UnitConversion::underlying_type>;
		return lhs < static_cast<CommonUnderlying>(rhs);
	}

	template<typename UnitConversion, typename T>
	constexpr std::enable_if_t<units::traits::is_dimensionless_unit_v<UnitConversion> && std::is_arithmetic_v<T>, bool>
	operator<(const UnitConversion& lhs, const T& rhs) noexcept
	{
		using CommonUnderlying = std::common_type_t<typename UnitConversion::underlying_type, T>;
		return static_cast<CommonUnderlying>(lhs) < rhs;
	}

	//----------------------------------
	//	POW
	//----------------------------------

	/** @cond */ // DOXYGEN IGNORE
	namespace detail
	{
		/// recursive exponential implementation
		template<int N, class U>
		struct power_of_unit
		{
			using type = typename units::detail::unit_multiply<U, typename power_of_unit<N - 1, U>::type>;
		};

		/// End recursion
		template<class U>
		struct power_of_unit<1, U>
		{
			using type = U;
		};
	}               // namespace detail
	/** @endcond */ // END DOXYGEN IGNORE

	/**
	 * @brief		computes the value of <i>value</i> raised to the <i>power</i>
	 * @details		Only implemented for linear_scale units. <i>Power</i> must be known at compile time, so the
	 *				resulting unit type can be deduced.
	 * @tparam		power exponential power to raise <i>value</i> by.
	 * @param[in]	value `unit` derived type to raise to the given <i>power</i>
	 * @returns		new unit, raised to the given exponent
	 */
	template<int power, class UnitType, std::enable_if_t<traits::has_linear_scale_v<UnitType>, int> = 0>
	constexpr auto pow(const UnitType& value) noexcept
		-> unit<traits::strong_t<typename units::detail::power_of_unit<power,
					typename units::traits::unit_traits<UnitType>::conversion_factor>::type>,
			detail::floating_point_promotion_t<typename units::traits::unit_traits<UnitType>::underlying_type>,
			linear_scale>
	{
		return unit<traits::strong_t<typename units::detail::power_of_unit<power,
						typename units::traits::unit_traits<UnitType>::conversion_factor>::type>,
			detail::floating_point_promotion_t<typename units::traits::unit_traits<UnitType>::underlying_type>,
			linear_scale>(pow(value(), power));
	}

	//------------------------------
	//	DECIBEL SCALE
	//------------------------------

	/**
	 * @brief		unit scale for representing decibel values.
	 * @details		internally stores linearized values. `operator()` returns the value in dB.
	 * @tparam		T	underlying storage type
	 * @sa			unit
	 */
	template<typename T>
	struct decibel_scale
	{
		constexpr decibel_scale()                     = default;
		constexpr decibel_scale(const decibel_scale&) = default;
		~decibel_scale()                              = default;
		decibel_scale& operator=(const decibel_scale&) = default;
		constexpr decibel_scale(decibel_scale&&)       = default;
		decibel_scale& operator=(decibel_scale&&) = default;
		constexpr decibel_scale(const T value) noexcept : m_value(std::pow(10, value / 10))
		{
		}
		template<class... Args>
		constexpr decibel_scale(const T value, std::true_type, Args&&...) noexcept : m_value(value)
		{
		}
		constexpr T operator()() const noexcept
		{
			return 10 * std::log10(m_value);
		}

		T m_value; ///< linearized value
	};

	//------------------------------
	//	dimensionless (DECIBEL) UNITS
	//------------------------------

	/**
	 * @brief		namespace for unit types and containers for units that have no dimension (dimensionless units)
	 * @sa			See unit for more information on unit type containers.
	 */
	template<class Underlying>
	using dB_t = unit<dimensionless_unit, Underlying, decibel_scale>;
#if !defined(UNIT_LIB_DISABLE_IOSTREAM)
	template<class Underlying>
	inline std::ostream& operator<<(std::ostream& os, const dB_t<Underlying>& obj)
	{
		os << obj() << " dB";
		return os;
	}
#endif
	template<class Underlying>
	using dBi_t = dB_t<Underlying>;

	//------------------------------
	//	DECIBEL ARITHMETIC
	//------------------------------

	/// Addition for convertible unit types with a decibel_scale
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::is_convertible_unit_v<UnitTypeLhs, UnitTypeRhs> &&
				traits::has_decibel_scale_v<UnitTypeLhs, UnitTypeRhs>,
			int> = 0>
	constexpr auto operator+(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
		-> unit<traits::strong_t<squared<typename units::traits::unit_traits<
					std::common_type_t<UnitTypeLhs, UnitTypeRhs>>::conversion_factor>>,
			typename std::common_type_t<UnitTypeLhs, UnitTypeRhs>::underlying_type, decibel_scale>
	{
		using CommonUnit       = std::common_type_t<UnitTypeLhs, UnitTypeRhs>;
		using CommonUnderlying = typename CommonUnit::underlying_type;

		return unit<traits::strong_t<squared<typename CommonUnit::conversion_factor>>, CommonUnderlying, decibel_scale>(
			CommonUnit(lhs).template toLinearized<CommonUnderlying>() *
				CommonUnit(rhs).template toLinearized<CommonUnderlying>(),
			std::true_type());
	}

	/// Addition between unit types with a decibel_scale and dimensionless dB units
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::has_decibel_scale_v<UnitTypeLhs, UnitTypeRhs> &&
				!traits::is_dimensionless_unit_v<UnitTypeLhs> && traits::is_dimensionless_unit_v<UnitTypeRhs>,
			int> = 0>
	constexpr unit<typename UnitTypeLhs::conversion_factor,
		std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>, decibel_scale>
	operator+(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnderlying =
			std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>;
		return unit<typename UnitTypeLhs::conversion_factor, CommonUnderlying, decibel_scale>(
			lhs.template toLinearized<CommonUnderlying>() * rhs.template toLinearized<CommonUnderlying>(),
			std::true_type());
	}

	/// Addition between unit types with a decibel_scale and dimensionless dB units
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::has_decibel_scale_v<UnitTypeLhs, UnitTypeRhs> &&
				traits::is_dimensionless_unit_v<UnitTypeLhs> && !traits::is_dimensionless_unit_v<UnitTypeRhs>,
			int> = 0>
	constexpr unit<typename UnitTypeRhs::conversion_factor,
		std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>, decibel_scale>
	operator+(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnderlying =
			std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>;
		return unit<typename UnitTypeRhs::conversion_factor, CommonUnderlying, decibel_scale>(
			lhs.template toLinearized<CommonUnderlying>() * rhs.template toLinearized<CommonUnderlying>(),
			std::true_type());
	}

	/// Subtraction for convertible unit types with a decibel_scale
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::is_convertible_unit_v<UnitTypeLhs, UnitTypeRhs> &&
				traits::has_decibel_scale_v<UnitTypeLhs, UnitTypeRhs>,
			int> = 0>
	constexpr auto operator-(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
		-> dB_t<typename std::common_type_t<UnitTypeLhs, UnitTypeRhs>::underlying_type>
	{
		using CommonUnit       = std::common_type_t<UnitTypeLhs, UnitTypeRhs>;
		using CommonUnderlying = typename CommonUnit::underlying_type;

		return dB_t<CommonUnderlying>(CommonUnit(lhs).template toLinearized<CommonUnderlying>() /
				CommonUnit(rhs).template toLinearized<CommonUnderlying>(),
			std::true_type());
	}

	/// Subtraction between unit types with a decibel_scale and dimensionless dB units
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::has_decibel_scale_v<UnitTypeLhs, UnitTypeRhs> &&
				!traits::is_dimensionless_unit_v<UnitTypeLhs> && traits::is_dimensionless_unit_v<UnitTypeRhs>,
			int> = 0>
	constexpr unit<typename UnitTypeLhs::conversion_factor,
		std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>, decibel_scale>
	operator-(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
	{
		using CommonUnderlying =
			std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>;
		return unit<typename UnitTypeLhs::conversion_factor, CommonUnderlying, decibel_scale>(
			lhs.template toLinearized<CommonUnderlying>() / rhs.template toLinearized<CommonUnderlying>(),
			std::true_type());
	}

	/// Subtraction between unit types with a decibel_scale and dimensionless dB units
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::has_decibel_scale_v<UnitTypeLhs, UnitTypeRhs> &&
				traits::is_dimensionless_unit_v<UnitTypeLhs> && !traits::is_dimensionless_unit_v<UnitTypeRhs>,
			int> = 0>
	constexpr auto operator-(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs) noexcept
		-> unit<traits::strong_t<inverse<typename units::traits::unit_traits<UnitTypeRhs>::conversion_factor>>,
			std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>,
			decibel_scale>
	{
		using UnitConversionRhs = typename units::traits::unit_traits<UnitTypeRhs>::conversion_factor;
		using CommonUnderlying =
			std::common_type_t<typename UnitTypeLhs::underlying_type, typename UnitTypeRhs::underlying_type>;

		return unit<traits::strong_t<inverse<UnitConversionRhs>>, CommonUnderlying, decibel_scale>(
			lhs.template toLinearized<CommonUnderlying>() / rhs.template toLinearized<CommonUnderlying>(),
			std::true_type());
	}

	//------------------------------
	//	LITERALS
	//------------------------------

	/**
	 * @namespace	units::literals
	 * @brief		namespace for unit literal definitions of all categories.
	 * @details		Literals allow for declaring unit types using suffix values. For example, a type
	 *				of `meter_t<double>(6.2)` could be declared as `6.2_m`. All literals use an underscore
	 *				followed by the abbreviation for the unit. To enable literal syntax in your code,
	 *				include the statement `using namespace units::literals`.
	 * @anchor		unitLiterals
	 * @sa			See unit for more information on unit type containers.
	 */

	//----------------------------------
	//	UNIT-ENABLED CMATH FUNCTIONS
	//----------------------------------

	//----------------------------------
	//	MIN/MAX FUNCTIONS
	//----------------------------------

	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::is_convertible_unit_v<UnitTypeLhs, UnitTypeRhs>, int> = 0>
	constexpr std::common_type_t<UnitTypeLhs, UnitTypeRhs> min(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs)
	{
		using CommonUnit = std::common_type_t<UnitTypeLhs, UnitTypeRhs>;
		return (lhs < rhs ? CommonUnit(lhs) : CommonUnit(rhs));
	}

	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::is_convertible_unit_v<UnitTypeLhs, UnitTypeRhs>, int> = 0>
	constexpr std::common_type_t<UnitTypeLhs, UnitTypeRhs> max(const UnitTypeLhs& lhs, const UnitTypeRhs& rhs)
	{
		using CommonUnit = std::common_type_t<UnitTypeLhs, UnitTypeRhs>;
		return (lhs > rhs ? CommonUnit(lhs) : CommonUnit(rhs));
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
	 * @param[in]	x	dimensionless value of the exponent.
	 * @returns		Exponential value of x.
	 *				If the magnitude of the result is too large to be represented by a value of the return type, the
	 *				function returns HUGE_VAL (or HUGE_VALF or HUGE_VALL) with the proper sign, and an overflow range
	 *				error occurs
	 */
	template<class dimensionlessUnit, std::enable_if_t<traits::is_dimensionless_unit_v<dimensionlessUnit>, int> = 0>
	dimensionless<detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>> exp(
		const dimensionlessUnit x) noexcept
	{
		return std::exp(x());
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute natural logarithm
	 * @details		Returns the natural logarithm of x.
	 * @param[in]	x	dimensionless value whose logarithm is calculated. If the argument is negative, a
	 *					domain error occurs.
	 * @sa			log10 for more common base-10 logarithms
	 * @returns		Natural logarithm of x.
	 */
	template<class dimensionlessUnit, std::enable_if_t<traits::is_dimensionless_unit_v<dimensionlessUnit>, int> = 0>
	dimensionless<detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>> log(
		const dimensionlessUnit x) noexcept
	{
		return std::log(x());
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute common logarithm
	 * @details		Returns the common (base-10) logarithm of x.
	 * @param[in]	x	Value whose logarithm is calculated. If the argument is negative, a
	 *					domain error occurs.
	 * @returns		Common logarithm of x.
	 */
	template<class dimensionlessUnit, std::enable_if_t<traits::is_dimensionless_unit_v<dimensionlessUnit>, int> = 0>
	dimensionless<detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>> log10(
		const dimensionlessUnit x) noexcept
	{
		return std::log10(x());
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Break into fractional and integral parts.
	 * @details		The integer part is stored in the object pointed by intpart, and the
	 *				fractional part is returned by the function. Both parts have the same sign
	 *				as x.
	 * @param[in]	x		dimensionless value to break into parts.
	 * @param[in]	intpart Pointer to an object (of the same type as x) where the integral part
	 *				is stored with the same sign as x.
	 * @returns		The fractional part of x, with the same sign.
	 */
	template<class dimensionlessUnit,
		std::enable_if_t<traits::is_dimensionless_unit_v<dimensionlessUnit> &&
				std::is_floating_point_v<typename dimensionlessUnit::underlying_type>,
			int> = 0>
	dimensionlessUnit modf(const dimensionlessUnit x, dimensionlessUnit* intpart) noexcept
	{
		typename dimensionlessUnit::underlying_type intp;
		dimensionlessUnit fracpart = std::modf(x(), &intp);
		*intpart                   = intp;
		return fracpart;
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute binary exponential function
	 * @details		Returns the base-2 exponential function of x, which is 2 raised to the power x: 2^x.
	 * @param[in]	x	Value of the exponent.
	 * @returns		2 raised to the power of x.
	 */
	template<class dimensionlessUnit, std::enable_if_t<traits::is_dimensionless_unit_v<dimensionlessUnit>, int> = 0>
	dimensionless<detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>> exp2(
		const dimensionlessUnit x) noexcept
	{
		return std::exp2(x());
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute exponential minus one
	 * @details		Returns e raised to the power x minus one: e^x-1. For small magnitude values
	 *				of x, expm1 may be more accurate than exp(x)-1.
	 * @param[in]	x	Value of the exponent.
	 * @returns		e raised to the power of x, minus one.
	 */
	template<class dimensionlessUnit, std::enable_if_t<traits::is_dimensionless_unit_v<dimensionlessUnit>, int> = 0>
	dimensionless<detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>> expm1(
		const dimensionlessUnit x) noexcept
	{
		return std::expm1(x());
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
	template<class dimensionlessUnit, std::enable_if_t<traits::is_dimensionless_unit_v<dimensionlessUnit>, int> = 0>
	dimensionless<detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>> log1p(
		const dimensionlessUnit x) noexcept
	{
		return std::log1p(x());
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute binary logarithm
	 * @details		Returns the binary (base-2) logarithm of x.
	 * @param[in]	x	Value whose logarithm is calculated. If the argument is negative, a
	 *					domain error occurs.
	 * @returns		The binary logarithm of x: log2x.
	 */
	template<class dimensionlessUnit, std::enable_if_t<traits::is_dimensionless_unit_v<dimensionlessUnit>, int> = 0>
	dimensionless<detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>> log2(
		const dimensionlessUnit x) noexcept
	{
		return std::log2(x());
	}

	//----------------------------------
	//	POWER FUNCTIONS
	//----------------------------------

	/* pow is implemented earlier in the library since a lot of the unit definitions depend on it */

	/**
	 * @ingroup		UnitMath
	 * @brief		computes the square root of <i>value</i>
	 * @details		Only implemented for linear_scale units.
	 * @param[in]	value `unit` derived type to compute the square root of.
	 * @returns		new unit, whose units are the square root of value's. E.g. if values
	 *				had units of `square_meter`, then the return type will have units of
	 *				`meter`.
	 * @note		`sqrt` provides a _rational approximation_ of the square root of <i>value</i>.
	 *				In some cases, _both_ the returned value _and_ conversion factor of the returned
	 *				unit type may have errors no larger than `1e-10`.
	 */
	template<class UnitType, std::enable_if_t<units::traits::has_linear_scale_v<UnitType>, int> = 0>
	constexpr auto sqrt(const UnitType& value) noexcept
		-> unit<traits::strong_t<square_root<typename units::traits::unit_traits<UnitType>::conversion_factor>>,
			detail::floating_point_promotion_t<typename units::traits::unit_traits<UnitType>::underlying_type>,
			linear_scale>
	{
		return unit<traits::strong_t<square_root<typename units::traits::unit_traits<UnitType>::conversion_factor>>,
			detail::floating_point_promotion_t<typename units::traits::unit_traits<UnitType>::underlying_type>,
			linear_scale>(sqrt(value()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Computes the square root of the sum-of-squares of x and y.
	 * @details		Only implemented for linear_scale units.
	 * @param[in]	x	unit type value
	 * @param[in]	y	unit type value
	 * @returns		square root of the sum-of-squares of x and y in the same units
	 *				as x.
	 */
	template<class UnitTypeLhs, class UnitTypeRhs,
		std::enable_if_t<traits::is_convertible_unit_v<UnitTypeLhs, UnitTypeRhs> &&
				traits::has_linear_scale_v<UnitTypeLhs, UnitTypeRhs>,
			int> = 0>
	detail::floating_point_promotion_t<std::common_type_t<UnitTypeLhs, UnitTypeRhs>> hypot(
		const UnitTypeLhs& x, const UnitTypeRhs& y)
	{
		using CommonUnit = detail::floating_point_promotion_t<std::common_type_t<UnitTypeLhs, UnitTypeRhs>>;
		return CommonUnit(std::hypot(CommonUnit(x)(), CommonUnit(y)()));
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
	template<class UnitType, class = std::enable_if_t<traits::is_unit_v<UnitType>>>
	detail::floating_point_promotion_t<UnitType> ceil(const UnitType x) noexcept
	{
		return detail::floating_point_promotion_t<UnitType>(std::ceil(x()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Round down value
	 * @details		Rounds x downward, returning the largest integral value that is not greater than x.
	 * @param[in]	x	Unit value to round down.
	 * @returns		The value of x rounded downward.
	 */
	template<class UnitType, class = std::enable_if_t<traits::is_unit_v<UnitType>>>
	detail::floating_point_promotion_t<UnitType> floor(const UnitType x) noexcept
	{
		return detail::floating_point_promotion_t<UnitType>(std::floor(x()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute remainder of division
	 * @details		Returns the floating-point remainder of numer/denom (rounded towards zero).
	 * @param[in]	numer	Value of the quotient numerator.
	 * @param[in]	denom	Value of the quotient denominator.
	 * @returns		The remainder of dividing the arguments.
	 */
	template<class UnitTypeLhs, class UnitTypeRhs,
		class = std::enable_if_t<traits::is_unit_v<UnitTypeLhs> && traits::is_unit_v<UnitTypeRhs> &&
			traits::is_convertible_unit_v<UnitTypeLhs, UnitTypeRhs>>>
	detail::floating_point_promotion_t<std::common_type_t<UnitTypeLhs, UnitTypeRhs>> fmod(
		const UnitTypeLhs numer, const UnitTypeRhs denom) noexcept
	{
		using CommonUnit = detail::floating_point_promotion_t<std::common_type_t<UnitTypeLhs, UnitTypeRhs>>;
		return CommonUnit(std::fmod(CommonUnit(numer)(), CommonUnit(denom)()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Truncate value
	 * @details		Rounds x toward zero, returning the nearest integral value that is not
	 *				larger in magnitude than x. Effectively rounds towards 0.
	 * @param[in]	x	Value to truncate
	 * @returns		The nearest integral value that is not larger in magnitude than x.
	 */
	template<class UnitType, class = std::enable_if_t<traits::is_unit_v<UnitType>>>
	detail::floating_point_promotion_t<UnitType> trunc(const UnitType x) noexcept
	{
		return detail::floating_point_promotion_t<UnitType>(std::trunc(x()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Round to nearest
	 * @details		Returns the integral value that is nearest to x, with halfway cases rounded
	 *				away from zero.
	 * @param[in]	x	value to round.
	 * @returns		The value of x rounded to the nearest integral.
	 */
	template<class UnitType, class = std::enable_if_t<traits::is_unit_v<UnitType>>>
	detail::floating_point_promotion_t<UnitType> round(const UnitType x) noexcept
	{
		return detail::floating_point_promotion_t<UnitType>(std::round(x()));
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
	template<class UnitTypeLhs, class UnitTypeRhs,
		class = std::enable_if_t<traits::is_unit_v<UnitTypeLhs> && traits::is_unit_v<UnitTypeRhs>>>
	detail::floating_point_promotion_t<UnitTypeLhs> copysign(const UnitTypeLhs x, const UnitTypeRhs y) noexcept
	{
		return detail::floating_point_promotion_t<UnitTypeLhs>(
			std::copysign(x(), y())); // no need for conversion to get the correct sign.
	}

	/// Overload to copy the sign from a raw double
	template<class UnitTypeLhs, typename T,
		class = std::enable_if_t<std::is_arithmetic_v<T> && traits::is_unit_v<UnitTypeLhs>>>
	detail::floating_point_promotion_t<UnitTypeLhs> copysign(const UnitTypeLhs x, const T& y) noexcept
	{
		return detail::floating_point_promotion_t<UnitTypeLhs>(std::copysign(x(), y));
	}

	//----------------------------------
	//	MIN / MAX / DIFFERENCE
	//----------------------------------

	/**
	 * @ingroup		UnitMath
	 * @brief		Positive difference
	 * @details		The function returns x-y if x>y, and zero otherwise, in their common type.
	 * @param[in]	x	Values whose difference is calculated.
	 * @param[in]	y	Values whose difference is calculated.
	 * @returns		The positive difference between x and y.
	 */
	template<class UnitTypeLhs, class UnitTypeRhs,
		class = std::enable_if_t<traits::is_unit_v<UnitTypeLhs> && traits::is_unit_v<UnitTypeRhs> &&
			traits::is_convertible_unit_v<UnitTypeLhs, UnitTypeRhs>>>
	detail::floating_point_promotion_t<std::common_type_t<UnitTypeLhs, UnitTypeRhs>> fdim(
		const UnitTypeLhs x, const UnitTypeRhs y) noexcept
	{
		using CommonUnit = detail::floating_point_promotion_t<std::common_type_t<UnitTypeLhs, UnitTypeRhs>>;
		return CommonUnit(std::fdim(CommonUnit(x)(), CommonUnit(y)()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Maximum value
	 * @details		Returns the larger of its arguments: either x or y, in their common type.
	 * @param[in]	x	Values among which the function selects a maximum.
	 * @param[in]	y	Values among which the function selects a maximum.
	 * @returns		The maximum numeric value of its arguments.
	 */
	template<class UnitTypeLhs, class UnitTypeRhs,
		class = std::enable_if_t<traits::is_unit_v<UnitTypeLhs> && traits::is_unit_v<UnitTypeRhs> &&
			traits::is_convertible_unit_v<UnitTypeLhs, UnitTypeRhs>>>
	detail::floating_point_promotion_t<std::common_type_t<UnitTypeLhs, UnitTypeRhs>> fmax(
		const UnitTypeLhs x, const UnitTypeRhs y) noexcept
	{
		using CommonUnit = detail::floating_point_promotion_t<std::common_type_t<UnitTypeLhs, UnitTypeRhs>>;
		return CommonUnit(std::fmax(CommonUnit(x)(), CommonUnit(y)()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Minimum value
	 * @details		Returns the smaller of its arguments: either x or y, in their common type.
	 *				If one of the arguments in a NaN, the other is returned.
	 * @param[in]	x	Values among which the function selects a minimum.
	 * @param[in]	y	Values among which the function selects a minimum.
	 * @returns		The minimum numeric value of its arguments.
	 */
	template<class UnitTypeLhs, class UnitTypeRhs,
		class = std::enable_if_t<traits::is_unit_v<UnitTypeLhs> && traits::is_unit_v<UnitTypeRhs> &&
			traits::is_convertible_unit_v<UnitTypeLhs, UnitTypeRhs>>>
	detail::floating_point_promotion_t<std::common_type_t<UnitTypeLhs, UnitTypeRhs>> fmin(
		const UnitTypeLhs x, const UnitTypeRhs y) noexcept
	{
		using CommonUnit = detail::floating_point_promotion_t<std::common_type_t<UnitTypeLhs, UnitTypeRhs>>;
		return CommonUnit(std::fmin(CommonUnit(x)(), CommonUnit(y)()));
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
	template<class UnitType, class = std::enable_if_t<traits::is_unit_v<UnitType>>>
	detail::floating_point_promotion_t<UnitType> fabs(const UnitType x) noexcept
	{
		return detail::floating_point_promotion_t<UnitType>(std::fabs(x()));
	}

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute absolute value
	 * @details		Returns the absolute value of x, i.e. |x|.
	 * @param[in]	x	Value whose absolute value is returned.
	 * @returns		The absolute value of x.
	 */
	template<class UnitType, class = std::enable_if_t<traits::is_unit_v<UnitType>>>
	UnitType abs(const UnitType x) noexcept
	{
		return UnitType(std::abs(x()));
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
	template<class UnitTypeLhs, class UnitMultiply, class UnitAdd,
		class = std::enable_if_t<traits::is_unit_v<UnitTypeLhs> && traits::is_unit_v<UnitMultiply> &&
			traits::is_unit_v<UnitAdd> &&
			traits::is_convertible_conversion_factor_v<
				compound_conversion_factor<typename units::traits::unit_traits<UnitTypeLhs>::conversion_factor,
					typename units::traits::unit_traits<UnitMultiply>::conversion_factor>,
				typename units::traits::unit_traits<UnitAdd>::conversion_factor>>>
	auto fma(const UnitTypeLhs x, const UnitMultiply y, const UnitAdd z) noexcept
		-> std::common_type_t<decltype(detail::floating_point_promotion_t<UnitTypeLhs>(x) *
								  detail::floating_point_promotion_t<UnitMultiply>(y)),
			UnitAdd>
	{
		using CommonUnit = std::common_type_t<decltype(detail::floating_point_promotion_t<UnitTypeLhs>(x) *
												  detail::floating_point_promotion_t<UnitMultiply>(y)),
			UnitAdd>;
		return CommonUnit(std::fma(x(), y(), CommonUnit(z)()));
	}
} // end namespace units

//------------------------------
//	std::hash
//------------------------------

namespace std
{
	template<class UnitConversion, typename T, template<typename> class NonLinearScale>
	struct hash<units::unit<UnitConversion, T, NonLinearScale>>
	{
		template<typename U = T>
		constexpr std::size_t operator()(const units::unit<UnitConversion, T, NonLinearScale>& x) const noexcept
		{
			if constexpr (std::is_integral_v<U>)
			{
				return x.template toLinearized<T>();
			}
			else
			{
				return hash<T>()(x.template toLinearized<T>());
			}
		}
	};

	//------------------------------
	//	std::numeric_limits
	//------------------------------

	template<class UnitConversion, typename T, template<typename> class NonLinearScale>
	class numeric_limits<units::unit<UnitConversion, T, NonLinearScale>> : public std::numeric_limits<T>
	{
	};
} // namespace std

#endif // units_core_h__

// For Emacs
// Local Variables:
// Mode: C++
// c-basic-offset: 2
// fill-column: 116
// tab-width: 4
// End:
