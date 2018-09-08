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
#include <units/force.h>
#include <units/frequency.h>
#include <units/illuminance.h>
#include <units/impedance.h>
#include <units/inductance.h>
#include <units/length.h>
#include <units/luminous_flux.h>
#include <units/luminous_intensity.h>
#include <units/magnetic_field_strength.h>
#include <units/magnetic_flux.h>
#include <units/mass.h>
#include <units/power.h>
#include <units/pressure.h>
#include <units/radiation.h>
#include <units/solid_angle.h>
#include <units/substance.h>
#include <units/temperature.h>
#include <units/time.h>
#include <units/torque.h>
#include <units/velocity.h>
#include <units/voltage.h>
#include <units/volume.h>

namespace units
{
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
			struct PI : conversion_factor<std::ratio<1>, dimensionless_unit, std::ratio<1>>
			{
			};
		} // namespace detail

		// clang-format off
		inline constexpr const unit<detail::PI>																												pi(1);											///< Ratio of a circle's circumference to its diameter.
		inline constexpr const velocity::meters_per_second_t<double>																								c(299792458.0);									///< Speed of light in vacuum.
		inline constexpr const unit<compound_conversion_factor<cubed<length::meters>, inverse<mass::kilogram>, inverse<squared<time::seconds>>>>				G(6.67408e-11);									///< Newtonian constant of gravitation.
		inline constexpr const unit<compound_conversion_factor<energy::joule, time::seconds>>																	h(6.626070040e-34);								///< Planck constant.
		inline constexpr const unit<compound_conversion_factor<force::newtons, inverse<squared<current::ampere>>>>											mu0(pi * 4.0e-7 * force::newton_t<double>(1) / pow<2>(current::ampere_t<double>(1)));					///< vacuum permeability.
		inline constexpr const unit<compound_conversion_factor<capacitance::farad, inverse<length::meter>>>													epsilon0(1.0 / (mu0 * pow<2>(c)));				///< vacuum permitivity.
		inline constexpr const impedance::ohm_t<double>																												Z0(mu0 * c);									///< characteristic impedance of vacuum.
		inline constexpr const unit<compound_conversion_factor<force::newtons, area::square_meter, inverse<squared<charge::coulomb>>>>						k_e(1.0 / (4 * pi * epsilon0));					///< Coulomb's constant.
		inline constexpr const charge::coulomb_t<double>																											e(1.6021766208e-19);							///< elementary charge.
		inline constexpr const mass::kilogram_t<double>																												m_e(9.10938356e-31);							///< electron mass.
		inline constexpr const mass::kilogram_t<double>																												m_p(1.672621898e-27);							///< proton mass.
		inline constexpr const unit<compound_conversion_factor<energy::joules, inverse<magnetic_field_strength::tesla>>>										mu_B(e * h / (4 * pi *m_e));					///< Bohr magneton.
		inline constexpr const unit<inverse<substance::mol>>																								N_A(6.022140857e23);							///< Avagadro's Number.
		inline constexpr const unit<compound_conversion_factor<energy::joules, inverse<temperature::kelvin>, inverse<substance::moles>>>						R(8.3144598);									///< Gas constant.
		inline constexpr const unit<compound_conversion_factor<energy::joules, inverse<temperature::kelvin>>>													k_B(R / N_A);									///< Boltzmann constant.
		inline constexpr const unit<compound_conversion_factor<charge::coulomb, inverse<substance::mol>>>														F(N_A * e);										///< Faraday constant.
		inline constexpr const unit<compound_conversion_factor<power::watts, inverse<area::square_meters>, inverse<squared<squared<temperature::kelvin>>>>>	sigma((2 * pow<5>(pi) * pow<4>(R)) / (15 * pow<3>(h) * pow<2>(c) * pow<4>(N_A)));	///< Stefan-Boltzmann constant.
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
