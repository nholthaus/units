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
/// @file	units/volume.h
/// @brief	units representing volume values
//
//--------------------------------------------------------------------------------------------------

#pragma once

#ifndef units_volume_h__
#define units_volume_h__

#include <units/length.h>

namespace units
{
	/**
	 * @namespace	units::volume
	 * @brief		namespace for unit types and containers representing volume values
	 * @details		The SI unit for volume is `cubic_meters`, and the corresponding `dimension` dimension is
	 *				`volume_unit`.
	 * @anchor		volumeContainers
	 * @sa			See unit for more information on unit type containers.
	 */
	UNIT_ADD(volume, cubic_meter, cubic_meters, cu_m, conversion_factor<std::ratio<1>, units::dimension::volume>)
	UNIT_ADD(volume, cubic_millimeter, cubic_millimeters, cu_mm, cubed<length::millimeter_conversion_factor>)
	UNIT_ADD(volume, cubic_kilometer, cubic_kilometers, cu_km, cubed<length::kilometer_conversion_factor>)
	UNIT_ADD_WITH_METRIC_PREFIXES(volume, liter, liters, L, cubed<deci<length::meter_conversion_factor>>)
	UNIT_ADD(volume, cubic_inch, cubic_inches, cu_in, cubed<length::inch_conversion_factor>)
	UNIT_ADD(volume, cubic_foot, cubic_feet, cu_ft, cubed<length::foot_conversion_factor>)
	UNIT_ADD(volume, cubic_yard, cubic_yards, cu_yd, cubed<length::yard_conversion_factor>)
	UNIT_ADD(volume, cubic_mile, cubic_miles, cu_mi, cubed<length::mile_conversion_factor>)
	UNIT_ADD(volume, gallon, gallons, gal, conversion_factor<std::ratio<231>, cubic_inch_conversion_factor>)
	UNIT_ADD(volume, quart, quarts, qt, conversion_factor<std::ratio<1, 4>, gallon_conversion_factor>)
	UNIT_ADD(volume, pint, pints, pt, conversion_factor<std::ratio<1, 2>, quart_conversion_factor>)
	UNIT_ADD(volume, cup, cups, c, conversion_factor<std::ratio<1, 2>, pint_conversion_factor>)
	UNIT_ADD(volume, fluid_ounce, fluid_ounces, fl_oz, conversion_factor<std::ratio<1, 8>, cup_conversion_factor>)
	UNIT_ADD(volume, barrel, barrels, bl, conversion_factor<std::ratio<42>, gallon_conversion_factor>)
	UNIT_ADD(volume, bushel, bushels, bu, conversion_factor<std::ratio<215042, 100>, cubic_inch_conversion_factor>)
	UNIT_ADD(volume, cord, cords, cord, conversion_factor<std::ratio<128>, cubic_foot_conversion_factor>)
	UNIT_ADD(volume, cubic_fathom, cubic_fathoms, cu_fm, cubed<length::fathom_conversion_factor>)
	UNIT_ADD(volume, tablespoon, tablespoons, tbsp, conversion_factor<std::ratio<1, 2>, fluid_ounce_conversion_factor>)
	UNIT_ADD(volume, teaspoon, teaspoons, tsp, conversion_factor<std::ratio<1, 6>, fluid_ounce_conversion_factor>)
	UNIT_ADD(volume, pinch, pinches, pinch, conversion_factor<std::ratio<1, 8>, teaspoon_conversion_factor>)
	UNIT_ADD(volume, dash, dashes, dash, conversion_factor<std::ratio<1, 2>, pinch_conversion_factor>)
	UNIT_ADD(volume, drop, drops, drop, conversion_factor<std::ratio<1, 360>, fluid_ounce_conversion_factor>)
	UNIT_ADD(volume, fifth, fifths, fifth, conversion_factor<std::ratio<1, 5>, gallon_conversion_factor>)
	UNIT_ADD(volume, dram, drams, dr, conversion_factor<std::ratio<1, 8>, fluid_ounce_conversion_factor>)
	UNIT_ADD(volume, gill, gills, gi, conversion_factor<std::ratio<4>, fluid_ounce_conversion_factor>)
	UNIT_ADD(volume, peck, pecks, pk, conversion_factor<std::ratio<1, 4>, bushel_conversion_factor>)
	UNIT_ADD(volume, sack, sacks, sacks, conversion_factor<std::ratio<3>, bushel_conversion_factor>)
	UNIT_ADD(volume, shot, shots, shots, conversion_factor<std::ratio<3, 2>, fluid_ounce_conversion_factor>)
	UNIT_ADD(volume, strike, strikes, strikes, conversion_factor<std::ratio<2>, bushel_conversion_factor>)

	UNIT_ADD_DIMENSION_TRAIT(volume)
} // namespace units

#endif // units_volume_h__
