#pragma once
#include <units/length.h>
#include <units/time.h>
// generic handler always available:
template<class U> double handle(U v){ return 200.0 + v.value(); }
