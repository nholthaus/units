/// @file    defining_a_unit.cpp
/// @brief   Define a new named unit with UNIT_ADD: one line yields the type, its literal, and its name.
/// @details UNIT_ADD registers the unit, its abbreviation-based literal, and the machinery that lets
///          diagnostics print the friendly name. A unit author never hand-writes a strong-type
///          specialization. See docs/how-to/defining-new-units.md (and, for the machinery,
///          docs/explain/internals-named-types.md).

//! [defining]
#include <units/length.h>
#include <iostream>

namespace units
{
    // The smoot: a length unit equal to 67 inches (1.7018 m), defined relative to meters.
    // UNIT_ADD(dimension-namespace, plural-name, abbreviation, conversion_factor<ratio, base-unit>)
    UNIT_ADD(length, smoots, smoot, conversion_factor<std::ratio<17018, 10000>, meters<>>)
}

int main()
{
    using namespace units;
    using namespace units::literals;

    auto  bridge  = 364.4_smoot;     // the length of the Harvard Bridge
    meters in_meters = bridge;        // converts like any other length

    std::cout << bridge << " = " << in_meters << '\n';   // 364.4 smoot = 620.136 m
}
//! [defining]
