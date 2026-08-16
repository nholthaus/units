// ---------------------------------------------------------------------------------------------------------------------
//			                 __
//			               _/ /          /
//			    __________/  /__ _______/
//			   /  _______   ___//  ____/
//			  /  /___   /  /   /  /   /   SYSTEMS
//			  \____  \ /  /   /  /   /    & TECHNOLOGY
//			 _____/  //  /___/  /   /     RESEARCH
//			/_______/ \________/   /
//			                      /
//			                     /
// ---------------------------------------------------------------------------------------------------------------------
//
/// @file       wrapperTest_general.h
/// @brief      Adversarial tests for the `absolute<>` / `delta<>` wrappers over GENERAL (non-affine) units.
/// @details    Exercises the point/amount wrappers on units whose datum is zero (length, time, mass, velocity,
///             pressure, torque, angle, dimensionless), where `absolute` and `delta` coincide numerically. The
///             angle cases prove the delta's scale-only conversion still carries the pi exponent. Warts found
///             during authoring are captured as `Wart_*` cases that PIN the current behavior — a future fix
///             flips the assertion deliberately.
//
// ---------------------------------------------------------------------------------------------------------------------

#pragma once

#include <cmath>
#include <compare>
#include <gtest/gtest.h>
#include <type_traits>
#include <units.h>

namespace
{
	using namespace units;
	using namespace units::length;
	using namespace units::time;
	using namespace units::mass;
	using namespace units::velocity;
	using namespace units::pressure;
	using namespace units::angle;
	using namespace units::torque;

	// A generous floating-point tolerance for the general-wrapper assertions (pi conversions, unit reconciliation).
	constexpr double kGeneralWrapperTol = 1e-9;

	// Detection idiom used to prove which wrapper operators are (SFINAE-)formable. NOTE: a `true` here means the
	// operator PARTICIPATES in overload resolution — it does NOT prove the body instantiates (see Wart_LossyInt).
	template<class A, class B, class = void>
	struct genWrap_hasAdd : std::false_type
	{
	};
	template<class A, class B>
	struct genWrap_hasAdd<A, B, std::void_t<decltype(std::declval<A>() + std::declval<B>())>> : std::true_type
	{
	};

	template<class A, class B, class = void>
	struct genWrap_hasSub : std::false_type
	{
	};
	template<class A, class B>
	struct genWrap_hasSub<A, B, std::void_t<decltype(std::declval<A>() - std::declval<B>())>> : std::true_type
	{
	};

	template<class A, class B, class = void>
	struct genWrap_hasMul : std::false_type
	{
	};
	template<class A, class B>
	struct genWrap_hasMul<A, B, std::void_t<decltype(std::declval<A>() * std::declval<B>())>> : std::true_type
	{
	};
} // namespace

//======================================================================================================================
//	POINT / DELTA ALGEBRA over a zero-datum unit (length)
//======================================================================================================================

TEST(GeneralWrapper, LengthPointMinusPointIsDelta)
{
	const auto d = absolute<meters<double>>(meters<double>(5)) - absolute<meters<double>>(meters<double>(3));
	static_assert(std::is_same_v<std::remove_const_t<decltype(d)>, delta<meters<double>>>, "point - point -> delta");
	EXPECT_DOUBLE_EQ(2.0, d.value());
}

TEST(GeneralWrapper, LengthPointPlusDeltaIsPoint)
{
	const auto p = absolute<meters<double>>(meters<double>(5)) + delta<meters<double>>(meters<double>(3));
	static_assert(std::is_same_v<std::remove_const_t<decltype(p)>, absolute<meters<double>>>, "point + delta -> point");
	// 5 m + 3 m = 8 m (a zero-datum unit: absolute and delta add like plain numbers).
	EXPECT_DOUBLE_EQ(8.0, p.value());
}

TEST(GeneralWrapper, LengthPointMinusDeltaIsPoint)
{
	const auto p = absolute<meters<double>>(meters<double>(5)) - delta<meters<double>>(meters<double>(3));
	static_assert(std::is_same_v<std::remove_const_t<decltype(p)>, absolute<meters<double>>>, "point - delta -> point");
	EXPECT_DOUBLE_EQ(2.0, p.value());
}

