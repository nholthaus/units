// This translation unit exercises the deliberately-lossy compound-assignment path — scaling an
// integer-backed unit by a floating-point factor — which narrows and, by design, emits the compiler's
// float-to-integer conversion diagnostic (-Wfloat-conversion on GCC/Clang, C4244 on MSVC). The narrowing
// is the intended, documented behavior (a warning, never a hard failure), so the diagnostic is disabled
// for this file alone; the rest of the suite in main.cpp stays warning-clean and the diagnostic itself is
// proven by the test/errorMessages/cases entries.
#ifdef _MSC_VER
#pragma warning(disable : 4244)
#elif defined(__clang__)
#pragma clang diagnostic ignored "-Wfloat-conversion"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <gtest/gtest.h>
#include <type_traits>
#include <units.h>

namespace
{
	class LossyCompoundAssign : public ::testing::Test
	{
	};

	// A floating-point factor scaling an integer unit truncates toward zero and keeps the integer
	// underlying type — the value contract of the lossy multiply path.
	TEST_F(LossyCompoundAssign, multiplyIntegerUnitByFloatingFactorTruncates)
	{
		units::meters<int> len(10);
		len *= 2.0; // exact-valued factor: 20, no truncation, still narrows through double
		EXPECT_EQ(20, len.value());
		static_assert(std::is_same_v<typename decltype(len)::underlying_type, int>);

		len *= 1.5; // 20 * 1.5 == 30.0 -> 30 (exact)
		EXPECT_EQ(30, len.value());

		units::meters<int> odd(10);
		odd *= 2.5; // 25.0 -> 25 (exact)
		EXPECT_EQ(25, odd.value());

		units::meters<int> trunc(3);
		trunc *= 1.5; // 4.5 -> 4 (truncated toward zero)
		EXPECT_EQ(4, trunc.value());

		units::meters<int> neg(-3);
		neg *= 1.5; // -4.5 -> -4 (truncated toward zero)
		EXPECT_EQ(-4, neg.value());
	}

	// The same lossy narrowing applies through a floating-point dimensionless factor (e.g. a percent).
	TEST_F(LossyCompoundAssign, multiplyIntegerUnitByDimensionlessFloatingFactorTruncates)
	{
		using namespace units::literals;
		units::meters<int> len(10);
		len *= 250.0_pct; // scale by 2.5 -> 25
		EXPECT_EQ(25, len.value());
		static_assert(std::is_same_v<typename decltype(len)::underlying_type, int>);

		units::dimensionless<int> d(10);
		d *= 2.5; // 25.0 -> 25
		EXPECT_EQ(25, d.value());
		static_assert(std::is_same_v<typename decltype(d)::underlying_type, int>);
	}

	// The lossy divide path narrows identically.
	TEST_F(LossyCompoundAssign, divideIntegerUnitByFloatingFactorTruncates)
	{
		units::meters<int> len(30);
		len /= 2.0; // 15.0 -> 15 (exact)
		EXPECT_EQ(15, len.value());
		static_assert(std::is_same_v<typename decltype(len)::underlying_type, int>);

		units::meters<int> trunc(10);
		trunc /= 3.0; // 3.333... -> 3 (truncated)
		EXPECT_EQ(3, trunc.value());

		units::meters<int> neg(-10);
		neg /= 3.0; // -3.333... -> -3 (truncated toward zero)
		EXPECT_EQ(-3, neg.value());

		units::dimensionless<int> d(10);
		d /= 4.0; // 2.5 -> 2 (truncated)
		EXPECT_EQ(2, d.value());
	}
}
