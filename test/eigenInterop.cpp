// Tests for the optional Eigen interoperability header (units/eigen.h). The whole file is guarded by
// __has_include(<Eigen/Core>): where Eigen is available (the CI Linux jobs install it) the fixtures run; where
// it is not, the file compiles to nothing so the suite still builds. units carries no dependency on Eigen.
#if defined __has_include
#if __has_include(<Eigen/Core>)
#define UNITS_HAVE_EIGEN 1
#endif
#endif

#include <gtest/gtest.h>

#ifdef UNITS_HAVE_EIGEN

#include <Eigen/Core>
#include <cmath>
#include <type_traits>
#include <units.h>

using namespace units;
using namespace units::literals;

namespace
{
	class EigenInterop : public ::testing::Test
	{
	};

	using Vector3m  = Eigen::Matrix<meters<double>, 3, 1>;
	using Vector3s  = Eigen::Matrix<seconds<double>, 3, 1>;

	// The scalar-trait shim makes a units type a legal Eigen scalar.
	TEST_F(EigenInterop, unitIsAValidScalar)
	{
		static_assert(std::is_same_v<Eigen::NumTraits<meters<double>>::Real, meters<double>>);
		static_assert(std::is_same_v<Eigen::NumTraits<meters<double>>::Literal, double>);
		static_assert(Eigen::NumTraits<meters<int>>::IsInteger == 1);
		static_assert(Eigen::NumTraits<meters<double>>::IsInteger == 0);
		static_assert(Eigen::NumTraits<meters<double>>::IsSigned == 1);
		// A vector of units is default-constructible and holds the unit type.
		static_assert(std::is_same_v<Vector3m::Scalar, meters<double>>);
	}

	TEST_F(EigenInterop, constructionAndCoefficientAccess)
	{
		Vector3m v;
		v << meters<double>(1.0), meters<double>(2.0), meters<double>(3.0);
		EXPECT_DOUBLE_EQ(1.0, v(0).value());
		EXPECT_DOUBLE_EQ(2.0, v(1).value());
		EXPECT_DOUBLE_EQ(3.0, v(2).value());
		EXPECT_EQ(3, v.rows());
		EXPECT_EQ(1, v.cols());
	}

	TEST_F(EigenInterop, additionAndSubtractionStayInDimension)
	{
		Vector3m a;
		a << 1.0_m, 2.0_m, 3.0_m;
		Vector3m b;
		b << 10.0_m, 20.0_m, 30.0_m;

		Vector3m sum = a + b;
		EXPECT_DOUBLE_EQ(11.0, sum(0).value());
		EXPECT_DOUBLE_EQ(22.0, sum(1).value());
		EXPECT_DOUBLE_EQ(33.0, sum(2).value());

		Vector3m diff = b - a;
		EXPECT_DOUBLE_EQ(9.0, diff(0).value());
		EXPECT_DOUBLE_EQ(18.0, diff(1).value());
		EXPECT_DOUBLE_EQ(27.0, diff(2).value());

		static_assert(std::is_same_v<decltype(sum)::Scalar, meters<double>>);
	}

	TEST_F(EigenInterop, scalingByAPlainScalarBothSides)
	{
		Vector3m a;
		a << 1.0_m, 2.0_m, 4.0_m;

		Vector3m right = a * 2.0;
		Vector3m left  = 2.0 * a;
		EXPECT_DOUBLE_EQ(2.0, right(0).value());
		EXPECT_DOUBLE_EQ(4.0, right(1).value());
		EXPECT_DOUBLE_EQ(8.0, right(2).value());
		EXPECT_DOUBLE_EQ(right(0).value(), left(0).value());
		EXPECT_DOUBLE_EQ(right(1).value(), left(1).value());
		EXPECT_DOUBLE_EQ(right(2).value(), left(2).value());

		Vector3m halved = a / 2.0;
		EXPECT_DOUBLE_EQ(0.5, halved(0).value());
		EXPECT_DOUBLE_EQ(1.0, halved(1).value());
		EXPECT_DOUBLE_EQ(2.0, halved(2).value());
		static_assert(std::is_same_v<decltype(right)::Scalar, meters<double>>);
	}

	TEST_F(EigenInterop, sumReductionStaysInDimension)
	{
		Vector3m a;
		a << 1.0_m, 2.0_m, 3.0_m;
		meters<double> total = a.sum();
		EXPECT_DOUBLE_EQ(6.0, total.value());
		static_assert(std::is_same_v<decltype(a.sum()), meters<double>>);
	}

