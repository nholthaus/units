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
/// @file	units.h
/// @brief	Complete implementation of `units` - a compile-time, header-only, unit conversion
///			library built on c++14 with no dependencies.
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_h__
#define units_h__

#include <units/acceleration.h>
#include <units/angle.h>
#include <units/angular_velocity.h>
#include <units/capacitance.h>
#include <units/charge.h>
#include <units/current.h>
#include <units/energy.h>
#include <units/force.h>
#include <units/frequency.h>
#include <units/impedance.h>
#include <units/length.h>
#include <units/luminous_intensity.h>
#include <units/mass.h>
#include <units/power.h>
#include <units/pressure.h>
#include <units/solid_angle.h>
#include <units/substance.h>
#include <units/temperature.h>
#include <units/time.h>
#include <units/velocity.h>
#include <units/voltage.h>

namespace units
{
	// clang-format off

	//------------------------------
	//	UNITS OF CONDUCTANCE
	//------------------------------

	/**
	 * @namespace	units::conductance
	 * @brief		namespace for unit types and containers representing conductance values
	 * @details		The SI unit for conductance is `siemens`, and the corresponding `dimension` dimension is
	 *				`conductance_unit`.
	 * @anchor		conductanceContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_CONDUCTANCE_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(conductance, siemens, siemens, S, unit_conversion<std::ratio<1>, units::dimension::conductance>)

	UNIT_ADD_DIMENSION_TRAIT(conductance)
#endif

	//------------------------------
	//	UNITS OF MAGNETIC FLUX
	//------------------------------

	/**
	 * @namespace	units::magnetic_flux
	 * @brief		namespace for unit types and containers representing magnetic_flux values
	 * @details		The SI unit for magnetic_flux is `webers`, and the corresponding `dimension` dimension is
	 *				`magnetic_flux_unit`.
	 * @anchor		magneticFluxContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_MAGNETIC_FLUX_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(magnetic_flux, weber, webers, Wb, unit_conversion<std::ratio<1>, units::dimension::magnetic_flux>)
	UNIT_ADD(magnetic_flux, maxwell, maxwells, Mx, unit_conversion<std::ratio<1, 100000000>, webers>)

	UNIT_ADD_DIMENSION_TRAIT(magnetic_flux)
#endif

	//----------------------------------------
	//	UNITS OF MAGNETIC FIELD STRENGTH
	//----------------------------------------

	/**
	 * @namespace	units::magnetic_field_strength
	 * @brief		namespace for unit types and containers representing magnetic_field_strength values
	 * @details		The SI unit for magnetic_field_strength is `teslas`, and the corresponding `dimension` dimension is
	 *				`magnetic_field_strength_unit`.
	 * @anchor		magneticFieldStrengthContainers
	 * @sa			See unit for more information on unit type containers.
	 */
	// Unfortunately `_T` is a WINAPI macro, so we have to use `_Te` as the tesla abbreviation.
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_MAGNETIC_FIELD_STRENGTH_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(magnetic_field_strength, tesla, teslas, Te, unit_conversion<std::ratio<1>, units::dimension::magnetic_field_strength>)
	UNIT_ADD(magnetic_field_strength, gauss, gauss, G, compound_unit_conversion<magnetic_flux::maxwell, inverse<squared<length::centimeter>>>)

	UNIT_ADD_DIMENSION_TRAIT(magnetic_field_strength)
#endif

	//------------------------------
	//	UNITS OF INDUCTANCE
	//------------------------------

