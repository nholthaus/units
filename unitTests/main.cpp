#include <units.h>
#include <gtest/gtest.h>

using namespace units;
using namespace units::length;

namespace {

	class UnitTest : public ::testing::Test {
	protected:

		UnitTest() {};
		virtual ~UnitTest() {};
		virtual void SetUp() {};
		virtual void TearDown() {};
	};
}

TEST_F(UnitTest, isRatio)
{
	EXPECT_TRUE(units::is_ratio<std::ratio<1>>::value);
	EXPECT_FALSE(units::is_ratio<double>::value);
}

TEST_F(UnitTest, isUnit)
{
	EXPECT_FALSE(units::is_unit<std::ratio<1>>::value);
	EXPECT_FALSE(units::is_unit<double>::value);
	EXPECT_TRUE(units::is_unit<meters>::value);
	EXPECT_TRUE(units::is_unit<feet>::value);
}

TEST_F(UnitTest, lengthConversionFactors)
{
	EXPECT_NEAR(1.0, meters::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(0.3048, feet::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(1609.344, miles::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(0.0254,inches::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(1852.0,nauticalMiles::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(149597870700,astronicalUnits::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(9460730472580800,lightyears::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(3.0856776e16, parsec::conversionFactor(), 5.0e-20);

}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}