TEST(GeneralWrapper, LengthDeltaPlusPointCommutes)
{
	const auto p = delta<meters<double>>(meters<double>(3)) + absolute<meters<double>>(meters<double>(5));
	static_assert(std::is_same_v<std::remove_const_t<decltype(p)>, absolute<meters<double>>>, "delta + point -> point");
	EXPECT_DOUBLE_EQ(8.0, p.value());
}

TEST(GeneralWrapper, LengthDeltaPlusDelta)
{
	const auto d = delta<meters<double>>(meters<double>(3)) + delta<meters<double>>(meters<double>(4));
	static_assert(std::is_same_v<std::remove_const_t<decltype(d)>, delta<meters<double>>>, "delta + delta -> delta");
	EXPECT_DOUBLE_EQ(7.0, d.value());
}

TEST(GeneralWrapper, LengthDeltaMinusDelta)
{
	const auto d = delta<meters<double>>(meters<double>(7)) - delta<meters<double>>(meters<double>(4));
	static_assert(std::is_same_v<std::remove_const_t<decltype(d)>, delta<meters<double>>>, "delta - delta -> delta");
	EXPECT_DOUBLE_EQ(3.0, d.value());
}

TEST(GeneralWrapper, LengthCompoundAssignMovesPoint)
{
	absolute<meters<double>> p(meters<double>(5));
	p += delta<meters<double>>(meters<double>(3));
	EXPECT_DOUBLE_EQ(8.0, p.value());
	p -= delta<meters<double>>(meters<double>(1));
	EXPECT_DOUBLE_EQ(7.0, p.value());
}

//======================================================================================================================
//	CROSS-UNIT RECONCILIATION (scale only for zero-datum units)
//======================================================================================================================

TEST(GeneralWrapper, LengthCrossUnitPointDifference)
{
	// 5 m - 3 ft (= 0.9144 m) = 4.0856 m, expressed in the common (finer) unit. Compare via a conversion to a
	// known unit so the test is independent of which unit std::common_type selects.
	const auto d = absolute<meters<double>>(meters<double>(5)) - absolute<feet<double>>(feet<double>(3));
	EXPECT_NEAR(4.0856, d.template to<meters<double>>().value(), kGeneralWrapperTol);
}

TEST(GeneralWrapper, LengthCrossUnitDeltaSumScaleReconciles)
{
	// 1 km delta + 500 m delta = 1500 m delta (scale reconciliation to the common/finer unit, no datum).
	const auto d = delta<kilometers<double>>(kilometers<double>(1)) + delta<meters<double>>(meters<double>(500));
	EXPECT_DOUBLE_EQ(1.5, d.template to<kilometers<double>>().value());
}

TEST(GeneralWrapper, ZeroDatumAbsoluteAndDeltaConversionsAgree)
{
	// For a non-affine (zero-datum) unit, `absolute::to` (offset applied) and `delta::to` (scale only) MUST agree,
	// because the offset is zero. This is the property that separates general units from temperatures.
	const auto ptFeet    = absolute<meters<double>>(meters<double>(2)).to<feet<double>>();
	const auto deltaFeet = delta<meters<double>>(meters<double>(2)).to<feet<double>>();
	static_assert(std::is_same_v<std::remove_const_t<decltype(ptFeet)>, absolute<feet<double>>>, "absolute::to -> absolute<V>");
	static_assert(std::is_same_v<std::remove_const_t<decltype(deltaFeet)>, delta<feet<double>>>, "delta::to -> delta<V>");
	EXPECT_DOUBLE_EQ(ptFeet.value(), deltaFeet.value());
	EXPECT_NEAR(6.56167979, ptFeet.value(), 1e-6);
}

//======================================================================================================================
//	ANGLE — the pi-carrying units (highest-value wart target)
//======================================================================================================================

TEST(GeneralWrapper, AngleDeltaDegreesToRadiansCarriesPi)
{
	// `delta::to` strips the datum but MUST keep the pi exponent: 90 deg delta = pi/2 rad delta.
	const auto rad = delta<degrees<double>>(degrees<double>(90)).to<radians<double>>();
	EXPECT_NEAR(units::detail::PI_VAL / 2.0, rad.value(), kGeneralWrapperTol);
}

