// Exhaustive tests for the affine absolute<>/delta<> point/amount wrappers. An adversarial wart-hunt across the
// affine (temperature), pi-carrying (angle), ratio-dimensionless (percent), and general slices — over
// double/float/int and commensurable/incommensurable/mixed-underlying operands — plus the type-safety boundaries
// (trivial-copyability, explicit ctors, plain/wrapper rejection, point+point rejection, traits/concepts, printing,
// math). The central claim under test is the LHS-unit tie-break: a wrapper operator keeps the LEFT operand's unit
// so .value() reads in the unit the user wrote (absolute<celsius> - absolute<fahrenheit> is 100 CELSIUS-degrees,
// not a common sub-unit), promoting only the underlying when a coarse integer LHS cannot hold the RHS losslessly.

#include <gtest/gtest.h>
#include <cmath>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_set>

#include <units/angle.h>
#include <units/concentration.h>
#include <units/core.h>
#include <units/energy.h>
#include <units/frequency.h>
#include <units/kind.h>
#include <units/length.h>
#include <units/temperature.h>
#include <units/time.h>
#include <units/torque.h>

// The unit namespaces are inline, so `using namespace units;` exposes every unit type (celsius, meters, ...) AND
// every single-letter literal-abbreviation variable (`d` days, `m` meters, `s` seconds, `span`) at `units::` scope.
// Local test variables are therefore named to avoid those tokens (`del`/`sum`/`extremum`/`arc`/`len`, never a bare
// `d`/`m`/`s`/`span`) so the suite stays clean under -Wshadow.
using namespace units;
using namespace units::length;
using namespace units::angle;
using namespace units::temperature;
using namespace units::concentration;
using namespace units::time;

namespace
{
	// Terse wrappers so the assertions read like the math: `A`/`D` name the point/delta wrapper types.
	template<class U>
	using A = absolute<U>;
	template<class U>
	using D = delta<U>;

	// The unit_type of a wrapper (its wrapped unit).
	template<class W>
	using wrapped_t = typename W::unit_type;

	// The underlying numeric type of a wrapper.
	template<class W>
	using under_t = typename W::underlying_type;

	// The value of a wrapper (absolute/delta/kind) OR a plain unit, as a double — one accessor for both, so a
	// test reads the same whether it holds a wrapper or has unwrapped to a plain unit.
	template<class T>
	constexpr double val(const T& x) noexcept
	{
		return static_cast<double>(x.value());
	}
} // namespace

// Assert a wrapper/unit's value equals `expected` (double), to ~1e-9 — absorbs the ULP noise from datum/scale
// round-trips (273.15 offsets, 5/9 ratios) so a test states the physical answer, not a bit pattern. Pair form
// `EXPECT_UNIT_EQ(a, b)` compares two wrappers/units by value.
#define EXPECT_UNIT_NEAR(wrapperOrUnit, expected) EXPECT_NEAR((expected), val(wrapperOrUnit), 1e-9)
#define EXPECT_UNIT_EQ(lhs, rhs) EXPECT_NEAR(val(lhs), val(rhs), 1e-9)

//======================================================================================================================
//	CONSTRUCTION, ACCESSORS, TYPE PROPERTIES
//======================================================================================================================

TEST(WrapperConstruction, unitAndScalarConstructors)
{
	// From a plain unit.
	const A<celsius<double>> fromUnit(celsius<double>(20.0));
	EXPECT_DOUBLE_EQ(20.0, fromUnit.value());

	// From the underlying numeric value directly (in U's own unit) — gap #3.
	const A<celsius<double>> fromScalar(20.0);
	EXPECT_DOUBLE_EQ(20.0, fromScalar.value());
	EXPECT_TRUE(fromScalar == fromUnit);

	const D<celsius<double>> deltaFromScalar(5.0);
	EXPECT_DOUBLE_EQ(5.0, deltaFromScalar.value());
	const D<celsius<double>> deltaFromUnit(celsius<double>(5.0));
	EXPECT_TRUE(deltaFromScalar == deltaFromUnit);

	// Default-constructed is zero.
	const A<celsius<double>> zeroPoint;
	const D<celsius<double>> zeroDelta;
	EXPECT_DOUBLE_EQ(0.0, zeroPoint.value());
	EXPECT_DOUBLE_EQ(0.0, zeroDelta.value());
}

TEST(WrapperConstruction, ctorsAreExplicit)
{
	// A wrapper is never implicitly constructible from its underlying or its unit.
	EXPECT_FALSE((std::is_convertible_v<double, A<celsius<double>>>));
	EXPECT_FALSE((std::is_convertible_v<celsius<double>, A<celsius<double>>>));
	EXPECT_FALSE((std::is_convertible_v<double, D<meters<double>>>));
	EXPECT_FALSE((std::is_convertible_v<meters<double>, D<meters<double>>>));
	// But directly constructible.
	EXPECT_TRUE((std::is_constructible_v<A<celsius<double>>, double>));
	EXPECT_TRUE((std::is_constructible_v<A<celsius<double>>, celsius<double>>));
}

TEST(WrapperConstruction, trivialCopyAndSameSize)
{
	// Zero-overhead: trivially copyable and exactly the size of the wrapped unit.
	static_assert(std::is_trivially_copyable_v<A<celsius<double>>>);
	static_assert(std::is_trivially_copyable_v<D<celsius<double>>>);
	static_assert(std::is_trivially_copyable_v<A<meters<int>>>);
	static_assert(std::is_trivially_copyable_v<D<radians<float>>>);
	static_assert(sizeof(A<celsius<double>>) == sizeof(celsius<double>));
	static_assert(sizeof(D<celsius<double>>) == sizeof(celsius<double>));
	static_assert(sizeof(A<meters<int>>) == sizeof(meters<int>));
	static_assert(sizeof(D<radians<float>>) == sizeof(radians<float>));
	SUCCEED();
}

TEST(WrapperConstruction, accessorsQuantityValueRaw)
{
	const A<kelvin<double>> p(celsius<double>(0.0)); // 0 degC = 273.15 K
	// to<PlainUnit>() unwraps to the plain unit (datum intact).
	EXPECT_DOUBLE_EQ(273.15, p.to<kelvin<double>>().value());
	EXPECT_DOUBLE_EQ(273.15, p.value());
	EXPECT_DOUBLE_EQ(273.15, p.raw());

	const D<kilometers<double>> del(kilometers<double>(2.0));
	EXPECT_DOUBLE_EQ(2.0, del.value());
	EXPECT_DOUBLE_EQ(2.0, del.raw()); // raw is the stored value in the unit's own scale (km), not linearized
}

//======================================================================================================================
//	INLINE NAMESPACE kind — top-level and qualified names are the SAME type
//======================================================================================================================

TEST(WrapperKindNamespace, inlineNamespaceNamesSameType)
{
	static_assert(std::is_same_v<absolute<celsius<double>>, affine::absolute<celsius<double>>>);
	static_assert(std::is_same_v<delta<meters<int>>, affine::delta<meters<int>>>);
	static_assert(std::is_same_v<units::absolute<radians<float>>, units::affine::absolute<radians<float>>>);
	SUCCEED();
}

//======================================================================================================================
//	TRAITS + CONCEPTS  (gap #4)
//======================================================================================================================

TEST(WrapperTraits, isAbsoluteIsDelta)
{
	static_assert(traits::is_absolute_v<A<celsius<double>>>);
	static_assert(traits::is_absolute_v<const A<celsius<double>>>);
	static_assert(!traits::is_absolute_v<D<celsius<double>>>);
	static_assert(!traits::is_absolute_v<celsius<double>>); // plain unit
	static_assert(!traits::is_absolute_v<double>);          // non-unit

	static_assert(traits::is_delta_v<D<celsius<double>>>);
	static_assert(traits::is_delta_v<const D<celsius<double>>>);
	static_assert(!traits::is_delta_v<A<celsius<double>>>);
	static_assert(!traits::is_delta_v<meters<int>>);
	static_assert(!traits::is_delta_v<int>);
	SUCCEED();
}

TEST(WrapperTraits, conceptsConstrainCorrectly)
{
	static_assert(AbsoluteType<A<celsius<double>>>);
	static_assert(!AbsoluteType<D<celsius<double>>>);
	static_assert(!AbsoluteType<celsius<double>>);

	static_assert(DeltaType<D<meters<int>>>);
	static_assert(!DeltaType<A<meters<int>>>);
	static_assert(!DeltaType<meters<int>>);
	SUCCEED();
}

//======================================================================================================================
//	AFFINE (TEMPERATURE) SLICE — the datum-carrying edge where affine math breaks
//======================================================================================================================

TEST(WrapperAffine, pointMinusPointCancelsDatumAndIsADelta)
{
	// Same unit: a straightforward difference; the datum cancels.
	const A<celsius<double>> hot(30.0);
	const A<celsius<double>> cold(20.0);
	auto del = hot - cold;
	static_assert(traits::is_delta_v<decltype(del)>);
	EXPECT_DOUBLE_EQ(10.0, del.value());
}

TEST(WrapperAffine, celsiusMinusFahrenheitReads100CelsiusDegrees)
{
	// THE headline LHS-unit case: boiling (100 degC) minus freezing (32 degF == 0 degC) is a 100 CELSIUS-degree
	// delta — NOT 900 (which is what a common-sub-unit reconciliation would read) and NOT 180 (fahrenheit-degrees).
	const A<celsius<double>>    boiling(100.0);
	const A<fahrenheit<double>> freezing(32.0);
	auto                        del = boiling - freezing;
	static_assert(traits::is_delta_v<decltype(del)>);
	// result unit keeps the LHS (celsius); value reads in celsius-degrees.
	static_assert(std::is_same_v<wrapped_t<decltype(del)>, celsius<double>>);
	EXPECT_DOUBLE_EQ(100.0, del.value());

	// Reverse order keeps the fahrenheit unit: 32 degF - 100 degC (== 212 degF) = -180 fahrenheit-degrees.
	auto dRev = freezing - boiling;
	static_assert(std::is_same_v<wrapped_t<decltype(dRev)>, fahrenheit<double>>);
	EXPECT_DOUBLE_EQ(-180.0, dRev.value());
}

TEST(WrapperAffine, pointPlusDeltaMovesThePointInLhsUnit)
{
	// point + delta -> point, kept in the point's unit.
	const A<celsius<double>> p(20.0);
	const D<celsius<double>> warm(5.0);
	auto                     warmer = p + warm;
	static_assert(traits::is_absolute_v<decltype(warmer)>);
	static_assert(std::is_same_v<wrapped_t<decltype(warmer)>, celsius<double>>);
	EXPECT_DOUBLE_EQ(25.0, warmer.value());

	// point - delta -> point (cool down).
	auto cooler = p - warm;
	static_assert(traits::is_absolute_v<decltype(cooler)>);
	EXPECT_DOUBLE_EQ(15.0, cooler.value());

	// delta + point is commutative and keeps the POINT's unit.
	auto commuted = warm + p;
	static_assert(traits::is_absolute_v<decltype(commuted)>);
	static_assert(std::is_same_v<wrapped_t<decltype(commuted)>, celsius<double>>);
	EXPECT_DOUBLE_EQ(25.0, commuted.value());
}

TEST(WrapperAffine, pointPlusMixedUnitDeltaScaleOnly)
{
	// A fahrenheit-degree delta added to a celsius point moves the point by the delta's DEGREE SIZE (scale only),
	// never applying the fahrenheit datum. 18 degF-degrees == 10 celsius-degrees, so 20 degC + 18 degF-delta = 30.
	const A<celsius<double>> p(20.0);
	const D<fahrenheit<double>> warm(18.0);
	auto                        warmer = p + warm;
	static_assert(std::is_same_v<wrapped_t<decltype(warmer)>, celsius<double>>);
	EXPECT_DOUBLE_EQ(30.0, warmer.value());
}

TEST(WrapperAffine, deltaConversionIsScaleOnlyNotAffine)
{
	// A 10 degC delta is an 18 degF delta (scale only), NOT an absolute 50 degF. to<PlainUnit>() unwraps to a
	// plain fahrenheit; to<delta<fahrenheit>>() keeps it a delta — both are the same scale-only 18.
	const D<celsius<double>> del(10.0);
	const fahrenheit<double> plainF = del.to<fahrenheit<double>>(); // unwraps to a plain unit
	EXPECT_DOUBLE_EQ(18.0, plainF.value());
	const D<fahrenheit<double>> deltaF = del.to<D<fahrenheit<double>>>(); // stays a delta
	EXPECT_UNIT_NEAR(deltaF, 18.0);

	// And a zero delta carries no offset: 0 degC delta -> 0 degF delta (not 32).
	const D<celsius<double>> z(0.0);
	EXPECT_UNIT_NEAR(z.to<fahrenheit<double>>(), 0.0);

	// A point conversion DOES apply the datum: 0 degC point -> 32 degF point.
	const A<celsius<double>> zeroPoint(0.0);
	EXPECT_UNIT_NEAR(zeroPoint.to<fahrenheit<double>>(), 32.0);
	EXPECT_UNIT_NEAR(zeroPoint.to<kelvin<double>>(), 273.15);
}