	TEST_F(EigenInterop, blockAndMapViews)
	{
		Vector3m a;
		a << 1.0_m, 2.0_m, 3.0_m;
		Eigen::Matrix<meters<double>, 2, 1> head = a.head<2>();
		EXPECT_DOUBLE_EQ(1.0, head(0).value());
		EXPECT_DOUBLE_EQ(2.0, head(1).value());

		meters<double>          storage[3] = {5.0_m, 6.0_m, 7.0_m};
		Eigen::Map<Vector3m>    mapped(storage);
		EXPECT_DOUBLE_EQ(6.0, mapped(1).value());
	}

	TEST_F(EigenInterop, castToDifferentUnderlyingType)
	{
		Eigen::Matrix<meters<int>, 3, 1> ints;
		ints << meters<int>(1), meters<int>(2), meters<int>(3);
		Eigen::Matrix<meters<double>, 3, 1> doubles = ints.cast<meters<double>>();
		EXPECT_DOUBLE_EQ(2.0, doubles(1).value());
		static_assert(std::is_same_v<decltype(doubles)::Scalar, meters<double>>);
	}

	// The helpers whose result changes dimension.

	TEST_F(EigenInterop, unitDotYieldsProductDimension)
	{
		Vector3m a;
		a << 3.0_m, 4.0_m, 0.0_m;
		auto squared = unit_dot(a, a);
		EXPECT_DOUBLE_EQ(25.0, squared.value()); // 9 + 16
		static_assert(traits::is_area_unit_v<decltype(squared)>);

		// Cross-dimension dot: meters . seconds is (meter*second), not either operand.
		Vector3s t;
		t << 1.0_s, 1.0_s, 1.0_s;
		auto mixed = unit_dot(a, t);
		EXPECT_DOUBLE_EQ(7.0, mixed.value()); // 3 + 4 + 0, in meter-seconds
		static_assert(!traits::is_area_unit_v<decltype(mixed)>);
		static_assert(!std::is_same_v<decltype(mixed), meters<double>>);
	}

	TEST_F(EigenInterop, unitSquaredNormMatchesDot)
	{
		Vector3m a;
		a << 1.0_m, 2.0_m, 2.0_m;
		auto sq = unit_squared_norm(a);
		EXPECT_DOUBLE_EQ(9.0, sq.value()); // 1 + 4 + 4
		static_assert(traits::is_area_unit_v<decltype(sq)>);
	}

	TEST_F(EigenInterop, unitNormReturnsOriginalDimension)
	{
		Vector3m a;
		a << 3.0_m, 4.0_m, 0.0_m;
		meters<double> n = unit_norm(a);
		EXPECT_DOUBLE_EQ(5.0, n.value());
		static_assert(std::is_same_v<decltype(unit_norm(a)), meters<double>>);

		Vector3m unitAxis;
		unitAxis << 0.0_m, 0.0_m, 1.0_m;
		EXPECT_DOUBLE_EQ(1.0, unit_norm(unitAxis).value());
	}

	TEST_F(EigenInterop, unitNormalizedIsDimensionlessUnitVector)
	{
		Vector3m a;
		a << 3.0_m, 4.0_m, 0.0_m;
		Eigen::Matrix<double, 3, 1> dir = unit_normalized(a);
		EXPECT_DOUBLE_EQ(0.6, dir(0));
		EXPECT_DOUBLE_EQ(0.8, dir(1));
		EXPECT_DOUBLE_EQ(0.0, dir(2));
		EXPECT_NEAR(1.0, dir.norm(), 1e-12); // direction has unit length
		static_assert(std::is_same_v<decltype(dir)::Scalar, double>);
	}

	TEST_F(EigenInterop, unitCrossYieldsProductDimension)
	{
		Vector3m x;
		x << 1.0_m, 0.0_m, 0.0_m;
		Vector3m y;
		y << 0.0_m, 1.0_m, 0.0_m;
		auto z = unit_cross(x, y); // x cross y = +z, magnitude in m^2
		EXPECT_DOUBLE_EQ(0.0, z(0).value());
		EXPECT_DOUBLE_EQ(0.0, z(1).value());
		EXPECT_DOUBLE_EQ(1.0, z(2).value());
		static_assert(traits::is_area_unit_v<decltype(z)::Scalar>);

		// anticommutativity: y cross x = -z
		auto negz = unit_cross(y, x);
		EXPECT_DOUBLE_EQ(-1.0, negz(2).value());
	}

