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
/// @file       wrapperTest_safety.h
/// @author     Nic Holthaus
/// @date       8/16/2026
/// @copyright  (c) 2026 STR. The use of this software is subject to the terms and conditions outlined
///             in the LICENSE file. By using this software, the user agrees to be bound by the terms and
///             conditions set forth in the LICENSE file.
//
// ---------------------------------------------------------------------------------------------------------------------
//
/// @brief      Adversarial type-safety tests for the `absolute<U>` / `delta<U>` affine wrappers.
/// @details    Covers the safety boundaries where warts hide: point+point and wrong-dimension rejection,
///             plain/wrapper mixing, implicit-conversion rejection, trivial-copyability (the zero-cost claim),
///             common_type / hashing / cmath / formatting behavior, and constexpr usability. Rejection cases are
///             proven WITHOUT a compile-fail file by asserting the operation is SFINAE-undetectable, so the whole
///             surface is exercised in one compiling translation unit.
//
// ---------------------------------------------------------------------------------------------------------------------

#pragma once

#include <functional>
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <type_traits>
#include <units.h>

using namespace units;

namespace
{
	// Concrete unit types under test. Fully qualified and prefixed to sidestep the single-letter globals that
	// UNIT_ADD_CONSTANT injects into `units` (e.g. `C` = coulombs, `ct` = carats), which a `using namespace units`
	// makes ambiguous with a short alias.
	using Meter     = units::length::meters<double>;
	using Foot      = units::length::feet<double>;
	using MeterInt  = units::length::meters<int>;
	using Second    = units::time::seconds<double>;
	using Cel       = units::temperature::celsius<double>;
	using Fah       = units::temperature::fahrenheit<double>;
	using Kel       = units::temperature::kelvin<double>;

	//------------------------------------------------------------------------------------------------------------------
	//	SFINAE detection idiom
	//------------------------------------------------------------------------------------------------------------------
	// A generic "is this expression well-formed?" probe: `Detected<Op, Args...>` is true iff `Op<Args...>` names a
	// type. Applying it to `decltype`-based aliases lets a negative (should-not-compile) case be asserted as a
	// COMPILING `static_assert(!Detected<...>)`, so no separate compile-fail harness is needed.

	template<class, template<class...> class Op, class... Args>
	struct detector : std::false_type
	{
	};
	template<template<class...> class Op, class... Args>
	struct detector<std::void_t<Op<Args...>>, Op, Args...> : std::true_type
	{
	};
	template<template<class...> class Op, class... Args>
	inline constexpr bool Detected = detector<void, Op, Args...>::value;

	// Expression aliases (each is well-formed only if the operator/function resolves).
	template<class A, class B> using AddExpr    = decltype(std::declval<A>() + std::declval<B>());
	template<class A, class B> using SubExpr    = decltype(std::declval<A>() - std::declval<B>());
	template<class A, class B> using MulExpr    = decltype(std::declval<A>() * std::declval<B>());
	template<class A, class B> using DivExpr    = decltype(std::declval<A>() / std::declval<B>());
	template<class A, class B> using EqExpr     = decltype(std::declval<A>() == std::declval<B>());
	template<class A, class B> using PlusEqExpr = decltype(std::declval<A&>() += std::declval<B>());
	template<class A>          using NegExpr    = decltype(-std::declval<A>());
	template<class A>          using StreamExpr = decltype(std::declval<std::ostringstream&>() << std::declval<A>());
	template<class A>          using ToStringExpr = decltype(units::to_string(std::declval<A>()));
	template<class A>          using AbsExpr    = decltype(units::abs(std::declval<A>()));
	template<class A>          using SqrtExpr   = decltype(units::sqrt(std::declval<A>()));
	template<class A, class B> using FmaxExpr   = decltype(units::fmax(std::declval<A>(), std::declval<B>()));
	template<class A>          using HashExpr   = decltype(std::hash<A>{}(std::declval<A>()));
	template<class A, class B> using CommonType = std::common_type_t<A, B>;
} // namespace

//======================================================================================================================
//	TRIVIAL-COPYABILITY & LAYOUT  (the zero-cost claim)
//======================================================================================================================
// The wrappers must add ZERO representation overhead: same size as the wrapped unit, trivially copyable, standard
// layout. A regression here silently defeats the library's zero-cost promise, so these are compile-time invariants.