TEST(WrapperAffine, pointConversionRoundTrips)
{
	// Round-trip staying in the wrapper: to<absolute<V>> keeps it a point at each hop.
	const A<celsius<double>> body(37.0);
	const auto               k    = body.to<A<kelvin<double>>>();
	const auto               back = k.to<A<celsius<double>>>();
	EXPECT_UNIT_NEAR(back, 37.0);
}

TEST(WrapperAffine, minMaxClampOfPoints)
{
	const A<celsius<double>> a(10.0);
	const A<celsius<double>> b(30.0);
	EXPECT_DOUBLE_EQ(10.0, affine::min(a, b).value());
	EXPECT_DOUBLE_EQ(30.0, affine::max(a, b).value());

	// min/max reconcile the rhs affinely but keep the LHS unit.
	const A<fahrenheit<double>> f(32.0); // == 0 degC
	auto                        extremum = affine::min(a, f);
	static_assert(std::is_same_v<wrapped_t<decltype(extremum)>, celsius<double>>);
	EXPECT_DOUBLE_EQ(0.0, extremum.value()); // the colder of 10 degC and 0 degC is 0 degC

	// clamp of a point.
	const A<celsius<double>> lo(0.0), hi(20.0), v(37.0);
	EXPECT_DOUBLE_EQ(20.0, affine::clamp(v, lo, hi).value());
	const A<celsius<double>> v2(-5.0);
	EXPECT_DOUBLE_EQ(0.0, affine::clamp(v2, lo, hi).value());
}

//======================================================================================================================
//	DELTA ARITHMETIC + COMPOUND ASSIGNMENT  (gap #2)
//======================================================================================================================

TEST(WrapperDelta, deltaPlusMinusDelta)
{
	const D<celsius<double>> a(10.0);
	const D<celsius<double>> b(3.0);
	auto                     sum = a + b;
	static_assert(traits::is_delta_v<decltype(sum)>);
	EXPECT_DOUBLE_EQ(13.0, sum.value());
	EXPECT_DOUBLE_EQ(7.0, (a - b).value());

	// unary negate.
	auto neg = -a;
	static_assert(traits::is_delta_v<decltype(neg)>);
	EXPECT_DOUBLE_EQ(-10.0, neg.value());
}

TEST(WrapperDelta, deltaScalarMultiplyDivide)
{
	const D<meters<double>> del(4.0);
	EXPECT_DOUBLE_EQ(12.0, (del * 3.0).value());
	EXPECT_DOUBLE_EQ(12.0, (3.0 * del).value());
	EXPECT_DOUBLE_EQ(2.0, (del / 2.0).value());
	static_assert(traits::is_delta_v<decltype(del * 3.0)>);
	static_assert(traits::is_delta_v<decltype(3.0 * del)>);
	static_assert(traits::is_delta_v<decltype(del / 2.0)>);
}

TEST(WrapperDelta, integerDeltaScaledByFloatPromotes)
{
	// The wrapper is never less precise than the unit it wraps: an int delta scaled by a float yields a float delta.
	const D<meters<int>> del(5);
	auto                 scaled = del * 1.5; // 7.5 meters
	static_assert(std::is_floating_point_v<under_t<decltype(scaled)>>);
	EXPECT_DOUBLE_EQ(7.5, scaled.value());

	auto halved = del / 2.0; // 2.5 meters
	static_assert(std::is_floating_point_v<under_t<decltype(halved)>>);
	EXPECT_DOUBLE_EQ(2.5, halved.value());
}

TEST(WrapperDelta, compoundAssignDelta)
{
	D<celsius<double>> del(10.0);
	del += D<celsius<double>>(5.0);
	EXPECT_DOUBLE_EQ(15.0, del.value());
	del -= D<celsius<double>>(3.0);
	EXPECT_DOUBLE_EQ(12.0, del.value());
	del *= 2.0;
	EXPECT_DOUBLE_EQ(24.0, del.value());
	del /= 4.0;
	EXPECT_DOUBLE_EQ(6.0, del.value());
}

TEST(WrapperDelta, compoundAssignMixedUnitDeltaScaleOnly)
{
	// A fahrenheit-degree delta compound-added to a celsius delta converts by degree size, keeping the lhs unit.
	D<celsius<double>> del(10.0);
	del += D<fahrenheit<double>>(18.0); // +10 celsius-degrees
	EXPECT_DOUBLE_EQ(20.0, del.value());
	del -= D<fahrenheit<double>>(9.0); // -5 celsius-degrees
	EXPECT_DOUBLE_EQ(15.0, del.value());
}

TEST(WrapperDelta, compoundMovePointByDelta)
{
	// absolute += / -= delta (gap: the wrapper's in-place point move).
	A<celsius<double>> p(20.0);
	p += D<celsius<double>>(5.0);
	EXPECT_DOUBLE_EQ(25.0, p.value());
	p -= D<celsius<double>>(10.0);
	EXPECT_DOUBLE_EQ(15.0, p.value());
	// mixed-unit delta, scale-only.
	p += D<fahrenheit<double>>(18.0); // +10 celsius-degrees
	EXPECT_DOUBLE_EQ(25.0, p.value());
}

TEST(WrapperDelta, integerDeltaCompoundScaleStaysInteger)
{
	// matching the plain unit's own operator*=: an int delta scaled by an int stays int in place.
	D<meters<int>> del(5);
	del *= 3;
	static_assert(std::is_integral_v<under_t<decltype(del)>>);
	EXPECT_EQ(15, del.value());
}

//======================================================================================================================
//	LHS-UNIT TIE-BREAK — commensurable, incommensurable, mixed underlying, over double/float/int
//======================================================================================================================

TEST(WrapperLhsRule, pointDifferenceKeepsLhsUnitDouble)
{
	const A<kilometers<double>> a(1.0);
	const A<meters<double>>     b(500.0);
	auto                        del = a - b; // 0.5 km
	static_assert(std::is_same_v<wrapped_t<decltype(del)>, kilometers<double>>);
	EXPECT_DOUBLE_EQ(0.5, del.value());

	auto dRev = b - a; // 500 m - 1000 m = -500 m
	static_assert(std::is_same_v<wrapped_t<decltype(dRev)>, meters<double>>);
	EXPECT_DOUBLE_EQ(-500.0, dRev.value());
}

TEST(WrapperLhsRule, coarseIntegerLhsPromotesUnderlyingKeepsUnit)
{
	// LHS km<int>, RHS m<int>: keeping km<int> would narrow (500 m is not a whole km), so the underlying promotes
	// to floating point but the UNIT stays kilometers. Result: 0.5 km (as a floating km delta).
	const A<kilometers<int>> a(1);
	const A<meters<int>>     b(500);
	auto                     del = a - b;
	static_assert(traits::is_delta_v<decltype(del)>);
	static_assert(std::is_floating_point_v<under_t<decltype(del)>>);
	// still km scale: the value reads as 0.5 (kilometers), not 500 (meters).
	EXPECT_DOUBLE_EQ(0.5, del.value());

	// The reverse — m<int> LHS, km<int> RHS — is lossless (km converts into m as whole meters), so it STAYS int.
	auto dRev = b - a; // 500 m - 1000 m = -500 m
	static_assert(std::is_integral_v<under_t<decltype(dRev)>>);
	static_assert(std::is_same_v<wrapped_t<decltype(dRev)>, meters<int>>);
	EXPECT_EQ(-500, dRev.value());
}

TEST(WrapperLhsRule, deltaSumKeepsLhsUnitAcrossTypes)
{
	// double: km + m -> km
	{
		const D<kilometers<double>> a(1.0);
		const D<meters<double>>     b(500.0);
		auto                        sum = a + b; // 1.5 km
		static_assert(std::is_same_v<wrapped_t<decltype(sum)>, kilometers<double>>);
		EXPECT_DOUBLE_EQ(1.5, sum.value());
	}
	// float: m + km -> m
	{
		const D<meters<float>>     a(500.0f);
		const D<kilometers<float>> b(1.0f);
		auto                       sum = a + b; // 1500 m
		static_assert(std::is_same_v<wrapped_t<decltype(sum)>, meters<float>>);
		EXPECT_FLOAT_EQ(1500.0f, sum.value());
	}
	// int coarse lhs promotes underlying, keeps km unit
	{
		const D<kilometers<int>> a(2);
		const D<meters<int>>     b(500);
		auto                     sum = a + b; // 2.5 km
		static_assert(std::is_floating_point_v<under_t<decltype(sum)>>);
		EXPECT_DOUBLE_EQ(2.5, sum.value());
	}
	// int lossless (m lhs) stays int
	{
		const D<meters<int>>     a(500);
		const D<kilometers<int>> b(1);
		auto                     sum = a + b; // 1500 m
		static_assert(std::is_integral_v<under_t<decltype(sum)>>);
		EXPECT_EQ(1500, sum.value());
	}
}

TEST(WrapperLhsRule, mixedUnderlyingDeltaOperands)
{
	// km<double> - m<int>: lhs is floating, so it stays km<double> (lossless), value 0.5.
	const D<kilometers<double>> a(1.0);
	const D<meters<int>>        b(500);
	auto                        del = a - b;
	static_assert(std::is_same_v<wrapped_t<decltype(del)>, kilometers<double>>);
	EXPECT_DOUBLE_EQ(0.5, del.value());
}

//======================================================================================================================
//	PI-CARRYING (ANGLE) SLICE — an irrational scale factor between degrees and radians
//======================================================================================================================

TEST(WrapperAngle, degreesRadiansDeltaConversion)
{
	const D<degrees<double>> del(180.0);
	const auto               r = del.to<radians<double>>();
	EXPECT_NEAR(units::detail::PI_VAL, r.value(), 1e-12);

	// delta<degrees> - delta<radians> keeps degrees (LHS): 180 deg - pi rad(==180 deg) = 0 deg.
	const D<radians<double>> half(units::detail::PI_VAL);
	auto                     zero = del - half;
	static_assert(std::is_same_v<wrapped_t<decltype(zero)>, degrees<double>>);
	EXPECT_NEAR(0.0, zero.value(), 1e-9);
}

TEST(WrapperAngle, pointArithmeticOnAngles)
{
	// Angles are non-affine (datum zero), so absolute and delta coincide numerically, but the point/amount ROLE
	// still type-checks: point + delta -> point, point - point -> delta.
	const A<degrees<double>> here(90.0);
	const D<degrees<double>> quarter(90.0);
	auto                     there = here + quarter; // 180 deg
	static_assert(traits::is_absolute_v<decltype(there)>);
	EXPECT_DOUBLE_EQ(180.0, there.value());

	auto arc = there - here; // 90 deg delta
	static_assert(traits::is_delta_v<decltype(arc)>);
	EXPECT_DOUBLE_EQ(90.0, arc.value());
}

TEST(WrapperAngle, radiansDeltaScaling)
{
	const D<radians<double>> r(units::detail::PI_VAL);
	auto                     two = r * 2.0;
	EXPECT_NEAR(2.0 * units::detail::PI_VAL, two.value(), 1e-12);
}

//======================================================================================================================
//	RATIO-DIMENSIONLESS (PERCENT) SLICE — a fractional conversion ratio
//======================================================================================================================

// percent is a ratio-dimensionless unit: `percent<double>(20.0).value()` is the FRACTION (0.20), while `.raw()`
// is the percentage number (20). The wrapper faithfully forwards both. The assertions below read the percentage
// number through `.raw()` (the intuitive "25%") and confirm the fraction through `.value()`, so the ratio
// semantics are proven, not glossed.
TEST(WrapperPercent, percentDeltaArithmetic)
{
	const D<percent<double>> a(20.0);
	const D<percent<double>> b(5.0);
	EXPECT_DOUBLE_EQ(25.0, (a + b).raw());   // 20% + 5% = 25%
	EXPECT_DOUBLE_EQ(0.25, (a + b).value()); // as a fraction
	EXPECT_DOUBLE_EQ(15.0, (a - b).raw());   // 20% - 5% = 15%
	// scaling a percent by a bare number returns a plain (ratio-stripped) dimensionless delta, whose value IS the
	// fraction: 0.20 fraction * 2 = 0.40. The wrapper forwards the wrapped unit's own operator* verbatim.
	EXPECT_DOUBLE_EQ(0.40, (a * 2.0).value());
}

TEST(WrapperPercent, percentPointMoveByDelta)
{
	const A<percent<double>> level(50.0);
	const D<percent<double>> bump(10.0);
	auto                     higher = level + bump;
	static_assert(traits::is_absolute_v<decltype(higher)>);
	EXPECT_DOUBLE_EQ(60.0, higher.raw()); // 50% + 10% = 60%
}

TEST(WrapperPercent, percentPointDifferenceIsDelta)
{
	const A<percent<double>> a(75.0);
	const A<percent<double>> b(25.0);
	auto                     del = a - b;
	static_assert(traits::is_delta_v<decltype(del)>);
	EXPECT_DOUBLE_EQ(50.0, del.raw()); // 75% - 25% = 50%
}

//======================================================================================================================
//	COMPARISONS — reconcile to the common (finer) unit, never narrowing
//======================================================================================================================

