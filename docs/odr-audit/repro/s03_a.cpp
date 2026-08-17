#include <units/velocity.h>   // registration visible -> R = meters_per_second -> custom hash 0xBEEF
#include "s03_shared.h"
std::size_t from_a() { return hash_of(5.0, 2.0); }