	TEST_F(EigenInterop, unitTransformRotatesPreservingDimension)
	{
		Vector3m pos;
		pos << 1.0_m, 0.0_m, 0.0_m;

		// 90-degree rotation about z: (1,0,0) -> (0,1,0)
		Eigen::Matrix<double, 3, 3> rotation;
		rotation << 0.0, -1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0;

		Eigen::Matrix<meters<double>, 3, 1> rotated = unit_transform(rotation, pos);
		EXPECT_NEAR(0.0, rotated(0).value(), 1e-12);
		EXPECT_NEAR(1.0, rotated(1).value(), 1e-12);
		EXPECT_NEAR(0.0, rotated(2).value(), 1e-12);
		static_assert(std::is_same_v<decltype(rotated)::Scalar, meters<double>>);

		// A transform's magnitude is preserved by a pure rotation.
		EXPECT_NEAR(unit_norm(pos).value(), unit_norm(rotated).value(), 1e-12);
	}

	TEST_F(EigenInterop, worksForOtherDimensions)
	{
		Eigen::Matrix<seconds<double>, 3, 1> t;
		t << 2.0_s, 3.0_s, 6.0_s;
		EXPECT_DOUBLE_EQ(11.0, t.sum().value());
		EXPECT_DOUBLE_EQ(7.0, unit_norm(t).value()); // sqrt(4+9+36) = 7
		static_assert(std::is_same_v<decltype(unit_norm(t)), seconds<double>>);
	}

	// Regression guards. Each fails on the pre-fix header and passes on the fixed one.

	// A dynamically-sized vector is normalized without out-of-bounds indexing (issue #406): the output is
	// sized from the runtime length, not a zero compile-time size.
	TEST_F(EigenInterop, unitNormalizedHandlesDynamicSize)
	{
		Eigen::Matrix<meters<double>, Eigen::Dynamic, 1> v(3);
		v << 3.0_m, 4.0_m, 0.0_m;

		auto dir = unit_normalized(v);
		EXPECT_EQ(3, dir.size());
		EXPECT_DOUBLE_EQ(0.6, dir(0));
		EXPECT_DOUBLE_EQ(0.8, dir(1));
		EXPECT_DOUBLE_EQ(0.0, dir(2));
		EXPECT_NEAR(1.0, dir.norm(), 1e-12);
	}

	// An integral underlying type is floating-point promoted before division (issue #407), so a direction is
	// the true ratio rather than a truncated (1,1).
	TEST_F(EigenInterop, unitNormalizedPromotesIntegralScalar)
	{
		Eigen::Matrix<meters<int>, 2, 1> v;
		v << meters<int>(1), meters<int>(1);

		auto dir = unit_normalized(v);
		EXPECT_NEAR(0.70710678, dir(0), 1e-8);
		EXPECT_NEAR(0.70710678, dir(1), 1e-8);
		static_assert(std::is_floating_point_v<decltype(dir)::Scalar>);
	}

	// The norm of a ratio-scaled dimensionless unit reads in the unit's own raw scale (issue #392): a single
	// 50% component has a raw norm of 50, and a (30%,40%) vector has a raw norm of 50.
	TEST_F(EigenInterop, unitNormRatioScaledDimensionless)
	{
		Eigen::Matrix<concentration::percent<double>, 1, 1> one;
		one << concentration::percent<double>(50);
		EXPECT_DOUBLE_EQ(50.0, unit_norm(one).raw());

		Eigen::Matrix<concentration::percent<double>, 2, 1> two;
		two << concentration::percent<double>(30), concentration::percent<double>(40);
		EXPECT_DOUBLE_EQ(50.0, unit_norm(two).raw()); // sqrt(30^2 + 40^2) on the percent scale
	}

	// A dynamically-sized transform sizes its intermediates from the runtime length (issue #406) instead of
	// indexing a zero-length matrix.
	TEST_F(EigenInterop, unitTransformHandlesDynamicSize)
	{
		Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> rotation(3, 3);
		rotation << 0.0, -1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0;

		Eigen::Matrix<meters<double>, Eigen::Dynamic, 1> pos(3);
		pos << 1.0_m, 0.0_m, 0.0_m;

		auto rotated = unit_transform(rotation, pos);
		EXPECT_EQ(3, rotated.size());
		EXPECT_NEAR(0.0, rotated(0).value(), 1e-12);
		EXPECT_NEAR(1.0, rotated(1).value(), 1e-12);
		EXPECT_NEAR(0.0, rotated(2).value(), 1e-12);
	}