TEST(WrapperCompare, pointComparisonsApplyDatum)
{
	const A<celsius<double>>    c(0.0);
	const A<fahrenheit<double>> f(32.0); // == 0 degC
	EXPECT_TRUE(c == f);
	EXPECT_FALSE(c < f);
	EXPECT_TRUE(c <= f);

	const A<celsius<double>> warmer(100.0);
	EXPECT_TRUE(f < warmer);
	EXPECT_TRUE(warmer > f);
	EXPECT_TRUE(warmer != f);
}

TEST(WrapperCompare, deltaComparisonsScaleOnly)
{
	const D<celsius<double>>    ten(10.0);
	const D<fahrenheit<double>> eighteen(18.0); // == 10 celsius-degrees
	EXPECT_TRUE(ten == eighteen);
	EXPECT_FALSE(ten < eighteen);

	const D<celsius<double>> five(5.0);
	EXPECT_TRUE(five < ten);
	EXPECT_TRUE(ten > five);
}

TEST(WrapperCompare, integerMixedUnitComparisonNoNarrowing)
{
	// A comparison reconciles to the common (finer) unit, so a coarse-integer mixed comparison is well-formed.
	const D<kilometers<int>> a(1);
	const D<meters<int>>     b(1000);
	EXPECT_TRUE(a == b);
	const D<meters<int>> c(999);
	EXPECT_TRUE(c < a);
}

//======================================================================================================================
//	PRINTING  (gap #5)
//======================================================================================================================

TEST(WrapperPrint, streamAndToStringDistinguishPointVsDelta)
{
	const A<celsius<double>> p(20.0);
	const D<celsius<double>> del(5.0);

	std::ostringstream ossP, ossD;
	ossP << p;
	ossD << del;
	// A point prints bare (the wrapped quantity); a delta carries a leading "delta " marker.
	EXPECT_EQ("20 degC", ossP.str());
	EXPECT_EQ("delta 5 degC", ossD.str());

	EXPECT_EQ("20 degC", units::to_string(p));
	EXPECT_EQ("delta 5 degC", units::to_string(del));
}

//======================================================================================================================
//	DELTA MATH — abs / min / max / clamp  (gap #6)
//======================================================================================================================

TEST(WrapperMath, absOfDelta)
{
	const D<celsius<double>> neg(-7.5);
	auto                     mag = affine::abs(neg);
	static_assert(traits::is_delta_v<decltype(mag)>);
	EXPECT_DOUBLE_EQ(7.5, mag.value());

	// integer delta promotes to floating (matching the plain unit's abs promotion contract).
	const D<meters<int>> negi(-4);
	auto                 magi = affine::abs(negi);
	static_assert(std::is_floating_point_v<under_t<decltype(magi)>>);
	EXPECT_DOUBLE_EQ(4.0, magi.value());
}

TEST(WrapperMath, minMaxClampOfDeltas)
{
	const D<celsius<double>> a(10.0), b(30.0);
	EXPECT_DOUBLE_EQ(10.0, affine::min(a, b).value());
	EXPECT_DOUBLE_EQ(30.0, affine::max(a, b).value());

	// mixed unit min/max keeps the LHS unit.
	const D<fahrenheit<double>> f(18.0); // == 10 celsius-degrees
	auto                        extremum = affine::max(a, f);
	static_assert(std::is_same_v<wrapped_t<decltype(extremum)>, celsius<double>>);
	EXPECT_DOUBLE_EQ(10.0, extremum.value()); // 10 degC and 10-celsius-degree delta are equal; max keeps lhs

	// clamp.
	const D<celsius<double>> lo(0.0), hi(20.0), v(30.0);
	EXPECT_DOUBLE_EQ(20.0, affine::clamp(v, lo, hi).value());
	const D<celsius<double>> v2(-5.0);
	EXPECT_DOUBLE_EQ(0.0, affine::clamp(v2, lo, hi).value());
}

//======================================================================================================================
//	CONSTEXPR — the full algebra is usable at compile time
//======================================================================================================================

TEST(WrapperConstexpr, algebraIsConstexpr)
{
	constexpr A<celsius<double>> boiling(100.0);
	constexpr A<fahrenheit<double>> freezing(32.0);
	constexpr auto del = boiling - freezing;
	static_assert(del.value() == 100.0);

	constexpr D<celsius<double>> warm(5.0);
	constexpr auto               warmer = boiling + warm;
	static_assert(warmer.value() == 105.0);

	constexpr D<meters<double>> len(4.0);
	static_assert((len * 2.0).value() == 8.0);
	static_assert((len / 2.0).value() == 2.0);
	SUCCEED();
}

//======================================================================================================================
//	NON-AFFINE GENERAL SLICE — absolute and delta coincide numerically for a zero-datum unit
//======================================================================================================================

TEST(WrapperGeneral, timePointsAndDeltas)
{
	// epochs vs durations: a point difference is a duration (delta), moving a point by a duration is a point.
	const A<seconds<double>> t0(10.0);
	const A<seconds<double>> t1(35.0);
	auto                     elapsed = t1 - t0;
	static_assert(traits::is_delta_v<decltype(elapsed)>);
	EXPECT_DOUBLE_EQ(25.0, elapsed.value());

	const D<minutes<double>> wait(1.0); // 60 s delta
	auto                     later = t1 + wait;
	static_assert(traits::is_absolute_v<decltype(later)>);
	static_assert(std::is_same_v<wrapped_t<decltype(later)>, seconds<double>>);
	EXPECT_DOUBLE_EQ(95.0, later.value());
}

TEST(WrapperGeneral, lengthDisplacements)
{
	const A<meters<double>> here(100.0);
	const A<meters<double>> there(250.0);
	auto                    displacement = there - here;
	static_assert(traits::is_delta_v<decltype(displacement)>);
	EXPECT_DOUBLE_EQ(150.0, displacement.value());
}

//======================================================================================================================
//	EDGE-UNIT HARDENING (adversarial): integer-datum truncation, angle/pi, percent, promotion, NaN/inf
//======================================================================================================================

// An integer POINT converted across a datum-differing pair whose ratio is 1 (celsius<->kelvin) must NOT truncate the
// fractional datum. `absolute_result_unit_t` promotes the underlying because the translation ratios differ, so the
// 273.15 offset survives. (Before the affine-aware predicate, this silently returned 100.0.)
TEST(WrapperEdge, integerCelsiusMinusKelvinKeepsDatum)
{
	const A<celsius<int>> hot(100);
	const A<kelvin<int>>  cold(273); // 273 K == -0.15 degC
	auto                  diff = hot - cold;
	static_assert(traits::is_delta_v<decltype(diff)>);
	EXPECT_DOUBLE_EQ(100.15, static_cast<double>(diff.value())); // 100 - (-0.15)
}

TEST(WrapperEdge, integerPointMaxAcrossDatumKeepsDatum)
{
	// 280 K == 6.85 degC, warmer than 0 degC. (Near, not exact: the 273.15 datum round-trip through the
	// linearized scale carries a few ULPs of floating-point error — the value is 6.85 to ~13 digits.)
	auto hotter = affine::max(A<celsius<int>>(0), A<kelvin<int>>(280));
	EXPECT_NEAR(6.85, static_cast<double>(hotter.value()), 1e-9);
}

TEST(WrapperEdge, floatingPointDatumWasAlwaysCorrect)
{
	// The double path is the correctness oracle the integer fix must match.
	EXPECT_DOUBLE_EQ(100.15, (A<celsius<double>>(100.0) - A<kelvin<double>>(273.0)).value());
}

TEST(WrapperEdge, fahrenheitPairPromotesByRatio)
{
	// C<->F ratio is 5/9 (not 1), so the underlying already promotes; the datum survives regardless.
	auto diff = A<celsius<int>>(100) - A<fahrenheit<int>>(32); // 100 degC vs 0 degC
	EXPECT_DOUBLE_EQ(100.0, static_cast<double>(diff.value()));
	EXPECT_TRUE((std::is_floating_point_v<typename decltype(diff)::underlying_type>));
}

// A DELTA is offset-free: its conversion is scale-only, so a same-ratio integer delta pair (celsius<->kelvin deltas)
// must STAY integral — the datum-aware promotion of the point path must NOT leak into the delta path.
TEST(WrapperEdge, integerDeltaSameRatioStaysIntegral)
{
	auto sum = D<celsius<int>>(5) + D<celsius<int>>(3);
	static_assert(std::is_same_v<decltype(sum), D<celsius<int>>>);
	static_assert(std::is_same_v<typename decltype(sum)::underlying_type, int>);
	EXPECT_EQ(8, sum.to<celsius<int>>().raw());

	auto mixed = D<celsius<int>>(10) + D<kelvin<int>>(5); // deltas ignore datum; C and K share ratio 1
	static_assert(std::is_same_v<typename decltype(mixed)::underlying_type, int>);
	EXPECT_EQ(15, mixed.to<celsius<int>>().raw());
}

TEST(WrapperEdge, integerDeltaCoarseRhsStaysIntegralFineLhsPromotes)
{
	auto lossless = D<meters<int>>(500) + D<kilometers<int>>(1); // 1 km into m is exact
	static_assert(std::is_same_v<typename decltype(lossless)::underlying_type, int>);
	EXPECT_EQ(1500, lossless.value());

	auto lossy = D<kilometers<int>>(1) - D<meters<int>>(500); // 500 m into km<int> would truncate -> promote
	EXPECT_TRUE((std::is_floating_point_v<typename decltype(lossy)::underlying_type>));
	EXPECT_DOUBLE_EQ(0.5, static_cast<double>(lossy.value()));
}

TEST(WrapperEdge, angleDeltaKeepsPiRatio)
{
	constexpr double pi = 3.14159265358979323846;
	EXPECT_DOUBLE_EQ(pi, D<degrees<double>>(180.0).to<radians<double>>().value());
	EXPECT_TRUE(D<degrees<double>>(90.0) == D<radians<double>>(pi / 2.0));
	EXPECT_DOUBLE_EQ(pi, (D<radians<double>>(pi / 2.0) + D<degrees<double>>(90.0)).value());   // LHS radians
	EXPECT_DOUBLE_EQ(180.0, (D<degrees<double>>(90.0) + D<radians<double>>(pi / 2.0)).value()); // LHS degrees
}

TEST(WrapperEdge, percentValueVsRawMatchesPlainUnit)
{
	// A percent's ratio is 1/100, so .value() is 0.5 and .raw() is 50 — the wrapper mirrors the plain unit.
	EXPECT_DOUBLE_EQ(0.5, D<percent<double>>(50.0).value());
	EXPECT_DOUBLE_EQ(50.0, static_cast<double>(D<percent<double>>(50.0).raw()));
	auto p = A<percent<double>>(50.0) + D<percent<double>>(10.0);
	EXPECT_DOUBLE_EQ(60.0, static_cast<double>(p.raw()));
}

TEST(WrapperEdge, deltaScaleNeverLeaksDatum)
{
	const D<celsius<double>> warmBy10(10.0);
	EXPECT_DOUBLE_EQ(20.0, (warmBy10 * 2.0).value());
	EXPECT_DOUBLE_EQ(20.0, (2.0 * warmBy10).value());
	EXPECT_DOUBLE_EQ(5.0, (warmBy10 / 2.0).value());
	EXPECT_DOUBLE_EQ(-10.0, (-warmBy10).value());
	EXPECT_DOUBLE_EQ(273.15, affine::abs(D<celsius<double>>(-273.15)).value()); // magnitude, no datum
}

TEST(WrapperEdge, clampIsTotalEvenForInvertedRange)
{
	// affine::clamp is min(max(v,lo),hi): total (returns hi for an inverted range), NOT std::clamp's UB.
	EXPECT_DOUBLE_EQ(2.0, affine::clamp(D<meters<double>>(5.0), D<meters<double>>(10.0), D<meters<double>>(2.0)).value());
	EXPECT_DOUBLE_EQ(5.0, affine::clamp(D<meters<double>>(5.0), D<meters<double>>(2.0), D<meters<double>>(10.0)).value());
}

TEST(WrapperEdge, nanAndInfinityFollowStandardSemantics)
{
	const double nan = std::numeric_limits<double>::quiet_NaN();
	const double inf = std::numeric_limits<double>::infinity();
	EXPECT_TRUE(std::isnan(affine::abs(D<meters<double>>(nan)).value()));
	EXPECT_TRUE(std::isinf(affine::abs(D<meters<double>>(-inf)).value()));
	// min/max follow std::min/std::max (asymmetric, first-argument biased under NaN) — pinned so nobody "fixes" it.
	EXPECT_DOUBLE_EQ(1.0, affine::min(D<meters<double>>(nan), D<meters<double>>(1.0)).value());
	EXPECT_TRUE(std::isnan(affine::min(D<meters<double>>(1.0), D<meters<double>>(nan)).value()));
	EXPECT_FALSE(D<meters<double>>(nan) == D<meters<double>>(nan));
	EXPECT_EQ(std::partial_ordering::unordered, (D<meters<double>>(nan) <=> D<meters<double>>(1.0)));
}

// The wrapper free functions are found by ARGUMENT-DEPENDENT LOOKUP — an UNqualified `abs`/`min`/`max`/`clamp`/
// `to_string` call on a wrapper resolves via the associated namespace (`units`) of the wrapper argument, with no
// `affine::` prefix and no `using`. This is the idiomatic call form and must keep working (a generic algorithm that
// says `using std::min; min(a, b);` picks up the wrapper's `min` for wrapper arguments through ADL).
TEST(WrapperEdge, freeFunctionsFoundViaAdl)
{
	const D<meters<double>> a(3.0);
	const D<meters<double>> b(5.0);

	// unqualified — resolved by ADL on the delta<meters<double>> argument, NOT affine::/units:: qualified
	EXPECT_DOUBLE_EQ(3.0, abs(D<meters<double>>(-3.0)).value());
	EXPECT_DOUBLE_EQ(3.0, min(a, b).value());
	EXPECT_DOUBLE_EQ(5.0, max(a, b).value());
	EXPECT_DOUBLE_EQ(4.0, clamp(D<meters<double>>(4.0), a, b).value());

	// to_string via ADL (no units:: qualifier), and operator<< via the stream
	using std::to_string; // ADL still selects the wrapper overload for a wrapper argument
	EXPECT_EQ("delta 3 m", to_string(a));
	EXPECT_EQ("delta 5 m", to_string(b));

	std::ostringstream os;
	os << a; // operator<< found by ADL
	EXPECT_EQ("delta 3 m", os.str());

	// points, too: min/max via ADL
	const A<meters<double>> p(10.0);
	const A<meters<double>> q(20.0);
	EXPECT_DOUBLE_EQ(10.0, min(p, q).value());
	EXPECT_DOUBLE_EQ(20.0, max(p, q).value());
}

// A wrapper is hashable exactly like the unit it wraps — it drops into an unordered container, and equal wrappers
// hash equally.
TEST(WrapperEdge, hashableInUnorderedContainer)
{
	std::unordered_set<D<meters<int>>> amounts;
	amounts.insert(D<meters<int>>(3));
	amounts.insert(D<meters<int>>(5));
	amounts.insert(D<meters<int>>(3)); // duplicate
	EXPECT_EQ(2u, amounts.size());
	EXPECT_EQ(1u, amounts.count(D<meters<int>>(3)));

	// equal wrappers hash equally
	EXPECT_EQ(std::hash<D<meters<int>>>()(D<meters<int>>(7)), std::hash<D<meters<int>>>()(D<meters<int>>(7)));
	EXPECT_EQ(std::hash<A<meters<int>>>()(A<meters<int>>(7)), std::hash<A<meters<int>>>()(A<meters<int>>(7)));
}

// numeric_limits is specialized (NOT the silent-zero default): max() is the largest representable wrapper.
TEST(WrapperEdge, numericLimitsSpecialized)
{
	static_assert(std::numeric_limits<D<meters<double>>>::is_specialized);
	static_assert(std::numeric_limits<A<meters<double>>>::is_specialized);
	EXPECT_DOUBLE_EQ(std::numeric_limits<double>::max(), std::numeric_limits<D<meters<double>>>::max().value());
	EXPECT_DOUBLE_EQ(std::numeric_limits<double>::lowest(), std::numeric_limits<A<meters<double>>>::lowest().value());
}

//======================================================================================================================
//	GENERIC STRING-TAGGED `kind<Tag, U>` — quantities that share a unit+dimension but are distinct kinds
//======================================================================================================================

TEST(WrapperKind, sameTagInteroperatesKeepsLhsUnit)
{
	const kind<"radial", meters<double>> a(5.0);
	const kind<"radial", meters<double>> b(3.0);
	auto sum = a + b; // same tag -> a radial kind, LHS unit
	static_assert(traits::is_kind_v<decltype(sum)>);
	static_assert(decltype(sum)::tag() == fixed_string("radial"));
	EXPECT_UNIT_NEAR(sum, 8.0);
	EXPECT_UNIT_NEAR(a - b, 2.0);
	EXPECT_UNIT_NEAR(a * 2.0, 10.0);
	EXPECT_UNIT_NEAR(b / 3.0, 1.0);
	EXPECT_UNIT_NEAR(-a, -5.0);
	const kind<"radial", meters<double>> aCopy(5.0);
	EXPECT_TRUE(a == aCopy);
	EXPECT_TRUE(b < a);
}

TEST(WrapperKind, toKeepsTagOrUnwraps)
{
	const kind<"radial", meters<double>> r(5.0);
	// to<kind<sameTag, V>> stays a radial kind, converting the unit.
	auto radialFeet = r.to<kind<"radial", feet<double>>>();
	static_assert(traits::is_kind_v<decltype(radialFeet)>);
	static_assert(decltype(radialFeet)::tag() == fixed_string("radial"));
	EXPECT_UNIT_NEAR(radialFeet, 16.404199475);
	// to<PlainUnit> unwraps (drops the tag).
	const feet<double> plainFeet = r.to<feet<double>>();
	EXPECT_NEAR(16.404199475, plainFeet.value(), 1e-6);
	const meters<double> plainMeters = r.to<meters<double>>(); // same-unit unwrap
	EXPECT_DOUBLE_EQ(5.0, plainMeters.value());
}

TEST(WrapperKind, constructibleFromPlainButNotInterchangeable)
{
	// CONSTRUCTIBLE: a plain unit converts into a kind by copy-initialization / assignment (clear intent).
	kind<"radial", meters<double>> r = meters<double>(3.0);
	EXPECT_UNIT_NEAR(r, 3.0);
	r = meters<double>(4.0);
	EXPECT_UNIT_NEAR(r, 4.0);
	// NOT INTERCHANGEABLE: no IMPLICIT conversion, and mixing in arithmetic is ill-formed (compile-time; the
	// errorMessages suite covers the readable diagnostics). Here we assert the trait boundaries.
	static_assert(!std::is_convertible_v<kind<"radial", meters<double>>, meters<double>>);   // no implicit unwrap
	static_assert(std::is_constructible_v<kind<"radial", meters<double>>, meters<double>>);   // explicit wrap OK
	SUCCEED();
}

TEST(WrapperKind, differentTagsAreDistinctTypes)
{
	// radial vs straight: same unit+dimension, DIFFERENT type — cannot be assigned or compared without unwrapping.
	static_assert(!std::is_same_v<kind<"radial", meters<double>>, kind<"straight", meters<double>>>);
	static_assert(!std::is_convertible_v<kind<"radial", meters<double>>, kind<"straight", meters<double>>>);

	// torque vs energy: the ISO "kind of quantity" headline — same dimension (both N·m), different kind.
	using torque_kind = kind<"torque", units::torque::newton_meters<double>>;
	using energy_kind = kind<"energy", units::energy::joules<double>>;
	static_assert(!std::is_same_v<torque_kind, energy_kind>);
	SUCCEED();
}

TEST(WrapperKind, formattingShowsTag)
{
	std::ostringstream os;
	os << kind<"radial", meters<double>>(5.0);
	EXPECT_EQ("[radial] 5 m", os.str());
	EXPECT_EQ("[radial] 5 m", to_string(kind<"radial", meters<double>>(5.0)));
}

TEST(WrapperKind, traitsAndTrivialProperties)
{
	static_assert(traits::is_kind_v<kind<"radial", meters<double>>>);
	static_assert(!traits::is_kind_v<A<meters<double>>>);
	static_assert(!traits::is_kind_v<D<meters<double>>>);
	static_assert(!traits::is_kind_v<meters<double>>);
	static_assert(KindType<kind<"radial", meters<double>>>);
	static_assert(!KindType<meters<double>>);

	static_assert(std::is_trivially_copyable_v<kind<"radial", meters<double>>>);
	static_assert(sizeof(kind<"radial", meters<double>>) == sizeof(meters<double>));
	SUCCEED();
}

//======================================================================================================================
//	API PARITY WITH THE PLAIN UNIT — a wrapper exposes the same accessors the wrapped unit does
//======================================================================================================================

// to<Arithmetic>() gives the numeric value in the wrapper's own unit, cast to the target type — matching the
// plain unit's to<Arithmetic>() (a truncating cast for an integral target).
TEST(WrapperParity, toArithmeticMatchesPlainUnit)
{
	EXPECT_EQ(20, A<celsius<double>>(20.7).to<int>());
	EXPECT_DOUBLE_EQ(20.7, A<celsius<double>>(20.7).to<double>());
	EXPECT_EQ(5, D<meters<double>>(5.7).to<int>());
	EXPECT_DOUBLE_EQ(5.7, D<meters<double>>(5.7).to<double>());
	EXPECT_EQ(5, (kind<"radial", meters<double>>(5.7).to<int>()));
	EXPECT_DOUBLE_EQ(5.7, (kind<"radial", meters<double>>(5.7).to<double>()));
	// same value the wrapped unit would give
	EXPECT_EQ(meters<double>(5.7).to<int>(), D<meters<double>>(5.7).to<int>());
}

// to<Arithmetic>() must NOT collide with to<Unit>()/to<Wrapper>(): a plain-unit target still unwraps, a wrapper
// target still stays wrapped.
TEST(WrapperParity, toDispatchStillDisjointWithArithmeticOverload)
{
	const D<meters<double>> change(5.0);
	static_assert(std::is_same_v<decltype(change.to<double>()), double>);            // arithmetic -> number
	static_assert(std::is_same_v<decltype(change.to<meters<double>>()), meters<double>>);   // plain unit -> unwrap
	static_assert(std::is_same_v<decltype(change.to<D<feet<double>>>()), D<feet<double>>>); // wrapper -> stays
	SUCCEED();
}

TEST(WrapperParity, toLinearizedForwardsToWrappedUnit)
{
	EXPECT_DOUBLE_EQ(meters<double>(5.0).to_linearized(), D<meters<double>>(5.0).to_linearized());
	EXPECT_DOUBLE_EQ(celsius<double>(0.0).to_linearized(), A<celsius<double>>(0.0).to_linearized());
	EXPECT_DOUBLE_EQ(meters<double>(5.0).to_linearized(), (kind<"radial", meters<double>>(5.0).to_linearized()));
}

// name()/abbreviation(): absolute/delta forward to the wrapped unit; a kind's abbreviation is the unit's, and its
// name is "<tag> <unitname>".
TEST(WrapperParity, nameAndAbbreviation)
{
	EXPECT_STREQ("meters", D<meters<double>>(1.0).name());
	EXPECT_STREQ("m", D<meters<double>>(1.0).abbreviation());
	EXPECT_STREQ("celsius", A<celsius<double>>(1.0).name());

	const kind<"radial", meters<double>> k(1.0);
	EXPECT_EQ("radial meters", k.name());        // tag + unit
	EXPECT_STREQ("m", k.abbreviation());          // unit's abbreviation, unchanged
}

//======================================================================================================================
//	CASE STUDIES (mp-units-informed): chained arithmetic, boundaries, kind ratio/parity, std concepts
//======================================================================================================================

TEST(WrapperCaseStudy, chainedPointDeltaAcrossUnitsAndDatum)
{
	// (point - point) is a delta; + a delta stays a delta, keeping the FIRST operand's unit at each step.
	const A<celsius<double>>    hot(100.0);
	const A<fahrenheit<double>> freezing(32.0); // == 0 degC
	auto                        chain = (hot - freezing) + D<celsius<double>>(5.0);
	static_assert(traits::is_delta_v<decltype(chain)>);
	static_assert(std::is_same_v<wrapped_t<decltype(chain)>, celsius<double>>);
	EXPECT_UNIT_NEAR(chain, 105.0);

	// point + delta - delta round-trips the point (datum preserved through the chain).
	const A<celsius<double>>    p(20.0);
	const D<fahrenheit<double>> up(18.0); // +10 celsius-degrees, scale only
	auto                        back = (p + up) - up;
	static_assert(traits::is_absolute_v<decltype(back)>);
	EXPECT_UNIT_NEAR(back, 20.0);

	// Associativity where the LHS-unit tie-break matters: ((km - m) + m) keeps km throughout.
	auto res = (A<kilometers<double>>(2.0) - A<meters<double>>(500.0)) + D<meters<double>>(250.0);
	static_assert(std::is_same_v<wrapped_t<decltype(res)>, kilometers<double>>);
	EXPECT_UNIT_NEAR(res, 1.75);
}

TEST(WrapperCaseStudy, affineBoundaryValues)
{
	// Absolute zero across scales, as POINTS (datum applied both sides).
	EXPECT_TRUE(A<kelvin<double>>(0.0) == A<celsius<double>>(-273.15));
	// -40 is the C==F crossover.
	EXPECT_TRUE(A<celsius<double>>(-40.0) == A<fahrenheit<double>>(-40.0));
	// A negative point minus a positive point is a negative delta, LHS unit kept.
	EXPECT_UNIT_NEAR(A<celsius<double>>(-40.0) - A<celsius<double>>(10.0), -50.0);
	// Zero delta is the additive identity and carries no datum.
	EXPECT_UNIT_NEAR(A<celsius<double>>(21.0) + D<fahrenheit<double>>(0.0), 21.0);
	EXPECT_UNIT_NEAR(D<celsius<double>>(0.0).to<fahrenheit<double>>(), 0.0); // 0, not 32
	// Large-magnitude point round-trip stability (datum must not swamp a big value).
	EXPECT_NEAR(1.0e6, A<kelvin<double>>(1.0e6).to<A<celsius<double>>>().to<A<kelvin<double>>>().value(), 1e-3);
}

TEST(WrapperCaseStudy, stdConceptParity)
{
	static_assert(std::regular<A<meters<double>>>);
	static_assert(std::regular<D<meters<double>>>);
	static_assert(std::regular<kind<"radial", meters<double>>>);
	static_assert(std::totally_ordered<A<meters<double>>>);
	static_assert(std::totally_ordered<D<meters<double>>>);
	static_assert(std::totally_ordered<kind<"radial", meters<double>>>);
	static_assert(std::three_way_comparable<A<meters<double>>>);
	static_assert(std::three_way_comparable<D<meters<double>>>);
	SUCCEED();
}

TEST(WrapperCaseStudy, sameTagRatioIsDimensionless)
{
	// The ratio of two same-tag kinds cancels the tag AND the unit, yielding a plain dimensionless number.
	const kind<"radial", meters<double>> a(10.0);
	const kind<"radial", meters<double>> b(2.0);
	EXPECT_DOUBLE_EQ(5.0, static_cast<double>((a / b).value()));
	// kind / scalar still stays a kind.
	static_assert(traits::is_kind_v<decltype(a / 2.0)>);
	EXPECT_UNIT_NEAR(a / 2.0, 5.0);
}

TEST(WrapperCaseStudy, kindParityHashLimitsMathCompound)
{
	// Full parity with absolute/delta: hash (unordered container), numeric_limits, abs/min/max/clamp, *=//=.
	std::unordered_set<kind<"radial", meters<int>>> radialSet;
	radialSet.insert(kind<"radial", meters<int>>(3));
	radialSet.insert(kind<"radial", meters<int>>(3));
	EXPECT_EQ(1u, radialSet.size());

	static_assert(std::numeric_limits<kind<"radial", meters<double>>>::is_specialized);

	EXPECT_UNIT_NEAR(abs(kind<"radial", meters<double>>(-5.0)), 5.0);
	EXPECT_UNIT_NEAR(min(kind<"radial", meters<double>>(3.0), kind<"radial", meters<double>>(5.0)), 3.0);
	EXPECT_UNIT_NEAR(max(kind<"radial", meters<double>>(3.0), kind<"radial", meters<double>>(5.0)), 5.0);
	EXPECT_UNIT_NEAR(clamp(kind<"radial", meters<double>>(7.0), kind<"radial", meters<double>>(0.0), kind<"radial", meters<double>>(5.0)), 5.0);

	kind<"radial", meters<double>> c(10.0);
	c *= 2.0;
	c /= 4.0;
	EXPECT_UNIT_NEAR(c, 5.0);
	// min/max/abs keep the tag.
	static_assert(decltype(abs(kind<"radial", meters<double>>(1.0)))::tag() == fixed_string("radial"));
}

TEST(WrapperCaseStudy, kindMixedUnitSameTagTieBreak)
{
	const kind<"radial", kilometers<int>> a(1);
	const kind<"radial", meters<int>>     b(500);
	// A kind's arithmetic DELEGATES to the wrapped units', so that the wrapped unit's rules -- including its
	// refusals -- apply to the tagged quantity too. The result unit is therefore the plain operator's: a coarse
	// integer LHS reconciles to the common (finest) unit rather than promoting the underlying, so this is 1500 m
	// exactly rather than 1.5 km. The tag is kept either way.
	auto sum = a + b;
	static_assert(decltype(sum)::tag() == fixed_string("radial"));
	static_assert(std::is_same_v<wrapped_t<decltype(sum)>, meters<int>>);
	EXPECT_UNIT_NEAR(sum, 1500.0);
}

TEST(WrapperCaseStudy, sameDimensionDifferentKindsAreDistinct)
{
	// Torque and energy share the N*m dimension; frequency is its own — three distinct kinds.
	using torque_k = kind<"torque", units::torque::newton_meters<double>>;
	using energy_k = kind<"energy", units::energy::joules<double>>;
	using freq_k   = kind<"frequency", units::frequency::hertz<double>>;
	static_assert(!std::is_same_v<torque_k, energy_k>);
	static_assert(!std::is_same_v<torque_k, freq_k>);
	static_assert(!std::is_same_v<energy_k, freq_k>);
	// Same-tag torque adds; the result is still a torque.
	auto torqueSum = torque_k(3.0) + torque_k(4.0);
	static_assert(decltype(torqueSum)::tag() == fixed_string("torque"));
	EXPECT_UNIT_NEAR(torqueSum, 7.0);
}

//======================================================================================================================
//	CORRECTNESS PARITY — the wrappers inherit the core's value-based (signedness-safe) comparison
//======================================================================================================================

// A signed-rep and an unsigned-rep wrapper of the same dimension compare by mathematical value, not by C++'s
// int/unsigned wraparound — the wrappers are not left behind by the core comparison fix.
TEST(WrapperCorrectness, mixedSignednessComparesByValue)
{
	// delta
	EXPECT_TRUE(D<meters<int>>(-1) < D<meters<unsigned>>(1u));
	EXPECT_FALSE(D<meters<int>>(-1) == D<meters<unsigned>>(1u));
	EXPECT_TRUE(D<meters<int>>(-5) != D<meters<unsigned>>(5u));
	EXPECT_TRUE(D<meters<unsigned>>(1u) > D<meters<int>>(-1));
	// absolute
	EXPECT_TRUE(A<meters<int>>(-1) < A<meters<unsigned>>(1u));
	EXPECT_FALSE(A<meters<int>>(-1) >= A<meters<unsigned>>(1u));
	// kind
	EXPECT_TRUE((kind<"radial", meters<int>>(-1) < kind<"radial", meters<unsigned>>(1u)));
	EXPECT_TRUE((kind<"radial", meters<int>>(2) == kind<"radial", meters<unsigned>>(2u)));

	// Same-signedness and cross-unit wrapper comparisons remain correct.
	EXPECT_TRUE(D<meters<int>>(3) < D<meters<int>>(5));
	EXPECT_TRUE(A<meters<int>>(1000) == A<kilometers<int>>(1));
}

//======================================================================================================================
//	POINT-ALGEBRA GUARDS — the operations the affine type system must REJECT
//======================================================================================================================
// Named concepts (SFINAE-friendly) probe whether an operation is even well-formed, so a `static_assert(!can_X<...>)`
// asserts the operation is ill-formed WITHOUT hard-erroring the translation unit (an inline `requires` in an
// evaluated context would compile the offending expression and fire its `static_assert(dependent_false)`). Each probe
// requires the exact expression `absolute<>/delta<>` forbids: scaling a point, moving a point onto a point.

namespace
{
	/// `a *= s` compiles (a delta scales in place; a point does not — there is no `absolute::operator*=`).
	template<class W, class Scalar>
	concept can_compound_scale = requires(W w, Scalar s) { w *= s; };
	/// `a /= s` compiles.
	template<class W, class Scalar>
	concept can_compound_divide = requires(W w, Scalar s) { w /= s; };
	/// `a += b` compiles.
	template<class A2, class B2>
	concept can_compound_add = requires(A2 a, B2 b) { a += b; };
	/// `a - b` between two point wrappers yields a delta WITHOUT firing a static_assert (the ALLOWED point-minus-point).
	template<class A2, class B2>
	concept can_subtract_points = requires(A2 a, B2 b) {
		{ a - b } -> DeltaType;
	};
	/// `a + b` where the left is a delta and the right an absolute yields a point (the ALLOWED commutative move).
	template<class D2, class A2>
	concept can_add_delta_to_point = requires(D2 d, A2 a) {
		{ d + a } -> AbsoluteType;
	};
} // namespace

// The point algebra's PERMITTED operations, and the forbidden ones that a concept CAN observe.
//
// A refusal expressed as a `static_assert` in a selected overload's body cannot be probed: the overload resolves, so a
// bare `requires` reports the operation as valid, and deducing the result type to force the body instantiates OUTSIDE
// the immediate context and hard-errors instead of reporting false. `absolute * scalar` and `absolute + absolute` are
// of that kind and are graded by the errorMessages harness (wrapper_absolute_times_scalar,
// wrapper_absolute_plus_absolute, wrapper_absolute_over_absolute, wrapper_delta_minus_absolute), which compiles each
// standalone and checks its diagnostic text.
//
// Where the wrapper declares NO overload at all the failure IS a clean substitution failure, so those cases are
// asserted here directly.
TEST(WrapperGuards, deltaScalesAndTheAllowedPointAlgebraIsWellFormed)
{
	// A delta (an amount) scales, in place and by value, on an affine and a non-affine dimension alike.
	static_assert(can_compound_scale<D<celsius<double>>, double>);
	static_assert(can_compound_divide<D<celsius<double>>, double>);

	// A point declares no compound scaling, no compound divide, no `+=` from another point, and a delta declares no
	// subtraction of a point -- four refusals with no overload behind them, so a concept observes each.
	static_assert(!can_compound_scale<A<celsius<double>>, double>, "a point declares no *=");
	static_assert(!can_compound_divide<A<celsius<double>>, double>, "a point declares no /=");
	static_assert(!can_compound_add<A<celsius<double>>, A<celsius<double>>>, "a point declares no += from a point");
	static_assert(!can_subtract_points<D<celsius<double>>, A<celsius<double>>>, "a delta declares no point subtraction");
	static_assert(traits::is_delta_v<decltype(D<celsius<double>>(2.5) * 2.0)>, "scaling a delta yields a delta");
	static_assert(traits::is_delta_v<decltype(D<meters<double>>(2.5) * 2.0)>);
	EXPECT_DOUBLE_EQ(5.0, (D<celsius<double>>(2.5) * 2.0).value());
	EXPECT_DOUBLE_EQ(5.0, (D<meters<double>>(2.5) * 2.0).value());

	// point += delta moves the point (same scale and cross-scale); point - point yields a delta; delta + point
	// yields a point.
	static_assert(can_compound_add<A<celsius<double>>, D<celsius<double>>>);
	static_assert(can_compound_add<A<celsius<double>>, D<fahrenheit<double>>>);
	static_assert(can_subtract_points<A<celsius<double>>, A<celsius<double>>>);
	static_assert(can_subtract_points<A<celsius<double>>, A<fahrenheit<double>>>);
	static_assert(can_add_delta_to_point<D<celsius<double>>, A<celsius<double>>>);
	static_assert(can_add_delta_to_point<D<fahrenheit<double>>, A<celsius<double>>>);
	SUCCEED();
}

//======================================================================================================================
//	DELTA SCALE-IN-PLACE VALUE — a change scales coordinate-free (spec cases, exact)
//======================================================================================================================

TEST(WrapperDelta, celsiusDeltaCompoundScaleAndDivideValue)
{
	// A change scales without any datum: 5 celsius-degrees doubled is 10; 10 halved is 5.
	D<celsius<double>> grow(5.0);
	grow *= 2.0;
	EXPECT_DOUBLE_EQ(10.0, grow.value());

	D<celsius<double>> shrink(10.0);
	shrink /= 2.0;
	EXPECT_DOUBLE_EQ(5.0, shrink.value());

	// Non-compound forms of the same spec cases.
	EXPECT_DOUBLE_EQ(10.0, (D<celsius<double>>(5.0) * 2.0).value());
	EXPECT_DOUBLE_EQ(5.0, (D<celsius<double>>(10.0) / 2.0).value());
}

//======================================================================================================================
//	CROSS-SCALE POINT + DELTA — a fahrenheit/celsius/kelvin delta moves a point by DEGREE SIZE only
//======================================================================================================================

TEST(WrapperAffine, celsiusPointPlusNineFahrenheitDeltaIsFiveDegreesWarmer)
{
	// 9 fahrenheit-degrees == 9 * 5/9 == 5 celsius-degrees, so 20 degC warmed by a 9 degF change is 25 degC — the
	// fahrenheit DATUM (the +32 offset) is never applied to a delta.
	const A<celsius<double>>    p(20.0);
	const D<fahrenheit<double>> warm(9.0);
	auto                        warmer = p + warm;
	static_assert(traits::is_absolute_v<decltype(warmer)>);
	static_assert(std::is_same_v<wrapped_t<decltype(warmer)>, celsius<double>>);
	EXPECT_DOUBLE_EQ(25.0, warmer.value());

	// delta + point commutes and keeps the point's (celsius) unit.
	auto commuted = warm + p;
	static_assert(traits::is_absolute_v<decltype(commuted)>);
	static_assert(std::is_same_v<wrapped_t<decltype(commuted)>, celsius<double>>);
	EXPECT_DOUBLE_EQ(25.0, commuted.value());
}

