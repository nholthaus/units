/// @file    dimensional_analysis.cpp
/// @brief   Arithmetic carries dimensions: products and quotients deduce the correct result unit,
///          and the compiler verifies the dimensional analysis for you.
/// @details See docs/explain/dimensional-analysis.md.

//! [dimensional-analysis]
#include <units/length.h>
#include <units/time.h>
#include <units/area.h>
#include <units/velocity.h>
#include <iostream>

int main()
{
    using namespace units;
    using namespace units::literals;

    // Multiplying two lengths yields an area; dividing a length by a time yields a velocity.
    // The result type is deduced from the operands, not asserted by you.
    square_meters       floor = 4.0_m * 3.0_m;      // 12 m^2
    meters_per_second   speed = 100.0_m / 8.0_s;    // 12.5 m/s

    std::cout << floor << " and " << speed << '\n';  // prints: 12 m^2 and 12.5 mps

    // A relationship that must hold is a compile-time fact, checked once and never at run time:
    static_assert(1.0_km + 1.0_m == 1001.0_m);
}
//! [dimensional-analysis]
