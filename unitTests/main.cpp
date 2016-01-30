#include <units.h>
#include <gtest/gtest.h>

using namespace units;
using namespace units::length;
using namespace units::mass;
using namespace units::time;
using namespace units::frequency;
using namespace units::area;
using namespace units::velocity;

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

TEST_F(UnitTest, unitTraits)
{
	bool isntUnit = std::is_same<void, units::unit_traits<double>>::value;
	bool isUnit = std::is_same<void, units::unit_traits<meters>>::value;
	EXPECT_TRUE(units::is_unit<meters>::value);
	EXPECT_TRUE(units::is_unit<feet>::value);
}

TEST_F(UnitTest, areConvertibleUnitsLength)
{
	bool test1 = units::are_convertible_units<meters, meters>::value;
	bool test2 = units::are_convertible_units<meters, astronicalUnits>::value;
	bool test3 = units::are_convertible_units<meters, parsecs>::value;
	
	bool test4 = units::are_convertible_units<meters, meters>::value;
	bool test5 = units::are_convertible_units<astronicalUnits, meters>::value;
	bool test6 = units::are_convertible_units<parsecs, meters>::value;

	bool test7 = units::are_convertible_units<meters, seconds>::value;
	bool test8 = units::are_convertible_units<seconds, meters>::value;

	EXPECT_TRUE(test1);
	EXPECT_TRUE(test2);
	EXPECT_TRUE(test3);

	EXPECT_TRUE(test4);
	EXPECT_TRUE(test5);
	EXPECT_TRUE(test6);

	EXPECT_FALSE(test7);
	EXPECT_FALSE(test8);
}

TEST_F(UnitTest, areConvertibleUnitsTime)
{
	bool shouldBeTrue;
	bool shouldBeFalse;

	shouldBeTrue = units::are_convertible_units<years, weeks>::value;
	EXPECT_TRUE(shouldBeTrue);

	shouldBeFalse = units::are_convertible_units<years, meters>::value;
	EXPECT_FALSE(shouldBeFalse);
}

TEST_F(UnitTest, lengthConversionFactors)
{
	EXPECT_NEAR(0.000000001, nanometers::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(0.000001, micrometers::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(0.001, millimeters::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(0.01, centimeters::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(1000.0, kilometers::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(1.0, meters::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(0.3048, feet::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(1609.344, miles::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(0.0254,inches::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(1852.0,nauticalMiles::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(149597870700.0,astronicalUnits::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(9460730472580800.0,lightyears::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(3.08567758e16, parsec::conversionFactor(), 5.0e7);
}

TEST_F(UnitTest, massConversionFactors)
{
	EXPECT_NEAR(1.0e-3, grams::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(1.0e-9,micrograms::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(1.0e-6,milligrams::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(1.0,kilograms::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(1000.0,metric_tons::conversionFactor(), 5.0e-20);
	EXPECT_NEAR(0.453592,pounds::conversionFactor(), 5.0e-6);
	EXPECT_NEAR(1016.05,imperial_tons::conversionFactor(), 5.0e-3);
	EXPECT_NEAR(907.185,us_tons::conversionFactor(), 5.0e-4);
	EXPECT_NEAR(0.0283495,ounces::conversionFactor(), 5.0e-7);
	EXPECT_NEAR(0.0002,carats::conversionFactor(), 5.0e-6);
}

TEST_F(UnitTest, areaConversionFactors)
{
	EXPECT_NEAR(4046.8564224, acre::conversionFactor(), 5.0e-8);
}

TEST_F(UnitTest, inverseUnits)
{
	using htz = units::inverse<seconds>;
	bool shouldBeTrue = std::is_same<htz, hertz>::value;

	EXPECT_TRUE(shouldBeTrue);
}

TEST_F(UnitTest, baseUnitOf)
{
	using base = units::base_unit_of<years>;
	bool shouldBeTrue = std::is_same<base, category::time_unit>::value;

	EXPECT_TRUE(shouldBeTrue);
}

TEST_F(UnitTest, squared)
{
	using feet_squared = units::squared<feet>;
	EXPECT_NEAR(0.092903, feet_squared::conversionFactor(), 5.0e-7);
}

TEST_F(UnitTest, cubed)
{
	using feet_squared = units::squared<feet>;
	EXPECT_NEAR(0.092903, feet_squared::conversionFactor(), 5.0e-7);
}

TEST_F(UnitTest, compoundUnits)
{
	using acceleration1 = unit<std::ratio<1>, category::acceleration_unit>;
	using acceleration2 = compound_unit<meters, inverse<seconds>, inverse<seconds>>;
	using acceleration3 = unit<std::ratio<1>, base_unit<std::ratio<1>, std::ratio<0>, std::ratio<-2>>>;
	using acceleration4 = compound_unit<meters, inverse<squared<seconds>>>;
	using acceleration5 = compound_unit<meters, squared<inverse<seconds>>>;

	bool areSame12 = std::is_same<acceleration1, acceleration2>::value;
	bool areSame23 = std::is_same<acceleration2, acceleration3>::value;
	bool areSame34 = std::is_same<acceleration3, acceleration4>::value;
	bool areSame45 = std::is_same<acceleration4, acceleration5>::value;

	EXPECT_TRUE(areSame12);
	EXPECT_TRUE(areSame23);
	EXPECT_TRUE(areSame34);
	EXPECT_TRUE(areSame45);
}

TEST_F(UnitTest, conversion)
{
	double test;

	test = convert<hectares, acres>(6.3);
	EXPECT_NEAR(15.5676, test, 5.0e-5);

	test = convert<years, weeks>(2.0);
	EXPECT_NEAR(104.2857142857143, test, 5.0e-14);

	test = convert<feet, feet>(6.3);
	EXPECT_NEAR(6.3, test, 5.0e-5);
}

TEST_F(UnitTest, dimensionalAnalysis)
{
	// these look like 'compound units', but the dimensional analysis can be REALLY handy if the
	// unit types aren't know (i.e. they themselves are template parameters), as you can get the resulting unit of the
	// operation.

	using velocity = unit_divide<meters, second>;
	bool shouldBeTrue = std::is_same<meters_per_second, velocity>::value;
	EXPECT_TRUE(shouldBeTrue);

	using acceleration1 = unit<std::ratio<1>, category::acceleration_unit>;
	using acceleration2 = unit_divide<meters, unit_multiply<seconds, seconds>>;
	shouldBeTrue = std::is_same<acceleration1, acceleration2>::value;
	EXPECT_TRUE(shouldBeTrue);
}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}