TEST(WrapperAffine, kelvinPointPlusCelsiusDelta)
{
	// celsius and kelvin share a degree size (ratio 1), so a 5 degC change is a 5 K change: 300 K + 5 = 305 K.
	const A<kelvin<double>>  p(300.0);
	const D<celsius<double>> warm(5.0);
	auto                     warmer = p + warm;
	static_assert(traits::is_absolute_v<decltype(warmer)>);
	static_assert(std::is_same_v<wrapped_t<decltype(warmer)>, kelvin<double>>);
	EXPECT_DOUBLE_EQ(305.0, warmer.value());

	// And cooling back down returns the point (datum preserved, kelvin kept).
	auto back = warmer - warm;
	static_assert(traits::is_absolute_v<decltype(back)>);
	EXPECT_DOUBLE_EQ(300.0, back.value());
}

TEST(WrapperAffine, kelvinMinusCelsiusPointDifferenceIsDeltaInKelvin)
{
	// point - point across the kelvin/celsius datum: 305 K minus 20 degC (== 293.15 K) is 11.85 kelvin-degrees,
	// expressed in the LHS (kelvin) unit. Computed by hand: 20 degC = 293.15 K; 305 - 293.15 = 11.85.
	const A<kelvin<double>>  hot(305.0);
	const A<celsius<double>> warm(20.0);
	auto                     diff = hot - warm;
	static_assert(traits::is_delta_v<decltype(diff)>);
	static_assert(std::is_same_v<wrapped_t<decltype(diff)>, kelvin<double>>);
	EXPECT_NEAR(11.85, diff.value(), 1e-9);
}

//======================================================================================================================
//	DELTA CROSS-SCALE ARITHMETIC VALUE — delta +/- delta across scales, LHS unit kept
//======================================================================================================================

TEST(WrapperDelta, deltaPlusMinusDeltaCrossScaleValue)
{
	// same scale (already exercised elsewhere, re-pinned here for the value): 10 + 3 = 13 celsius-degrees.
	EXPECT_DOUBLE_EQ(13.0, (D<celsius<double>>(10.0) + D<celsius<double>>(3.0)).value());

	// cross scale: a fahrenheit-degree delta added to a celsius-degree delta converts by degree size (9 degF = 5 degC),
	// keeping the LHS (celsius) unit: 10 + 5 = 15 celsius-degrees.
	auto sum = D<celsius<double>>(10.0) + D<fahrenheit<double>>(9.0);
	static_assert(traits::is_delta_v<decltype(sum)>);
	static_assert(std::is_same_v<wrapped_t<decltype(sum)>, celsius<double>>);
	EXPECT_DOUBLE_EQ(15.0, sum.value());

	// cross-scale subtract, LHS fahrenheit: 18 degF minus a 5 degC change (== 9 degF) = 9 fahrenheit-degrees.
	auto diff = D<fahrenheit<double>>(18.0) - D<celsius<double>>(5.0);
	static_assert(std::is_same_v<wrapped_t<decltype(diff)>, fahrenheit<double>>);
	EXPECT_DOUBLE_EQ(9.0, diff.value());
}

//======================================================================================================================
//	CONVERSIONS — datum applied for a point, scale-only for a delta (fractional, hand-verified factors)
//======================================================================================================================

TEST(WrapperConvert, deltaConversionIsScaleOnlyPointConversionAppliesDatum)
{
	// A 5 celsius-degree change is a 9 fahrenheit-degree change (5 * 9/5 = 9), scale only — never an absolute point.
	EXPECT_DOUBLE_EQ(9.0, D<celsius<double>>(5.0).to<D<fahrenheit<double>>>().value());
	EXPECT_DOUBLE_EQ(9.0, D<celsius<double>>(5.0).to<fahrenheit<double>>().value()); // plain-unit unwrap, same scale

	// A point conversion DOES apply the datum: 0 degC is 273.15 K, staying a point across the hop.
	auto k = A<celsius<double>>(0.0).to<A<kelvin<double>>>();
	static_assert(traits::is_absolute_v<decltype(k)>);
	EXPECT_DOUBLE_EQ(273.15, k.value());

	// A fractional point value survives the datum precisely: 37.5 degC = 310.65 K (37.5 + 273.15).
	EXPECT_NEAR(310.65, A<celsius<double>>(37.5).to<A<kelvin<double>>>().value(), 1e-9);

	// A fractional delta value scales precisely: a 2.375 celsius-degree change = 2.375 * 9/5 = 4.275 fahrenheit-degrees.
	EXPECT_NEAR(4.275, D<celsius<double>>(2.375).to<D<fahrenheit<double>>>().value(), 1e-9);
}

TEST(WrapperConvert, toKeepsPointAPointAndDeltaADeltaAcrossAHop)
{
	// to<absolute<V>> keeps a point a point and to<delta<V>> keeps a delta a delta, but asserting that through
	// `decltype` only restates the template argument written at the call site -- `to<WrapperTarget>` returns
	// `WrapperTarget`. The VALUE is what the hop must get right: a point applies the datum, a delta scales only.
	static_assert(traits::is_absolute_v<decltype(A<celsius<double>>(0.0).to<A<kelvin<double>>>())>);
	static_assert(traits::is_delta_v<decltype(D<celsius<double>>(5.0).to<D<fahrenheit<double>>>())>);
	EXPECT_DOUBLE_EQ(273.15, A<celsius<double>>(0.0).to<A<kelvin<double>>>().value());
	EXPECT_DOUBLE_EQ(9.0, D<celsius<double>>(5.0).to<D<fahrenheit<double>>>().value());
	EXPECT_DOUBLE_EQ(0.0, A<celsius<double>>(0.0).to<A<kelvin<double>>>().to<A<celsius<double>>>().value());
	EXPECT_DOUBLE_EQ(5.0, D<celsius<double>>(5.0).to<D<fahrenheit<double>>>().to<D<celsius<double>>>().value());
}

//======================================================================================================================
//	RANKINE AS A WRAPPED POINT — a pure-ratio absolute scale off kelvin, no per-unit offset
//======================================================================================================================
// rankine is `conversion_factor<ratio<5,9>, kelvin>`: 0 Ra == 0 K (absolute zero), and one rankine-degree is 5/9 of a
// kelvin/celsius-degree, so 491.67 Ra == 273.15 K == 0 degC. The wrappers must treat it exactly like the celsius /
// fahrenheit / kelvin wrappers — a full affine-dimension point even though it adds no per-unit datum of its own.

TEST(WrapperRankine, pointConstructionAndAbsoluteZero)
{
	// 0 Ra is absolute zero, equal to 0 K and to -273.15 degC as POINTS (the datum is applied on each side).
	EXPECT_TRUE(A<rankine<double>>(0.0) == A<kelvin<double>>(0.0));
	EXPECT_TRUE(A<rankine<double>>(0.0) == A<celsius<double>>(-273.15));

	// The freezing point of water: 273.15 K == 491.67 Ra (273.15 * 9/5). Verified by hand.
	EXPECT_NEAR(491.67, A<kelvin<double>>(273.15).to<A<rankine<double>>>().value(), 1e-9);
	EXPECT_TRUE(A<rankine<double>>(491.67) == A<celsius<double>>(0.0));
}

TEST(WrapperRankine, pointMoveByDeltaAndPointDifference)
{
	// A rankine point moved by a rankine-degree change stays a rankine point.
	A<rankine<double>> p(491.67);
	p += D<rankine<double>>(9.0); // +9 rankine-degrees == +5 kelvin/celsius-degrees
	static_assert(traits::is_absolute_v<decltype(A<rankine<double>>(0.0) + D<rankine<double>>(0.0))>);
	EXPECT_NEAR(500.67, p.value(), 1e-9);
	p -= D<rankine<double>>(9.0);
	EXPECT_NEAR(491.67, p.value(), 1e-9);

	// point - point across scales into a rankine delta: 500.67 Ra minus 491.67 Ra is 9 rankine-degrees.
	auto diff = A<rankine<double>>(500.67) - A<rankine<double>>(491.67);
	static_assert(traits::is_delta_v<decltype(diff)>);
	static_assert(std::is_same_v<wrapped_t<decltype(diff)>, rankine<double>>);
	EXPECT_NEAR(9.0, diff.value(), 1e-9);

	// A 9 rankine-degree delta is a 5 celsius-degree delta (9 * 5/9 = 5), scale-only.
	EXPECT_NEAR(5.0, D<rankine<double>>(9.0).to<D<celsius<double>>>().value(), 1e-9);
}

TEST(WrapperRankine, deltaArithmeticAndScaleAndPointDifference)
{
	// delta + delta and scaling behave identically to the celsius/fahrenheit wrappers.
	EXPECT_DOUBLE_EQ(15.0, (D<rankine<double>>(9.0) + D<rankine<double>>(6.0)).value());
	EXPECT_DOUBLE_EQ(18.0, (D<rankine<double>>(9.0) * 2.0).value());
	EXPECT_DOUBLE_EQ(4.5, (D<rankine<double>>(9.0) / 2.0).value());

	// A rankine delta scales, on the same terms as a celsius one: a delta holds a magnitude whatever it wraps.
	static_assert(traits::is_delta_v<decltype(D<rankine<double>>(2.5) * 2.0)>, "scaling a delta yields a delta");
	EXPECT_DOUBLE_EQ(5.0, (D<rankine<double>>(2.5) * 2.0).value());

	// point - point across the kelvin/rankine pair is allowed and yields a delta.
	static_assert(can_subtract_points<A<rankine<double>>, A<kelvin<double>>>);
	SUCCEED();
}

//======================================================================================================================
//	POINT MIN/MAX/CLAMP CROSS-SCALE — value correctness with a fahrenheit rhs reconciled affinely
//======================================================================================================================

TEST(WrapperConvert, pointMinMaxClampCrossScaleKeepsLhsUnitAndDatum)
{
	// min/max reconcile the rhs affinely (datum applied) but keep the LHS (celsius) unit and value.
	const A<celsius<double>>    c(20.0);
	const A<fahrenheit<double>> f(212.0); // == 100 degC
	auto                        cooler = affine::min(c, f);
	static_assert(std::is_same_v<wrapped_t<decltype(cooler)>, celsius<double>>);
	EXPECT_DOUBLE_EQ(20.0, cooler.value()); // 20 degC is cooler than 100 degC
	auto warmer = affine::max(c, f);
	EXPECT_DOUBLE_EQ(100.0, warmer.value()); // the warmer, expressed in celsius

	// clamp a celsius point into a range whose bounds are a mix of scales, all reconciled affinely to celsius.
	const A<fahrenheit<double>> lo(32.0); // == 0 degC
	const A<celsius<double>>    hi(50.0);
	EXPECT_DOUBLE_EQ(50.0, affine::clamp(A<celsius<double>>(75.0), lo, hi).value());
	EXPECT_DOUBLE_EQ(0.0, affine::clamp(A<celsius<double>>(-10.0), lo, hi).value());
	EXPECT_DOUBLE_EQ(37.5, affine::clamp(A<celsius<double>>(37.5), lo, hi).value()); // in-range fractional passes through
}

// The wrapper is the REMEDY the bare-unit diagnostics name, so it has to actually work for an affine unit. A plain
// affine reading refuses to scale (a datum-relative value has no meaningful multiple), which means a wrapper cannot
// implement its own scaling by delegating to the wrapped unit's `operator*` -- it holds a MAGNITUDE, so it scales its
// own value and rebuilds the unit. These pin that, since a regression here silently turns the documented remedy into
// a library-internal compile error.
TEST(WrapperDelta, anAffineDeltaScalesItsOwnMagnitude)
{
	using units::temperature::celsius;
	using units::temperature::fahrenheit;

	D<celsius<double>> change(20.5);
	EXPECT_DOUBLE_EQ(41.0, (change * 2.0).value());
	EXPECT_DOUBLE_EQ(41.0, (2.0 * change).value());
	EXPECT_DOUBLE_EQ(8.2, (change / 2.5).value());
	change *= 2.0;
	EXPECT_DOUBLE_EQ(41.0, change.value());
	change /= 4.0;
	EXPECT_DOUBLE_EQ(10.25, change.value());

	// the wrapped unit is preserved, so the magnitude stays in its own degrees rather than sliding to the base scale
	D<fahrenheit<double>> inFahrenheit(18.0);
	EXPECT_DOUBLE_EQ(27.0, (inFahrenheit * 1.5).value());
	static_assert(std::is_same_v<D<fahrenheit<double>>, std::remove_cv_t<decltype(inFahrenheit * 1.5)>>,
		"scaling a delta keeps both the wrapper and the unit it wraps");

	// scaling an integer-backed delta by a floating factor promotes, exactly as the plain unit's operator* does
	D<celsius<int>> integral(21);
	EXPECT_DOUBLE_EQ(52.5, (integral * 2.5).value());
	static_assert(std::is_same_v<double, typename std::remove_cv_t<decltype(integral * 2.5)>::underlying_type>,
		"an integer delta scaled by a floating factor promotes");

	// and a scaled delta moves a point
	A<celsius<double>> reading(20.0);
	reading += D<celsius<double>>(2.5) * 2.0;
	EXPECT_DOUBLE_EQ(25.0, reading.value());
}

