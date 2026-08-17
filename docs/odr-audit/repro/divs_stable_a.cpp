// Include EVERY dimension header that registers a named class, to prove NONE can perturb meters/kilograms
// (no named type exists for length^1 mass^-1, so there is nothing for rewrap_to_named_t to find).
#include <units/velocity.h>
#include <units/frequency.h>
#include <units/force.h>
#include <units/pressure.h>
#include <units/energy.h>
#include <units/power.h>
#include <units/area.h>
#include <units/volume.h>
#include "divs_stable_shared.h"
std::string from_a() { return describe_lm(6.0, 2.0); }
