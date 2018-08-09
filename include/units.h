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

#include <units/core.h>

namespace units
{
	// clang-format off

	//------------------------------
	//	LENGTH UNITS
	//------------------------------

	/**
	 * @namespace	units::length
	 * @brief		namespace for unit types and containers representing length values
	 * @details		The SI unit for length is `meters`, and the corresponding `dimension` dimension is
	 *				`length_unit`.
	 * @anchor		lengthContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_LENGTH_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(length, meter, meters, m, unit_conversion<std::ratio<1>, units::dimension::length>)
	UNIT_ADD(length, foot, feet, ft, unit_conversion<std::ratio<381, 1250>, meters>)
	UNIT_ADD(length, mil, mils, mil, unit_conversion<std::ratio<1000>, feet>)
	UNIT_ADD(length, inch, inches, in, unit_conversion<std::ratio<1, 12>, feet>)
	UNIT_ADD(length, mile, miles, mi, unit_conversion<std::ratio<5280>, feet>)
	UNIT_ADD(length, nauticalMile, nauticalMiles, nmi, unit_conversion<std::ratio<1852>, meters>)
	UNIT_ADD(length, astronomicalUnit, astronomicalUnits, au, unit_conversion<std::ratio<149597870700>, meters>)
	UNIT_ADD(length, lightyear, lightyears, ly, unit_conversion<std::ratio<9460730472580800>, meters>)
	UNIT_ADD(length, parsec, parsecs, pc, unit_conversion<std::ratio<648000>, astronomicalUnits, std::ratio<-1>>)
	UNIT_ADD(length, angstrom, angstroms, angstrom, unit_conversion<std::ratio<1, 10>, nanometers>)
	UNIT_ADD(length, cubit, cubits, cbt, unit_conversion<std::ratio<18>, inches>)
	UNIT_ADD(length, fathom, fathoms, ftm, unit_conversion<std::ratio<6>, feet>)
	UNIT_ADD(length, chain, chains, ch, unit_conversion<std::ratio<66>, feet>)
	UNIT_ADD(length, furlong, furlongs, fur, unit_conversion<std::ratio<10>, chains>)
	UNIT_ADD(length, hand, hands, hand, unit_conversion<std::ratio<4>, inches>)
	UNIT_ADD(length, league, leagues, lea, unit_conversion<std::ratio<3>, miles>)
	UNIT_ADD(length, nauticalLeague, nauticalLeagues, nl, unit_conversion<std::ratio<3>, nauticalMiles>)
	UNIT_ADD(length, yard, yards, yd, unit_conversion<std::ratio<3>, feet>)

	// british spelling aliases
	using metre_t = length::meter_t;

	UNIT_ADD_DIMENSION_TRAIT(length)
#endif

	//------------------------------
	//	MASS UNITS
	//------------------------------

	/**
	 * @namespace	units::mass
	 * @brief		namespace for unit types and containers representing mass values
	 * @details		The SI unit for mass is `kilograms`, and the corresponding `dimension` dimension is
	 *				`mass_unit`.
	 * @anchor		massContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_MASS_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(mass, gram, grams, g, unit_conversion<std::ratio<1, 1000>, units::dimension::mass>)
	UNIT_ADD(mass, metric_ton, metric_tons, t, unit_conversion<std::ratio<1000>, kilograms>)
	UNIT_ADD(mass, pound, pounds, lb, unit_conversion<std::ratio<45359237, 100000000>, kilograms>)
	UNIT_ADD(mass, long_ton, long_tons, ln_t, unit_conversion<std::ratio<2240>, pounds>)
	UNIT_ADD(mass, short_ton, short_tons, sh_t, unit_conversion<std::ratio<2000>, pounds>)
	UNIT_ADD(mass, stone, stone, st, unit_conversion<std::ratio<14>, pounds>)
	UNIT_ADD(mass, ounce, ounces, oz, unit_conversion<std::ratio<1, 16>, pounds>)
	UNIT_ADD(mass, carat, carats, ct, unit_conversion<std::ratio<200>, milligrams>)
	UNIT_ADD(mass, slug, slugs, slug, unit_conversion<std::ratio<145939029, 10000000>, kilograms>)

	UNIT_ADD_DIMENSION_TRAIT(mass)
#endif

	//------------------------------
	//	TIME UNITS
	//------------------------------

	/**
	 * @namespace	units::time
	 * @brief		namespace for unit types and containers representing time values
	 * @details		The SI unit for time is `seconds`, and the corresponding `dimension` dimension is
	 *				`time_unit`.
	 * @anchor		timeContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_TIME_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(time, second, seconds, s, unit_conversion<std::ratio<1>, units::dimension::time>)
	UNIT_ADD(time, minute, minutes, min, unit_conversion<std::ratio<60>, seconds>)
	UNIT_ADD(time, hour, hours, hr, unit_conversion<std::ratio<60>, minutes>)
	UNIT_ADD(time, day, days, d, unit_conversion<std::ratio<24>, hours>)
	UNIT_ADD(time, week, weeks, wk, unit_conversion<std::ratio<7>, days>)
	UNIT_ADD(time, year, years, yr, unit_conversion<std::ratio<365>, days>)
	UNIT_ADD(time, julian_year, julian_years, a_j,	unit_conversion<std::ratio<31557600>, seconds>)
	UNIT_ADD(time, gregorian_year, gregorian_years, a_g, unit_conversion<std::ratio<31556952>, seconds>)

	UNIT_ADD_DIMENSION_TRAIT(time)
#endif

	//------------------------------
	//	ANGLE UNITS
	//------------------------------

	/**
	 * @namespace	units::angle
	 * @brief		namespace for unit types and containers representing angle values
	 * @details		The SI unit for angle is `radians`, and the corresponding `dimension` dimension is
	 *				`angle_unit`.
	 * @anchor		angleContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(angle, radian, radians, rad, unit_conversion<std::ratio<1>, units::dimension::angle>)
	UNIT_ADD(angle, degree, degrees, deg, unit_conversion<std::ratio<1, 180>, radians, std::ratio<1>>)
	UNIT_ADD(angle, arcminute, arcminutes, arcmin, unit_conversion<std::ratio<1, 60>, degrees>)
	UNIT_ADD(angle, arcsecond, arcseconds, arcsec, unit_conversion<std::ratio<1, 60>, arcminutes>)
	UNIT_ADD(angle, milliarcsecond, milliarcseconds, mas, milli<arcseconds>)
	UNIT_ADD(angle, turn, turns, tr, unit_conversion<std::ratio<2>, radians, std::ratio<1>>)
	UNIT_ADD(angle, gradian, gradians, gon, unit_conversion<std::ratio<1, 400>, turns>)

	UNIT_ADD_DIMENSION_TRAIT(angle)
#endif

	//------------------------------
	//	UNITS OF CURRENT
	//------------------------------
	/**
	 * @namespace	units::current
	 * @brief		namespace for unit types and containers representing current values
	 * @details		The SI unit for current is `amperes`, and the corresponding `dimension` dimension is
	 *				`current_unit`.
	 * @anchor		currentContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_CURRENT_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(current, ampere, amperes, A, unit_conversion<std::ratio<1>, units::dimension::current>)

	UNIT_ADD_DIMENSION_TRAIT(current)
#endif

	//------------------------------
	//	UNITS OF TEMPERATURE
	//------------------------------

	// NOTE: temperature units have special conversion overloads, since they
	// require translations and aren't a reversible transform.

	/**
	 * @namespace	units::temperature
	 * @brief		namespace for unit types and containers representing temperature values
	 * @details		The SI unit for temperature is `kelvin`, and the corresponding `dimension` dimension is
	 *				`temperature_unit`.
	 * @anchor		temperatureContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_TEMPERATURE_UNITS)
	UNIT_ADD(temperature, kelvin, kelvin, K, unit_conversion<std::ratio<1>, units::dimension::temperature>)
	UNIT_ADD(temperature, celsius, celsius, degC, unit_conversion<std::ratio<1>, kelvin, std::ratio<0>, std::ratio<27315, 100>>)
	UNIT_ADD(temperature, fahrenheit, fahrenheit, degF, unit_conversion<std::ratio<5, 9>, celsius, std::ratio<0>, std::ratio<-160, 9>>)
	UNIT_ADD(temperature, reaumur, reaumur, Re, unit_conversion<std::ratio<10, 8>, celsius>)
	UNIT_ADD(temperature, rankine, rankine, Ra, unit_conversion<std::ratio<5, 9>, kelvin>)

	UNIT_ADD_DIMENSION_TRAIT(temperature)
#endif

	//------------------------------
	//	UNITS OF AMOUNT OF SUBSTANCE
	//------------------------------

	/**
	 * @namespace	units::substance
	 * @brief		namespace for unit types and containers representing substance values
	 * @details		The SI unit for substance is `moles`, and the corresponding `dimension` dimension is
	 *				`substance_unit`.
	 * @anchor		substanceContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_SUBSTANCE_UNITS)
	UNIT_ADD(substance, mole, moles, mol, unit_conversion<std::ratio<1>, units::dimension::substance>)

	UNIT_ADD_DIMENSION_TRAIT(substance)
#endif

	//------------------------------
	//	UNITS OF LUMINOUS INTENSITY
	//------------------------------

	/**
	 * @namespace	units::luminous_intensity
	 * @brief		namespace for unit types and containers representing luminous_intensity values
	 * @details		The SI unit for luminous_intensity is `candelas`, and the corresponding `dimension` dimension is
	 *				`luminous_intensity_unit`.
	 * @anchor		luminousIntensityContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_LUMINOUS_INTENSITY_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(luminous_intensity, candela, candelas, cd, unit_conversion<std::ratio<1>, units::dimension::luminous_intensity>)

	UNIT_ADD_DIMENSION_TRAIT(luminous_intensity)
#endif

	//------------------------------
	//	UNITS OF SOLID ANGLE
	//------------------------------

	/**
	 * @namespace	units::solid_angle
	 * @brief		namespace for unit types and containers representing solid_angle values
	 * @details		The SI unit for solid_angle is `steradians`, and the corresponding `dimension` dimension is
	 *				`solid_angle_unit`.
	 * @anchor		solidAngleContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_SOLID_ANGLE_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(solid_angle, steradian, steradians, sr, unit_conversion<std::ratio<1>, units::dimension::solid_angle>)
	UNIT_ADD(solid_angle, degree_squared, degrees_squared, sq_deg, squared<angle::degrees>)
	UNIT_ADD(solid_angle, spat, spats, sp, unit_conversion<std::ratio<4>, steradians, std::ratio<1>>)

	UNIT_ADD_DIMENSION_TRAIT(solid_angle)
#endif

	//------------------------------
	//	FREQUENCY UNITS
	//------------------------------

	/**
	 * @namespace	units::frequency
	 * @brief		namespace for unit types and containers representing frequency values
	 * @details		The SI unit for frequency is `hertz`, and the corresponding `dimension` dimension is
	 *				`frequency_unit`.
	 * @anchor		frequencyContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_FREQUENCY_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(frequency, hertz, hertz, Hz, unit_conversion<std::ratio<1>, units::dimension::frequency>)

	UNIT_ADD_DIMENSION_TRAIT(frequency)
#endif

	//------------------------------
	//	VELOCITY UNITS
	//------------------------------

	/**
	 * @namespace	units::velocity
	 * @brief		namespace for unit types and containers representing velocity values
	 * @details		The SI unit for velocity is `meters_per_second`, and the corresponding `dimension` dimension is
	 *				`velocity_unit`.
	 * @anchor		velocityContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_VELOCITY_UNITS)
	UNIT_ADD(velocity, meters_per_second, meters_per_second, mps, unit_conversion<std::ratio<1>, units::dimension::velocity>)
	UNIT_ADD(velocity, feet_per_second, feet_per_second, fps, compound_unit_conversion<length::feet, inverse<time::seconds>>)
	UNIT_ADD(velocity, miles_per_hour, miles_per_hour, mph, compound_unit_conversion<length::miles, inverse<time::hour>>)
	UNIT_ADD(velocity, kilometers_per_hour, kilometers_per_hour, kph, compound_unit_conversion<length::kilometers, inverse<time::hour>>)
	UNIT_ADD(velocity, knot, knots, kts, compound_unit_conversion<length::nauticalMiles, inverse<time::hour>>)

	UNIT_ADD_DIMENSION_TRAIT(velocity)
#endif

	//------------------------------
	//	ANGULAR VELOCITY UNITS
	//------------------------------

	/**
	 * @namespace	units::angular_velocity
	 * @brief		namespace for unit types and containers representing angular velocity values
	 * @details		The SI unit for angular velocity is `radians_per_second`, and the corresponding `dimension` dimension is
	 *				`angular_velocity_unit`.
	 * @anchor		angularVelocityContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGULAR_VELOCITY_UNITS)
	UNIT_ADD(angular_velocity, radians_per_second, radians_per_second, rad_per_s, unit_conversion<std::ratio<1>, units::dimension::angular_velocity>)
	UNIT_ADD(angular_velocity, degrees_per_second, degrees_per_second, deg_per_s, compound_unit_conversion<angle::degrees, inverse<time::seconds>>)
	UNIT_ADD(angular_velocity, revolutions_per_minute, revolutions_per_minute, rpm, unit_conversion<std::ratio<2, 60>, radians_per_second, std::ratio<1>>)
	UNIT_ADD(angular_velocity, milliarcseconds_per_year, milliarcseconds_per_year, mas_per_yr, compound_unit_conversion<angle::milliarcseconds, inverse<time::year>>)

	UNIT_ADD_DIMENSION_TRAIT(angular_velocity)
#endif

	//------------------------------
	//	UNITS OF ACCELERATION
	//------------------------------

	/**
	 * @namespace	units::acceleration
	 * @brief		namespace for unit types and containers representing acceleration values
	 * @details		The SI unit for acceleration is `meters_per_second_squared`, and the corresponding `dimension` dimension is
	 *				`acceleration_unit`.
	 * @anchor		accelerationContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ACCELERATION_UNITS)
	UNIT_ADD(acceleration, meters_per_second_squared, meters_per_second_squared, mps_sq, unit_conversion<std::ratio<1>, units::dimension::acceleration>)
	UNIT_ADD(acceleration, feet_per_second_squared, feet_per_second_squared, fps_sq, compound_unit_conversion<length::feet, inverse<squared<time::seconds>>>)
	UNIT_ADD(acceleration, standard_gravity, standard_gravity, SG, unit_conversion<std::ratio<980665, 100000>, meters_per_second_squared>)

	UNIT_ADD_DIMENSION_TRAIT(acceleration)
#endif

	//------------------------------
	//	UNITS OF FORCE
	//------------------------------

	/**
	 * @namespace	units::force
	 * @brief		namespace for unit types and containers representing force values
	 * @details		The SI unit for force is `newtons`, and the corresponding `dimension` dimension is
	 *				`force_unit`.
	 * @anchor		forceContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_FORCE_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(force, newton, newtons, N, unit_conversion<std::ratio<1>, units::dimension::force>)
	UNIT_ADD(force, pound, pounds, lbf, compound_unit_conversion<mass::slug, length::foot, inverse<squared<time::seconds>>>)
	UNIT_ADD(force, dyne, dynes, dyn, unit_conversion<std::ratio<1, 100000>, newtons>)
	UNIT_ADD(force, kilopond, kiloponds, kp, compound_unit_conversion<acceleration::standard_gravity, mass::kilograms>)
	UNIT_ADD(force, poundal, poundals, pdl, compound_unit_conversion<mass::pound, length::foot, inverse<squared<time::seconds>>>)

	UNIT_ADD_DIMENSION_TRAIT(force)
#endif

}

#include <units/pressure.h>

namespace units
{

	//------------------------------
	//	UNITS OF CHARGE
	//------------------------------

	/**
	 * @namespace	units::charge
	 * @brief		namespace for unit types and containers representing charge values
	 * @details		The SI unit for charge is `coulombs`, and the corresponding `dimension` dimension is
	 *				`charge_unit`.
	 * @anchor		chargeContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_CHARGE_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(charge, coulomb, coulombs, C, unit_conversion<std::ratio<1>, units::dimension::charge>)
	UNIT_ADD_WITH_METRIC_PREFIXES(charge, ampere_hour, ampere_hours, Ah, compound_unit_conversion<current::ampere, time::hours>)

	UNIT_ADD_DIMENSION_TRAIT(charge)
#endif

	//------------------------------
	//	UNITS OF ENERGY
	//------------------------------

	/**
	 * @namespace	units::energy
	 * @brief		namespace for unit types and containers representing energy values
	 * @details		The SI unit for energy is `joules`, and the corresponding `dimension` dimension is
	 *				`energy_unit`.
	 * @anchor		energyContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ENERGY_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(energy, joule, joules, J, unit_conversion<std::ratio<1>, units::dimension::energy>)
	UNIT_ADD_WITH_METRIC_PREFIXES(energy, calorie, calories, cal, unit_conversion<std::ratio<4184, 1000>, joules>)
	UNIT_ADD(energy, kilowatt_hour, kilowatt_hours, kWh, unit_conversion<std::ratio<36, 10>, megajoules>)
	UNIT_ADD(energy, watt_hour, watt_hours, Wh, unit_conversion<std::ratio<1, 1000>, kilowatt_hours>)
	UNIT_ADD(energy, british_thermal_unit, british_thermal_units, BTU, unit_conversion<std::ratio<105505585262, 100000000>, joules>)
	UNIT_ADD(energy, british_thermal_unit_iso, british_thermal_units_iso, BTU_iso, unit_conversion<std::ratio<1055056, 1000>, joules>)
	UNIT_ADD(energy, british_thermal_unit_59, british_thermal_units_59, BTU59, unit_conversion<std::ratio<1054804, 1000>, joules>)
	UNIT_ADD(energy, therm, therms, thm, unit_conversion<std::ratio<100000>, british_thermal_units_59>)
	UNIT_ADD(energy, foot_pound, foot_pounds, ftlbf, unit_conversion<std::ratio<13558179483314004, 10000000000000000>, joules>)

	UNIT_ADD_DIMENSION_TRAIT(energy)
#endif

	//------------------------------
	//	UNITS OF POWER
	//------------------------------

	/**
	 * @namespace	units::power
	 * @brief		namespace for unit types and containers representing power values
	 * @details		The SI unit for power is `watts`, and the corresponding `dimension` dimension is
	 *				`power_unit`.
	 * @anchor		powerContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_POWER_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(power, watt, watts, W, unit_conversion<std::ratio<1>, units::dimension::power>)
	UNIT_ADD(power, horsepower, horsepower, hp, unit_conversion<std::ratio<7457, 10>, watts>)
	UNIT_ADD_DECIBEL(power, watt, dBW)
	UNIT_ADD_DECIBEL(power, milliwatt, dBm)

	UNIT_ADD_DIMENSION_TRAIT(power)
#endif

	//------------------------------
	//	UNITS OF VOLTAGE
	//------------------------------

	/**
	 * @namespace	units::voltage
	 * @brief		namespace for unit types and containers representing voltage values
	 * @details		The SI unit for voltage is `volts`, and the corresponding `dimension` dimension is
	 *				`voltage_unit`.
	 * @anchor		voltageContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_VOLTAGE_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(voltage, volt, volts, V, unit_conversion<std::ratio<1>, units::dimension::voltage>)
	UNIT_ADD(voltage, statvolt, statvolts, statV, unit_conversion<std::ratio<1000000, 299792458>, volts>)
	UNIT_ADD(voltage, abvolt, abvolts, abV, unit_conversion<std::ratio<1, 100000000>, volts>)

	UNIT_ADD_DIMENSION_TRAIT(voltage)
#endif

	//------------------------------
	//	UNITS OF CAPACITANCE
	//------------------------------

	/**
	 * @namespace	units::capacitance
	 * @brief		namespace for unit types and containers representing capacitance values
	 * @details		The SI unit for capacitance is `farads`, and the corresponding `dimension` dimension is
	 *				`capacitance_unit`.
	 * @anchor		capacitanceContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_CAPACITANCE_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(capacitance, farad, farads, F, unit_conversion<std::ratio<1>, units::dimension::capacitance>)

	UNIT_ADD_DIMENSION_TRAIT(capacitance)
#endif

	//------------------------------
	//	UNITS OF IMPEDANCE
	//------------------------------

	/**
	 * @namespace	units::impedance
	 * @brief		namespace for unit types and containers representing impedance values
	 * @details		The SI unit for impedance is `ohms`, and the corresponding `dimension` dimension is
	 *				`impedance_unit`.
	 * @anchor		impedanceContainers
	 * @sa			See unit for more information on unit type containers.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_IMPEDANCE_UNITS)
	UNIT_ADD_WITH_METRIC_PREFIXES(impedance, ohm, ohms, Ohm, unit_conversion<std::ratio<1>, units::dimension::impedance>)

	UNIT_ADD_DIMENSION_TRAIT(impedance)
#endif

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

	//----------------------------------
	//	UNIT-ENABLED CMATH FUNCTIONS
	//----------------------------------

	//----------------------------------
	//	TRIGONOMETRIC FUNCTIONS
	//----------------------------------

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute cosine
	 * @details		The input value can be in any unit of angle, including radians or degrees.
	 * @tparam		AngleUnit	any `unit` type of `dimension::angle`.
	 * @param[in]	angle		angle to compute the cosine of
	 * @returns		Returns the cosine of <i>angle</i>
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
	template<class AngleUnit, std::enable_if_t<traits::is_angle_unit_v<AngleUnit>, int> = 0>
	unit<dimensionless_unit, detail::floating_point_promotion_t<typename AngleUnit::underlying_type>> cos(
		const AngleUnit angle) noexcept
	{
		return std::cos(
			convert<unit<angle::radian, detail::floating_point_promotion_t<typename AngleUnit::underlying_type>>>(
				angle)());
	}
#endif

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute sine
	 * @details		The input value can be in any unit of angle, including radians or degrees.
	 * @tparam		AngleUnit	any `unit` type of `dimension::angle`.
	 * @param[in]	angle		angle to compute the since of
	 * @returns		Returns the sine of <i>angle</i>
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
	template<class AngleUnit, std::enable_if_t<traits::is_angle_unit_v<AngleUnit>, int> = 0>
	unit<dimensionless_unit, detail::floating_point_promotion_t<typename AngleUnit::underlying_type>> sin(
		const AngleUnit angle) noexcept
	{
		return std::sin(
			convert<unit<angle::radian, detail::floating_point_promotion_t<typename AngleUnit::underlying_type>>>(
				angle)());
	}
#endif
	/**
	 * @ingroup		UnitMath
	 * @brief		Compute tangent
	 * @details		The input value can be in any unit of angle, including radians or degrees.
	 * @tparam		AngleUnit	any `unit` type of `dimension::angle`.
	 * @param[in]	angle		angle to compute the tangent of
	 * @returns		Returns the tangent of <i>angle</i>
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
	template<class AngleUnit, std::enable_if_t<traits::is_angle_unit_v<AngleUnit>, int> = 0>
	unit<dimensionless_unit, detail::floating_point_promotion_t<typename AngleUnit::underlying_type>> tan(
		const AngleUnit angle) noexcept
	{
		return std::tan(
			convert<unit<angle::radian, detail::floating_point_promotion_t<typename AngleUnit::underlying_type>>>(
				angle)());
	}
#endif

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute arc cosine
	 * @details		Returns the principal value of the arc cosine of x, expressed in radians.
	 * @param[in]	x		Value whose arc cosine is computed, in the interval [-1,+1].
	 * @returns		Principal arc cosine of x, in the interval [0,pi] radians.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
	template<class dimensionlessUnit, std::enable_if_t<traits::is_dimensionless_unit_v<dimensionlessUnit>, int> = 0>
	unit<angle::radian, detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>> acos(
		const dimensionlessUnit x) noexcept
	{
		return unit<angle::radian, detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>>(
			std::acos(x()));
	}
#endif

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute arc sine
	 * @details		Returns the principal value of the arc sine of x, expressed in radians.
	 * @param[in]	x		Value whose arc sine is computed, in the interval [-1,+1].
	 * @returns		Principal arc sine of x, in the interval [-pi/2,+pi/2] radians.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
	template<class dimensionlessUnit, std::enable_if_t<traits::is_dimensionless_unit_v<dimensionlessUnit>, int> = 0>
	unit<angle::radian, detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>> asin(
		const dimensionlessUnit x) noexcept
	{
		return unit<angle::radian, detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>>(
			std::asin(x()));
	}
#endif

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute arc tangent
	 * @details		Returns the principal value of the arc tangent of x, expressed in radians.
	 *				Notice that because of the sign ambiguity, the function cannot determine with
	 *				certainty in which quadrant the angle falls only by its tangent value. See
	 *				atan2 for an alternative that takes a fractional argument instead.
	 * @tparam		AngleUnit	any `unit` type of `dimension::angle`.
	 * @param[in]	x		Value whose arc tangent is computed, in the interval [-1,+1].
	 * @returns		Principal arc tangent of x, in the interval [-pi/2,+pi/2] radians.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
	template<class dimensionlessUnit, std::enable_if_t<traits::is_dimensionless_unit_v<dimensionlessUnit>, int> = 0>
	unit<angle::radian, detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>> atan(
		const dimensionlessUnit x) noexcept
	{
		return unit<angle::radian, detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>>(
			std::atan(x()));
	}
#endif

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute arc tangent with two parameters
	 * @details		To compute the value, the function takes into account the sign of both arguments in order to
	 *				determine the quadrant.
	 * @param[in]	y		y-component of the triangle expressed.
	 * @param[in]	x		x-component of the triangle expressed.
	 * @returns		Returns the principal value of the arc tangent of <i>y/x</i>, expressed in radians.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
	template<class Y, class X,
		std::enable_if_t<traits::is_dimensionless_unit_v<decltype(std::declval<Y>() / std::declval<X>())>, int> = 0>
	unit<angle::radian,
		detail::floating_point_promotion_t<
			std::common_type_t<typename X::underlying_type, typename Y::underlying_type>>>
	atan2(const Y y, const X x) noexcept
	{
		using CommonUnit = std::common_type_t<X, Y>;
		// X and Y could be different length units, so normalize them
		return unit<angle::radian, detail::floating_point_promotion_t<typename CommonUnit::underlying_type>>(
			std::atan2(CommonUnit(y)(), CommonUnit(x)()));
	}
#endif

	//----------------------------------
	//	HYPERBOLIC TRIG FUNCTIONS
	//----------------------------------

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute hyperbolic cosine
	 * @details		The input value can be in any unit of angle, including radians or degrees.
	 * @tparam		AngleUnit	any `unit` type of `dimension::angle`.
	 * @param[in]	angle		angle to compute the hyperbolic cosine of
	 * @returns		Returns the hyperbolic cosine of <i>angle</i>
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
	template<class AngleUnit, std::enable_if_t<traits::is_angle_unit_v<AngleUnit>, int> = 0>
	unit<dimensionless_unit, detail::floating_point_promotion_t<typename AngleUnit::underlying_type>> cosh(
		const AngleUnit angle) noexcept
	{
		return std::cosh(
			convert<unit<angle::radian, detail::floating_point_promotion_t<typename AngleUnit::underlying_type>>>(
				angle)());
	}
#endif

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute hyperbolic sine
	 * @details		The input value can be in any unit of angle, including radians or degrees.
	 * @tparam		AngleUnit	any `unit` type of `dimension::angle`.
	 * @param[in]	angle		angle to compute the hyperbolic sine of
	 * @returns		Returns the hyperbolic sine of <i>angle</i>
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
	template<class AngleUnit, std::enable_if_t<traits::is_angle_unit_v<AngleUnit>, int> = 0>
	unit<dimensionless_unit, detail::floating_point_promotion_t<typename AngleUnit::underlying_type>> sinh(
		const AngleUnit angle) noexcept
	{
		return std::sinh(
			convert<unit<angle::radian, detail::floating_point_promotion_t<typename AngleUnit::underlying_type>>>(
				angle)());
	}
#endif

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute hyperbolic tangent
	 * @details		The input value can be in any unit of angle, including radians or degrees.
	 * @tparam		AngleUnit	any `unit` type of `dimension::angle`.
	 * @param[in]	angle		angle to compute the hyperbolic tangent of
	 * @returns		Returns the hyperbolic tangent of <i>angle</i>
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
	template<class AngleUnit, std::enable_if_t<traits::is_angle_unit_v<AngleUnit>, int> = 0>
	unit<dimensionless_unit, detail::floating_point_promotion_t<typename AngleUnit::underlying_type>> tanh(
		const AngleUnit angle) noexcept
	{
		return std::tanh(
			convert<unit<angle::radian, detail::floating_point_promotion_t<typename AngleUnit::underlying_type>>>(
				angle)());
	}
#endif

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute arc hyperbolic cosine
	 * @details		Returns the nonnegative arc hyperbolic cosine of x, expressed in radians.
	 * @param[in]	x	Value whose arc hyperbolic cosine is computed. If the argument is less
	 *					than 1, a domain error occurs.
	 * @returns		Nonnegative arc hyperbolic cosine of x, in the interval [0,+INFINITY] radians.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
	template<class dimensionlessUnit, std::enable_if_t<traits::is_dimensionless_unit_v<dimensionlessUnit>, int> = 0>
	unit<angle::radian, detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>> acosh(
		const dimensionlessUnit x) noexcept
	{
		return unit<angle::radian, detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>>(
			std::acosh(x()));
	}
#endif

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute arc hyperbolic sine
	 * @details		Returns the arc hyperbolic sine of x, expressed in radians.
	 * @param[in]	x	Value whose arc hyperbolic sine is computed.
	 * @returns		Arc hyperbolic sine of x, in radians.
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
	template<class dimensionlessUnit, std::enable_if_t<traits::is_dimensionless_unit_v<dimensionlessUnit>, int> = 0>
	unit<angle::radian, detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>> asinh(
		const dimensionlessUnit x) noexcept
	{
		return unit<angle::radian, detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>>(
			std::asinh(x()));
	}
#endif

	/**
	 * @ingroup		UnitMath
	 * @brief		Compute arc hyperbolic tangent
	 * @details		Returns the arc hyperbolic tangent of x, expressed in radians.
	 * @param[in]	x	Value whose arc hyperbolic tangent is computed, in the interval [-1,+1].
	 *					If the argument is out of this interval, a domain error occurs. For
	 *					values of -1 and +1, a pole error may occur.
	 * @returns		units::angle::radian_t
	 */
#if !defined(DISABLE_PREDEFINED_UNITS) || defined(ENABLE_PREDEFINED_ANGLE_UNITS)
	template<class dimensionlessUnit, std::enable_if_t<traits::is_dimensionless_unit_v<dimensionlessUnit>, int> = 0>
	unit<angle::radian, detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>> atanh(
		const dimensionlessUnit x) noexcept
	{
		return unit<angle::radian, detail::floating_point_promotion_t<typename dimensionlessUnit::underlying_type>>(
			std::atanh(x()));
	}
#endif
} // end namespace units

#endif // units_h__

// For Emacs
// Local Variables:
// Mode: C++
// c-basic-offset: 2
// fill-column: 116
// tab-width: 4
// End:
