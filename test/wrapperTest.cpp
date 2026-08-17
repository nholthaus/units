// Exhaustive tests for the affine absolute<>/delta<> point/amount wrappers. An adversarial wart-hunt across the
// affine (temperature), pi-carrying (angle), ratio-dimensionless (percent), and general slices — over
// double/float/int and commensurable/incommensurable/mixed-underlying operands — plus the type-safety boundaries
// (trivial-copyability, explicit ctors, plain/wrapper rejection, point+point rejection, traits/concepts, printing,
// math). The central claim under test is the LHS-unit tie-break: a wrapper operator keeps the LEFT operand's unit
// so .value() reads in the unit the user wrote (absolute<celsius> - absolute<fahrenheit> is 100 CELSIUS-degrees,
// not a common sub-unit), promoting only the underlying when a coarse integer LHS cannot hold the RHS losslessly.

#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <type_traits>

#include <units/angle.h>
#include <units/concentration.h>
#include <units/core.h>
#include <units/length.h>
#include <units/temperature.h>
#include <units/time.h>

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
} // namespace

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
	// quantity() gives the wrapped plain unit (datum intact).
	EXPECT_DOUBLE_EQ(273.15, p.quantity().value());
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
	static_assert(std::is_same_v<absolute<celsius<double>>, kind::absolute<celsius<double>>>);
	static_assert(std::is_same_v<delta<meters<int>>, kind::delta<meters<int>>>);
	static_assert(std::is_same_v<units::absolute<radians<float>>, units::kind::absolute<radians<float>>>);
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
	// A 10 degC delta is an 18 degF delta (scale only), NOT an absolute 50 degF.
	const D<celsius<double>> del(10.0);
	const D<fahrenheit<double>> f = del.to<fahrenheit<double>>();
	EXPECT_DOUBLE_EQ(18.0, f.value());

	// And a zero delta carries no offset: 0 degC delta -> 0 degF delta (not 32).
	const D<celsius<double>> z(0.0);
	EXPECT_DOUBLE_EQ(0.0, z.to<fahrenheit<double>>().value());

	// A point conversion DOES apply the datum: 0 degC point -> 32 degF point.
	const A<celsius<double>> zeroPoint(0.0);
	EXPECT_DOUBLE_EQ(32.0, zeroPoint.to<fahrenheit<double>>().value());
	EXPECT_DOUBLE_EQ(273.15, zeroPoint.to<kelvin<double>>().value());
}

TEST(WrapperAffine, pointConversionRoundTrips)
{
	const A<celsius<double>> body(37.0);
	const auto               k    = body.to<kelvin<double>>();
	const auto               back = k.to<celsius<double>>();
	EXPECT_NEAR(37.0, back.value(), 1e-9);
}

TEST(WrapperAffine, minMaxClampOfPoints)
{
	const A<celsius<double>> a(10.0);
	const A<celsius<double>> b(30.0);
	EXPECT_DOUBLE_EQ(10.0, kind::min(a, b).value());
	EXPECT_DOUBLE_EQ(30.0, kind::max(a, b).value());

	// min/max reconcile the rhs affinely but keep the LHS unit.
	const A<fahrenheit<double>> f(32.0); // == 0 degC
	auto                        extremum = kind::min(a, f);
	static_assert(std::is_same_v<wrapped_t<decltype(extremum)>, celsius<double>>);
	EXPECT_DOUBLE_EQ(0.0, extremum.value()); // the colder of 10 degC and 0 degC is 0 degC

	// clamp of a point.
	const A<celsius<double>> lo(0.0), hi(20.0), v(37.0);
	EXPECT_DOUBLE_EQ(20.0, kind::clamp(v, lo, hi).value());
	const A<celsius<double>> v2(-5.0);
	EXPECT_DOUBLE_EQ(0.0, kind::clamp(v2, lo, hi).value());
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
	auto                     mag = kind::abs(neg);
	static_assert(traits::is_delta_v<decltype(mag)>);
	EXPECT_DOUBLE_EQ(7.5, mag.value());

	// integer delta promotes to floating (matching the plain unit's abs promotion contract).
	const D<meters<int>> negi(-4);
	auto                 magi = kind::abs(negi);
	static_assert(std::is_floating_point_v<under_t<decltype(magi)>>);
	EXPECT_DOUBLE_EQ(4.0, magi.value());
}

TEST(WrapperMath, minMaxClampOfDeltas)
{
	const D<celsius<double>> a(10.0), b(30.0);
	EXPECT_DOUBLE_EQ(10.0, kind::min(a, b).value());
	EXPECT_DOUBLE_EQ(30.0, kind::max(a, b).value());

	// mixed unit min/max keeps the LHS unit.
	const D<fahrenheit<double>> f(18.0); // == 10 celsius-degrees
	auto                        extremum = kind::max(a, f);
	static_assert(std::is_same_v<wrapped_t<decltype(extremum)>, celsius<double>>);
	EXPECT_DOUBLE_EQ(10.0, extremum.value()); // 10 degC and 10-celsius-degree delta are equal; max keeps lhs

	// clamp.
	const D<celsius<double>> lo(0.0), hi(20.0), v(30.0);
	EXPECT_DOUBLE_EQ(20.0, kind::clamp(v, lo, hi).value());
	const D<celsius<double>> v2(-5.0);
	EXPECT_DOUBLE_EQ(0.0, kind::clamp(v2, lo, hi).value());
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