TEST(GeneralWrapper, AngleAbsoluteDegreesToRadiansCarriesPi)
{
	const auto rad = absolute<degrees<double>>(degrees<double>(90)).to<radians<double>>();
	EXPECT_NEAR(units::detail::PI_VAL / 2.0, rad.value(), kGeneralWrapperTol);
}

TEST(GeneralWrapper, AngleDeltaRadiansToDegrees)
{
	const auto deg = delta<radians<double>>(radians<double>(units::detail::PI_VAL)).to<degrees<double>>();
	EXPECT_NEAR(180.0, deg.value(), kGeneralWrapperTol);
}

TEST(GeneralWrapper, AngleDeltaTurnsConvertBothWays)
{
	const auto deg = delta<turns<double>>(turns<double>(1)).to<degrees<double>>();
	EXPECT_NEAR(360.0, deg.value(), kGeneralWrapperTol);
	const auto rad = delta<turns<double>>(turns<double>(1)).to<radians<double>>();
	EXPECT_NEAR(2.0 * units::detail::PI_VAL, rad.value(), kGeneralWrapperTol);
}

TEST(GeneralWrapper, AngleDeltaGradiansAndArcminutes)
{
	// 100 gradians = 90 degrees; 60 arcminutes = 1 degree (pure ratio units, no pi of their own beyond degrees').
	EXPECT_NEAR(90.0, delta<gradians<double>>(gradians<double>(100)).to<degrees<double>>().value(), kGeneralWrapperTol);
	EXPECT_NEAR(1.0, delta<arcminutes<double>>(arcminutes<double>(60)).to<degrees<double>>().value(), kGeneralWrapperTol);
}

TEST(GeneralWrapper, AngleMixedPiDeltaSumReconciles)
{
	// delta 90 deg + delta pi/2 rad = 180 deg (pi reconciliation through the scale-only delta path).
	const auto d = delta<degrees<double>>(degrees<double>(90)) + delta<radians<double>>(radians<double>(units::detail::PI_VAL / 2.0));
	EXPECT_NEAR(180.0, d.value(), kGeneralWrapperTol);
}

TEST(GeneralWrapper, AngleAbsoluteCrossUnitDifferenceReconcilesPi)
{
	const auto d = absolute<degrees<double>>(degrees<double>(180)) - absolute<radians<double>>(radians<double>(units::detail::PI_VAL));
	EXPECT_NEAR(0.0, d.value(), kGeneralWrapperTol);
}

TEST(GeneralWrapper, AngleAbsolutePointMovedByAngularDelta)
{
	// pi rad point moved +90 deg = 3pi/2 rad.
	const auto p = absolute<radians<double>>(radians<double>(units::detail::PI_VAL)) + delta<degrees<double>>(degrees<double>(90));
	EXPECT_NEAR(3.0 * units::detail::PI_VAL / 2.0, p.value(), kGeneralWrapperTol);
}

//======================================================================================================================
//	OTHER DIMENSIONS — proving "general" is genuinely general
//======================================================================================================================

TEST(GeneralWrapper, TimeEpochMinusEpochIsDuration)
{
	const auto d = absolute<seconds<double>>(seconds<double>(100)) - absolute<seconds<double>>(seconds<double>(40));
	EXPECT_DOUBLE_EQ(60.0, d.value());
	// cross-unit: 2 minutes epoch - 30 s epoch = 90 s, expressed in minutes.
	const auto d2 = absolute<minutes<double>>(minutes<double>(2)) - absolute<seconds<double>>(seconds<double>(30));
	EXPECT_NEAR(1.5, d2.value(), kGeneralWrapperTol);
}

TEST(GeneralWrapper, MassPointArithmetic)
{
	const auto d = absolute<kilograms<double>>(kilograms<double>(5)) - absolute<grams<double>>(grams<double>(500));
	EXPECT_NEAR(4.5, d.value(), kGeneralWrapperTol);
}

