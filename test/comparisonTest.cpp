#include <cstdint>
#include <gtest/gtest.h>
#include <limits>
#include <units/angle.h>
#include <units/length.h>
#include <units/temperature.h>
#include <units/time.h>

namespace
{
	template<class Ratio = std::ratio<1>, class Pi = std::ratio<0>, class Datum = std::ratio<0>, class Rep = std::int64_t>
	using Length = units::unit<units::conversion_factor<Ratio, units::dimension::length, Pi, Datum>, Rep>;

	TEST(IntegerComparison, affineCoordinates)
	{
		const units::celsius<int> freezing(0);
		const units::kelvin<int>  belowFreezing(273);
		EXPECT_FALSE(freezing == units::kelvin<int>(0));
		EXPECT_TRUE(freezing == units::fahrenheit<int>(32));
		EXPECT_TRUE(freezing > belowFreezing);
		EXPECT_TRUE(freezing >= belowFreezing);
		EXPECT_FALSE(freezing < belowFreezing);
		EXPECT_FALSE(freezing <= belowFreezing);
		EXPECT_TRUE(freezing != belowFreezing);
		EXPECT_TRUE(belowFreezing < freezing);
		EXPECT_TRUE(belowFreezing <= freezing);
		EXPECT_FALSE(belowFreezing > freezing);
		EXPECT_FALSE(belowFreezing >= freezing);
		EXPECT_TRUE(units::celsius<int>(-40) == units::fahrenheit<int>(-40));
	}

	TEST(IntegerComparison, fractionalDatumRemainsExactAtLargeMagnitudes)
	{
		constexpr auto value = std::numeric_limits<std::int64_t>::max() - 1;
		using QuarterOffset  = Length<std::ratio<1>, std::ratio<0>, std::ratio<1, 4>>;
		EXPECT_TRUE(QuarterOffset(value) > Length<>(value));
		EXPECT_FALSE(QuarterOffset(value) == Length<>(value));
		EXPECT_TRUE(Length<>(value) < QuarterOffset(value));
	}

	TEST(IntegerComparison, piCoordinates)
	{
		EXPECT_TRUE(units::degrees<int>(57) < units::radians<int>(1));
		EXPECT_TRUE(units::degrees<int>(180) > units::radians<int>(3));
		EXPECT_FALSE(units::degrees<int>(180) == units::radians<int>(1));
		using Pi        = Length<std::ratio<1>, std::ratio<1>>;
		using ShiftedPi = Length<std::ratio<1>, std::ratio<1>, std::ratio<1>>;
		EXPECT_TRUE(Pi(2) > ShiftedPi(1)); // 2*pi > pi + 1
	}

	TEST(IntegerComparison, commonPiTermsCancelBeforeRounding)
	{
		using Pi             = Length<std::ratio<1>, std::ratio<1>, std::ratio<0>, std::uint64_t>;
		using ShiftedPi      = Length<std::ratio<1>, std::ratio<1>, std::ratio<1, 4>, std::uint64_t>;
		constexpr auto value = std::numeric_limits<std::uint64_t>::max() - 1;
		EXPECT_TRUE(ShiftedPi(value) > Pi(value));
		EXPECT_TRUE(Pi(value) > ShiftedPi(value - 1)); // difference is pi - 1/4
		EXPECT_TRUE(ShiftedPi(value - 1) < Pi(value));
		constexpr auto boundary = std::uint64_t(1) << 63;
		EXPECT_TRUE(Pi(boundary) > ShiftedPi(boundary - 1));
	}

	TEST(IntegerComparison, preservesRangeAndSignedness)
	{
		EXPECT_TRUE(units::microseconds<int>(1) < units::seconds<int>(3600));
		constexpr auto adjacent = (std::int64_t(1) << 53);
		static_assert(Length<>(adjacent) < Length<>(adjacent + 1));
		EXPECT_TRUE(Length<>(adjacent) != Length<>(adjacent + 1));
		using Unsigned         = Length<std::ratio<1>, std::ratio<0>, std::ratio<0>, std::uint64_t>;
		constexpr auto maximum = std::numeric_limits<std::uint64_t>::max();
		EXPECT_TRUE(Unsigned(maximum - 1) < Unsigned(maximum));
		EXPECT_TRUE(Length<>(-1) < Unsigned(maximum));
		EXPECT_FALSE(Length<>(-1) == Unsigned(maximum));
		EXPECT_TRUE(Unsigned(maximum) > Length<>(std::numeric_limits<std::int64_t>::max()));
	}