static_assert(std::is_trivially_copyable_v<absolute<Cel>>, "absolute<affine> must stay trivially copyable");
static_assert(std::is_trivially_copyable_v<delta<Cel>>, "delta<affine> must stay trivially copyable");
static_assert(std::is_trivially_copyable_v<absolute<Meter>>, "absolute<linear> must stay trivially copyable");
static_assert(std::is_trivially_copyable_v<delta<Meter>>, "delta<linear> must stay trivially copyable");
static_assert(std::is_standard_layout_v<absolute<Cel>>, "absolute must stay standard layout");
static_assert(std::is_standard_layout_v<delta<Cel>>, "delta must stay standard layout");
static_assert(sizeof(absolute<Cel>) == sizeof(Cel), "absolute must be the size of its wrapped unit");
static_assert(sizeof(delta<Cel>) == sizeof(Cel), "delta must be the size of its wrapped unit");
static_assert(std::is_trivially_destructible_v<absolute<Cel>>, "absolute must be trivially destructible");
static_assert(std::is_nothrow_default_constructible_v<absolute<Cel>>, "absolute default-ctor must be noexcept");
static_assert(std::is_nothrow_move_constructible_v<absolute<Cel>>, "absolute move-ctor must be noexcept");

//======================================================================================================================
//	EXPLICIT CONSTRUCTION / NO IMPLICIT CONVERSION
//======================================================================================================================
// The unit->wrapper constructor is `explicit`: a bare unit must NOT silently become a point or a delta (that is the
// exact point-vs-delta ambiguity the wrappers exist to prevent), and a wrapper must not silently decay to a unit.

static_assert(std::is_constructible_v<absolute<Cel>, Cel>, "absolute must be explicitly constructible from its unit");
static_assert(std::is_constructible_v<delta<Cel>, Cel>, "delta must be explicitly constructible from its unit");
static_assert(!std::is_convertible_v<Cel, absolute<Cel>>, "unit must NOT implicitly convert to a point");
static_assert(!std::is_convertible_v<Cel, delta<Cel>>, "unit must NOT implicitly convert to a delta");
static_assert(!std::is_convertible_v<absolute<Cel>, Cel>, "a point must NOT implicitly decay to a bare unit");
static_assert(!std::is_convertible_v<delta<Cel>, Cel>, "a delta must NOT implicitly decay to a bare unit");
// A point and a delta of the same unit are DISTINCT types with no cross-conversion (the whole reason they exist).
static_assert(!std::is_convertible_v<absolute<Cel>, delta<Cel>>, "point must NOT convert to delta");
static_assert(!std::is_convertible_v<delta<Cel>, absolute<Cel>>, "delta must NOT convert to point");
static_assert(!std::is_constructible_v<absolute<Cel>, delta<Cel>>, "point must NOT be constructible from a delta");
static_assert(!std::is_constructible_v<delta<Cel>, absolute<Cel>>, "delta must NOT be constructible from a point");

//======================================================================================================================
//	POINT + POINT AND WRONG-DIMENSION ARITHMETIC ARE REJECTED
//======================================================================================================================
// The affine algebra: absolute+absolute is meaningless, and any cross-dimension operation is a category error. All of
// these must be SFINAE-undetectable (soft-fail), which keeps the operators well-behaved in overload resolution AND
// proves the compile-time rejection without a compile-fail file.

static_assert(!Detected<AddExpr, absolute<Cel>, absolute<Cel>>, "absolute + absolute (point + point) must not compile");
static_assert(!Detected<AddExpr, absolute<Fah>, absolute<Cel>>, "point + point must be rejected even cross-unit");
static_assert(!Detected<AddExpr, absolute<Meter>, delta<Second>>, "point + delta of wrong dimension must not compile");
static_assert(!Detected<SubExpr, absolute<Meter>, absolute<Second>>, "point - point of wrong dimension must not compile");
static_assert(!Detected<AddExpr, delta<Meter>, delta<Second>>, "delta + delta of wrong dimension must not compile");
static_assert(!Detected<SubExpr, delta<Meter>, delta<Second>>, "delta - delta of wrong dimension must not compile");
static_assert(!Detected<AddExpr, absolute<Cel>, delta<Meter>>, "point + delta of wrong dimension must not compile");
static_assert(!Detected<SubExpr, absolute<Cel>, delta<Second>>, "point - delta of wrong dimension must not compile");
// Comparisons: cross-dimension compare is a category error and must be a SOFT fail (SFINAE), not a hard error.
static_assert(!Detected<EqExpr, absolute<Meter>, absolute<Second>>, "cross-dimension point compare must not compile");
static_assert(!Detected<EqExpr, delta<Meter>, delta<Second>>, "cross-dimension delta compare must not compile");
// A point and a delta never compare or add to each other directly (they are different roles).
static_assert(!Detected<EqExpr, absolute<Cel>, delta<Cel>>, "point vs delta comparison must not compile");
// += / -= only accept a delta on the rhs, never another point.
static_assert(!Detected<PlusEqExpr, absolute<Cel>, absolute<Cel>>, "absolute += absolute must not compile");
static_assert(Detected<PlusEqExpr, absolute<Cel>, delta<Cel>>, "absolute += delta MUST compile");