TEST(GeneralWrapper, VelocityCompoundUnitWraps)
{
	const auto d = absolute<meters_per_second<double>>(meters_per_second<double>(10)) - absolute<meters_per_second<double>>(meters_per_second<double>(4));
	EXPECT_DOUBLE_EQ(6.0, d.value());
	const auto dv = delta<meters_per_second<double>>(meters_per_second<double>(10)).to<kilometers_per_hour<double>>();
	EXPECT_NEAR(36.0, dv.value(), kGeneralWrapperTol);
}

TEST(GeneralWrapper, PressureGaugeStyleDifference)
{
	// absolute (bar) minus reference (bar) = gauge-style delta.
	const auto d = absolute<pascals<double>>(pascals<double>(101325)) - absolute<pascals<double>>(pascals<double>(101000));
	EXPECT_DOUBLE_EQ(325.0, d.value());
}

TEST(GeneralWrapper, TorqueDerivedNamedUnitWraps)
{
	const auto d = absolute<newton_meters<double>>(newton_meters<double>(10)) - absolute<newton_meters<double>>(newton_meters<double>(4));
	EXPECT_DOUBLE_EQ(6.0, d.value());
}

TEST(GeneralWrapper, DimensionlessWrappersAreSane)
{
	const auto sum = absolute<dimensionless<double>>(dimensionless<double>(5)) + delta<dimensionless<double>>(dimensionless<double>(3));
	EXPECT_DOUBLE_EQ(8.0, sum.value());
	const auto diff = absolute<dimensionless<double>>(dimensionless<double>(5)) - absolute<dimensionless<double>>(dimensionless<double>(2));
	EXPECT_DOUBLE_EQ(3.0, diff.value());
}

//======================================================================================================================
//	SCALAR MULTIPLICATION
//======================================================================================================================

TEST(GeneralWrapper, DeltaScaledByScalar)
{
	const auto a = delta<meters<double>>(meters<double>(4)) * 3;
	static_assert(std::is_same_v<std::remove_const_t<decltype(a)>, delta<meters<double>>>, "delta * scalar -> delta");
	EXPECT_DOUBLE_EQ(12.0, a.value());
	// commutative form.
	const auto b = 2.5 * delta<meters<double>>(meters<double>(4));
	EXPECT_DOUBLE_EQ(10.0, b.value());
}

//======================================================================================================================
//	INTEGER UNDERLYING
//======================================================================================================================

TEST(GeneralWrapper, IntegerPointArithmetic)
{
	const auto d = absolute<meters<int>>(meters<int>(5)) - absolute<meters<int>>(meters<int>(3));
	static_assert(std::is_same_v<std::remove_const_t<decltype(d)>, delta<meters<int>>>, "int point diff -> int delta");
	EXPECT_EQ(2, d.value());
	const auto p = absolute<meters<int>>(meters<int>(5)) + delta<meters<int>>(meters<int>(3));
	EXPECT_EQ(8, p.value());
}

TEST(GeneralWrapper, IntegerLosslessCrossUnitDeltaConversion)
{
	// km -> m is lossless (x1000), so this compiles and is exact.
	const auto m = delta<kilometers<int>>(kilometers<int>(2)).to<meters<int>>();
	static_assert(std::is_same_v<std::remove_const_t<decltype(m)>, delta<meters<int>>>, "int delta km->m");
	EXPECT_EQ(2000, m.value());
}

TEST(GeneralWrapper, IntegerDeltaScaledByInteger)
{
	const auto a = delta<meters<int>>(meters<int>(7)) * 3;
	static_assert(std::is_same_v<std::remove_const_t<decltype(a)>, delta<meters<int>>>, "int delta * int -> int delta");
	EXPECT_EQ(21, a.value());
}

//======================================================================================================================
//	MIXED UNDERLYING — result carries the LHS unit type
//======================================================================================================================

TEST(GeneralWrapper, MixedUnderlyingPointDifferenceWidening)
{
	// lhs is double, rhs int: rhs widens losslessly into double; result carries the lhs unit (delta<meters<double>>).
	const auto d = absolute<meters<double>>(meters<double>(5.5)) - absolute<meters<int>>(meters<int>(2));
	static_assert(std::is_same_v<std::remove_const_t<decltype(d)>, delta<meters<double>>>, "mixed -> lhs unit (double)");
	EXPECT_DOUBLE_EQ(3.5, d.value());
}