	// Broad coverage of paths the fixed-size floating-point tests miss.

	// Dynamic-size dot, squared-norm, and norm match their fixed-size equivalents.
	TEST_F(EigenInterop, dynamicSizeMatchesFixedSize)
	{
		Vector3m fixed;
		fixed << 3.0_m, 4.0_m, 0.0_m;

		Eigen::Matrix<meters<double>, Eigen::Dynamic, 1> dyn(3);
		dyn << 3.0_m, 4.0_m, 0.0_m;

		EXPECT_DOUBLE_EQ(unit_dot(fixed, fixed).value(), unit_dot(dyn, dyn).value());
		EXPECT_DOUBLE_EQ(unit_squared_norm(fixed).value(), unit_squared_norm(dyn).value());
		EXPECT_DOUBLE_EQ(unit_norm(fixed).value(), unit_norm(dyn).value());
		EXPECT_DOUBLE_EQ(25.0, unit_squared_norm(dyn).value());
		EXPECT_DOUBLE_EQ(5.0, unit_norm(dyn).value());
	}

	// The norm and direction helpers work on a non-length dimension at dynamic size.
	TEST_F(EigenInterop, normAndNormalizedForSecondsAtDynamicSize)
	{
		Eigen::Matrix<seconds<double>, Eigen::Dynamic, 1> t(3);
		t << 2.0_s, 3.0_s, 6.0_s;

		seconds<double> n = unit_norm(t);
		EXPECT_DOUBLE_EQ(7.0, n.value()); // sqrt(4 + 9 + 36) = 7
		static_assert(std::is_same_v<decltype(unit_norm(t)), seconds<double>>);

		auto dir = unit_normalized(t);
		EXPECT_EQ(3, dir.size());
		EXPECT_NEAR(2.0 / 7.0, dir(0), 1e-12);
		EXPECT_NEAR(3.0 / 7.0, dir(1), 1e-12);
		EXPECT_NEAR(6.0 / 7.0, dir(2), 1e-12);
		EXPECT_NEAR(1.0, dir.norm(), 1e-12);
	}

	// An integral underlying type keeps an exact integer result for a dot and squared norm whose value is whole.
	TEST_F(EigenInterop, integralDotAndSquaredNormStayExact)
	{
		Eigen::Matrix<meters<int>, 3, 1> a;
		a << meters<int>(3), meters<int>(4), meters<int>(0);

		auto dot = unit_dot(a, a);
		auto sq  = unit_squared_norm(a);
		EXPECT_EQ(25, dot.value()); // 9 + 16
		EXPECT_EQ(25, sq.value());
		static_assert(std::is_integral_v<decltype(sq)::underlying_type>);
		static_assert(traits::is_area_unit_v<decltype(sq)>);
	}

	// Normalizing an axis-aligned vector returns that axis exactly.
	TEST_F(EigenInterop, unitNormalizedOfSingleAxisIsExact)
	{
		Vector3m axis;
		axis << 0.0_m, 0.0_m, 1.0_m;

		Eigen::Matrix<double, 3, 1> dir = unit_normalized(axis);
		EXPECT_DOUBLE_EQ(0.0, dir(0));
		EXPECT_DOUBLE_EQ(0.0, dir(1));
		EXPECT_DOUBLE_EQ(1.0, dir(2));
	}

	// A non-square transform's result follows the matrix rows, and the dimension is preserved: a 2x3
	// projection of a 3-vector of meters yields a 2-vector of meters.
	TEST_F(EigenInterop, unitTransformWithNonSquareMatrix)
	{
		Eigen::Matrix<double, 2, 3> projection;
		projection << 1.0, 0.0, 0.0, 0.0, 1.0, 0.0;

		Vector3m pos;
		pos << 5.0_m, 7.0_m, 9.0_m;

		auto projected = unit_transform(projection, pos);
		EXPECT_EQ(2, projected.size());
		EXPECT_DOUBLE_EQ(5.0, projected(0).value());
		EXPECT_DOUBLE_EQ(7.0, projected(1).value());
		static_assert(std::is_same_v<decltype(projected)::Scalar, meters<double>>);
	}

