#include "invf_sig_shared.h"    // frequency NOT visible: 1.0/seconds is plain unit<...>; consume_named mangles with it
// Expose both entities so the symbols land in this .o for the nm dump.
int          call_consume_b() { return consume_named(1.0 / units::seconds<double>(4)); }
unsigned long call_layout_b() { return layout_probe(4); }