	/**
	 * @namespace	units::inductance
	 * @brief		namespace for unit types and containers representing inductance values
	 * @details		The SI unit for inductance is `henrys`, and the corresponding `dimension` dimension is
	 *				`inductance_unit`.
	 * @anchor		inductanceContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_INDUCTANCE_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(inductance, henry, henries, H, unit_conversion<std::ratio<1>, units::dimension::inductance>)

	UNIT_ADD_DIMENSION_TRAIT(inductance)
#endif

	//------------------------------
	//	UNITS OF LUMINOUS FLUX
	//------------------------------

	/**
	 * @namespace	units::luminous_flux
	 * @brief		namespace for unit types and containers representing luminous_flux values
	 * @details		The SI unit for luminous_flux is `lumens`, and the corresponding `dimension` dimension is
	 *				`luminous_flux_unit`.
	 * @anchor		luminousFluxContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_LUMINOUS_FLUX_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(luminous_flux, lumen, lumens, lm, unit_conversion<std::ratio<1>, units::dimension::luminous_flux>)

	UNIT_ADD_DIMENSION_TRAIT(luminous_flux)
#endif

	//------------------------------
	//	UNITS OF ILLUMINANCE
	//------------------------------

	/**
	 * @namespace	units::illuminance
	 * @brief		namespace for unit types and containers representing illuminance values
	 * @details		The SI unit for illuminance is `luxes`, and the corresponding `dimension` dimension is
	 *				`illuminance_unit`.
	 * @anchor		illuminanceContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ILLUMINANCE_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(illuminance, lux, luxes, lx, unit_conversion<std::ratio<1>, units::dimension::illuminance>)
	UNIT_ADD(illuminance, footcandle, footcandles, fc, compound_unit_conversion<luminous_flux::lumen, inverse<squared<length::foot>>>)
	UNIT_ADD(illuminance, lumens_per_square_inch, lumens_per_square_inch, lm_per_in_sq, compound_unit_conversion<luminous_flux::lumen, inverse<squared<length::inch>>>)
	UNIT_ADD(illuminance, phot, phots, ph, compound_unit_conversion<luminous_flux::lumens, inverse<squared<length::centimeter>>>)

	UNIT_ADD_DIMENSION_TRAIT(illuminance)
#endif

	//------------------------------
	//	UNITS OF RADIATION
	//------------------------------

	/**
	 * @namespace	units::radiation
	 * @brief		namespace for unit types and containers representing radiation values
	 * @details		The SI units for radiation are:
	 *				- source activity:	becquerel
	 *				- absorbed dose:	gray
	 *				- equivalent dose:	sievert
	 * @anchor		radiationContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_RADIATION_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(radiation, becquerel, becquerels, Bq, unit_conversion<std::ratio<1>, units::frequency::hertz>)
	UNIT_ADD_WITH_METRIC_PREFIXES(radiation, gray, grays, Gy, compound_unit_conversion<energy::joules, inverse<mass::kilogram>>)
	UNIT_ADD_WITH_METRIC_PREFIXES(radiation, sievert, sieverts, Sv, unit_conversion<std::ratio<1>, grays>)
	UNIT_ADD(radiation, curie, curies, Ci, unit_conversion<std::ratio<37>, gigabecquerels>)
	UNIT_ADD(radiation, rutherford, rutherfords, rd, unit_conversion<std::ratio<1>, megabecquerels>)
	UNIT_ADD(radiation, rad, rads, rads, unit_conversion<std::ratio<1>, centigrays>)

	UNIT_ADD_DIMENSION_TRAIT(radioactivity)
#endif

	//------------------------------
	//	UNITS OF TORQUE
	//------------------------------

	/**
	 * @namespace	units::torque
	 * @brief		namespace for unit types and containers representing torque values
	 * @details		The SI unit for torque is `newton_meters`, and the corresponding `dimension` dimension is
	 *				`torque_units`.
	 * @anchor		torqueContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_TORQUE_UNITS)
	UNIT_ADD(torque, newton_meter, newton_meters, Nm, unit_conversion<std::ratio<1>, units::energy::joule>)
	UNIT_ADD(torque, foot_pound, foot_pounds, ftlb, compound_unit_conversion<length::foot, force::pounds>)
	UNIT_ADD(torque, foot_poundal, foot_poundals, ftpdl, compound_unit_conversion<length::foot, force::poundal>)
	UNIT_ADD(torque, inch_pound, inch_pounds, inlb, compound_unit_conversion<length::inch, force::pounds>)
	UNIT_ADD(torque, meter_kilogram, meter_kilograms, mkgf, compound_unit_conversion<length::meter, force::kiloponds>)

	UNIT_ADD_DIMENSION_TRAIT(torque)
#endif

	//------------------------------
	//	AREA UNITS
	//------------------------------

	/**
	 * @namespace	units::area
	 * @brief		namespace for unit types and containers representing area values
	 * @details		The SI unit for area is `square_meters`, and the corresponding `dimension` dimension is
	 *				`area_unit`.
	 * @anchor		areaContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_AREA_UNITS)
	UNIT_ADD(area, square_meter, square_meters, sq_m, unit_conversion<std::ratio<1>, units::dimension::area>)
	UNIT_ADD(area, square_foot, square_feet, sq_ft, squared<length::feet>)
	UNIT_ADD(area, square_inch, square_inches, sq_in, squared<length::inch>)
	UNIT_ADD(area, square_mile, square_miles, sq_mi, squared<length::miles>)
	UNIT_ADD(area, square_kilometer, square_kilometers, sq_km, squared<length::kilometers>)
	UNIT_ADD(area, hectare, hectares, ha, unit_conversion<std::ratio<10000>, square_meters>)
	UNIT_ADD(area, acre, acres, acre, unit_conversion<std::ratio<43560>, square_feet>)

	UNIT_ADD_DIMENSION_TRAIT(area)
#endif

	//------------------------------
	//	UNITS OF VOLUME
	//------------------------------

	/**
	 * @namespace	units::volume
	 * @brief		namespace for unit types and containers representing volume values
	 * @details		The SI unit for volume is `cubic_meters`, and the corresponding `dimension` dimension is
	 *				`volume_unit`.
	 * @anchor		volumeContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_VOLUME_UNITS)
	UNIT_ADD(volume, cubic_meter, cubic_meters, cu_m, unit_conversion<std::ratio<1>, units::dimension::volume>)
	UNIT_ADD(volume, cubic_millimeter, cubic_millimeters, cu_mm, cubed<length::millimeter>)
	UNIT_ADD(volume, cubic_kilometer, cubic_kilometers, cu_km, cubed<length::kilometer>)
	UNIT_ADD_WITH_METRIC_PREFIXES(volume, liter, liters, L, cubed<deci<length::meter>>)
	UNIT_ADD(volume, cubic_inch, cubic_inches, cu_in, cubed<length::inches>)
	UNIT_ADD(volume, cubic_foot, cubic_feet, cu_ft, cubed<length::feet>)
	UNIT_ADD(volume, cubic_yard, cubic_yards, cu_yd, cubed<length::yards>)
	UNIT_ADD(volume, cubic_mile, cubic_miles, cu_mi, cubed<length::miles>)
	UNIT_ADD(volume, gallon, gallons, gal, unit_conversion<std::ratio<231>, cubic_inches>)
	UNIT_ADD(volume, quart, quarts, qt, unit_conversion<std::ratio<1, 4>, gallons>)
	UNIT_ADD(volume, pint, pints, pt, unit_conversion<std::ratio<1, 2>, quarts>)
	UNIT_ADD(volume, cup, cups, c, unit_conversion<std::ratio<1, 2>, pints>)
	UNIT_ADD(volume, fluid_ounce, fluid_ounces, fl_oz, unit_conversion<std::ratio<1, 8>, cups>)
	UNIT_ADD(volume, barrel, barrels, bl, unit_conversion<std::ratio<42>, gallons>)
	UNIT_ADD(volume, bushel, bushels, bu, unit_conversion<std::ratio<215042, 100>, cubic_inches>)
	UNIT_ADD(volume, cord, cords, cord, unit_conversion<std::ratio<128>, cubic_feet>)
	UNIT_ADD(volume, cubic_fathom, cubic_fathoms, cu_fm, cubed<length::fathom>)
	UNIT_ADD(volume, tablespoon, tablespoons, tbsp, unit_conversion<std::ratio<1, 2>, fluid_ounces>)
	UNIT_ADD(volume, teaspoon, teaspoons, tsp, unit_conversion<std::ratio<1, 6>, fluid_ounces>)
	UNIT_ADD(volume, pinch, pinches, pinch, unit_conversion<std::ratio<1, 8>, teaspoons>)
	UNIT_ADD(volume, dash, dashes, dash, unit_conversion<std::ratio<1, 2>, pinches>)
	UNIT_ADD(volume, drop, drops, drop, unit_conversion<std::ratio<1, 360>, fluid_ounces>)
	UNIT_ADD(volume, fifth, fifths, fifth, unit_conversion<std::ratio<1, 5>, gallons>)
	UNIT_ADD(volume, dram, drams, dr, unit_conversion<std::ratio<1, 8>, fluid_ounces>)
	UNIT_ADD(volume, gill, gills, gi, unit_conversion<std::ratio<4>, fluid_ounces>)
	UNIT_ADD(volume, peck, pecks, pk, unit_conversion<std::ratio<1, 4>, bushels>)
	UNIT_ADD(volume, sack, sacks, sacks, unit_conversion<std::ratio<3>, bushels>)
	UNIT_ADD(volume, shot, shots, shots, unit_conversion<std::ratio<3, 2>, fluid_ounces>)
	UNIT_ADD(volume, strike, strikes, strikes, unit_conversion<std::ratio<2>, bushels>)

	UNIT_ADD_DIMENSION_TRAIT(volume)
#endif

	//------------------------------
	//	UNITS OF DENSITY
	//------------------------------

	/**
	 * @namespace	units::density
	 * @brief		namespace for unit types and containers representing density values
	 * @details		The SI unit for density is `kilograms_per_cubic_meter`, and the corresponding `dimension` dimension is
	 *				`density_unit`.
	 * @anchor		densityContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_DENSITY_UNITS)
	UNIT_ADD(density, kilograms_per_cubic_meter, kilograms_per_cubic_meter, kg_per_cu_m, unit_conversion<std::ratio<1>, units::dimension::density>)
	UNIT_ADD(density, grams_per_milliliter, grams_per_milliliter, g_per_mL, compound_unit_conversion<mass::grams, inverse<volume::milliliter>>)
	UNIT_ADD(density, kilograms_per_liter, kilograms_per_liter, kg_per_L, unit_conversion<std::ratio<1>, compound_unit_conversion<mass::grams, inverse<volume::milliliter>>>)
	UNIT_ADD(density, ounces_per_cubic_foot, ounces_per_cubic_foot, oz_per_cu_ft, compound_unit_conversion<mass::ounces, inverse<volume::cubic_foot>>)
	UNIT_ADD(density, ounces_per_cubic_inch, ounces_per_cubic_inch, oz_per_cu_in, compound_unit_conversion<mass::ounces, inverse<volume::cubic_inch>>)
	UNIT_ADD(density, ounces_per_gallon, ounces_per_gallon, oz_per_gal, compound_unit_conversion<mass::ounces, inverse<volume::gallon>>)
	UNIT_ADD(density, pounds_per_cubic_foot, pounds_per_cubic_foot, lb_per_cu_ft, compound_unit_conversion<mass::pounds, inverse<volume::cubic_foot>>)
	UNIT_ADD(density, pounds_per_cubic_inch, pounds_per_cubic_inch, lb_per_cu_in, compound_unit_conversion<mass::pounds, inverse<volume::cubic_inch>>)
	UNIT_ADD(density, pounds_per_gallon, pounds_per_gallon, lb_per_gal, compound_unit_conversion<mass::pounds, inverse<volume::gallon>>)
	UNIT_ADD(density, slugs_per_cubic_foot, slugs_per_cubic_foot, slug_per_cu_ft, compound_unit_conversion<mass::slugs, inverse<volume::cubic_foot>>)

	UNIT_ADD_DIMENSION_TRAIT(density)
#endif

	//------------------------------
	//	UNITS OF CONCENTRATION
	//------------------------------

	/**
	 * @namespace	units::concentration
	 * @brief		namespace for unit types and containers representing concentration values
	 * @details		The SI unit for concentration is `parts_per_million`, and the corresponding `dimension` dimension is
	 *				`dimensionless_unit`.
	 * @anchor		concentrationContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_CONCENTRATION_UNITS)
	UNIT_ADD(concentration, ppm, parts_per_million, ppm, unit_conversion<std::ratio<1, 1000000>, units::dimension::dimensionless>)
	UNIT_ADD(concentration, ppb, parts_per_billion, ppb, unit_conversion<std::ratio<1, 1000>, parts_per_million>)
	UNIT_ADD(concentration, ppt, parts_per_trillion, ppt, unit_conversion<std::ratio<1, 1000>, parts_per_billion>)
	UNIT_ADD(concentration, percent, percent, pct, unit_conversion<std::ratio<1, 100>, units::dimension::dimensionless>)

	UNIT_ADD_DIMENSION_TRAIT(concentration)
#endif

	//------------------------------
	//	UNITS OF DATA
	//------------------------------

	/**
	 * @namespace	units::data
	 * @brief		namespace for unit types and containers representing data values
	 * @details		The dimension for data is `bytes`, and the corresponding `dimension` dimension is
	 *				`data_unit`.
	 * @anchor		dataContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_DATA_UNITS)
	UNIT_ADD_WITH_METRIC_AND_BINARY_PREFIXES(data, byte, bytes, B, unit_conversion<std::ratio<1>, units::dimension::data>)
	UNIT_ADD(data, exabyte, exabytes, EB, unit_conversion<std::ratio<1000>, petabytes>)
	UNIT_ADD_WITH_METRIC_AND_BINARY_PREFIXES(data, bit, bits, b, unit_conversion<std::ratio<1, 8>, byte>)
	UNIT_ADD(data, exabit, exabits, Eb, unit_conversion<std::ratio<1000>, petabits>)

	UNIT_ADD_DIMENSION_TRAIT(data)
#endif

	//------------------------------
	//	UNITS OF DATA TRANSFER
	//------------------------------

	/**
	* @namespace	units::data_transfer_rate
	* @brief		namespace for unit types and containers representing data values
	* @details		The dimension for data is `bytes`, and the corresponding `dimension` dimension is
	*				`data_unit`.
	* @anchor		dataContainers
	* @sa			See unit for more information on unit type containers.
	*/
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_DATA_TRANSFER_RATE_UNITS)
	UNIT_ADD_WITH_METRIC_AND_BINARY_PREFIXES(data_transfer_rate, bytes_per_second, bytes_per_second, Bps, unit_conversion<std::ratio<1>, units::dimension::data_transfer_rate>)
	UNIT_ADD(data_transfer_rate, exabytes_per_second, exabytes_per_second, EBps, unit_conversion<std::ratio<1000>, petabytes_per_second>)
	UNIT_ADD_WITH_METRIC_AND_BINARY_PREFIXES(data_transfer_rate, bits_per_second, bits_per_second, bps, unit_conversion<std::ratio<1, 8>, bytes_per_second>)
	UNIT_ADD(data_transfer_rate, exabits_per_second, exabits_per_second, Ebps, unit_conversion<std::ratio<1000>, petabits_per_second>)

	UNIT_ADD_DIMENSION_TRAIT(data_transfer_rate)