//======================================================================================================================
//	COMPARISONS
//======================================================================================================================

TEST(GeneralWrapper, PointComparisonsCrossUnit)
{
	EXPECT_TRUE(absolute<meters<double>>(meters<double>(1)) < absolute<feet<double>>(feet<double>(10)));
	EXPECT_TRUE(absolute<meters<double>>(meters<double>(1)) == absolute<centimeters<double>>(centimeters<double>(100)));
	EXPECT_FALSE(absolute<meters<double>>(meters<double>(2)) == absolute<meters<double>>(meters<double>(3)));
}

TEST(GeneralWrapper, DeltaComparisonsCrossUnitScaleOnly)
{
	EXPECT_TRUE(delta<kilometers<double>>(kilometers<double>(1)) == delta<meters<double>>(meters<double>(1000)));
	EXPECT_TRUE(delta<meters<double>>(meters<double>(1)) < delta<kilometers<double>>(kilometers<double>(1)));
}

TEST(GeneralWrapper, ComparisonOrderingCategories)
{
	// floating point -> partial_ordering; integer -> strong_ordering.
	const auto ordf = absolute<meters<double>>(meters<double>(1)) <=> absolute<meters<double>>(meters<double>(2));
	static_assert(std::is_same_v<std::remove_const_t<decltype(ordf)>, std::partial_ordering>, "double point -> partial_ordering");
	EXPECT_EQ(std::partial_ordering::less, ordf);

	const auto ordi = delta<meters<int>>(meters<int>(2)) <=> delta<meters<int>>(meters<int>(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(ordi)>, std::strong_ordering>, "int delta -> strong_ordering");
	EXPECT_EQ(std::strong_ordering::greater, ordi);
}

TEST(GeneralWrapper, NanPointComparesUnordered)
{
	const auto ord = absolute<meters<double>>(meters<double>(std::nan(""))) <=> absolute<meters<double>>(meters<double>(1));
	EXPECT_EQ(std::partial_ordering::unordered, ord);
}

//======================================================================================================================
//	TYPE ALGEBRA — what MUST be ill-formed (via the detection idiom)
//======================================================================================================================

TEST(GeneralWrapper, ForbiddenOperationsAreIllFormed)
{
	using Abs = absolute<meters<double>>;
	using Del = delta<meters<double>>;

	// point + point has no meaning (sum of two datums) — MUST be ill-formed.
	static_assert(!genWrap_hasAdd<Abs, Abs>::value, "absolute + absolute is ill-formed");
	// a point does not scale — only a delta does.
	static_assert(!genWrap_hasMul<Abs, double>::value, "absolute * scalar is ill-formed");
	// delta * delta is not a length (would be an area); no such operator.
	static_assert(!genWrap_hasMul<Del, Del>::value, "delta * delta is ill-formed");
	// delta - point is not defined (point - delta is; delta - point is not).
	static_assert(!genWrap_hasSub<Del, Abs>::value, "delta - absolute is ill-formed");

	// the sanctioned operations ARE formable.
	static_assert(genWrap_hasSub<Abs, Abs>::value, "absolute - absolute is well-formed");
	static_assert(genWrap_hasAdd<Abs, Del>::value, "absolute + delta is well-formed");
	static_assert(genWrap_hasAdd<Del, Abs>::value, "delta + absolute is well-formed");
	static_assert(genWrap_hasAdd<Del, Del>::value, "delta + delta is well-formed");
	static_assert(genWrap_hasSub<Del, Del>::value, "delta - delta is well-formed");
	static_assert(genWrap_hasMul<Del, double>::value, "delta * scalar is well-formed");
	SUCCEED();
}

//======================================================================================================================
//	CONSTEXPR — the wrappers are fully usable at compile time
//======================================================================================================================

TEST(GeneralWrapper, ConstexprAlgebra)
{
	constexpr auto d = absolute<meters<double>>(meters<double>(5)) - absolute<meters<double>>(meters<double>(3));
	static_assert(d.value() == 2.0, "constexpr point - point");
	constexpr auto p = absolute<meters<double>>(meters<double>(5)) + delta<meters<double>>(meters<double>(3));
	static_assert(p.value() == 8.0, "constexpr point + delta");
	constexpr auto s = delta<meters<double>>(meters<double>(4)) * 3;
	static_assert(s.value() == 12.0, "constexpr delta * scalar");
	constexpr bool eq = delta<kilometers<double>>(kilometers<double>(1)) == delta<meters<double>>(meters<double>(1000));
	static_assert(eq, "constexpr delta comparison");
	constexpr auto conv = delta<meters<int>>(meters<int>(2)).to<centimeters<int>>();
	static_assert(conv.value() == 200, "constexpr int delta conversion");
	SUCCEED();
}

//======================================================================================================================
//	WARTS — cases that PIN the current (surprising) behavior. A future fix flips these deliberately.
//======================================================================================================================

TEST(GeneralWrapper, IntegerDeltaScaledByFractionPromotes)
{
	// A `delta<U<int>> * <double>` PROMOTES exactly as the wrapped plain unit does — the wrapper is never less
	// precise than the type it wraps. (This previously truncated to an int; the delta scalar operator now
	// delegates to the wrapped unit's own promoting `operator*`.)
	const auto scaled = delta<meters<int>>(meters<int>(3)) * 2.5;
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(scaled.raw())>, double>, "delta<int> * double promotes to double");
	EXPECT_DOUBLE_EQ(7.5, static_cast<double>(scaled.value()));

	// It matches the wrapped plain unit exactly.
	const auto promoted = meters<int>(3) * 2.5;
	EXPECT_DOUBLE_EQ(static_cast<double>(promoted.value()), static_cast<double>(scaled.value()));

	// An integer factor keeps the integer underlying (like the plain unit).
	const auto intScaled = delta<meters<int>>(meters<int>(3)) * 4;
	static_assert(std::is_same_v<std::remove_cvref_t<decltype(intScaled.raw())>, int>, "delta<int> * int stays int");
	EXPECT_EQ(12, intScaled.value());
}

