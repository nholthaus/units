/// @file    pythagoras.cpp
/// @brief   Unit-aware <cmath>: sqrt and pow<N> found by argument-dependent lookup, no units:: prefix.
/// @details The math wrappers live in namespace units and are found by ADL, so an unqualified sqrt on
///          a units type resolves to the unit-aware overload. See docs/how-to/math-functions.md.

//! [pythagoras]
#include <units/length.h>
#include <units/area.h>
#include <iostream>

int main()
{
    using namespace units;
    using namespace units::literals;

    meters a = 3.0_m;
    meters b = 4.0_m;

    // pow<2> returns square_meters; sqrt of square_meters returns meters. Both are found by ADL.
    meters hypotenuse = sqrt(pow<2>(a) + pow<2>(b));

    std::cout << hypotenuse << '\n';   // prints: 5 m
}
//! [pythagoras]