	TEST(IntegerComparison, negativeAndZeroConversionRatios)
	{
		using Reversed         = Length<std::ratio<-1>>;
		using Unsigned         = Length<std::ratio<1>, std::ratio<0>, std::ratio<0>, std::uint64_t>;
		constexpr auto minimum = std::numeric_limits<std::int64_t>::min();
		static_assert(Reversed(2) < Reversed(1));
		EXPECT_TRUE(Reversed(minimum) == Unsigned(std::uint64_t(1) << 63));
		EXPECT_TRUE(Reversed(minimum) > Unsigned((std::uint64_t(1) << 63) - 1));
		using Constant = Length<std::ratio<0>, std::ratio<0>, std::ratio<3, 2>>;
		EXPECT_TRUE(Constant(minimum) == Constant(0));
		EXPECT_TRUE(Constant(minimum) > Length<>(1));
		EXPECT_TRUE(Constant(minimum) < Length<>(2));
	}

	// These original ratios are valid, but their common denominator cannot be represented by std::ratio.
	// The two scale cross-products differ by exactly one, so rounding the scales first would lose the order.
	constexpr auto ratioMaximum = std::numeric_limits<std::intmax_t>::max();
	using LargerRatio           = std::ratio<ratioMaximum - 1, ratioMaximum - 2>;
	using SmallerRatio          = std::ratio<ratioMaximum, ratioMaximum - 1>;

	TEST(IntegerComparison, extremeRatiosAndDatumsCancelExactly)
	{
		using Left           = Length<LargerRatio, std::ratio<0>, SmallerRatio, std::uint64_t>;
		using Right          = Length<SmallerRatio, std::ratio<0>, LargerRatio, std::uint64_t>;
		constexpr auto value = std::numeric_limits<std::uint64_t>::max();
		// Difference: (value - 1) * (LargerRatio - SmallerRatio).
		static_assert(Left(1) == Right(1));
		EXPECT_TRUE(Left(value) > Right(value));
		EXPECT_TRUE(Right(value) < Left(value));
		EXPECT_TRUE(Left(0) < Right(0));
		using NegativeLeft  = Length<LargerRatio, std::ratio<0>, std::ratio<-SmallerRatio::num, SmallerRatio::den>>;
		using NegativeRight = Length<SmallerRatio, std::ratio<0>, std::ratio<-LargerRatio::num, LargerRatio::den>>;
		static_assert(NegativeLeft(-1) == NegativeRight(-1));
		EXPECT_TRUE(NegativeLeft(std::numeric_limits<std::int64_t>::min()) < NegativeRight(std::numeric_limits<std::int64_t>::min()));
	}

	TEST(IntegerComparison, affineSignCombinationsMatchIntegerCoordinates)
	{
		using Left  = Length<std::ratio<2, 3>, std::ratio<0>, std::ratio<-5, 7>>;
		using Right = Length<std::ratio<-3, 5>, std::ratio<0>, std::ratio<7, 11>>;
		for (int x = -4; x <= 4; ++x)
		{
			for (int y = -4; y <= 4; ++y)
			{
				SCOPED_TRACE(::testing::Message() << "x=" << x << ", y=" << y);
				// Both physical values expressed in exact multiples of 1/1155.
				const int left  = 770 * x - 825;
				const int right = -693 * y + 735;
				EXPECT_EQ(left < right, Left(x) < Right(y));
				EXPECT_EQ(left <= right, Left(x) <= Right(y));
				EXPECT_EQ(left > right, Left(x) > Right(y));
				EXPECT_EQ(left >= right, Left(x) >= Right(y));
				EXPECT_EQ(left == right, Left(x) == Right(y));
				EXPECT_EQ(left != right, Left(x) != Right(y));
			}
		}
	}

	TEST(IntegerComparison, commonPiAndDatumCancelExactly)
	{
		using TwicePi        = Length<std::ratio<2>, std::ratio<1>, std::ratio<1, 4>, std::uint64_t>;
		using Pi             = Length<std::ratio<1>, std::ratio<1>, std::ratio<1, 4>, std::uint64_t>;
		constexpr auto value = (std::uint64_t(1) << 63) - 1;
		static_assert(TwicePi(value) == Pi(2 * value));
		EXPECT_TRUE(TwicePi(value) > Pi(2 * value - 1));
		EXPECT_TRUE(Pi(2 * value - 1) < TwicePi(value));
	}

	struct IntegerOnlyScale
	{
		template<class T>
			requires std::same_as<T, int>
		static constexpr int linearize(T value)
		{
			return value * 2;
		}
		template<class T>
			requires std::same_as<T, int>
		static constexpr int scale(T value)
		{
			return value / 2;
		}
	};

