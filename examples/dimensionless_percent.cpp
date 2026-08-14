/// @file    dimensionless_percent.cpp
/// @brief   Dimensionless quantities interoperate with built-in arithmetic; value() vs raw() for percent.
/// @details A pure dimensionless quantity converts to and from double implicitly. A ratio-dimensionless
///          quantity such as percent stores its point count: raw() is the points, value() is the fraction.
///          See docs/reference/cheat-sheet.md and the value()/raw() note in docs/meta/faq.md.

//! [dimensionless]
#include <units/concentration.h>
#include <iostream>

int main()
{
    using namespace units;
    using namespace units::literals;

    // A pure dimensionless quantity is interchangeable with double in both directions.
    dimensionless<double> ratio = 0.25;   // double -> dimensionless
    double as_double = ratio;             // dimensionless -> double (implicit)

    // percent is dimensionless but scaled: raw() is the points (50), value() is the fraction (0.5).
    auto half = 50.0_pct;
    std::cout << "raw()=" << half.raw() << "  value()=" << half.value() << '\n';   // raw()=50  value()=0.5

    std::cout << "ratio as double = " << as_double << '\n';   // 0.25
}
//! [dimensionless]