TEST(GeneralWrapper, CrossUnitIntegerReconcilesToFinerUnit)
{
	// Cross-unit wrapper operators reconcile to the COMMON (finer) unit, so neither operand order narrows an
	// integer — both directions compile and compute correctly. (This previously hard-errored in one order,
	// where the result was forced into the coarser lhs unit.)
	static_assert(genWrap_hasAdd<delta<meters<int>>, delta<kilometers<int>>>::value, "m + km delta add");
	static_assert(genWrap_hasAdd<delta<kilometers<int>>, delta<meters<int>>>::value, "km + m delta add");

	const auto a = delta<meters<int>>(meters<int>(500)) + delta<kilometers<int>>(kilometers<int>(1));
	EXPECT_EQ(1500, a.value());    // in meters (the finer common unit)

	const auto b = delta<kilometers<int>>(kilometers<int>(1)) + delta<meters<int>>(meters<int>(500));
	EXPECT_EQ(1500, b.value());    // same result regardless of operand order

	// Comparison across the two integer units is likewise well-formed both ways.
	EXPECT_TRUE((delta<kilometers<int>>(kilometers<int>(1)) == delta<meters<int>>(meters<int>(1000))));
	EXPECT_TRUE((delta<meters<int>>(meters<int>(1000)) == delta<kilometers<int>>(kilometers<int>(1))));
}

TEST(GeneralWrapper, OffsetFreeUnitBehavesIdenticallyUnderAbsoluteAndDelta)
{
	// For a unit that is ALREADY offset-free (meters), wrapping it as absolute vs delta yields identical numeric
	// conversion results — the whole premise of the "general" (zero-datum) slice. (Contrast: temperatures differ.)
	constexpr double meterValue = 12.0;
	const auto asPoint = absolute<meters<double>>(meters<double>(meterValue)).to<feet<double>>();
	const auto asDelta = delta<meters<double>>(meters<double>(meterValue)).to<feet<double>>();
	EXPECT_DOUBLE_EQ(asPoint.value(), asDelta.value());
}