	TEST(IntegerComparison, usesAlreadyLinearizedCustomScale)
	{
		using Linear  = units::conversion_factor<std::ratio<1>, units::dimension::length>;
		using Pi      = units::conversion_factor<std::ratio<1>, units::dimension::length, std::ratio<1>>;
		using Shifted = units::conversion_factor<std::ratio<1>, units::dimension::length, std::ratio<0>, std::ratio<1, 4>>;
		static_assert(units::NumericalScaleType<IntegerOnlyScale, int>);
		static_assert(!units::NumericalScaleType<IntegerOnlyScale, long double>);
		EXPECT_TRUE((units::unit<Linear, int, IntegerOnlyScale>(3) == Length<>(6)));
		EXPECT_TRUE((units::unit<Shifted, int, IntegerOnlyScale>(3) > Length<>(6)));
		EXPECT_TRUE((units::unit<Pi, int, IntegerOnlyScale>(1) > Length<>(6)));
	}

	TEST(IntegerComparison, fractionalAndNegativePiExponents)
	{
		using RootPi    = Length<std::ratio<1>, std::ratio<1, 2>>;
		using InversePi = Length<std::ratio<1>, std::ratio<-1>>;
		EXPECT_TRUE(RootPi(1) > Length<>(1));
		EXPECT_TRUE(RootPi(1) < Length<>(2));
		EXPECT_TRUE((InversePi(1) < Length<std::ratio<1, 2>>(1)));
		EXPECT_TRUE(InversePi(1) > Length<>(0));
	}

	TEST(IntegerComparison, zeroConversionRatioDoesNotEvaluatePiPower)
	{
		using Constant = Length<std::ratio<0>, std::ratio<10000>, std::ratio<3, 2>>;
		// The ratio makes the pi-dependent term zero even when the pi power exceeds floating range.
		EXPECT_TRUE((Constant(7) == Length<std::ratio<1, 2>>(3)));
		EXPECT_TRUE(Constant(-7) > Length<>(1));
		EXPECT_TRUE(Constant(7) < Length<>(2));
	}

	TEST(IntegerComparison, piPrecisionIsIndependentOfDefaultStorage)
	{
		// The test target also builds with UNIT_LIB_DEFAULT_TYPE=int: pi must not become 3.
		EXPECT_TRUE(units::degrees<int>(180) > units::radians<int>(3));
		if constexpr (std::numeric_limits<long double>::digits >= 64)
		{
			// A known rational approximation just below pi; a double-precision pi constant reverses this order.
			using Pi = Length<std::ratio<1>, std::ratio<1>>;
			EXPECT_TRUE(Pi(78'256'779) > Length<>(245'850'922));
		}
	}

	TEST(IntegerComparison, smallIntegralRepresentations)
	{
		using Boolean   = Length<std::ratio<1>, std::ratio<0>, std::ratio<0>, bool>;
		using Character = Length<std::ratio<1>, std::ratio<0>, std::ratio<0>, char>;
		static_assert(Boolean(false) < Boolean(true));
		static_assert(Character(1) < Character(2));
		EXPECT_TRUE(Boolean(true) == Character(1));
	}

	TEST(IntegerComparison, floatingOperandsKeepTheirComparisonContract)
	{
		EXPECT_TRUE(units::meters<double>(1.0) == units::centimeters<int>(100));
		EXPECT_TRUE(units::meters<double>(1.0) > units::centimeters<int>(99));
		const units::meters<double> nan(std::numeric_limits<double>::quiet_NaN());
		EXPECT_FALSE(nan < units::meters<int>(1));
		EXPECT_FALSE(nan >= units::meters<int>(1));
		EXPECT_FALSE(nan == units::meters<int>(1));
	}

#if defined(__SIZEOF_INT128__) && !defined(__STRICT_ANSI__)
	TEST(IntegerComparison, extendedIntegerRepresentationsDoNotNarrow)
	{
		using U           = unsigned __int128;
		using S           = __int128;
		using Plain       = Length<std::ratio<1>, std::ratio<0>, std::ratio<0>, U>;
		using Triple      = Length<std::ratio<3>, std::ratio<0>, std::ratio<0>, U>;
		constexpr U value = U(1) << 100;
		static_assert(Triple(value) == Plain(3 * value));
		EXPECT_TRUE(Triple(value) > Plain(3 * value - 1));
		using Reversed = Length<std::ratio<-1>, std::ratio<0>, std::ratio<0>, S>;
		EXPECT_TRUE(Reversed(std::numeric_limits<S>::min()) == Plain(U(1) << 127));
		using Left  = Length<LargerRatio, std::ratio<0>, SmallerRatio, U>;
		using Right = Length<SmallerRatio, std::ratio<0>, LargerRatio, U>;
		EXPECT_TRUE(Left(std::numeric_limits<U>::max()) > Right(std::numeric_limits<U>::max()));
	}
#endif
} // namespace