// A kind wrapping an affine unit scales for the same reason a delta does; it also holds a magnitude with a tag.
TEST(WrapperDelta, aTaggedAmountAndATaggedReadingBothScaleInTheirOwnScale)
{
	using units::temperature::celsius;
	using units::kind;

	// A `kind` is the same quantity as the unit it wraps, only tagged, so its arithmetic delegates to that unit's --
	// which means the unit's refusals reach the tagged form. A tagged AMOUNT scales, because an amount does.
	using degrees = std::remove_cv_t<decltype(celsius<double>(1.0) - celsius<double>(0.0))>;
	kind<"cabin", degrees> tagged(20.5);
	EXPECT_DOUBLE_EQ(41.0, (tagged * 2.0).value());
	EXPECT_DOUBLE_EQ(8.2, (tagged / 2.5).value());
	static_assert(std::is_same_v<kind<"cabin", degrees>, std::remove_cv_t<decltype(tagged * 2.0)>>,
		"scaling a tagged amount keeps its tag and its unit");
	tagged *= 2.0;
	EXPECT_DOUBLE_EQ(41.0, tagged.value());

	// A tagged READING delegates to the wrapped unit too, so it follows the same scale-bound rule the plain reading
	// does: the number is read in the reading's own scale. Both operations are well-formed, and the tagged result
	// agrees with the untagged one.
	kind<"cabin", celsius<double>> reading(20.5);
	EXPECT_DOUBLE_EQ(20.5, reading.value());
	EXPECT_DOUBLE_EQ(41.0, (reading * 2.0).value());
	EXPECT_DOUBLE_EQ((celsius<double>(20.5) * 2.0).raw(), (reading * 2.0).value());
	EXPECT_DOUBLE_EQ(25.0, (reading + kind<"cabin", celsius<double>>(4.5)).value());
	EXPECT_DOUBLE_EQ((celsius<double>(20.5) + celsius<double>(4.5)).raw(),
		(reading + kind<"cabin", celsius<double>>(4.5)).value());
	// a tagged reading moves by a tagged amount, which is the operation that makes sense
	reading += kind<"cabin", degrees>(2.5);
	EXPECT_DOUBLE_EQ(23.0, reading.value());
}

//======================================================================================================================
//	THE POINT/AMOUNT CALCULUS AGREES WITH THE PLAIN AFFINE MODEL
//
//	`absolute<>`/`delta<>` (and the tagged `kind<>`) state in the type what the plain units state by rule: a point
//	moves by an AMOUNT, and only the amount's SCALE FACTOR crosses into the point's unit -- never its datum. Every
//	expected number is derived at its own assertion from the definitions in `units/temperature.h`: celsius is kelvin
//	shifted by 27315/100; fahrenheit is 5/9 of a celsius degree, shifted; reaumur is 5/4 of a celsius degree on
//	celsius's datum; rankine is 5/9 of a kelvin with no datum.
//======================================================================================================================

// A point moves by an amount written on any commensurable scale, and only that amount's scale factor crosses over.
// Derivations: 9 fahrenheit-degrees is 9 * 5/9 == 5 celsius-degrees; 4 reaumur-degrees is 4 * 5/4 == 5
// celsius-degrees; 5 kelvin is 5 celsius-degrees; 9 rankine-degrees is 9 * 5/9 == 5 kelvin. Each carries 20 degC to
// 20 + 5 == 25 degC, and down to 20 - 5 == 15 degC.
TEST(WrapperAmountSpelling, everySpellingOfOneAmountMovesAPointAlike)
{
	EXPECT_NEAR(25.0, (absolute<celsius<double>>(20.0) + delta<fahrenheit<double>>(9.0)).value(), 5.0e-12);
	EXPECT_NEAR(25.0, (absolute<celsius<double>>(20.0) + delta<reaumur<double>>(4.0)).value(), 5.0e-12);
	EXPECT_NEAR(25.0, (absolute<celsius<double>>(20.0) + delta<kelvin<double>>(5.0)).value(), 5.0e-12);
	EXPECT_NEAR(25.0, (absolute<celsius<double>>(20.0) + delta<rankine<double>>(9.0)).value(), 5.0e-12);

	EXPECT_NEAR(15.0, (absolute<celsius<double>>(20.0) - delta<fahrenheit<double>>(9.0)).value(), 5.0e-12);
	EXPECT_NEAR(15.0, (absolute<celsius<double>>(20.0) - delta<kelvin<double>>(5.0)).value(), 5.0e-12);

	// the amount may be written on the left; the result keeps the POINT's unit either way
	EXPECT_NEAR(25.0, (delta<fahrenheit<double>>(9.0) + absolute<celsius<double>>(20.0)).value(), 5.0e-12);
	static_assert(std::is_same_v<absolute<celsius<double>>,
					  std::remove_cv_t<decltype(delta<fahrenheit<double>>(9.0) + absolute<celsius<double>>(20.0))>>,
		"an amount plus a point is a point in the point's own unit");

	// 5 celsius-degrees is 5 / (5/9) == 9 fahrenheit-degrees, so 68 degF moves to 68 + 9 == 77 degF
	EXPECT_NEAR(77.0, (absolute<fahrenheit<double>>(68.0) + delta<celsius<double>>(5.0)).value(), 5.0e-12);
	// 5 celsius-degrees is 5 / (5/4) == 4 reaumur-degrees, so 16 degRe (== 20 degC) moves to 16 + 4 == 20 degRe
	EXPECT_NEAR(20.0, (absolute<reaumur<double>>(16.0) + delta<celsius<double>>(5.0)).value(), 5.0e-12);

	// the three spellings of ONE amount: 2.5 kelvin, 4.5 rankine-degrees (4.5 * 5/9 == 2.5 kelvin) and 4.5
	// fahrenheit-degrees (4.5 * 5/9 == 2.5 celsius-degrees) each carry 20.5 degC to 20.5 + 2.5 == 23 degC
	EXPECT_NEAR(23.0, (absolute<celsius<double>>(20.5) + delta<kelvin<double>>(2.5)).value(), 5.0e-12);
	EXPECT_NEAR(23.0, (absolute<celsius<double>>(20.5) + delta<rankine<double>>(4.5)).value(), 5.0e-12);
	EXPECT_NEAR(23.0, (absolute<celsius<double>>(20.5) + delta<fahrenheit<double>>(4.5)).value(), 5.0e-12);
	// 4.5 * 5/9 is exactly 2.5 in binary floating point, so those two agree to the last bit
	EXPECT_DOUBLE_EQ((absolute<celsius<double>>(20.5) + delta<kelvin<double>>(2.5)).value(),
		(absolute<celsius<double>>(20.5) + delta<rankine<double>>(4.5)).value());
	// and downward: 20.5 - 2.5 == 18 degC
	EXPECT_NEAR(18.0, (absolute<celsius<double>>(20.5) - delta<rankine<double>>(4.5)).value(), 5.0e-12);

	// the by-value form agrees with the compound one, operand for operand
	absolute<celsius<double>> byFahrenheit(20.0);
	byFahrenheit += delta<fahrenheit<double>>(9.0);
	EXPECT_NEAR(25.0, byFahrenheit.value(), 5.0e-12);
	EXPECT_DOUBLE_EQ(byFahrenheit.value(), (absolute<celsius<double>>(20.0) + delta<fahrenheit<double>>(9.0)).value());

	absolute<celsius<double>> byReaumur(20.0);
	byReaumur += delta<reaumur<double>>(4.0);
	EXPECT_NEAR(25.0, byReaumur.value(), 5.0e-12);
	EXPECT_DOUBLE_EQ(byReaumur.value(), (absolute<celsius<double>>(20.0) + delta<reaumur<double>>(4.0)).value());

	absolute<fahrenheit<double>> fahrenheitByCelsius(68.0);
	fahrenheitByCelsius += delta<celsius<double>>(5.0);
	EXPECT_NEAR(77.0, fahrenheitByCelsius.value(), 5.0e-12);

	absolute<reaumur<double>> reaumurByCelsius(16.0);
	reaumurByCelsius += delta<celsius<double>>(5.0);
	EXPECT_NEAR(20.0, reaumurByCelsius.value(), 5.0e-12);

	// an amount converted between scales carries no datum: 9 fahrenheit-degrees IS 9 * 5/9 == 5 celsius-degrees, and
	// 2.5 kelvin IS 2.5 / (5/9) == 4.5 rankine-degrees
	EXPECT_NEAR(5.0, delta<fahrenheit<double>>(9.0).to<celsius<double>>().value(), 5.0e-12);
	EXPECT_TRUE(delta<kelvin<double>>(2.5) == delta<rankine<double>>(4.5));
}

// An OFFSET-FREE point moves by an amount written on an affine scale on exactly the same terms: only the amount's
// scale factor applies. Derivations: 5 celsius-degrees is 5 kelvin, so 300 K warms to 300 + 5 == 305 K -- reading
// delta<celsius>(5) as the absolute temperature 5 + 273.15 == 278.15 K would instead give 578.15 K. 9
// fahrenheit-degrees is 9 * 5/9 == 5 kelvin and 4 reaumur-degrees is 4 * 5/4 == 5 celsius-degrees == 5 kelvin.
// Rankine counts in 5/9 of a kelvin, so 540 degRa is 540 * 5/9 == 300 K and 540 + 9 == 549 degRa is 305 K.
TEST(WrapperOffsetFreePoint, anOffsetFreePointMovesByTheAmountsScaleFactorOnly)
{
	EXPECT_NEAR(305.0, (absolute<kelvin<double>>(300.0) + delta<celsius<double>>(5.0)).value(), 5.0e-12);
	EXPECT_NEAR(305.0, (absolute<kelvin<double>>(300.0) + delta<fahrenheit<double>>(9.0)).value(), 5.0e-12);
	EXPECT_NEAR(305.0, (absolute<kelvin<double>>(300.0) + delta<reaumur<double>>(4.0)).value(), 5.0e-12);
	EXPECT_NEAR(549.0, (absolute<rankine<double>>(540.0) + delta<celsius<double>>(5.0)).value(), 5.0e-12);

	// the point keeps its own unit, so a move never launders a reading into the amount's scale
	static_assert(std::is_same_v<absolute<kelvin<double>>,
					  std::remove_cv_t<decltype(absolute<kelvin<double>>(300.0) + delta<celsius<double>>(5.0))>>,
		"a moved point stays in its own unit");
	static_assert(std::is_same_v<absolute<rankine<double>>,
					  std::remove_cv_t<decltype(absolute<rankine<double>>(540.0) + delta<celsius<double>>(5.0))>>,
		"a moved point stays in its own unit");

	absolute<kelvin<double>> kelvinByCelsius(300.0);
	kelvinByCelsius += delta<celsius<double>>(5.0);
	EXPECT_NEAR(305.0, kelvinByCelsius.value(), 5.0e-12);

	absolute<kelvin<double>> kelvinByFahrenheit(300.0);
	kelvinByFahrenheit += delta<fahrenheit<double>>(9.0);
	EXPECT_NEAR(305.0, kelvinByFahrenheit.value(), 5.0e-12);

	absolute<kelvin<double>> kelvinByReaumur(300.0);
	kelvinByReaumur += delta<reaumur<double>>(4.0);
	EXPECT_NEAR(305.0, kelvinByReaumur.value(), 5.0e-12);

	// and downward: 300 - 5 == 295 K for every spelling of the same amount
	absolute<kelvin<double>> kelvinDownByCelsius(300.0);
	kelvinDownByCelsius -= delta<celsius<double>>(5.0);
	EXPECT_NEAR(295.0, kelvinDownByCelsius.value(), 5.0e-12);

	absolute<kelvin<double>> kelvinDownByFahrenheit(300.0);
	kelvinDownByFahrenheit -= delta<fahrenheit<double>>(9.0);
	EXPECT_NEAR(295.0, kelvinDownByFahrenheit.value(), 5.0e-12);

	absolute<kelvin<double>> kelvinDownByReaumur(300.0);
	kelvinDownByReaumur -= delta<reaumur<double>>(4.0);
	EXPECT_NEAR(295.0, kelvinDownByReaumur.value(), 5.0e-12);

	absolute<rankine<double>> rankineByCelsius(540.0);
	rankineByCelsius += delta<celsius<double>>(5.0);
	EXPECT_NEAR(549.0, rankineByCelsius.value(), 5.0e-12);

	absolute<rankine<double>> rankineByFahrenheit(540.0);
	rankineByFahrenheit += delta<fahrenheit<double>>(9.0);
	EXPECT_NEAR(549.0, rankineByFahrenheit.value(), 5.0e-12);

	absolute<rankine<double>> rankineByReaumur(540.0);
	rankineByReaumur += delta<reaumur<double>>(4.0);
	EXPECT_NEAR(549.0, rankineByReaumur.value(), 5.0e-12);

	// and downward: 540 - 9 == 531 degRa, which is 531 * 5/9 == 295 K
	absolute<rankine<double>> rankineDownByCelsius(540.0);
	rankineDownByCelsius -= delta<celsius<double>>(5.0);
	EXPECT_NEAR(531.0, rankineDownByCelsius.value(), 5.0e-12);

	absolute<rankine<double>> rankineDownByFahrenheit(540.0);
	rankineDownByFahrenheit -= delta<fahrenheit<double>>(9.0);
	EXPECT_NEAR(531.0, rankineDownByFahrenheit.value(), 5.0e-12);

	absolute<rankine<double>> rankineDownByReaumur(540.0);
	rankineDownByReaumur -= delta<reaumur<double>>(4.0);
	EXPECT_NEAR(531.0, rankineDownByReaumur.value(), 5.0e-12);

	// the difference of two points is an amount in the LEFT point's unit, and the two datums cancel: 50 degF is
	// (50 - 32) * 5/9 == 10 degC, so 30 degC less 50 degF is a 30 - 10 == 20 celsius-degree step; 10 degC is
	// 10 * 9/5 + 32 == 50 degF, so 86 degF less 10 degC is an 86 - 50 == 36 fahrenheit-degree step
	EXPECT_NEAR(20.0, (absolute<celsius<double>>(30.0) - absolute<celsius<double>>(10.0)).value(), 5.0e-12);
	EXPECT_NEAR(20.0, (absolute<celsius<double>>(30.0) - absolute<fahrenheit<double>>(50.0)).value(), 5.0e-12);
	EXPECT_NEAR(36.0, (absolute<fahrenheit<double>>(86.0) - absolute<celsius<double>>(10.0)).value(), 5.0e-12);
}