//======================================================================================================================
//	PLAIN-UNIT / WRAPPER MIXING IS REJECTED  (a plain unit is role-ambiguous)
//======================================================================================================================
// A bare `meters` is ambiguous — is it a point or an amount? Mixing it with a wrapper must NOT compile in either
// order; the user must state the role by wrapping. This is the correct, non-footgun behavior.

static_assert(!Detected<AddExpr, absolute<Meter>, Meter>, "point + plain-unit must not compile (role-ambiguous)");
static_assert(!Detected<AddExpr, Meter, absolute<Meter>>, "plain-unit + point must not compile (role-ambiguous)");
static_assert(!Detected<SubExpr, absolute<Meter>, Meter>, "point - plain-unit must not compile (role-ambiguous)");
static_assert(!Detected<AddExpr, delta<Meter>, Meter>, "delta + plain-unit must not compile (role-ambiguous)");
static_assert(!Detected<AddExpr, Meter, delta<Meter>>, "plain-unit + delta must not compile (role-ambiguous)");
static_assert(!Detected<EqExpr, absolute<Meter>, Meter>, "point == plain-unit must not compile (role-ambiguous)");
static_assert(!Detected<PlusEqExpr, absolute<Meter>, Meter>, "point += plain-unit must not compile (role-ambiguous)");

//======================================================================================================================
//	MULTIPLICATION / DIVISION / NEGATION SURFACE  (documented gaps)
//======================================================================================================================
// A delta scales by a bare scalar (both orders). It intentionally does NOT multiply by another delta (a delta*delta
// would change dimension and lose the affine role), and — a usability GAP flagged for the author — there is no
// `delta / scalar` and no unary `-delta`. These asserts pin the CURRENT surface so a future addition is a conscious
// change, not an accident.
static_assert(Detected<MulExpr, delta<Meter>, double>, "delta * scalar MUST compile");
static_assert(Detected<MulExpr, double, delta<Meter>>, "scalar * delta MUST compile");
static_assert(!Detected<MulExpr, delta<Meter>, delta<Meter>>, "delta * delta must not compile");
static_assert(Detected<DivExpr, delta<Meter>, double>, "delta / scalar MUST compile");
static_assert(Detected<NegExpr, delta<Meter>>, "unary -delta MUST compile");
static_assert(!Detected<SubExpr, delta<Meter>, absolute<Meter>>, "delta - point is not provided (only point - delta)");

//======================================================================================================================
//	LIBRARY-FUNCTION / FORMATTING BEHAVIOR  (documented non-support)
//======================================================================================================================
// The wrappers are unformatted and outside the cmath/hash surface. These are pinned as CURRENT behavior; a user who
// expects `std::cout << point` or `units::abs(delta)` to work should reach through `.quantity()`.
static_assert(!Detected<StreamExpr, absolute<Cel>>, "operator<< on a wrapper is not provided (unformatted)");
static_assert(!Detected<StreamExpr, delta<Cel>>, "operator<< on a wrapper is not provided (unformatted)");
static_assert(!Detected<ToStringExpr, absolute<Cel>>, "units::to_string on a wrapper is not provided");
static_assert(!Detected<AbsExpr, delta<Meter>>, "units::abs on a wrapper is not provided");
static_assert(!Detected<SqrtExpr, delta<Meter>>, "units::sqrt on a wrapper is not provided");
static_assert(!Detected<FmaxExpr, delta<Meter>, delta<Meter>>, "units::fmax on a wrapper is not provided");
static_assert(!Detected<HashExpr, absolute<Cel>>, "std::hash on a wrapper is not provided");
// common_type of two DIFFERENT wrappers is SFINAE-empty (no common type), even for same-dimension units. The
// identity case (same type) is defined by the standard and is the type itself.
static_assert(std::is_same_v<CommonType<absolute<Cel>, absolute<Cel>>, absolute<Cel>>, "common_type of identical wrappers is the type");
static_assert(!Detected<CommonType, absolute<Cel>, absolute<Fah>>, "common_type across differing units is undefined (SFINAE-empty)");
static_assert(!Detected<CommonType, delta<Cel>, delta<Fah>>, "common_type across differing delta units is undefined");
static_assert(!Detected<CommonType, absolute<Cel>, delta<Cel>>, "common_type of point vs delta is undefined");

