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
}

#endif // UNITS_HAVE_EIGEN
