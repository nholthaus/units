#include <units/frequency.h>   // 1.0/seconds is hertz<double>: consume_named mangles with hertz; Holder holds hertz
#include "invf_sig_shared.h"
// Expose both entities so the symbols land in this .o for the nm dump.
int          call_consume_a() { return consume_named(1.0 / units::seconds<double>(4)); }
unsigned long call_layout_a() { return layout_probe(4); }