//======================================================================================================================
//	CONSTEXPR USABILITY
//======================================================================================================================
// Every wrapper operation is usable in a constant expression (the operators and .to<> are constexpr).
constexpr absolute<Cel> kConstA{Cel(20.0)};
constexpr absolute<Cel> kConstB{Cel(5.0)};
constexpr delta<Cel>    kConstDiff = kConstA - kConstB;
static_assert(kConstDiff.value() == 15.0, "point - point is constexpr");
constexpr absolute<Cel> kConstMoved = kConstA + delta<Cel>{Cel(10.0)};
static_assert(kConstMoved.value() == 30.0, "point + delta is constexpr");
constexpr absolute<Cel> kConstMovedDown = kConstA - delta<Cel>{Cel(10.0)};
static_assert(kConstMovedDown.value() == 10.0, "point - delta is constexpr");
constexpr delta<Cel> kConstScaled = kConstDiff * 2.0;
static_assert(kConstScaled.value() == 30.0, "delta * scalar is constexpr");
static_assert(kConstA == kConstA, "point == point is constexpr");
static_assert(kConstB < kConstA, "point <=> point is constexpr");
static_assert(kConstDiff == delta<Cel>{Cel(15.0)}, "delta == delta is constexpr");

//======================================================================================================================
//	RUNTIME BEHAVIOR
//======================================================================================================================

//----------------------------------------------------------------------------------------------------------------------
//	TEST: pointMinusPointIsADelta
//----------------------------------------------------------------------------------------------------------------------
TEST(WrapperSafety, pointMinusPointIsADelta)
{
	const absolute<Cel> a{Cel(20.0)};
	const absolute<Cel> b{Cel(5.0)};
	const delta<Cel>    d = a - b;
	EXPECT_NEAR(15.0, d.value(), 5.0e-12);
	// The delta is truly a delta type (no datum re-applied on conversion).
	static_assert(std::is_same_v<decltype(d), const delta<Cel>>, "point - point yields a delta");
}

//----------------------------------------------------------------------------------------------------------------------
//	TEST: crossUnitPointSubtractionAppliesDatum
//----------------------------------------------------------------------------------------------------------------------
TEST(WrapperSafety, crossUnitPointSubtractionAppliesDatum)
{
	// 100 degC and 373.15 K are the same point; their difference is exactly zero.
	const absolute<Cel> c{Cel(100.0)};
	const absolute<Kel> k{Kel(373.15)};
	const auto          d = c - k;    // delta in the common unit
	EXPECT_NEAR(0.0, d.value(), 5.0e-11);
	// 100 degC point minus 32 degF point (= 0 degC) is a 100 degC delta.
	const absolute<Fah> f{Fah(32.0)};
	EXPECT_NEAR(100.0, (c - f).value(), 5.0e-11);
}

//----------------------------------------------------------------------------------------------------------------------
//	TEST: pointPlusDeltaMovesThepoint
//----------------------------------------------------------------------------------------------------------------------
TEST(WrapperSafety, pointPlusDeltaMovesThePoint)
{
	const absolute<Cel> a{Cel(20.0)};
	EXPECT_NEAR(30.0, (a + delta<Cel>{Cel(10.0)}).value(), 5.0e-12);
	EXPECT_NEAR(10.0, (a - delta<Cel>{Cel(10.0)}).value(), 5.0e-12);
	// delta + point is the commutative form and yields the SAME point (not a wart).
	EXPECT_NEAR(30.0, (delta<Cel>{Cel(10.0)} + a).value(), 5.0e-12);
}

