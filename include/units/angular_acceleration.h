//--------------------------------------------------------------------------------------------------
//
//	UnitConversion: A compile-time c++23 unit conversion library with no dependencies
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
/// @file	units/angular_acceleration.h
/// @brief	units representing angular acceleration values
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_angular_acceleration_h_
#define units_angular_acceleration_h_

#include <units/angle.h>
#include <units/time.h>

namespace units
{
	/**
	 * @namespace	units::angular_acceleration
	 * @brief		namespace for unit types and containers representing angular acceleration values
	 * @details		The SI unit for angular acceleration is `radians_per_second_squared`, and the corresponding
	 *				dimension concept is `AngularAcceleration` (backed by the `traits::is_angular_acceleration_unit_v`
	 *				trait).
	 * @anchor		angularAccelerationContainers
	 * @sa			See unit for more information on unit type containers.
	 */
	UNIT_ADD_WITH_METRIC_PREFIXES(angular_acceleration, radians_per_second_squared, rad_per_s_sq, conversion_factor<std::ratio<1>, dimension::angular_acceleration>)
	UNIT_ADD(angular_acceleration, degrees_per_second_squared, deg_per_s_sq, compound_conversion_factor<degrees_, inverse<squared<seconds_>>>)
	UNIT_ADD(angular_acceleration, gradians_per_second_squared, gon_per_s_sq, compound_conversion_factor<gradians_, inverse<squared<seconds_>>>)
	UNIT_ADD(angular_acceleration, revolutions_per_second_squared, rev_per_s_sq, compound_conversion_factor<turns_, inverse<squared<seconds_>>>)
	UNIT_ADD(angular_acceleration, revolutions_per_minute_squared, rev_per_min_sq, compound_conversion_factor<turns_, inverse<squared<minutes_>>>)

	UNIT_ADD_DIMENSION_TRAIT(angular_acceleration, AngularAcceleration)
} // namespace units

#endif // units_angular_acceleration_h_
