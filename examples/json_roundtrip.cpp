/// @file    json_roundtrip.cpp
/// @brief   Opt-in JSON serialization: include nlohmann/json before units and quantities round-trip.
/// @details The to_json / from_json hooks are enabled automatically when <nlohmann/json.hpp> is
///          visible at the point units is included; they are absent otherwise. This example compiles
///          and runs whether or not nlohmann/json is installed. See docs/how-to/json-serialization.md.

//! [json]
#if defined __has_include && __has_include(<nlohmann/json.hpp>)
#include <nlohmann/json.hpp>   // must be visible BEFORE units for the hooks to activate
#endif

#include <units/length.h>
#include <iostream>

int main()
{
    using namespace units;
    using namespace units::literals;

#if defined __has_include && __has_include(<nlohmann/json.hpp>)
    meters       distance = 42.0_m;
    nlohmann::json j = distance;      // to_json
    meters       restored = j.get<meters<double>>();   // from_json
    std::cout << "json: " << j.dump() << " -> " << restored << '\n';
#else
    std::cout << "nlohmann/json not available in this build; JSON hooks are inactive.\n";
#endif
}
//! [json]