#endif

	// clang-format on

	//------------------------------
	//	CONSTANTS
	//------------------------------

	/**
	 * @brief		namespace for physical constants like PI and Avogadro's Number.
	 * @sa			See unit for more information on unit type containers.
	 */
	namespace constants
	{
		/**
		 * @name Unit Containers
		 * @anchor constantContainers
		 * @{
		 */
		namespace detail
		{
			using PI = unit_conversion<std::ratio<1>, dimensionless_unit, std::ratio<1>>;
		}

		// clang-format off
		inline constexpr const unit<detail::PI>																												pi(1);											///< Ratio of a circle's circumference to its diameter.
		inline constexpr const velocity::meters_per_second_t																								c(299792458.0);									///< Speed of light in vacuum.
		inline constexpr const unit<compound_unit_conversion<cubed<length::meters>, inverse<mass::kilogram>, inverse<squared<time::seconds>>>>				G(6.67408e-11);									///< Newtonian constant of gravitation.
		inline constexpr const unit<compound_unit_conversion<energy::joule, time::seconds>>																	h(6.626070040e-34);								///< Planck constant.
		inline constexpr const unit<compound_unit_conversion<force::newtons, inverse<squared<current::ampere>>>>											mu0(pi * 4.0e-7 * force::newton_t(1) / pow<2>(current::ampere_t(1)));					///< vacuum permeability.
		inline constexpr const unit<compound_unit_conversion<capacitance::farad, inverse<length::meter>>>													epsilon0(1.0 / (mu0 * pow<2>(c)));				///< vacuum permitivity.
		inline constexpr const impedance::ohm_t																												Z0(mu0 * c);									///< characteristic impedance of vacuum.
		inline constexpr const unit<compound_unit_conversion<force::newtons, area::square_meter, inverse<squared<charge::coulomb>>>>						k_e(1.0 / (4 * pi * epsilon0));					///< Coulomb's constant.
		inline constexpr const charge::coulomb_t																											e(1.6021766208e-19);							///< elementary charge.
		inline constexpr const mass::kilogram_t																												m_e(9.10938356e-31);							///< electron mass.
		inline constexpr const mass::kilogram_t																												m_p(1.672621898e-27);							///< proton mass.
		inline constexpr const unit<compound_unit_conversion<energy::joules, inverse<magnetic_field_strength::tesla>>>										mu_B(e * h / (4 * pi *m_e));					///< Bohr magneton.
		inline constexpr const unit<inverse<substance::mol>>																								N_A(6.022140857e23);							///< Avagadro's Number.
		inline constexpr const unit<compound_unit_conversion<energy::joules, inverse<temperature::kelvin>, inverse<substance::moles>>>						R(8.3144598);									///< Gas constant.
		inline constexpr const unit<compound_unit_conversion<energy::joules, inverse<temperature::kelvin>>>													k_B(R / N_A);									///< Boltzmann constant.
		inline constexpr const unit<compound_unit_conversion<charge::coulomb, inverse<substance::mol>>>														F(N_A * e);										///< Faraday constant.
		inline constexpr const unit<compound_unit_conversion<power::watts, inverse<area::square_meters>, inverse<squared<squared<temperature::kelvin>>>>>	sigma((2 * pow<5>(pi) * pow<4>(R)) / (15 * pow<3>(h) * pow<2>(c) * pow<4>(N_A)));	///< Stefan-Boltzmann constant.
		/** @} */
		// clang-format on
	} // namespace constants
} // end namespace units

#endif // units_h__

// For Emacs
// Local Variables:
// Mode: C++
// c-basic-offset: 2
// fill-column: 116
// tab-width: 4
// End:
