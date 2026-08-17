#include <units/velocity.h>   // meters/seconds resolves to velocity::meters_per_second<double> in serialize_mps()
#include "divc_serial_shared.h"
SerRecord from_a() { return serialize_mps(); }