	// A dynamically-sized non-square transform sizes its result from the matrix rows.
	TEST_F(EigenInterop, unitTransformDynamicNonSquare)
	{
		Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> matrix(2, 3);
		matrix << 1.0, 1.0, 1.0, 2.0, 0.0, 0.0;

		Eigen::Matrix<meters<double>, Eigen::Dynamic, 1> v(3);
		v << 1.0_m, 2.0_m, 3.0_m;

		auto out = unit_transform(matrix, v);
		EXPECT_EQ(2, out.size());
		EXPECT_DOUBLE_EQ(6.0, out(0).value()); // 1 + 2 + 3
		EXPECT_DOUBLE_EQ(2.0, out(1).value()); // 2 * 1
		static_assert(std::is_same_v<decltype(out)::Scalar, meters<double>>);
	}

	// The cross product on integral meters yields the product dimension and stays anticommutative.
	TEST_F(EigenInterop, unitCrossWithIntegralScalar)
	{
		Eigen::Matrix<meters<int>, 3, 1> x;
		x << meters<int>(1), meters<int>(0), meters<int>(0);
		Eigen::Matrix<meters<int>, 3, 1> y;
		y << meters<int>(0), meters<int>(1), meters<int>(0);

		auto z = unit_cross(x, y); // x cross y = +z
		EXPECT_EQ(0, z(0).value());
		EXPECT_EQ(0, z(1).value());
		EXPECT_EQ(1, z(2).value());
		static_assert(traits::is_area_unit_v<decltype(z)::Scalar>);

		auto negz = unit_cross(y, x); // anticommutativity: y cross x = -z
		EXPECT_EQ(-1, negz(2).value());
	}

	// The norm of a zero vector is zero.
	TEST_F(EigenInterop, unitNormOfZeroVectorIsZero)
	{
		Vector3m zero;
		zero << 0.0_m, 0.0_m, 0.0_m;
		EXPECT_DOUBLE_EQ(0.0, unit_norm(zero).value());
	}

	// The norm of an integral vector whose magnitude is not whole is floating-point promoted, not truncated
	// (issue #407): (1,1) meters has a norm near sqrt(2), not 1.
	TEST_F(EigenInterop, unitNormOfIntegralVectorIsPromoted)
	{
		Eigen::Matrix<meters<int>, 2, 1> v;
		v << meters<int>(1), meters<int>(1);

		auto n = unit_norm(v);
		EXPECT_NEAR(std::sqrt(2.0), n.value(), 1e-12);
		static_assert(std::is_floating_point_v<decltype(n)::underlying_type>);
	}
}


// A quantity measured from an arbitrary origin -- an affine reading, or a decibel level -- breaks Eigen's assumption
// that a coefficient-wise binary operation is `op(T,T) -> T`. The scalar difference of two readings is an offset-free
// AMOUNT, so without naming that amount type in `ScalarBinaryOpTraits` Eigen assigns the difference back into the
// reading coefficient and `(v - w).eval()` on two EQUAL readings reads -273.15 rather than 0.
TEST_F(EigenInterop, affineMatrixDifferenceIsAnAmount)
{
	using units::temperature::celsius;

	Eigen::Matrix<celsius<double>, 3, 1> reading;
	reading << celsius<double>(12.5), celsius<double>(20.5), celsius<double>(37.25);
	Eigen::Matrix<celsius<double>, 3, 1> baseline;
	baseline << celsius<double>(12.5), celsius<double>(10.5), celsius<double>(2.25);

	const auto difference = (reading - baseline).eval();
	EXPECT_NEAR(0.0, difference(0).value(), 5.0e-12);      // equal readings differ by nothing, not by -273.15
	EXPECT_NEAR(10.0, difference(1).value(), 5.0e-12);
	EXPECT_NEAR(35.0, difference(2).value(), 5.0e-12);
	static_assert(!units::traits::is_affine_unit_v<std::decay_t<decltype(difference(0))>>,
		"the difference of two readings is an amount, so its coefficient type carries no datum");

	// and the amount matrix supports the arithmetic a magnitude should
	EXPECT_NEAR(20.0, (difference * 2.0).eval()(1).value(), 5.0e-12);
	EXPECT_NEAR(45.0, difference.sum().value(), 5.0e-12);

	// storing and converting readings is unaffected
	EXPECT_NEAR(20.5, reading(1).value(), 5.0e-12);
	EXPECT_NEAR(54.5, units::temperature::fahrenheit<double>(reading(0)).value(), 5.0e-12);    // 12.5 degC
}

