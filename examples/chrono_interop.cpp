/// @file    chrono_interop.cpp
/// @brief   Time quantities convert implicitly to and from std::chrono::duration in both directions.
/// @details See docs/how-to/chrono-interop.md.

//! [chrono]
#include <units/time.h>
#include <chrono>
#include <iostream>

int main()
{
    using namespace units;
    using namespace units::literals;

    // std::chrono::duration -> units time quantity (values convert; here 90 s == 1.5 min).
    std::chrono::seconds         from_std{90};
    units::time::minutes<double> as_minutes = from_std;

    // units time quantity -> std::chrono::duration
    auto                          five   = 5.0_s;
    std::chrono::duration<double> to_std = five;

    std::cout << as_minutes.value() << " min (from " << from_std.count() << " s), "
              << "and 5 s -> " << to_std.count() << " s\n";   // 1.5 min (from 90 s), and 5 s -> 5 s
}
//! [chrono]
