/// @file    hello_units.cpp
/// @brief   The five-line introduction: construct quantities, convert implicitly, print them.
/// @details Compiled and run as part of the examples build, so every snippet the documentation
///          quotes from this file is guaranteed to build under C++23. See docs/learn/getting-started.md.

//! [hello]
#include <units/length.h>
#include <iostream>

int main()
{
    using namespace units;
    using namespace units::literals;

    meters distance = 5.0_m;   // a length, deduced as meters<double>
    feet   in_feet  = distance; // implicit, lossless unit conversion

    std::cout << distance << " == " << in_feet << '\n';   // prints: 5 m == 16.4042 ft
}
//! [hello]