// A matrix operation is available exactly where the same operation on one of its coefficients is: a matrix of affine
// readings scales and reduces in its coefficients' own scale, and a matrix of decibel values does neither, because the
// scalar `dBW * 2.0` does not exist. The Eigen seam is gated on the coefficient's numerical
// scale -- what the scalar operators are gated on -- and cannot be used to launder an operation past them.
TEST_F(EigenInterop, theEigenSeamMatchesTheScalarRule)
{
	using units::temperature::celsius;
	using units::power::dBW;

	static_assert(units::traits::has_arbitrary_origin_v<celsius<double>>);
	static_assert(units::traits::has_arbitrary_origin_v<dBW<double>>);
	static_assert(!units::traits::has_arbitrary_origin_v<units::meters<double>>);
	static_assert(!units::traits::has_arbitrary_origin_v<std::decay_t<decltype(celsius<double>(1) - celsius<double>(0))>>);

	// a matrix of readings scales in the readings' own scale, as `celsius(20.0) * 2.0` does
	Eigen::Matrix<celsius<double>, 3, 1> reading;
	reading << celsius<double>(12.5), celsius<double>(20.5), celsius<double>(37.25);
	EXPECT_NEAR(25.0, (reading * 2.0).eval()(0).value(), 5.0e-12);
	EXPECT_NEAR(10.25, (reading / 2.0).eval()(1).value(), 5.0e-12);
	EXPECT_NEAR(25.0, (2.0 * reading).eval()(0).value(), 5.0e-12);
	static_assert(std::is_same_v<celsius<double>, std::decay_t<decltype((reading * 2.0).eval()(0))>>,
		"scaling a matrix of readings keeps the reading's unit, as the scalar operation does");
	EXPECT_NEAR(70.25, reading.sum().value(), 5.0e-12);
	EXPECT_NEAR(1964.0625, unit_squared_norm(reading).value(), 5.0e-9);        // 12.5^2 + 20.5^2 + 37.25^2
	EXPECT_NEAR(44.31774475, unit_norm(reading).value(), 5.0e-8);

	// NOT compilable for a matrix of decibel values: * 2.0, / 2.0, unit_dot, unit_norm, unit_squared_norm,
	// unit_normalized -- the scalar operations do not exist either. A matrix of LEVELS does difference into gains.
	Eigen::Matrix<dBW<double>, 2, 1> level, referenceLevel;
	level << dBW<double>(20.0), dBW<double>(12.5);
	referenceLevel << dBW<double>(10.0), dBW<double>(12.5);
	const auto gain = (level - referenceLevel).eval();
	EXPECT_NEAR(10.0, gain(0).value(), 5.0e-9);
	EXPECT_NEAR(0.0, gain(1).value(), 5.0e-9);
	static_assert(units::traits::is_dimensionless_unit_v<std::decay_t<decltype(gain(0))>>,
		"the difference of two levels is a dimensionless gain");

	// an ordinary matrix keeps the entire surface
	Eigen::Matrix<units::meters<double>, 3, 1> length;
	length << units::meters<double>(3.0), units::meters<double>(4.0), units::meters<double>(0.0);
	EXPECT_NEAR(5.0, unit_norm(length).value(), 5.0e-12);
	EXPECT_NEAR(25.0, unit_squared_norm(length).value(), 5.0e-12);
	EXPECT_NEAR(25.0, unit_dot(length, length).value(), 5.0e-12);
	EXPECT_NEAR(14.0, (length * 2.0).eval().sum().value(), 5.0e-12);
}

//======================================================================================================================
//	THE HELPERS TAKE ANY EIGEN EXPRESSION, NOT ONLY A CONCRETE MATRIX
//
//	Each helper's parameter is an `Eigen::MatrixBase<Derived>`, so a LAZY expression -- a sum, a difference, a scaled
//	vector -- binds directly and no caller has to `.eval()` first. Every expected number is computed by hand at its
//	own assertion from the two operands, which each test spells out for itself.
//======================================================================================================================

