#include <units.h>
#include <gtest/gtest.h>

using namespace units;
using namespace units::length;
using namespace units::mass;
using namespace units::time;
using namespace units::frequency;
using namespace units::area;
using namespace units::velocity;
using namespace units::temperature;
using namespace units::luminous_intensity;
using namespace units::solid_angle;
using namespace units::frequency;

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

TEST_F(UnitTest, lengthConversion)
{
	double test;
	test = convert<meters, nanometers>(0.000000001);
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = convert<meters, micrometers>(0.000001);
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = convert<meters, millimeters>(0.001);
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = convert<meters, centimeters>(0.01);
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = convert<meters, kilometers>(1000.0);
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = convert<meters, meters>(1.0);
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = convert<meters, feet>(0.3048);
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = convert<meters, miles>(1609.344);
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = convert<meters, inches>(0.0254);
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = convert<meters, nauticalMiles>(1852.0);
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = convert<meters, astronicalUnits>(149597870700.0);
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = convert<meters, lightyears>(9460730472580800.0);
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = convert<meters, parsec>(3.08567758e16);
	EXPECT_NEAR(1.0, test, 5.0e7);

	test = convert<feet, feet>(6.3);
	EXPECT_NEAR(6.3, test, 5.0e-5);
	test = convert<feet, inches>(6.0);
	EXPECT_NEAR(72.0, test, 5.0e-5);
	test = convert<inches, feet>(6.0);
	EXPECT_NEAR(0.5, test, 5.0e-5);
	test = convert<meter, feet>(1.0);
	EXPECT_NEAR(3.28084, test, 5.0e-5);
	test = convert<miles, nauticalMiles>(6.3);
	EXPECT_NEAR(5.47455, test, 5.0e-6);

}

TEST_F(UnitTest, massConversion)
{
	double test;

	test = convert<kilograms, grams>(1.0e-3);
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = convert<kilograms, micrograms>(1.0e-9);
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = convert<kilograms, milligrams>(1.0e-6);
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = convert<kilograms, kilograms>(1.0);
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = convert<kilograms, metric_tons>(1000.0);
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = convert<kilograms, pounds>(0.453592);
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = convert<kilograms, imperial_tons>(1016.05);
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = convert<kilograms, us_tons>(907.185);
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = convert<kilograms, ounces>(0.0283495);
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = convert<kilograms, carats>(0.0002);
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = convert<slugs, kilograms>(8.1);
	EXPECT_NEAR(118.211, test, 5.0e-4);

	test = convert<pounds, carats>(6.3);
	EXPECT_NEAR(14288.2, test, 5.0e-2);
}

TEST_F(UnitTest, timeConversion)
{
	double test;

	test = convert<seconds, seconds>(1.0);
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = convert<seconds, nanoseconds>(1.0e-9);
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = convert<seconds, microseconds>(1.0e-6);
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = convert<seconds, millseconds>(1.0e-3);
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = convert<seconds, minutes>(60.0);
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = convert<seconds, hours>(3600.0);
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = convert<seconds, days>(86400.0);
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = convert<seconds, weeks>(604800.0);
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = convert<seconds, years>(3.154e7);
	EXPECT_NEAR(1.0, test, 5.0e3);

	test = convert<years, weeks>(2.0);
	EXPECT_NEAR(104.2857142857143, test, 5.0e-14);
	test = convert<hours, minutes>(4.0);
	EXPECT_NEAR(240.0, test, 5.0e-14);
}

TEST_F(UnitTest, angleConversionFactors)
{
	double test;

	test = convert<angle::radians, angle::radians>(1.0);
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = convert<angle::radians, angle::milliradians>(0.001);
	EXPECT_NEAR(1.0, test, 5.0e-4);
	test = convert<angle::radians, angle::degrees>(0.0174533);
	EXPECT_NEAR(1.0, test, 5.0e-7);
	test = convert<angle::radians, angle::minutes>(0.000290888);
	EXPECT_NEAR(0.99999928265913, test, 5.0e-8);
	test = convert<angle::radians, angle::seconds>(4.8481e-6);
	EXPECT_NEAR(0.999992407, test, 5.0e-10);
	test = convert<angle::radians, angle::turns>(6.28319);
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = convert<angle::radians, angle::mils>(0.00015625);
	EXPECT_NEAR(1.0, test, 5.0e-9);
	test = convert<angle::radians, angle::gradians>(0.015708);
	EXPECT_NEAR(1.0, test, 5.0e-6);

	test = convert<angle::radians, angle::radians>(2.1);
	EXPECT_NEAR(2.1, test, 5.0e-6);
	test = convert<angle::seconds, angle::gradians>(2.1);
	EXPECT_NEAR(0.000648148, test, 5.0e-6);
	test = convert<angle::radians, angle::degrees>(units::PI);
	EXPECT_NEAR(180.0, test, 5.0e-6);
	test = convert<angle::degrees, angle::radians>(90.0);
	EXPECT_NEAR(PI / 2, test, 5.0e-6);
	test = convert<angle::degrees, angle::mils>(47.0);
	EXPECT_NEAR(5249.95039, test, 5.0e-6);

}

TEST_F(UnitTest, currentConversion)
{
	double test;

	test = convert<current::A, current::mA>(2.1);
	EXPECT_NEAR(2100.0, test, 5.0e-6);
}

TEST_F(UnitTest, temperature)
{
	// temp conversion are weird/hard since they involve translations AND scaling.
	double test;

	test = convert<kelvin, kelvin>(72.0);
	EXPECT_NEAR(72.0, test, 5.0e-5);
	test = convert<fahrenheit, fahrenheit>(72.0);
	EXPECT_NEAR(72.0, test, 5.0e-5);
	test = convert<kelvin, fahrenheit>(300.0);
	EXPECT_NEAR(80.33, test, 5.0e-5);
	test = convert<fahrenheit, kelvin>(451.0);
	EXPECT_NEAR(505.928, test, 5.0e-4);
	test = convert<kelvin, celsius>(300.0);
	EXPECT_NEAR(26.85, test, 5.0e-3);
	test = convert<celsius, kelvin>(451.0);
	EXPECT_NEAR(724.15, test, 5.0e-3);
	test = convert<fahrenheit, celsius>(72.0);
	EXPECT_NEAR(22.2222, test, 5.0e-5);
	test = convert<celsius, fahrenheit>(100.0);
	EXPECT_NEAR(212.0, test, 5.0e-5);
	test = convert<fahrenheit, celsius>(32.0);
	EXPECT_NEAR(0.0, test, 5.0e-5);
	test = convert<celsius, fahrenheit>(0.0);
	EXPECT_NEAR(32.0, test, 5.0e-5);
	test = convert<rankine, kelvin>(100.0);
	EXPECT_NEAR(55.5556, test, 5.0e-5);
	test = convert<kelvin, rankine>(100.0);
	EXPECT_NEAR(180.0, test, 5.0e-5);
	test = convert<fahrenheit, rankine>(100.0);
	EXPECT_NEAR(559.67, test, 5.0e-5);
	test = convert<rankine, fahrenheit>(72.0);
	EXPECT_NEAR(-387.67, test, 5.0e-5);
	test = convert<reaumur, kelvin>(100.0);
	EXPECT_NEAR(398.0, test, 5.0e-1);
	test = convert<reaumur, celsius>(80.0);
	EXPECT_NEAR(100.0, test, 5.0e-5);
	test = convert<celsius, reaumur>(212.0);
	EXPECT_NEAR(169.6, test, 5.0e-2);
	test = convert<reaumur, fahrenheit>(80.0);
	EXPECT_NEAR(212.0, test, 5.0e-5);
	test = convert<fahrenheit, reaumur>(37.0);
	EXPECT_NEAR(2.222, test, 5.0e-3);
}

TEST_F(UnitTest, luminousIntensityConversion)
{
	double test;

	test = convert<candela, millicandela>(72.0);
	EXPECT_NEAR(72000.0, test, 5.0e-5);
	test = convert<millicandela, candela>(376.0);
	EXPECT_NEAR(0.376, test, 5.0e-5);
}

TEST_F(UnitTest, solidAngleConversion)
{
	double test;

	test = convert<steradians, steradians>(72.0);
	EXPECT_NEAR(72.0, test, 5.0e-5);
	test = convert<steradians, degrees_squared>(1.0);
	EXPECT_NEAR(3282.8, test, 5.0e-2);
	test = convert<steradians, spats>(8.0);
	EXPECT_NEAR(0.636619772367582, test, 5.0e-14);
	test = convert<degrees_squared, steradians>(3282.8);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<degrees_squared, degrees_squared>(72.0);
	EXPECT_NEAR(72.0, test, 5.0e-5);
	test = convert<degrees_squared, spats>(3282.8);
	EXPECT_NEAR(1.0 / (4 * PI), test, 5.0e-5);
	test = convert<spats, steradians>(1.0 / (4 * PI));
	EXPECT_NEAR(1.0, test, 5.0e-14);
	test = convert<spats, degrees_squared>(1.0 / (4 * PI));
	EXPECT_NEAR(3282.8, test, 5.0e-2);
	test = convert<spats, spats>(72.0);
	EXPECT_NEAR(72.0, test, 5.0e-5);
}

TEST_F(UnitTest, frequencyConversion)
{
	double test;

	test = convert<hertz, kilohertz>(63000.0);
	EXPECT_NEAR(63.0, test, 5.0e-5);
	test = convert<hertz, hertz>(6.3);
	EXPECT_NEAR(6.3, test, 5.0e-5);
	test = convert<kilohertz, hertz>(5.0);
	EXPECT_NEAR(5000.0, test, 5.0e-5);
	test = convert<megahertz, hertz>(1.0);
	EXPECT_NEAR(1.0e6, test, 5.0e-5);
}

TEST_F(UnitTest, forceConversion)
{
	double test;

	test = convert<force::newton, force::newton>(1.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<force::newton, force::pounds>(6.3);
	EXPECT_NEAR(1.4163, test, 5.0e-5);
	test = convert<force::newton, force::dynes>(5.0);
	EXPECT_NEAR(500000.0, test, 5.0e-5);
	test = convert<force::newtons, force::poundals>(2.1);
	EXPECT_NEAR(15.1893, test, 5.0e-5);
	test = convert<force::newtons, force::kiloponds>(173.0);
	EXPECT_NEAR(17.6411, test, 5.0e-5);
	test = convert<force::poundals, force::kiloponds>(21.879);
	EXPECT_NEAR(0.308451933, test, 5.0e-10);
}

TEST_F(UnitTest, areaConversionFactors)
{
	double test;

	test = convert<hectares, acres>(6.3);
	EXPECT_NEAR(15.5676, test, 5.0e-5);
	
}

TEST_F(UnitTest, inverseUnits)
{
	double test;

	using htz = units::inverse<seconds>;
	bool shouldBeTrue = std::is_same<htz, hertz>::value;
	EXPECT_TRUE(shouldBeTrue);

	test = convert<inverse<celsius>, inverse<fahrenheit>>(1.0);
	EXPECT_NEAR(5.0 / 9.0, test, 5.0e-5);

	test = convert<inverse<kelvin>, inverse<fahrenheit>>(6.0);
	EXPECT_NEAR(10.0 /3.0, test, 5.0e-5);
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
	double test;

	test = convert<squared<meters>, feet_squared>(0.092903);
	EXPECT_NEAR(0.99999956944, test, 5.0e-12);
}

TEST_F(UnitTest, cubed)
{
	using feet_cubed = units::cubed<feet>;
	double test;

	test = convert<cubed<meters>, feet_cubed>(0.0283168);
	EXPECT_NEAR(0.999998354619, test, 5.0e-13);
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

	// test that thing with translations still compile
	using arbitary1 = compound_unit<meters, inverse<celsius>>;
	using arbitary2 = compound_unit<meters, celsius>;
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