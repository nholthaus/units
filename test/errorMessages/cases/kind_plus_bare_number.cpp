// Case: adding a bare number to a kind is ill-formed (only kind +/- same-tag kind, and kind * / scalar are
// defined). The common naive fumble gets a readable message, not a candidate wall.
//
// expect: fail
// expect-match: cannot add a bare number to a kind
// forbid-match: conversion_factor<std::ratio
// forbid-match-gcc: candidate
#include <units/kind.h>
#include <units/length.h>
using namespace units;
using namespace units::length;
auto bad = kind<"radial", meters<double>>(5.0) + 3.0; // ill-formed: kind + bare number
int main()
{
	(void)bad;
	return 0;
}
