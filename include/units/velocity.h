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
/// @file	units/velocity.h
/// @brief	units representing velocity values
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_velocity_h__
#define units_velocity_h__

#include <units/length.h>
#include <units/time.h>

namespace units
{
	/**
	 * @namespace	units::velocity
	 * @brief		namespace for unit types and containers representing velocity values
	 * @details		The SI unit for velocity is `meters_per_second`, and the corresponding `dimension` dimension is
	 *				`velocity_unit`.
	 * @anchor		velocityContainers
	 * @sa			See unit for more information on unit type containers.
	 */
	UNIT_ADD(
		velocity, meters_per_second, meters_per_second, mps, unit_conversion<std::ratio<1>, units::dimension::velocity>)
	UNIT_ADD(
		velocity, feet_per_second, feet_per_second, fps, compound_unit_conversion<length::feet, inverse<time::seconds>>)
	UNIT_ADD(
		velocity, miles_per_hour, miles_per_hour, mph, compound_unit_conversion<length::miles, inverse<time::hour>>)
	UNIT_ADD(velocity, kilometers_per_hour, kilometers_per_hour, kph,
		compound_unit_conversion<length::kilometers, inverse<time::hour>>)
	UNIT_ADD(velocity, knot, knots, kts, compound_unit_conversion<length::nauticalMiles, inverse<time::hour>>)

	UNIT_ADD_DIMENSION_TRAIT(velocity)
} // namespace units

#endif // units_velocity_h__