// v - w is (1, 2, 3) - (4, 6, 3) == (-3, -4, 0) metres, whose magnitude is sqrt(9 + 16 + 0) == sqrt(25) == 5 metres;
// the magnitude of a length vector is a length.
TEST(EigenLazyExpression, theMagnitudeOfALazyDifference)
{
	Eigen::Matrix<meters<double>, 3, 1> v, w;
	v << meters<double>(1.0), meters<double>(2.0), meters<double>(3.0);
	w << meters<double>(4.0), meters<double>(6.0), meters<double>(3.0);

	EXPECT_NEAR(5.0, unit_norm(v - w).value(), 5.0e-12);
	static_assert(std::is_same_v<meters<double>, std::decay_t<decltype(unit_norm(v - w))>>,
		"the magnitude of a length vector is a length");
}

// v + w is (1, 2, 3) + (4, 6, 3) == (5, 8, 6) metres, so (v + w) . w is 5*4 + 8*6 + 6*3 == 20 + 48 + 18 == 86 square
// metres; a dot of two length vectors is an area.
TEST(EigenLazyExpression, theDotProductOfALazySum)
{
	Eigen::Matrix<meters<double>, 3, 1> v, w;
	v << meters<double>(1.0), meters<double>(2.0), meters<double>(3.0);
	w << meters<double>(4.0), meters<double>(6.0), meters<double>(3.0);

	EXPECT_NEAR(86.0, unit_dot(v + w, w).value(), 5.0e-12);
	EXPECT_NEAR(86.0, unit_dot(w, v + w).value(), 5.0e-12);
	static_assert(std::is_same_v<units::area::square_meters<double>, std::decay_t<decltype(unit_dot(v + w, w))>>,
		"a dot of two length vectors is an area");
}

// 2 v is 2 * (1, 2, 3) == (2, 4, 6) metres, so its squared magnitude is 4 + 16 + 36 == 56 square metres.
TEST(EigenLazyExpression, theSquaredMagnitudeOfALazyScaling)
{
	Eigen::Matrix<meters<double>, 3, 1> v;
	v << meters<double>(1.0), meters<double>(2.0), meters<double>(3.0);

	EXPECT_NEAR(56.0, unit_squared_norm(2.0 * v).value(), 5.0e-12);
	EXPECT_NEAR(56.0, unit_squared_norm(v * 2.0).value(), 5.0e-12);
	static_assert(std::is_same_v<units::area::square_meters<double>, std::decay_t<decltype(unit_squared_norm(2.0 * v))>>,
		"a squared magnitude is an area");
}

// v + w is (5, 8, 6) metres, whose magnitude is sqrt(25 + 64 + 36) == sqrt(125) == 11.180339887498949, so its
// direction is (5, 8, 6) / sqrt(125) == (0.44721359549995793, 0.71554175279993271, 0.53665631459994945) -- a plain
// dimensionless vector of unit length.
TEST(EigenLazyExpression, theDirectionOfALazySum)
{
	Eigen::Matrix<meters<double>, 3, 1> v, w;
	v << meters<double>(1.0), meters<double>(2.0), meters<double>(3.0);
	w << meters<double>(4.0), meters<double>(6.0), meters<double>(3.0);

	const auto direction = unit_normalized(v + w);
	EXPECT_EQ(3, direction.rows());
	EXPECT_NEAR(0.44721359549995793, direction(0), 5.0e-12);
	EXPECT_NEAR(0.71554175279993271, direction(1), 5.0e-12);
	EXPECT_NEAR(0.53665631459994945, direction(2), 5.0e-12);
	EXPECT_NEAR(5.0 / std::sqrt(125.0), direction(0), 5.0e-12);
	EXPECT_NEAR(8.0 / std::sqrt(125.0), direction(1), 5.0e-12);
	EXPECT_NEAR(6.0 / std::sqrt(125.0), direction(2), 5.0e-12);
	EXPECT_NEAR(1.0, direction.norm(), 5.0e-12);
	static_assert(std::is_same_v<double, std::decay_t<decltype(direction(0))>>, "a direction has no dimension");
}

// v + w is (5, 8, 6) metres, so (v + w) x w with w == (4, 6, 3) is
//     8*3 - 6*6 == 24 - 36 == -12,   6*4 - 5*3 == 24 - 15 == 9,   5*6 - 8*4 == 30 - 32 == -2   (square metres)
TEST(EigenLazyExpression, theCrossProductOfALazySum)
{
	Eigen::Matrix<meters<double>, 3, 1> v, w;
	v << meters<double>(1.0), meters<double>(2.0), meters<double>(3.0);
	w << meters<double>(4.0), meters<double>(6.0), meters<double>(3.0);

	const auto product = unit_cross(v + w, w);
	EXPECT_NEAR(-12.0, product(0).value(), 5.0e-12);
	EXPECT_NEAR(9.0, product(1).value(), 5.0e-12);
	EXPECT_NEAR(-2.0, product(2).value(), 5.0e-12);
	static_assert(std::is_same_v<units::area::square_meters<double>, std::decay_t<decltype(product(0))>>,
		"a cross of two length vectors is an area vector");
}