//----------------------------------------------------------------------------------------------------------------------
//	TEST: pointPlusMixedUnitDeltaScalesOnly
//----------------------------------------------------------------------------------------------------------------------
TEST(WrapperSafety, pointPlusMixedUnitDeltaScalesOnly)
{
	// An 18 degF delta is a 10 degC delta (degree SIZE, no datum). Adding it to 20 degC yields 30 degC.
	const absolute<Cel> a{Cel(20.0)};
	const delta<Fah>    dF{Fah(18.0)};
	EXPECT_NEAR(30.0, (a + dF).value(), 5.0e-11);
	// A length example with feet: 10 m minus ~1 m expressed in feet.
	const absolute<Meter> pm{Meter(10.0)};
	const absolute<Foot>  pf{Foot(3.280839895013123)};    // 1 m
	EXPECT_NEAR(9.0, (pm - pf).value(), 5.0e-9);
}

//----------------------------------------------------------------------------------------------------------------------
//	TEST: deltaConversionIsScaleOnly
//----------------------------------------------------------------------------------------------------------------------
TEST(WrapperSafety, deltaConversionIsScaleOnly)
{
	// A 10 degC delta is an 18 degF delta (degree size), NOT an absolute 50 degF.
	const delta<Cel> d{Cel(10.0)};
	EXPECT_NEAR(18.0, d.to<Fah>().value(), 5.0e-11);
	// Round-trips back to itself.
	EXPECT_NEAR(10.0, d.to<Fah>().to<Cel>().value(), 5.0e-11);
	// Self-conversion is the identity.
	EXPECT_NEAR(10.0, d.to<Cel>().value(), 5.0e-12);
}

//----------------------------------------------------------------------------------------------------------------------
//	TEST: absoluteConversionAppliesDatum
//----------------------------------------------------------------------------------------------------------------------
TEST(WrapperSafety, absoluteConversionAppliesDatum)
{
	// 0 degC as an absolute point is 32 degF (the datum IS applied on a point conversion).
	const absolute<Cel> a{Cel(0.0)};
	EXPECT_NEAR(32.0, a.to<Fah>().value(), 5.0e-11);
	EXPECT_NEAR(273.15, a.to<Kel>().value(), 5.0e-11);
}

//----------------------------------------------------------------------------------------------------------------------
//	TEST: deltaArithmeticAndScaling
//----------------------------------------------------------------------------------------------------------------------
TEST(WrapperSafety, deltaArithmeticAndScaling)
{
	const delta<Cel> a{Cel(10.0)};
	const delta<Cel> b{Cel(4.0)};
	EXPECT_NEAR(14.0, (a + b).value(), 5.0e-12);
	EXPECT_NEAR(6.0, (a - b).value(), 5.0e-12);
	EXPECT_NEAR(30.0, (a * 3.0).value(), 5.0e-12);
	EXPECT_NEAR(20.0, (2.0 * a).value(), 5.0e-12);
	// A cross-unit delta sum reconciles by scale (5 degC delta + 9 degF delta[=5 degC] = 10 degC delta),
	// expressed in the common (finer) unit; compare via a conversion to celsius-degrees.
	const delta<Fah> f{Fah(9.0)};
	EXPECT_NEAR(10.0, (delta<Cel>{Cel(5.0)} + f).template to<Cel>().value(), 5.0e-11);
}

//----------------------------------------------------------------------------------------------------------------------
//	TEST: compoundAssignmentMovesInPlace
//----------------------------------------------------------------------------------------------------------------------
TEST(WrapperSafety, compoundAssignmentMovesInPlace)
{
	absolute<Cel> a{Cel(0.0)};
	a += delta<Cel>{Cel(5.0)};
	EXPECT_NEAR(5.0, a.value(), 5.0e-12);
	a -= delta<Cel>{Cel(2.0)};
	EXPECT_NEAR(3.0, a.value(), 5.0e-12);
	// += with a mixed-unit delta reconciles by scale (18 degF delta == 10 degC delta).
	a += delta<Fah>{Fah(18.0)};
	EXPECT_NEAR(13.0, a.value(), 5.0e-11);
	// The type stays an absolute point.
	static_assert(std::is_same_v<decltype(a), absolute<Cel>>, "compound assignment keeps the point type");
}

