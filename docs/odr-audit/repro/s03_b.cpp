#include "s03_shared.h"       // registration NOT visible -> R = plain unit<...> -> generic std::hash
std::size_t from_b() { return hash_of(5.0, 2.0); }
