// Exhaustive tests for the affine absolute<>/delta<> point/amount wrappers. An adversarial wart-hunt across the
// affine (temperature), pi-carrying (angle), ratio-dimensionless (percent), and general slices — over
// double/float/int and commensurable/incommensurable/mixed-underlying operands — plus the type-safety boundaries
// (trivial-copyability, explicit ctors, plain/wrapper rejection, point+point rejection, traits/concepts, printing,
// math). The central claim under test is the LHS-unit tie-break: a wrapper operator keeps the LEFT operand's unit
// so .value() reads in the unit the user wrote (absolute<celsius> - absolute<fahrenheit> is 100 CELSIUS-degrees,
// not a common sub-unit), promoting only the underlying when a coarse integer LHS cannot hold the RHS losslessly.

#include <gtest/gtest.h>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_set>

#include <units/angle.h>
#include <units/concentration.h>
#include <units/core.h>
#include <units/energy.h>
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