//----------------------------------------------------------------------------------------------------------------------
//	TEST: comparisonsReconcileUnits
//----------------------------------------------------------------------------------------------------------------------
TEST(WrapperSafety, comparisonsReconcileUnits)
{
	const absolute<Cel> a{Cel(20.0)};
	const absolute<Cel> b{Cel(5.0)};
	EXPECT_TRUE(a == a);
	EXPECT_FALSE(a == b);
	EXPECT_TRUE(b < a);
	EXPECT_TRUE(a > b);
	// Cross-unit point comparison reconciles: 0 degC == 32 degF as points.
	EXPECT_TRUE((absolute<Cel>{Cel(0.0)} == absolute<Fah>{Fah(32.0)}));
	// Cross-unit delta comparison reconciles by scale: 10 degC delta == 18 degF delta.
	EXPECT_TRUE((delta<Cel>{Cel(10.0)} == delta<Fah>{Fah(18.0)}));
	EXPECT_FALSE((delta<Cel>{Cel(10.0)} == delta<Fah>{Fah(10.0)}));
}

//----------------------------------------------------------------------------------------------------------------------
//	TEST: accessorsExposeUnderlying
//----------------------------------------------------------------------------------------------------------------------
TEST(WrapperSafety, accessorsExposeUnderlying)
{
	const absolute<Cel> a{Cel(25.0)};
	EXPECT_NEAR(25.0, a.value(), 5.0e-12);
	EXPECT_NEAR(25.0, a.raw(), 5.0e-12);
	EXPECT_NEAR(25.0, a.quantity().value(), 5.0e-12);
	static_assert(std::is_same_v<decltype(a.quantity()), const Cel&>, "quantity() returns the wrapped unit by const-ref");
	// A user reaches the formatted/cmath surface THROUGH quantity() (the wrappers themselves are unformatted).
	std::ostringstream os;
	os << a.quantity();
	EXPECT_FALSE(os.str().empty());
}

//----------------------------------------------------------------------------------------------------------------------
//	TEST: linearUnitPointAndDeltaCoincideNumerically
//----------------------------------------------------------------------------------------------------------------------
TEST(WrapperSafety, linearUnitPointAndDeltaCoincideNumerically)
{
	// For a non-affine unit the datum is zero, so a point and a delta convert identically.
	const absolute<Meter> ap{Meter(5.0)};
	const delta<Meter>    dp{Meter(5.0)};
	EXPECT_NEAR(ap.to<Foot>().value(), dp.to<Foot>().value(), 5.0e-9);
	EXPECT_NEAR(16.4041994750656, ap.to<Foot>().value(), 5.0e-9);
}

//----------------------------------------------------------------------------------------------------------------------
//	TEST: integerDeltaScalingPromotesLikeThePlainUnit
//----------------------------------------------------------------------------------------------------------------------
TEST(WrapperSafety, integerDeltaScalingPromotesLikeThePlainUnit)
{
	// A delta scales EXACTLY as its wrapped unit does: `meters<int>(7) * 2.5` promotes to `meters<double>(17.5)`,
	// so `delta<meters<int>> * 2.5` promotes to `delta<meters<double>>(17.5)` — the wrapper never loses precision
	// the plain unit would keep. Scaling by an integer keeps the integer underlying type.
	const delta<MeterInt> d{MeterInt(7)};
	const auto            byInt = d * 2;
	EXPECT_EQ(14, byInt.value());
	static_assert(std::is_same_v<decltype(byInt), const delta<MeterInt>>, "int * int delta stays integer-underlying");

	const auto byDouble = d * 2.5;
	EXPECT_NEAR(17.5, byDouble.value(), 5.0e-12);
	static_assert(std::is_same_v<decltype(byDouble), const delta<units::length::meters<double>>>,
		"a fractional scale promotes the delta's underlying type to the wrapped unit's promoted type");
}

//----------------------------------------------------------------------------------------------------------------------
//	TEST: defaultConstructedIsZero
//----------------------------------------------------------------------------------------------------------------------
TEST(WrapperSafety, defaultConstructedIsZero)
{
	const absolute<Cel> a{};
	const delta<Cel>    d{};
	EXPECT_NEAR(0.0, a.value(), 5.0e-12);
	EXPECT_NEAR(0.0, d.value(), 5.0e-12);
}