// A tagged quantity delegates to the unit it wraps, so the unit's amount rule reaches the tagged form: a tagged
// reading moved by a tagged amount reads only the amount's scale factor. Derivations: 9 fahrenheit-degrees is
// 9 * 5/9 == 5 celsius-degrees and 4 reaumur-degrees is 4 * 5/4 == 5 celsius-degrees, so each carries 20 degC to
// 25 degC; 5 celsius-degrees is 5 kelvin, so 300 K warms to 305 K; 540 degRa is 540 * 5/9 == 300 K, and 5 kelvin is
// 9 rankine-degrees, so it warms to 549 degRa.
TEST(WrapperKindAmountModel, aTaggedReadingMovesByTheTaggedAmountsScaleFactorOnly)
{
	using units::kind;

	// the amount types, spelled here: a difference of two readings on one scale, which carries no datum
	using celsiusDegrees    = std::remove_cv_t<decltype(celsius<double>(1.0) - celsius<double>(0.0))>;
	using fahrenheitDegrees = std::remove_cv_t<decltype(fahrenheit<double>(1.0) - fahrenheit<double>(0.0))>;
	using reaumurDegrees    = std::remove_cv_t<decltype(reaumur<double>(1.0) - reaumur<double>(0.0))>;

	static_assert(!units::traits::is_affine_unit_v<celsiusDegrees>);
	static_assert(!units::traits::is_affine_unit_v<fahrenheitDegrees>);
	static_assert(!units::traits::is_affine_unit_v<reaumurDegrees>);

	EXPECT_NEAR(25.0, (kind<"cabin", celsius<double>>(20.0) + kind<"cabin", fahrenheitDegrees>(9.0)).value(), 5.0e-12);
	EXPECT_NEAR(25.0, (kind<"cabin", celsius<double>>(20.0) + kind<"cabin", reaumurDegrees>(4.0)).value(), 5.0e-12);
	EXPECT_NEAR(305.0, (kind<"cabin", kelvin<double>>(300.0) + kind<"cabin", celsiusDegrees>(5.0)).value(), 5.0e-12);
	EXPECT_NEAR(305.0, (kind<"cabin", kelvin<double>>(300.0) + kind<"cabin", fahrenheitDegrees>(9.0)).value(), 5.0e-12);
	EXPECT_NEAR(549.0, (kind<"cabin", rankine<double>>(540.0) + kind<"cabin", celsiusDegrees>(5.0)).value(), 5.0e-12);

	kind<"cabin", celsius<double>> cabin(20.0);
	cabin += kind<"cabin", fahrenheitDegrees>(9.0);
	EXPECT_NEAR(25.0, cabin.value(), 5.0e-12);

	kind<"cabin", kelvin<double>> absoluteCabin(300.0);
	absoluteCabin += kind<"cabin", celsiusDegrees>(5.0);
	EXPECT_NEAR(305.0, absoluteCabin.value(), 5.0e-12);
	absoluteCabin -= kind<"cabin", celsiusDegrees>(5.0);
	EXPECT_NEAR(300.0, absoluteCabin.value(), 5.0e-12);

	// a tagged reading written on an affine scale is read as an amount by an offset-free tagged reading, exactly as
	// the plain units are: 5 celsius-degrees, not the absolute 278.15 K
	kind<"cabin", kelvin<double>> byPlainCelsius(300.0);
	byPlainCelsius += kind<"cabin", celsius<double>>(5.0);
	EXPECT_NEAR(305.0, byPlainCelsius.value(), 5.0e-12);

	kind<"cabin", kelvin<double>> byPlainFahrenheit(300.0);
	byPlainFahrenheit += kind<"cabin", fahrenheit<double>>(9.0);
	EXPECT_NEAR(305.0, byPlainFahrenheit.value(), 5.0e-12);

	kind<"cabin", kelvin<double>> byPlainReaumur(300.0);
	byPlainReaumur += kind<"cabin", reaumur<double>>(4.0);
	EXPECT_NEAR(305.0, byPlainReaumur.value(), 5.0e-12);
}

// A wrapper hashes on the value in the dimension's SI base unit, exactly as the unit it wraps does, so two spellings
// of one quantity hash alike. Derivations: 0 degC is 0 + 273.15 K and 26.85 degC is 26.85 + 273.15 == 300 K; 1 km is
// 1000 m; 4.5 rankine-degrees is 4.5 * 5/9 == 2.5 kelvin.
TEST(WrapperDatumFreeHash, aWrapperHashesOnTheQuantityNotTheSpelling)
{
	EXPECT_EQ(std::hash<absolute<celsius<double>>>{}(absolute<celsius<double>>(0.0)),
		std::hash<absolute<kelvin<double>>>{}(absolute<kelvin<double>>(273.15)));
	EXPECT_EQ(std::hash<absolute<celsius<double>>>{}(absolute<celsius<double>>(26.85)),
		std::hash<absolute<kelvin<double>>>{}(absolute<kelvin<double>>(300.0)));
	EXPECT_EQ(std::hash<delta<units::length::meters<double>>>{}(delta<units::length::meters<double>>(1000.0)),
		std::hash<delta<units::length::kilometers<double>>>{}(delta<units::length::kilometers<double>>(1.0)));
	EXPECT_EQ(std::hash<delta<kelvin<double>>>{}(delta<kelvin<double>>(2.5)),
		std::hash<delta<rankine<double>>>{}(delta<rankine<double>>(4.5)));

	// +0.0 and -0.0 are one quantity
	EXPECT_EQ(std::hash<delta<units::length::meters<double>>>{}(delta<units::length::meters<double>>(0.0)),
		std::hash<delta<units::length::meters<double>>>{}(delta<units::length::meters<double>>(-0.0)));

	// distinct quantities are not forced to collide
	EXPECT_NE(std::hash<absolute<celsius<double>>>{}(absolute<celsius<double>>(20.0)),
		std::hash<absolute<celsius<double>>>{}(absolute<celsius<double>>(21.0)));
}

// The datum-free shapes an affine scale can take reach the wrappers unchanged: `squared` drops the translation, so a
// squared celsius and a squared kelvin are one type and a delta of either is one delta. Were the translation kept,
// converting between them would apply it and 4 would read as 4 + 273.15 == 277.15.
TEST(WrapperDatumFreeShape, aDeltaOfASquaredAffineUnitCarriesNoDatum)
{
	// spelled here, in the one test that uses them
	using squaredCelsius = units::unit<units::squared<units::temperature::celsius_>, double>;
	using squaredKelvin  = units::unit<units::squared<units::temperature::kelvin_>, double>;

	static_assert(std::ratio_equal_v<std::ratio<0>, units::squared<units::temperature::celsius_>::translation_ratio>,
		"a squared unit has no origin");
	static_assert(std::is_same_v<squaredCelsius, squaredKelvin>, "the two spellings of one squared temperature are one type");
	static_assert(std::is_same_v<delta<squaredCelsius>, delta<squaredKelvin>>);
	static_assert(!units::traits::is_affine_unit_v<squaredCelsius>);

	EXPECT_DOUBLE_EQ(4.0, delta<squaredCelsius>(4.0).value());
	EXPECT_DOUBLE_EQ(4.0, delta<squaredKelvin>(delta<squaredCelsius>(4.0)).value());
}

//======================================================================================================================
//	SECOND-AUDIT REGRESSION GUARD
//======================================================================================================================

// A delta takes its magnitude from its OWN value, but it must clear a sign the same way `units::abs` does. Choosing
// the branch with an ORDERING test cannot: `-0.0 < 0.0` is false, so a negatively-signed zero passed straight
// through, and every comparison against a NaN is false, so a negative NaN did too. `abs(delta)` then disagreed with
// `abs` of the plain unit and with `abs` of a `kind`, neither of which changed.
TEST(WrapperDeltaMagnitude, aSignedZeroAndANegativeNanAreNormalisedAsUnitsAbsDoes)
{
	// |-0.0| is +0.0: the value compares equal either way, so the SIGN BIT is what the assertion has to read
	EXPECT_FALSE(std::signbit(units::abs(delta<meters<double>>(meters<double>(-0.0))).value()));
	EXPECT_FALSE(std::signbit(units::abs(delta<meters<float>>(meters<float>(-0.0f))).value()));
	EXPECT_FALSE(std::signbit(units::abs(delta<meters<long double>>(meters<long double>(-0.0L))).value()));
	// an affine unit's delta, the case the own-value magnitude exists for
	EXPECT_FALSE(std::signbit(units::abs(delta<celsius<double>>(celsius<double>(-0.0))).value()));

	// |-NaN| is a NaN with the sign cleared
	const auto negativeNan = units::abs(delta<meters<double>>(meters<double>(-std::numeric_limits<double>::quiet_NaN())));
	EXPECT_TRUE(std::isnan(negativeNan.value()));
	EXPECT_FALSE(std::signbit(negativeNan.value()));

	// and the ordinary magnitudes are unchanged: |-5.25| is 5.25
	EXPECT_DOUBLE_EQ(5.25, units::abs(delta<meters<double>>(meters<double>(-5.25))).value());
	EXPECT_DOUBLE_EQ(5.25, units::abs(delta<celsius<double>>(celsius<double>(-5.25))).value());
	EXPECT_DOUBLE_EQ(0.0, units::abs(delta<meters<double>>(meters<double>(0.0))).value());

	// it agrees with the plain unit's own magnitude, which is the point
	EXPECT_FALSE(std::signbit(units::abs(meters<double>(-0.0)).value()));
	EXPECT_DOUBLE_EQ(units::abs(meters<double>(-5.25)).value(), units::abs(delta<meters<double>>(meters<double>(-5.25))).value());
}

// A kind is a TAG on an existing unit, so its arithmetic must be the wrapped unit's arithmetic -- same result unit,
// same representation, same value. It is delegated rather than reimplemented, which means it also inherits the plain
// unit's limits: a mixed pair of integral units lands in the finer of the two, and a wide enough ratio overflows
// there exactly as it does without the tag. Pinned in both directions, because the pull to "improve" one side of
// this by promoting only the tagged form is what made the two disagree before.
TEST(WrapperKindDelegation, taggedArithmeticMatchesTheWrappedUnitExactly)
{
	// 1 km + 500 m is 1500 m: the result lands in metres, the finer unit, because converting metres into kilometres
	// would truncate an integral representation
	const auto plain  = kilometers<int>(1) + meters<int>(500);
	const auto tagged = units::kind<"t", kilometers<int>>(1) + units::kind<"t", meters<int>>(500);
	EXPECT_EQ(1500, plain.raw());
	EXPECT_EQ(1500, static_cast<int>(tagged.raw()));
	static_assert(std::is_same_v<meters<int>, std::decay_t<decltype(plain)>>);
	static_assert(std::is_same_v<units::kind<"t", meters<int>>, std::decay_t<decltype(tagged)>>);

	// the same for a difference, and for a floating pair where the left unit simply wins
	EXPECT_EQ(500, static_cast<int>((units::kind<"t", kilometers<int>>(1) - units::kind<"t", meters<int>>(500)).raw()));
	EXPECT_DOUBLE_EQ(1.5, static_cast<double>((units::kind<"t", kilometers<double>>(1.0) + units::kind<"t", meters<double>>(500.0)).raw()));

	// a tagged reading negates in its own scale, as the plain reading does -- it is not refused
	EXPECT_DOUBLE_EQ(-5.0, (-celsius<double>(5.0)).value());
	EXPECT_DOUBLE_EQ(-5.0, static_cast<double>((-units::kind<"t", celsius<double>>(5.0)).raw()));
	static_assert(std::is_same_v<units::kind<"t", celsius<double>>, std::decay_t<decltype(-units::kind<"t", celsius<double>>(5.0))>>);
}
