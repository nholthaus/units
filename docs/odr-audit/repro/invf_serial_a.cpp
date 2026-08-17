#include <units/frequency.h>   // 1.0/seconds resolves to frequency::hertz<double> inside serialize_inv()
#include "invf_serial_shared.h"
SerRecord from_a() { return serialize_inv(); }
