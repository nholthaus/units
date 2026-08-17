#pragma once
#include <units/velocity.h>
#include "corr_dispatch_generic.h"
// a MORE-SPECIALIZED overload for named speeds, in the velocity add-on header:
inline double handle(units::velocity::meters_per_second<double> v){ return 100.0 + v.value(); }
