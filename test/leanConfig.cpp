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
/// @file	test/leanConfig.cpp
/// @brief	Compile-and-run proof that the text-feature opt-out configurations still build.
/// @details	Each lean configuration (UNIT_LIB_DISABLE_IOSTREAM / _FORMAT / _STRING, and the
///			DISABLE_IOSTREAM + ENABLE_FORMAT opt-in) compiles this translation unit as a separate CTest
///			target. The core dimensional math must always work; text features are exercised only when the
///			active configuration compiles them in. This is the backward-compatibility guard: a lean build
///			the embedded audience pins must never silently break.
//
//--------------------------------------------------------------------------------------------------

#include <units.h>

#if defined(UNIT_LIB_ENABLE_FORMAT_EXPECTED)
#include <format>
#include <string>
#endif

using namespace units::literals;

int main()
{
	// Core dimensional arithmetic must compile and run in every configuration.
	const auto sum      = 6.0_m + 3.0_m;
	const auto velocity = 10.0_m / 2.0_s;
	const auto area     = 4.0_m * 5.0_m;

	bool ok = (sum.value() > 0.0) && (velocity.value() > 0.0) && (area.value() > 0.0);

#if defined(UNIT_LIB_ENABLE_FORMAT_EXPECTED)
	// When this config is expected to provide std::format, prove it renders.
	const std::string s = std::format("{:.2f}", 3.5_m);
	ok                  = ok && (s == "3.50 m");
#endif

	return ok ? 0 : 1;
}