// A quarter turn about the third axis sends (x, y, z) to (-y, x, z), so it carries v + w == (5, 8, 6) to
// (-8, 5, 6). The transform is dimensionless, so the result keeps the vector's unit.
TEST(EigenLazyExpression, aDimensionlessTransformOfALazySum)
{
	Eigen::Matrix<meters<double>, 3, 1> v, w;
	v << meters<double>(1.0), meters<double>(2.0), meters<double>(3.0);
	w << meters<double>(4.0), meters<double>(6.0), meters<double>(3.0);

	Eigen::Matrix<double, 3, 3> quarterTurn;
	quarterTurn << 0.0, -1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0;

	const auto turned = unit_transform(quarterTurn, v + w);
	EXPECT_NEAR(-8.0, turned(0).value(), 5.0e-12);
	EXPECT_NEAR(5.0, turned(1).value(), 5.0e-12);
	EXPECT_NEAR(6.0, turned(2).value(), 5.0e-12);
	static_assert(std::is_same_v<meters<double>, std::decay_t<decltype(turned(0))>>,
		"a dimensionless transform keeps the vector's unit");
}

// The lazy form reaches a matrix of AFFINE READINGS too, where the scalar trait names the difference's AMOUNT type:
// (30, 20, 10) degC less (26, 17, 10) degC is (4, 3, 0) celsius-degrees, whose squared magnitude is 16 + 9 == 25,
// whose magnitude is 5, and whose direction is (4, 3, 0) / 5 == (0.8, 0.6, 0). The amount carries no datum, so
// nothing re-applies the 273.15 translation on the way out.
TEST(EigenLazyExpression, aLazyDifferenceOfReadingsIsAnAmountVector)
{
	Eigen::Matrix<units::temperature::celsius<double>, 3, 1> reading, reference;
	reading << units::temperature::celsius<double>(30.0), units::temperature::celsius<double>(20.0),
		units::temperature::celsius<double>(10.0);
	reference << units::temperature::celsius<double>(26.0), units::temperature::celsius<double>(17.0),
		units::temperature::celsius<double>(10.0);

	static_assert(!units::traits::is_affine_unit_v<std::decay_t<decltype(unit_norm(reading - reference))>>,
		"the magnitude of a difference of readings carries no datum");
	EXPECT_NEAR(5.0, unit_norm(reading - reference).value(), 5.0e-12);
	EXPECT_NEAR(25.0, unit_squared_norm(reading - reference).value(), 5.0e-12);
	EXPECT_NEAR(25.0, unit_dot(reading - reference, reading - reference).value(), 5.0e-12);

	const auto direction = unit_normalized(reading - reference);
	EXPECT_NEAR(0.8, direction(0), 5.0e-12);
	EXPECT_NEAR(0.6, direction(1), 5.0e-12);
	EXPECT_NEAR(0.0, direction(2), 5.0e-12);
}

// An integral scalar is promoted by `sqrt`, so a magnitude that is a whole number is exact and one that is not is not
// truncated: |(1, 2, 2)| is sqrt(1 + 4 + 4) == sqrt(9) == 3 exactly, and |(1, 1, 1)| is sqrt(3) ==
// 1.7320508075688772.
TEST(EigenLazyExpression, anIntegralScalarIsPromotedByTheMagnitude)
{
	Eigen::Matrix<meters<int>, 3, 1> whole, unitary, origin;
	whole << meters<int>(1), meters<int>(2), meters<int>(2);
	unitary << meters<int>(1), meters<int>(1), meters<int>(1);
	origin << meters<int>(0), meters<int>(0), meters<int>(0);

	EXPECT_NEAR(3.0, unit_norm(whole - origin).value(), 5.0e-12);
	EXPECT_NEAR(1.7320508075688772, unit_norm(unitary - origin).value(), 5.0e-12);
	EXPECT_NEAR(std::sqrt(3.0), unit_norm(unitary - origin).value(), 5.0e-12);
}
#endif // UNITS_HAVE_EIGEN
