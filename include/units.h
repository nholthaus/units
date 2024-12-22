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
// https://physics.nist.gov/cuu/Constants/index.html
//
//--------------------------------------------------------------------------------------------------
//
/// @file	units.h
/// @brief	Complete implementation of `units` - a compile-time, header-only, unit conversion
///			library built on c++14 with no dependencies.
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_h_
#define units_h_

#include <units/acceleration.h>
#include <units/angle.h>
#include <units/angular_velocity.h>
#include <units/area.h>
#include <units/capacitance.h>
#include <units/charge.h>
#include <units/concentration.h>
#include <units/conductance.h>
#include <units/current.h>
#include <units/data.h>
#include <units/data_transfer_rate.h>
#include <units/density.h>
#include <units/energy.h>
#include <units/energy_density.h>
#include <units/force.h>
#include <units/frequency.h>
#include <units/illuminance.h>
#include <units/impedance.h>
#include <units/inductance.h>
#include <units/irradiance.h>
#include <units/jerk.h>
#include <units/length.h>
#include <units/luminance.h>
#include <units/luminous_flux.h>
#include <units/luminous_intensity.h>
#include <units/magnetic_field_strength.h>
#include <units/magnetic_flux.h>
#include <units/mass.h>
#include <units/power.h>
#include <units/pressure.h>
#include <units/radiance.h>
#include <units/radiant_intensity.h>
#include <units/radiation.h>
#include <units/solid_angle.h>
#include <units/spectral_flux.h>
#include <units/spectral_intensity.h>
#include <units/spectral_irradiance.h>
#include <units/spectral_radiance.h>
#include <units/substance.h>
#include <units/substance_concentration.h>
#include <units/substance_mass.h>
#include <units/temperature.h>
#include <units/time.h>
#include <units/torque.h>
#include <units/velocity.h>
#include <units/voltage.h>
#include <units/volume.h>

namespace units
{
	namespace detail
	{
		struct PI : conversion_factor<std::ratio<1>, dimensionless_, std::ratio<1>>
		{
		};
	} // namespace detail

	//------------------------------
	//	CONSTANTS
	//------------------------------

	/**
	 * @brief   namespace for physical constants like PI and Avogadro's Number.
	 * @source  https://physics.nist.gov/cuu/Constants/index.html (2019)
	 * @sa	    See unit for more information on unit type containers.
	 */
	inline namespace constants
	{
		/**
		 * @name Unit Containers
		 * @anchor constantContainers
		 * @{
		 */

		// clang-format off
		inline constexpr unit<detail::PI>																						pi(1);							///< Ratio of a circle's circumference to its diameter.
		inline constexpr meters_per_second																						c(299792458.0);				///< Speed of light in vacuum.
		inline constexpr unit<compound_conversion_factor<cubed<meters_>, inverse<kilograms_>, inverse<squared<seconds_>>>>		G(6.67430e-11);				///< Newtonian constant of gravitation.
		inline constexpr unit<compound_conversion_factor<joules_, seconds_>>													h(6.62607015e-34);				///< Planck constant.
		inline constexpr unit<compound_conversion_factor<joules_, seconds_>>													h_bar(1.054571817e-34);		///< Reduced Planck constant.
		inline constexpr unit<compound_conversion_factor<newtons_, inverse<squared<amperes_>>>>									mu0(1.25663706212e-6);			///< vacuum permeability.
		inline constexpr unit<compound_conversion_factor<farads_, inverse<meters_>>>											epsilon0(8.8541878128e-12);	///< vacuum permittivity.
		inline constexpr ohms																									Z0(376.730313668);				///< characteristic impedance of vacuum.
		inline constexpr unit<compound_conversion_factor<newtons_, square_meters_, inverse<squared<coulombs_>>>>			    k_e(8.9875517923e9);			///< Coulomb's constant.
		inline constexpr coulombs																								e(1.602176634e-19);			///< elementary charge.
		inline constexpr kilograms																								m_e(9.1093837015e-31);			///< electron mass.
		inline constexpr kilograms																								m_p(1.67262192369e-27);		///< proton mass.
		inline constexpr unit<compound_conversion_factor<joules_, inverse<teslas_>>>											mu_B(9.2740100783e-24);		///< Bohr Magneton.
		inline constexpr unit<inverse<mols_>>																					N_A(6.02214076e23);			///< Avogadro's Number.
		inline constexpr unit<compound_conversion_factor<joules_, inverse<kelvin_>, inverse<mols_>>>					    	R(8.314462618);				///< Gas constant.
		inline constexpr unit<compound_conversion_factor<joules_, inverse<kelvin_>>>											k_B(1.380649e-23);				///< Boltzmann constant.
		inline constexpr unit<compound_conversion_factor<coulombs_, inverse<mols_>>>											F(96485.33212);				///< Faraday constant.
		inline constexpr unit<compound_conversion_factor<watts_, inverse<square_meters_>, inverse<squared<squared<kelvin_>>>>>	sigma(5.670374419e-8);			///< Stefan-Boltzmann constant.
		/** @} */
		// clang-format on
	} // namespace constants
} // end namespace units

#endif // units_h_