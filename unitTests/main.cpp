#include <units.h>
#include <gtest/gtest.h>
#include <chrono>

using namespace units;
using namespace units::dimensionless;
using namespace units::length;
using namespace units::mass;
using namespace units::time;
using namespace units::frequency;
using namespace units::area;
using namespace units::velocity;
using namespace units::angular_velocity;
using namespace units::temperature;
using namespace units::luminous_intensity;
using namespace units::solid_angle;
using namespace units::frequency;
using namespace units::acceleration;
using namespace units::pressure;
using namespace units::charge;
using namespace units::energy;
using namespace units::power;
using namespace units::voltage;
using namespace units::capacitance;
using namespace units::impedance;
using namespace units::conductance;
using namespace units::magnetic_flux;
using namespace units::magnetic_field_strength;
using namespace units::inductance;
using namespace units::luminous_flux;
using namespace units::illuminance;
using namespace units::radiation;
using namespace units::torque;
using namespace units::volume;
using namespace units::density;
using namespace units::concentration;

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
	bool test1 = units::is_convertible_unit<meters, meters>::value;
	bool test2 = units::is_convertible_unit<meters, astronicalUnits>::value;
	bool test3 = units::is_convertible_unit<meters, parsecs>::value;
	
	bool test4 = units::is_convertible_unit<meters, meters>::value;
	bool test5 = units::is_convertible_unit<astronicalUnits, meters>::value;
	bool test6 = units::is_convertible_unit<parsecs, meters>::value;

	bool test7 = units::is_convertible_unit<meters, seconds>::value;
	bool test8 = units::is_convertible_unit<seconds, meters>::value;

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

	shouldBeTrue = units::is_convertible_unit<years, weeks>::value;
	EXPECT_TRUE(shouldBeTrue);

	shouldBeFalse = units::is_convertible_unit<years, meters>::value;
	EXPECT_FALSE(shouldBeFalse);
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
	EXPECT_NEAR(10.0 / 3.0, test, 5.0e-5);
}

TEST_F(UnitTest, baseUnitOf)
{
	using base = units::base_unit_of<years>;
	bool shouldBeTrue = std::is_same<base, category::time_unit>::value;

	EXPECT_TRUE(shouldBeTrue);
}

TEST_F(UnitTest, hasLinearScale)
{
	bool test;
	
	test = has_linear_scale<scalar_t>::value;
	EXPECT_TRUE(test);
	test = has_linear_scale<meter_t>::value;
	EXPECT_TRUE(test);
	test = has_linear_scale<foot_t>::value;
	EXPECT_TRUE(test);
	test = has_linear_scale<meters_per_second_t>::value;
	EXPECT_TRUE(test);
	test = has_linear_scale<dB_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, hasDecibelScale)
{
	bool test;

	test = has_decibel_scale<scalar_t>::value;
	EXPECT_FALSE(test);
	test = has_decibel_scale<meter_t>::value;
	EXPECT_FALSE(test);
	test = has_decibel_scale<foot_t>::value;
	EXPECT_FALSE(test);
	test = has_decibel_scale<dB_t>::value;
	EXPECT_TRUE(test);
	test = has_decibel_scale<dBW_t>::value;
	EXPECT_TRUE(test);
}

TEST_F(UnitTest, isSameScale)
{
	bool test;

	test = is_same_scale<scalar_t, dimensionless_t>::value;
	EXPECT_TRUE(test);
	test = is_same_scale<dB_t, dBW_t>::value;
	EXPECT_TRUE(test);
	test = is_same_scale<dB_t, scalar_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, isScalarUnit)
{
	bool test;

	test = is_scalar_unit<scalar_t>::value;
	EXPECT_TRUE(test);
	test = is_scalar_unit<dimensionless_t>::value;
	EXPECT_TRUE(test);
	test = is_scalar_unit<dB_t>::value;
	EXPECT_TRUE(test);
	test = is_scalar_unit<meter_t>::value;
	EXPECT_FALSE(test);
	test = is_scalar_unit<dBW_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, isLengthUnit)
{
	bool test;

	test = is_length_unit<meter>::value;
	EXPECT_TRUE(test);
	test = is_length_unit<cubit>::value;
	EXPECT_TRUE(test);
	test = is_length_unit<year>::value;
	EXPECT_FALSE(test);
	test = is_length_unit<double>::value;
	EXPECT_FALSE(test);
	
	test = is_length_unit<meter_t>::value;
	EXPECT_TRUE(test);
	test = std::is_same<base_unit_of<typename unit_t_traits<const meter_t>::unit_type>, category::length_unit>::value;
	EXPECT_TRUE(test);
	test = is_length_unit<const meter_t>::value;
	EXPECT_TRUE(test);
	test = is_length_unit<cubit_t>::value;
	EXPECT_TRUE(test);
	test = is_length_unit<year_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, ismass_unit)
{
	bool test;


	test = is_mass_unit<kilogram>::value;
	EXPECT_TRUE(test);
	test = is_mass_unit<stone>::value;
	EXPECT_TRUE(test);
	test = is_mass_unit<meter>::value;
	EXPECT_FALSE(test);
	test = is_mass_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_mass_unit<kilogram_t>::value;
	EXPECT_TRUE(test);
	test = is_mass_unit<stone_t>::value;
	EXPECT_TRUE(test);
	test = is_mass_unit<meter_t>::value;
	EXPECT_FALSE(test);
}


TEST_F(UnitTest, istime_unit)
{
	bool test;


	test = is_time_unit<second>::value;
	EXPECT_TRUE(test);
	test = is_time_unit<year>::value;
	EXPECT_TRUE(test);
	test = is_time_unit<meter>::value;
	EXPECT_FALSE(test);
	test = is_time_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_time_unit<second_t>::value;
	EXPECT_TRUE(test);
	test = is_time_unit<year_t>::value;
	EXPECT_TRUE(test);
	test = is_time_unit<meter_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, isangle_unit)
{
	bool test;


	test = is_angle_unit<angle::radian>::value;
	EXPECT_TRUE(test);
	test = is_angle_unit<angle::degree>::value;
	EXPECT_TRUE(test);
	test = is_angle_unit<watt>::value;
	EXPECT_FALSE(test);
	test = is_angle_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_angle_unit<angle::radian_t>::value;
	EXPECT_TRUE(test);
	test = is_angle_unit<angle::degree_t>::value;
	EXPECT_TRUE(test);
	test = is_angle_unit<watt_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, iscurrent_unit)
{
	bool test;


	test = is_current_unit<current::ampere>::value;
	EXPECT_TRUE(test);
	test = is_current_unit<volt>::value;
	EXPECT_FALSE(test);
	test = is_current_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_current_unit<current::ampere_t>::value;
	EXPECT_TRUE(test);
	test = is_current_unit<volt_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, istemperature_unit)
{
	bool test;


	test = is_temperature_unit<fahrenheit>::value;
	EXPECT_TRUE(test);
	test = is_temperature_unit<kelvin>::value;
	EXPECT_TRUE(test);
	test = is_temperature_unit<cubit>::value;
	EXPECT_FALSE(test);
	test = is_temperature_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_temperature_unit<fahrenheit_t>::value;
	EXPECT_TRUE(test);
	test = is_temperature_unit<kelvin_t>::value;
	EXPECT_TRUE(test);
	test = is_temperature_unit<cubit_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, issubstance_unit)
{
	bool test;


	test = is_substance_unit<substance::mol>::value;
	EXPECT_TRUE(test);
	test = is_substance_unit<year>::value;
	EXPECT_FALSE(test);
	test = is_substance_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_substance_unit<substance::mole_t>::value;
	EXPECT_TRUE(test);
	test = is_substance_unit<year_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, isluminous_intensity_unit)
{
	bool test;


	test = is_luminous_intensity_unit<candela>::value;
	EXPECT_TRUE(test);
	test = is_luminous_intensity_unit<rad>::value;
	EXPECT_FALSE(test);
	test = is_luminous_intensity_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_luminous_intensity_unit<candela_t>::value;
	EXPECT_TRUE(test);
	test = is_luminous_intensity_unit<rad_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, issolid_angle_unit)
{
	bool test;


	test = is_solid_angle_unit<steradian>::value;
	EXPECT_TRUE(test);
	test = is_solid_angle_unit<degree_squared>::value;
	EXPECT_TRUE(test);
	test = is_solid_angle_unit<angle::degree>::value;
	EXPECT_FALSE(test);
	test = is_solid_angle_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_solid_angle_unit<steradian_t>::value;
	EXPECT_TRUE(test);
	test = is_solid_angle_unit<degree_squared_t>::value;
	EXPECT_TRUE(test);
	test = is_solid_angle_unit<angle::degree_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, isfrequency_unit)
{
	bool test;


	test = is_frequency_unit<hertz>::value;
	EXPECT_TRUE(test);
	test = is_frequency_unit<second>::value;
	EXPECT_FALSE(test);
	test = is_frequency_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_frequency_unit<hertz_t>::value;
	EXPECT_TRUE(test);
	test = is_frequency_unit<second_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, isvelocity_unit)
{
	bool test;


	test = is_velocity_unit<meters_per_second>::value;
	EXPECT_TRUE(test);
	test = is_velocity_unit<miles_per_hour>::value;
	EXPECT_TRUE(test);
	test = is_velocity_unit<meters_per_second_squared>::value;
	EXPECT_FALSE(test);
	test = is_velocity_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_velocity_unit<meters_per_second_t>::value;
	EXPECT_TRUE(test);
	test = is_velocity_unit<miles_per_hour_t>::value;
	EXPECT_TRUE(test);
	test = is_velocity_unit<meters_per_second_squared_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, isacceleration_unit)
{
	bool test;


	test = is_acceleration_unit<meters_per_second_squared>::value;
	EXPECT_TRUE(test);
	test = is_acceleration_unit<acceleration::standard_gravity>::value;
	EXPECT_TRUE(test);
	test = is_acceleration_unit<inch>::value;
	EXPECT_FALSE(test);
	test = is_acceleration_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_acceleration_unit<meters_per_second_squared_t>::value;
	EXPECT_TRUE(test);
	test = is_acceleration_unit<standard_gravity_t>::value;
	EXPECT_TRUE(test);
	test = is_acceleration_unit<inch_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, isforce_unit)
{
	bool test;


	test = is_force_unit<force::newton>::value;
	EXPECT_TRUE(test);
	test = is_force_unit<force::dynes>::value;
	EXPECT_TRUE(test);
	test = is_force_unit<meter>::value;
	EXPECT_FALSE(test);
	test = is_force_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_force_unit<force::newton_t>::value;
	EXPECT_TRUE(test);
	test = is_force_unit<force::dyne_t>::value;
	EXPECT_TRUE(test);
	test = is_force_unit<watt_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, ispressure_unit)
{
	bool test;

	test = is_pressure_unit<pressure::pascals>::value;
	EXPECT_TRUE(test);
	test = is_pressure_unit<atmosphere>::value;
	EXPECT_TRUE(test);
	test = is_pressure_unit<year>::value;
	EXPECT_FALSE(test);
	test = is_pressure_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_pressure_unit<pascal_t>::value;
	EXPECT_TRUE(test);
	test = is_pressure_unit<atmosphere_t>::value;
	EXPECT_TRUE(test);
	test = is_pressure_unit<year_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, ischarge_unit)
{
	bool test;

	test = is_charge_unit<coulomb>::value;
	EXPECT_TRUE(test);
	test = is_charge_unit<watt>::value;
	EXPECT_FALSE(test);
	test = is_charge_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_charge_unit<coulomb_t>::value;
	EXPECT_TRUE(test);
	test = is_charge_unit<watt_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, isenergy_unit)
{
	bool test;

	test = is_energy_unit<joule>::value;
	EXPECT_TRUE(test);
	test = is_energy_unit<calorie>::value;
	EXPECT_TRUE(test);
	test = is_energy_unit<watt>::value;
	EXPECT_FALSE(test);
	test = is_energy_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_energy_unit<joule_t>::value;
	EXPECT_TRUE(test);
	test = is_energy_unit<calorie_t>::value;
	EXPECT_TRUE(test);
	test = is_energy_unit<watt_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, ispower_unit)
{
	bool test;

	test = is_power_unit<watt>::value;
	EXPECT_TRUE(test);
	test = is_power_unit<henry>::value;
	EXPECT_FALSE(test);
	test = is_power_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_power_unit<watt_t>::value;
	EXPECT_TRUE(test);
	test = is_power_unit<henry_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, isvoltage_unit)
{
	bool test;

	test = is_voltage_unit<volt>::value;
	EXPECT_TRUE(test);
	test = is_voltage_unit<henry>::value;
	EXPECT_FALSE(test);
	test = is_voltage_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_voltage_unit<volt_t>::value;
	EXPECT_TRUE(test);
	test = is_voltage_unit<henry_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, iscapacitance_unit)
{
	bool test;

	test = is_capacitance_unit<farad>::value;
	EXPECT_TRUE(test);
	test = is_capacitance_unit<ohm>::value;
	EXPECT_FALSE(test);
	test = is_capacitance_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_capacitance_unit<farad_t>::value;
	EXPECT_TRUE(test);
	test = is_capacitance_unit<ohm_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, isimpedance_unit)
{
	bool test;


	test = is_impedance_unit<ohm>::value;
	EXPECT_TRUE(test);
	test = is_impedance_unit<farad>::value;
	EXPECT_FALSE(test);
	test = is_impedance_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_impedance_unit<ohm_t>::value;
	EXPECT_TRUE(test);
	test = is_impedance_unit<farad_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, isconductance_unit)
{
	bool test;

	test = is_conductance_unit<siemen>::value;
	EXPECT_TRUE(test);
	test = is_conductance_unit<volt>::value;
	EXPECT_FALSE(test);
	test = is_conductance_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_conductance_unit<siemen_t>::value;
	EXPECT_TRUE(test);
	test = is_conductance_unit<volt_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, ismagnetic_flux_unit)
{
	bool test;

	test = is_magnetic_flux_unit<weber>::value;
	EXPECT_TRUE(test);
	test = is_magnetic_flux_unit<maxwell>::value;
	EXPECT_TRUE(test);
	test = is_magnetic_flux_unit<inch>::value;
	EXPECT_FALSE(test);
	test = is_magnetic_flux_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_magnetic_flux_unit<weber_t>::value;
	EXPECT_TRUE(test);
	test = is_magnetic_flux_unit<maxwell_t>::value;
	EXPECT_TRUE(test);
	test = is_magnetic_flux_unit<inch_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, ismagnetic_field_strength_unit)
{
	bool test;


	test = is_magnetic_field_strength_unit<tesla>::value;
	EXPECT_TRUE(test);
	test = is_magnetic_field_strength_unit<gauss>::value;
	EXPECT_TRUE(test);
	test = is_magnetic_field_strength_unit<volt>::value;
	EXPECT_FALSE(test);
	test = is_magnetic_field_strength_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_magnetic_field_strength_unit<tesla_t>::value;
	EXPECT_TRUE(test);
	test = is_magnetic_field_strength_unit<gauss_t>::value;
	EXPECT_TRUE(test);
	test = is_magnetic_field_strength_unit<volt_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, isinductance_unit)
{
	bool test;


	test = is_inductance_unit<henry>::value;
	EXPECT_TRUE(test);
	test = is_inductance_unit<farad>::value;
	EXPECT_FALSE(test);
	test = is_inductance_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_inductance_unit<henry_t>::value;
	EXPECT_TRUE(test);
	test = is_inductance_unit<farad_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, isluminous_flux_unit)
{
	bool test;


	test = is_luminous_flux_unit<lumen>::value;
	EXPECT_TRUE(test);
	test = is_luminous_flux_unit<pound>::value;
	EXPECT_FALSE(test);
	test = is_luminous_flux_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_luminous_flux_unit<lumen_t>::value;
	EXPECT_TRUE(test);
	test = is_luminous_flux_unit<pound_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, isilluminance_unit)
{
	bool test;


	test = is_illuminance_unit<illuminance::footcandle>::value;
	EXPECT_TRUE(test);
	test = is_illuminance_unit<illuminance::lux>::value;
	EXPECT_TRUE(test);
	test = is_illuminance_unit<meter>::value;
	EXPECT_FALSE(test);
	test = is_illuminance_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_illuminance_unit<footcandle_t>::value;
	EXPECT_TRUE(test);
	test = is_illuminance_unit<lux_t>::value;
	EXPECT_TRUE(test);
	test = is_illuminance_unit<meter_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, isradioactivity_unit)
{
	bool test;


	test = is_radioactivity_unit<becquerel>::value;
	EXPECT_TRUE(test);
	test = is_radioactivity_unit<year>::value;
	EXPECT_FALSE(test);
	test = is_radioactivity_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_radioactivity_unit<becquerel_t>::value;
	EXPECT_TRUE(test);
	test = is_radioactivity_unit<year_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, istorque_unit)
{
	bool test;


	test = is_torque_unit<torque::newton_meter>::value;
	EXPECT_TRUE(test);
	test = is_torque_unit<torque::foot_pound>::value;
	EXPECT_TRUE(test);
	test = is_torque_unit<volume::cubic_meter>::value;
	EXPECT_FALSE(test);
	test = is_torque_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_torque_unit<torque::newton_meter_t>::value;
	EXPECT_TRUE(test);
	test = is_torque_unit<torque::foot_pound_t>::value;
	EXPECT_TRUE(test);
	test = is_torque_unit<volume::cubic_meter_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, isarea_unit)
{
	bool test;


	test = is_area_unit<square_meter>::value;
	EXPECT_TRUE(test);
	test = is_area_unit<hectare>::value;
	EXPECT_TRUE(test);
	test = is_area_unit<astronicalUnit>::value;
	EXPECT_FALSE(test);
	test = is_area_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_area_unit<square_meter_t>::value;
	EXPECT_TRUE(test);
	test = is_area_unit<hectare_t>::value;
	EXPECT_TRUE(test);
	test = is_area_unit<astronicalUnit_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, isvolume_unit)
{
	bool test;


	test = is_volume_unit<cubic_meter>::value;
	EXPECT_TRUE(test);
	test = is_volume_unit<cubic_foot>::value;
	EXPECT_TRUE(test);
	test = is_volume_unit<square_feet>::value;
	EXPECT_FALSE(test);
	test = is_volume_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_volume_unit<cubic_meter_t>::value;
	EXPECT_TRUE(test);
	test = is_volume_unit<cubic_inch_t>::value;
	EXPECT_TRUE(test);
	test = is_volume_unit<foot_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, isdensity_unit)
{
	bool test;


	test = is_density_unit<kilograms_per_cubic_meter>::value;
	EXPECT_TRUE(test);
	test = is_density_unit<ounces_per_cubic_foot>::value;
	EXPECT_TRUE(test);
	test = is_density_unit<year>::value;
	EXPECT_FALSE(test);
	test = is_density_unit<double>::value;
	EXPECT_FALSE(test);

	test = is_density_unit<kilogram_per_cubic_meter_t>::value;
	EXPECT_TRUE(test);
	test = is_density_unit<ounce_per_cubic_foot_t>::value;
	EXPECT_TRUE(test);
	test = is_density_unit<year_t>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, squared)
{
	double test;

	test = convert<squared<meters>, square_feet>(0.092903);
	EXPECT_NEAR(0.99999956944, test, 5.0e-12);

	using scalar_2 = units::squared<scalar>;	// this is actually nonsensical, and should also result in a scalar.
	bool isSame = std::is_same<typename std::decay<scalar_t>::type, typename std::decay<unit_t<scalar_2>>::type>::value;
	EXPECT_TRUE(isSame);
}

TEST_F(UnitTest, cubed)
{
	double test;

	test = convert<cubed<meters>, cubic_feet>(0.0283168);
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

TEST_F(UnitTest, hasValueMember)
{
	bool test;

	test = units::has_value_member<linear_scale<double>, double>::value;
	EXPECT_TRUE(test);
	test = units::has_value_member<meter_t, double>::value;
	EXPECT_FALSE(test);
	test = units::has_value_member<meter, double>::value;
	EXPECT_FALSE(test);
}

TEST_F(UnitTest, unitTypeAddition)
{
	meter_t a_m(1.0), c_m;
	foot_t b_ft(3.28084);

	double d = units::convert<feet, meters>(b_ft());
	EXPECT_NEAR(1.0, d, 5.0e-5);

	c_m = a_m + b_ft;
	EXPECT_NEAR(2.0, c_m(), 5.0e-5);

	c_m = b_ft + meter_t(3);
	EXPECT_NEAR(4.0, c_m(), 5.0e-5);

	auto e_ft = b_ft + meter_t(3);
	EXPECT_NEAR(13.12336, e_ft(), 5.0e-6);

	scalar_t sresult = scalar_t(1.0) + scalar_t(1.0);
	EXPECT_NEAR(2.0, sresult, 5.0e-6);

	sresult = scalar_t(1.0) + 1.0;
	EXPECT_NEAR(2.0, sresult, 5.0e-6);

	sresult = 1.0 + scalar_t(1.0);
	EXPECT_NEAR(2.0, sresult, 5.0e-6);

	d = scalar_t(1.0) + scalar_t(1.0);
	EXPECT_NEAR(2.0, d, 5.0e-6);

	d = scalar_t(1.0) + 1.0;
	EXPECT_NEAR(2.0, d, 5.0e-6);

	d = 1.0 + scalar_t(1.0);
	EXPECT_NEAR(2.0, d, 5.0e-6);
}

TEST_F(UnitTest, unitTypeSubtraction)
{
	meter_t a_m(1.0), c_m;
	foot_t b_ft(3.28084);

	c_m = a_m - b_ft;
	EXPECT_NEAR(0.0, c_m(), 5.0e-5);

	c_m = b_ft - meter_t(1);
	EXPECT_NEAR(0.0, c_m(), 5.0e-5);

	auto e_ft = b_ft - meter_t(1);
	EXPECT_NEAR(0.0, e_ft(), 5.0e-6);

	scalar_t sresult = scalar_t(1.0) - scalar_t(1.0);
	EXPECT_NEAR(0.0, sresult, 5.0e-6);

	sresult = scalar_t(1.0) - 1.0;
	EXPECT_NEAR(0.0, sresult, 5.0e-6);

	sresult = 1.0 - scalar_t(1.0);
	EXPECT_NEAR(0.0, sresult, 5.0e-6);

	double d = scalar_t(1.0) - scalar_t(1.0);
	EXPECT_NEAR(0.0, d, 5.0e-6);

	d = scalar_t(1.0) - 1.0;
	EXPECT_NEAR(0.0, d, 5.0e-6);

	d = 1.0 - scalar_t(1.0);
	EXPECT_NEAR(0.0, d, 5.0e-6);
}

TEST_F(UnitTest, unitTypeMultiplication)
{
	meter_t a_m(1.0), b_m(2.0);
	foot_t a_ft(3.28084);

	auto c_m2 = a_m * b_m;
	EXPECT_NEAR(2.0, c_m2(), 5.0e-5);

	c_m2 = b_m * meter_t(2);
	EXPECT_NEAR(4.0, c_m2(), 5.0e-5);

	c_m2 = b_m *a_ft;
	EXPECT_NEAR(2.0, c_m2(), 5.0e-5);

	auto c_m = b_m * 2.0;
	EXPECT_NEAR(4.0, c_m(), 5.0e-5);

	c_m = 2.0 * b_m;
	EXPECT_NEAR(4.0, c_m(), 5.0e-5);

	double convert = scalar_t(3.14);
	EXPECT_NEAR(3.14, convert, 5.0e-5);
	
 	scalar_t sresult = scalar_t(5.0) * scalar_t(4.0);
 	EXPECT_NEAR(20.0, sresult(), 5.0e-5);
 
	sresult = scalar_t(5.0) * 4.0;
	EXPECT_NEAR(20.0, sresult(), 5.0e-5);

	sresult = 4.0 * scalar_t(5.0);
	EXPECT_NEAR(20.0, sresult(), 5.0e-5);

 	double result = scalar_t(5.0) * scalar_t(4.0);
 	EXPECT_NEAR(20.0, result, 5.0e-5);

	result = scalar_t(5.0) * 4.0;
	EXPECT_NEAR(20.0, result, 5.0e-5);

	result = 4.0 * scalar_t(5.0);
	EXPECT_NEAR(20.0, result, 5.0e-5);
}

TEST_F(UnitTest, unitTypeMixedUnitMultiplication)
{
	meter_t a_m(1.0);
	foot_t b_ft(3.28084);
	unit_t<inverse<meter>> i_m(2.0);

	// resultant unit is square of leftmost unit
	auto c_m2 = a_m * b_ft;
 	EXPECT_NEAR(1.0, c_m2(), 5.0e-5);

	auto c_ft2 = b_ft * a_m;
	EXPECT_NEAR(10.7639111056, c_ft2(), 5.0e-7);

	// you can get whatever (compatible) type you want if you ask explicitly
	square_meter_t d_m2 = b_ft * a_m;
	EXPECT_NEAR(1.0, d_m2(), 5.0e-5);

	// a unit times a sclar ends up with the same units.
	meter_t e_m = a_m * scalar_t(3.0);
	EXPECT_NEAR(3.0, e_m(), 5.0e-5);

	e_m = scalar_t(4.0) * a_m;
	EXPECT_NEAR(4.0, e_m(), 5.0e-5);

	// unit times its inverse results in a scalar
	scalar_t s = a_m * i_m;
	EXPECT_NEAR(2.0, s, 5.0e-5);
 
 	c_m2 = b_ft * meter_t(2);
 	EXPECT_NEAR(2.0, c_m2(), 5.0e-5);
 
 	auto e_ft2 = b_ft * meter_t(3);
 	EXPECT_NEAR(32.2917333168, e_ft2(), 5.0e-6);

	auto mps = meter_t(10.0) * unit_t<inverse<seconds>>(1.0);

}

TEST_F(UnitTest, unitTypeScalarMultiplication)
{
	meter_t a_m(1.0), c_m;
	foot_t b_ft(3.28084);

	auto result_m = scalar_t(3.0) * a_m;
	EXPECT_NEAR(3.0, result_m(), 5.0e-5);

	result_m = a_m * scalar_t(4.0);
	EXPECT_NEAR(4.0, result_m(), 5.0e-5);

	result_m = 3.0 * a_m;
	EXPECT_NEAR(3.0, result_m(), 5.0e-5);

	result_m = a_m * 4.0;
	EXPECT_NEAR(4.0, result_m(), 5.0e-5);

	bool isSame = std::is_same<decltype(result_m), meter_t>::value;
	EXPECT_TRUE(isSame);
}

TEST_F(UnitTest, unitTypeDivision)
{
	meter_t a_m(1.0), b_m(2.0);
	foot_t a_ft(3.28084);
	second_t a_sec(10.0);
	bool isSame;

	auto c = a_m / a_ft;
	EXPECT_NEAR(1.0, c, 5.0e-5);
	isSame = std::is_same<decltype(c), scalar_t>::value;
	EXPECT_TRUE(isSame);

	c = a_m / b_m;
	EXPECT_NEAR(0.5, c, 5.0e-5);
	isSame = std::is_same<decltype(c), scalar_t>::value;
	EXPECT_TRUE(isSame);

	c = a_ft / a_m;
	EXPECT_NEAR(1.0, c, 5.0e-5);
	isSame = std::is_same<decltype(c), scalar_t>::value;
	EXPECT_TRUE(isSame);

	c = scalar_t(1.0) / 2.0;
	EXPECT_NEAR(0.5, c, 5.0e-5);
	isSame = std::is_same<decltype(c), scalar_t>::value;
	EXPECT_TRUE(isSame);

	c = 1.0 / scalar_t(2.0);
	EXPECT_NEAR(0.5, c, 5.0e-5);
	isSame = std::is_same<decltype(c), scalar_t>::value;
	EXPECT_TRUE(isSame);

	double d = scalar_t(1.0) / 2.0;
	EXPECT_NEAR(0.5, d, 5.0e-5);

	auto e = a_m / a_sec;
	EXPECT_NEAR(0.1, e(), 5.0e-5);
	isSame = std::is_same<decltype(e), meters_per_second_t>::value;
	EXPECT_TRUE(isSame);

	auto f = a_m / 8.0;
	EXPECT_NEAR(0.125, f(), 5.0e-5);
	isSame = std::is_same<decltype(f), meter_t>::value;
	EXPECT_TRUE(isSame);

	auto g = 4.0 / b_m;
	EXPECT_NEAR(2.0, g(), 5.0e-5);
	isSame = std::is_same<decltype(g), unit_t<inverse<meters>>>::value;
	EXPECT_TRUE(isSame);

	auto mph = mile_t(60.0) / hour_t(1.0);
	meters_per_second_t mps = mph;
	EXPECT_NEAR(26.8224, mps(), 5.0e-5);
}

TEST_F(UnitTest, scalarTypeImplicitConversion)
{
	double test = scalar_t(3.0);
	EXPECT_DOUBLE_EQ(3.0, test);

	scalar_t testS = 3.0;
	EXPECT_DOUBLE_EQ(3.0, test);
}

TEST_F(UnitTest, valueMethod)
{
	double test = meter_t(3.0).value();
	EXPECT_DOUBLE_EQ(3.0, test);
}

TEST_F(UnitTest, convertMethod)
{
	double test = meter_t(3.0).convert<feet>().value();
	EXPECT_NEAR(9.84252, test, 5.0e-6);
}

TEST_F(UnitTest, unitPowers)
{
	bool isSame;
	meter_t value(10.0);

	auto sq = units::pow<2>(value);
	EXPECT_NEAR(100.0, sq(), 5.0e-2);
	isSame = std::is_same<decltype(sq), square_meter_t>::value;
	EXPECT_TRUE(isSame);

	auto cube = units::pow<3>(value);
	EXPECT_NEAR(1000.0, cube(), 5.0e-2);
	isSame = std::is_same<decltype(cube), unit_t<cubed<meter>>>::value;
	EXPECT_TRUE(isSame);

	auto fourth = units::pow<4>(value);
	EXPECT_NEAR(10000.0, fourth(), 5.0e-2);
	isSame = std::is_same<decltype(fourth), unit_t<compound_unit<squared<meter>, squared<meter>>>>::value;
	EXPECT_TRUE(isSame);
}

TEST_F(UnitTest, dBConversion)
{
	dBW_t a_dbw(23.1);
	watt_t a_w = a_dbw;
	dBm_t a_dbm = a_dbw;

	EXPECT_NEAR(204.173794, a_w(), 5.0e-7);
	EXPECT_NEAR(53.1, a_dbm(), 5.0e-7);

	milliwatt_t b_mw(100000.0);
	watt_t b_w = b_mw;
	dBm_t b_dbm = b_mw;
	dBW_t b_dbw = b_mw;

	EXPECT_NEAR(100.0, b_w(), 5.0e-7);
	EXPECT_NEAR(50.0, b_dbm(), 5.0e-7);
	EXPECT_NEAR(20.0, b_dbw(), 5.0e-7);
}

TEST_F(UnitTest, dBAddition)
{
	bool isSame;

	auto result_dbw = dBW_t(10.0) + dB_t(30.0);
	EXPECT_NEAR(40.0, result_dbw(), 5.0e-5);
	result_dbw = dB_t(12.0) + dBW_t(30.0);
	EXPECT_NEAR(42.0, result_dbw(), 5.0e-5);
	isSame = std::is_same<decltype(result_dbw), dBW_t>::value;
	EXPECT_TRUE(isSame);

	auto result_dbm = dB_t(30.0) + dBm_t(20.0);
	EXPECT_NEAR(50.0, result_dbm(), 5.0e-5);

	// adding dBW to dBW is something you probably shouldn't do, but let's see if it works...
	auto result_dBW2 = dBW_t(10.0) + dBm_t(40.0);
	EXPECT_NEAR(20.0, result_dBW2(), 5.0e-5);
	isSame = std::is_same<decltype(result_dBW2), unit_t<squared<watts>, double, decibel_scale>>::value;
	EXPECT_TRUE(isSame);
}

TEST_F(UnitTest, dBSubtraction)
{
	bool isSame;

	auto result_dbw = dBW_t(10.0) - dB_t(30.0);
	EXPECT_NEAR(-20.0, result_dbw(), 5.0e-5);
	isSame = std::is_same<decltype(result_dbw), dBW_t>::value;
	EXPECT_TRUE(isSame);

	auto result_dbm = dBm_t(100.0) - dB_t(30.0);
	EXPECT_NEAR(70.0, result_dbm(), 5.0e-5);
	isSame = std::is_same<decltype(result_dbm), dBm_t>::value;
	EXPECT_TRUE(isSame);

	auto result_db = dBW_t(100.0) - dBW_t(80.0);
	EXPECT_NEAR(20.0, result_db(), 5.0e-5);
	isSame = std::is_same<decltype(result_db), dB_t>::value;
	EXPECT_TRUE(isSame);

	result_db = dB_t(100.0) - dB_t(80.0);
	EXPECT_NEAR(20.0, result_db(), 5.0e-5);
	isSame = std::is_same<decltype(result_db), dB_t>::value;
	EXPECT_TRUE(isSame);
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
	test = convert<miles, meters>(11.0);
	EXPECT_NEAR(17702.8, test, 5.0e-2);
	test = convert<meters, chains>(1.0);
	EXPECT_NEAR(0.0497097, test, 5.0e-7);

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
	test = convert<kilograms, mass::ounces>(0.0283495);
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = convert<kilograms, carats>(0.0002);
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = convert<slugs, kilograms>(1.0);
	EXPECT_NEAR(14.593903, test, 5.0e-7);

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
	test = convert<angle::radians, angle::arcminutes>(0.000290888);
	EXPECT_NEAR(0.99999928265913, test, 5.0e-8);
	test = convert<angle::radians, angle::arcseconds>(4.8481e-6);
	EXPECT_NEAR(0.999992407, test, 5.0e-10);
	test = convert<angle::radians, angle::turns>(6.28319);
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = convert<angle::radians, angle::mils>(0.00015625);
	EXPECT_NEAR(1.0, test, 5.0e-9);
	test = convert<angle::radians, angle::gradians>(0.015708);
	EXPECT_NEAR(1.0, test, 5.0e-6);

	test = convert<angle::radians, angle::radians>(2.1);
	EXPECT_NEAR(2.1, test, 5.0e-6);
	test = convert<angle::arcseconds, angle::gradians>(2.1);
	EXPECT_NEAR(0.000648148, test, 5.0e-6);
	test = convert<angle::radians, angle::degrees>(units::constants::PI);
	EXPECT_NEAR(180.0, test, 5.0e-6);
	test = convert<angle::degrees, angle::radians>(90.0);
	EXPECT_NEAR(constants::PI / 2, test, 5.0e-6);
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
	bool same;

	same = std::is_same<base_unit_of<steradians>, base_unit_of<degrees_squared>>::value;
	EXPECT_TRUE(same);

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
	EXPECT_NEAR(1.0 / (4 * constants::PI), test, 5.0e-5);
	test = convert<spats, steradians>(1.0 / (4 * constants::PI));
	EXPECT_NEAR(1.0, test, 5.0e-14);
	test = convert<spats, degrees_squared>(1.0 / (4 * constants::PI));
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

TEST_F(UnitTest, velocityConversion)
{
	double test;
	bool same;

	same = std::is_same<meters_per_second, unit<std::ratio<1>, category::velocity_unit>>::value;
	EXPECT_TRUE(same);
	same = units::is_convertible_unit<miles_per_hour, meters_per_second>::value;
	EXPECT_TRUE(same);

	test = convert<meters_per_second, miles_per_hour>(1250.0);
	EXPECT_NEAR(2796.17, test, 5.0e-3);
	test = convert<feet_per_second, kilometers_per_hour>(2796.17);
	EXPECT_NEAR(3068.181418, test, 5.0e-7);
	test = convert<knots, miles_per_hour>(600.0);
	EXPECT_NEAR(690.468, test, 5.0e-4);
	test = convert<miles_per_hour, feet_per_second>(120.0);
	EXPECT_NEAR(176.0, test, 5.0e-5);
	test = convert<feet_per_second, meters_per_second>(10.0);
	EXPECT_NEAR(3.048, test, 5.0e-5);
}

TEST_F(UnitTest, angularVelocityConversion)
{
	double test;
	bool same;

	same = std::is_same<radians_per_second, unit<std::ratio<1>, category::angular_velocity_unit>>::value;
	EXPECT_TRUE(same);
	same = units::is_convertible_unit<rpm, radians_per_second>::value;
	EXPECT_TRUE(same);

	test = convert<radians_per_second, milliarcseconds_per_year>(1.0);
	EXPECT_NEAR(6.504e15, test, 1.0e12);
	test = convert<degrees_per_second, radians_per_second>(1.0);
	EXPECT_NEAR(0.0174533, test, 5.0e-8);
	test = convert<rpm, radians_per_second>(1.0);
	EXPECT_NEAR(0.10471975512, test, 5.0e-13);
	test = convert<milliarcseconds_per_year, radians_per_second>(1.0);
	EXPECT_NEAR(1.537e-16, test, 5.0e-20);
}

TEST_F(UnitTest, accelerationConversion)
{
	double test;

	test = convert<standard_gravity, meters_per_second_squared>(1.0);
	EXPECT_NEAR(9.80665, test, 5.0e-10);
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
	test = convert<square_miles, square_kilometers>(10.0);
	EXPECT_NEAR(25.8999, test, 5.0e-5);
	test = convert<square_inch, square_meter>(4.0);
	EXPECT_NEAR(0.00258064, test, 5.0e-9);
	test = convert<acre, square_foot>(5.0);
	EXPECT_NEAR(217800.0, test, 5.0e-5);
	test = convert<square_meter, square_foot>(1.0);
	EXPECT_NEAR(10.7639, test, 5.0e-5);
}

TEST_F(UnitTest, pressureConversion)
{
	double test;

	test = convert<pascals, torr>(1.0);
	EXPECT_NEAR(0.00750062, test, 5.0e-5);
	test = convert<bar, psi>(2.2);
	EXPECT_NEAR(31.9083, test, 5.0e-5);
	test = convert<atmospheres, bar>(4.0);
	EXPECT_NEAR(4.053, test, 5.0e-5);
	test = convert<torr, pascals>(800.0);
	EXPECT_NEAR(106657.89474, test, 5.0e-5);
	test = convert<psi, atmospheres>(38.0);
	EXPECT_NEAR(2.58575, test, 5.0e-5);
	test = convert<psi, pascals>(1.0);
	EXPECT_NEAR(6894.76, test, 5.0e-3);
	test = convert<pascals, bar>(0.25);
	EXPECT_NEAR(2.5e-6, test, 5.0e-5);
	test = convert<torr, atmospheres>(9.0);
	EXPECT_NEAR(0.0118421, test, 5.0e-8);
	test = convert<bar, torr>(12.0);
	EXPECT_NEAR(9000.74, test, 5.0e-3);
	test = convert<atmospheres, psi>(1.0);
	EXPECT_NEAR(14.6959, test, 5.0e-5);
}
	
TEST_F(UnitTest, chargeConversion)
{
	double test;

	test = convert<coulombs, ampere_hours>(4.0);
	EXPECT_NEAR(0.00111111, test, 5.0e-9);
	test = convert<ampere_hours, coulombs>(1.0);
	EXPECT_NEAR(3600.0, test, 5.0e-6);
}

TEST_F(UnitTest, energyConversion)
{
	double test;

	test = convert<joules, calories>(8000.000464);
	EXPECT_NEAR(1912.046, test, 5.0e-4);
	test = convert<therms, joules>(12.0);
	EXPECT_NEAR(1.266e+9, test, 5.0e5);
	test = convert<megajoules, watt_hours>(100.0);
	EXPECT_NEAR(27777.778, test, 5.0e-4);
	test = convert<kilocalories, megajoules>(56.0);
	EXPECT_NEAR(0.234304, test, 5.0e-7);
	test = convert<kilojoules, therms>(56.0);
	EXPECT_NEAR(0.000530904, test, 5.0e-5);
	test = convert<british_thermal_units, kilojoules>(18.56399995447);
	EXPECT_NEAR(19.5860568, test, 5.0e-5);
	test = convert<calories, energy::foot_pounds>(18.56399995447);
	EXPECT_NEAR(57.28776190423856, test, 5.0e-5);
	test = convert<megajoules, calories>(1.0);
	EXPECT_NEAR(239006.0, test, 5.0e-1);
	test = convert<kilocalories, kilowatt_hours>(2.0);
	EXPECT_NEAR(0.00232444, test, 5.0e-9);
	test = convert<therms, kilocalories>(0.1);
	EXPECT_NEAR(2521.04, test, 5.0e-3);
	test = convert<watt_hours, megajoules>(67.0);
	EXPECT_NEAR(0.2412, test, 5.0e-5);
	test = convert<british_thermal_units, watt_hours>(100.0);
	EXPECT_NEAR(29.3071, test, 5.0e-5);
	test = convert<calories, BTU>(100.0);
	EXPECT_NEAR(0.396567, test, 5.0e-5);
}

TEST_F(UnitTest, powerConversion)
{
	double test;

	test = convert<compound_unit<energy::foot_pounds, inverse<seconds>>, watts>(550.0);
	EXPECT_NEAR(745.7, test, 5.0e-2);
	test = convert<watts, gigawatts>(1000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-4);
	test = convert<microwatts, watts>(200000.0);
	EXPECT_NEAR(0.2, test, 5.0e-4);
	test = convert<horsepower, watts>(100.0);
	EXPECT_NEAR(74570.0, test, 5.0e-1);
	test = convert<horsepower, megawatts>(5.0);
	EXPECT_NEAR(0.0037284994, test, 5.0e-7);
	test = convert<kilowatts, horsepower>(232.0);
	EXPECT_NEAR(311.117, test, 5.0e-4);
	test = convert<milliwatts, horsepower>(1001.0);
	EXPECT_NEAR(0.001342363, test, 5.0e-9);
}

TEST_F(UnitTest, voltageConversion)
{
	double test;

	test = convert<volts, millivolts>(10.0);
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = convert<picovolts, volts>(1000000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<nanovolts, volts>(1000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<microvolts, volts>(1000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<millivolts, volts>(1000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<kilovolts, volts>(0.001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<megavolts, volts>(0.000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<gigavolts, volts>(0.000000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<statvolts, volts>(299.792458);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<millivolts, statvolts>(1000.0);
	EXPECT_NEAR(299.792458, test, 5.0e-5);
	test = convert<abvolts, nanovolts>(0.1);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<microvolts, abvolts>(0.01);
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(UnitTest, capacitanceConversion)
{
	double test;

	test = convert<farads, millifarads>(10.0);
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = convert<picofarads, farads>(1000000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<nanofarads, farads>(1000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<microfarads, farads>(1000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<millifarads, farads>(1000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<kilofarads, farads>(0.001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<megafarads, farads>(0.000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<gigafarads, farads>(0.000000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(UnitTest, impedanceConversion)
{
	double test;

	test = convert<ohms, milliohms>(10.0);
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = convert<picoohms, ohms>(1000000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<nanoohms, ohms>(1000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<microohms, ohms>(1000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<milliohms, ohms>(1000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<kiloohms, ohms>(0.001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<megaohms, ohms>(0.000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<gigaohms, ohms>(0.000000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(UnitTest, conductanceConversion)
{
	double test;

	test = convert<siemens, millisiemens>(10.0);
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = convert<picosiemens, siemens>(1000000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<nanosiemens, siemens>(1000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<microsiemens, siemens>(1000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<millisiemens, siemens>(1000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<kilosiemens, siemens>(0.001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<megasiemens, siemens>(0.000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<gigasiemens, siemens>(0.000000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(UnitTest, magneticFluxConversion)
{
	double test;

	test = convert<webers, milliwebers>(10.0);
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = convert<picowebers, webers>(1000000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<nanowebers, webers>(1000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<microwebers, webers>(1000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<milliwebers, webers>(1000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<kilowebers, webers>(0.001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<megawebers, webers>(0.000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<gigawebers, webers>(0.000000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<maxwells, webers>(100000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<nanowebers, maxwells>(10.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(UnitTest, magneticFieldStrengthConversion)
{
	double test;

	test = convert<teslas, milliteslas>(10.0);
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = convert<picoteslas, teslas>(1000000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<nanoteslas, teslas>(1000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<microteslas, teslas>(1000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<milliteslas, teslas>(1000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<kiloteslas, teslas>(0.001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<megateslas, teslas>(0.000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<gigateslas, teslas>(0.000000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<gauss, teslas>(10000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<nanoteslas, gauss>(100000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(UnitTest, inductanceConversion)
{
	double test;

	test = convert<henrys, millihenrys>(10.0);
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = convert<picohenrys, henrys>(1000000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<nanohenrys, henrys>(1000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<microhenrys, henrys>(1000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<millihenrys, henrys>(1000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<kilohenrys, henrys>(0.001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<megahenrys, henrys>(0.000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<gigahenrys, henrys>(0.000000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(UnitTest, luminousFluxConversion)
{
	double test;

	test = convert<lumens, millilumens>(10.0);
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = convert<picolumens, lumens>(1000000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<nanolumens, lumens>(1000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<microlumens, lumens>(1000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<millilumens, lumens>(1000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<kilolumens, lumens>(0.001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<megalumens, lumens>(0.000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<gigalumens, lumens>(0.000000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(UnitTest, illuminanceConversion)
{
	double test;

	test = convert<luxes, milliluxes>(10.0);
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = convert<picoluxes, luxes>(1000000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<nanoluxes, luxes>(1000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<microluxes, luxes>(1000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<milliluxes, luxes>(1000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<kiloluxes, luxes>(0.001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<megaluxes, luxes>(0.000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<gigaluxes, luxes>(0.000000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);

	test = convert<footcandles, luxes>(0.092903);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<lux, lumens_per_square_inch>(1550.0031000062);
	EXPECT_NEAR(1.0, test, 5.0e-13);
	test = convert<phots, luxes>(0.0001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(UnitTest, radiationConversion)
{
	double test;

	test = convert<becquerels, millibecquerels>(10.0);
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = convert<picobecquerels, becquerels>(1000000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<nanobecquerels, becquerels>(1000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<microbecquerels, becquerels>(1000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<millibecquerels, becquerels>(1000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<kilobecquerels, becquerels>(0.001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<megabecquerels, becquerels>(0.000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<gigabecquerels, becquerels>(0.000000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);

	test = convert<grays, milligrays>(10.0);
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = convert<picograys, grays>(1000000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<nanograys, grays>(1000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<micrograys, grays>(1000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<milligrays, grays>(1000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<kilograys, grays>(0.001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<megagrays, grays>(0.000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<gigagrays, grays>(0.000000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);

	test = convert<sieverts, millisieverts>(10.0);
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = convert<picosieverts, sieverts>(1000000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<nanosieverts, sieverts>(1000000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<microsieverts, sieverts>(1000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<millisieverts, sieverts>(1000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<kilosieverts, sieverts>(0.001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<megasieverts, sieverts>(0.000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<gigasieverts, sieverts>(0.000000001);
	EXPECT_NEAR(1.0, test, 5.0e-5);

	test = convert<becquerels, curies>(37.0e9);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<becquerels, rutherfords>(1000000.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<rads, grays>(100.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(UnitTest, torqueConversions)
{
	double test;

	test = convert<torque::foot_pounds, newton_meter>(1.0);
	EXPECT_NEAR(1.355817948, test, 5.0e-5);
	test = convert<inch_pounds, newton_meter>(1.0);
	EXPECT_NEAR(0.112984829, test, 5.0e-5);
	test = convert<foot_poundals, newton_meter>(1.0);
	EXPECT_NEAR(4.214011009e-2, test, 5.0e-5);
	test = convert<meter_kilograms, newton_meter>(1.0);
	EXPECT_NEAR(9.80665, test, 5.0e-5);
	test = convert<inch_pound, meter_kilogram>(86.79616930855788);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<foot_poundals, inch_pound>(2.681170713);
	EXPECT_NEAR(1.0, test, 5.0e-5);

}

TEST_F(UnitTest, volumeConversion)
{
	double test;

	test = convert<cubic_meters, cubic_meter>(1.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<cubic_millimeters, cubic_meter>(1.0);
	EXPECT_NEAR(1.0e-9, test, 5.0e-5);
	test = convert<cubic_kilometers, cubic_meter>(1.0);
	EXPECT_NEAR(1.0e9, test, 5.0e-5);
	test = convert<liters, cubic_meter>(1.0);
	EXPECT_NEAR(0.001, test, 5.0e-5);
	test = convert<milliliters, cubic_meter>(1.0);
	EXPECT_NEAR(1.0e-6, test, 5.0e-5);
	test = convert<cubic_inches, cubic_meter>(1.0);
	EXPECT_NEAR(1.6387e-5, test, 5.0e-10);
	test = convert<cubic_feet, cubic_meter>(1.0);
	EXPECT_NEAR(0.0283168, test, 5.0e-8);
	test = convert<cubic_yards, cubic_meter>(1.0);
	EXPECT_NEAR(0.764555, test, 5.0e-7);
	test = convert<cubic_miles, cubic_meter>(1.0);
	EXPECT_NEAR(4.168e+9, test, 5.0e5);
	test = convert<gallons, cubic_meter>(1.0);
	EXPECT_NEAR(0.00378541, test, 5.0e-8);
	test = convert<quarts, cubic_meter>(1.0);
	EXPECT_NEAR(0.000946353, test, 5.0e-10);
	test = convert<pints, cubic_meter>(1.0);
	EXPECT_NEAR(0.000473176, test, 5.0e-10);
	test = convert<cups, cubic_meter>(1.0);
	EXPECT_NEAR(0.00024, test, 5.0e-6);
	test = convert<volume::ounces, cubic_meter>(1.0);
	EXPECT_NEAR(2.9574e-5, test, 5.0e-5);
	test = convert<barrels, cubic_meter>(1.0);
	EXPECT_NEAR(0.158987294928, test, 5.0e-13);
	test = convert<bushels, cubic_meter>(1.0);
	EXPECT_NEAR(0.0352391, test, 5.0e-8);
	test = convert<cords, cubic_meter>(1.0);
	EXPECT_NEAR(3.62456, test, 5.0e-6);
	test = convert<cubic_fathoms, cubic_meter>(1.0);
	EXPECT_NEAR(6.11644, test, 5.0e-6);
	test = convert<tablespoons, cubic_meter>(1.0);
	EXPECT_NEAR(1.4787e-5, test, 5.0e-10);
	test = convert<teaspoons, cubic_meter>(1.0);
	EXPECT_NEAR(4.9289e-6, test, 5.0e-11);
	test = convert<pinches, cubic_meter>(1.0);
	EXPECT_NEAR(616.11519921875e-9, test, 5.0e-20);
	test = convert<dashes, cubic_meter>(1.0);
	EXPECT_NEAR(308.057599609375e-9, test, 5.0e-20);
	test = convert<drops, cubic_meter>(1.0);
	EXPECT_NEAR(82.14869322916e-9, test, 5.0e-9);
	test = convert<fifths, cubic_meter>(1.0);
	EXPECT_NEAR(0.00075708236, test, 5.0e-12);
	test = convert<drams, cubic_meter>(1.0);
	EXPECT_NEAR(3.69669e-6, test, 5.0e-12);
	test = convert<gills, cubic_meter>(1.0);
	EXPECT_NEAR(0.000118294, test, 5.0e-10);
	test = convert<pecks, cubic_meter>(1.0);
	EXPECT_NEAR(0.00880977, test, 5.0e-9);
	test = convert<sacks, cubic_meter>(9.4591978);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<shots, cubic_meter>(1.0);
	EXPECT_NEAR(4.43603e-5, test, 5.0e-11);
	test = convert<strikes, cubic_meter>(1.0);
	EXPECT_NEAR(0.07047814033376 , test, 5.0e-5);
	test = convert<volume::ounces, milliliters>(1.0);
	EXPECT_NEAR(29.5735, test, 5.0e-5);
}

TEST_F(UnitTest, densityConversion)
{
	double test;

	test = convert<kilogram_per_cubic_meter, kilograms_per_cubic_meter>(1.0);
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = convert<gram_per_milliliter, kilograms_per_cubic_meter>(1.0);
	EXPECT_NEAR(1000.0, test, 5.0e-5);
	test = convert<kilogram_per_liter, kilograms_per_cubic_meter>(1.0);
	EXPECT_NEAR(1000.0, test, 5.0e-5);
	test = convert<ounce_per_cubic_foot, kilograms_per_cubic_meter>(1.0);
	EXPECT_NEAR(1.001153961, test, 5.0e-10);
	test = convert<ounce_per_cubic_inch, kilograms_per_cubic_meter>(1.0);
	EXPECT_NEAR(1.729994044e3, test, 5.0e-7);
	test = convert<ounce_per_gallon, kilograms_per_cubic_meter>(1.0);
	EXPECT_NEAR(7.489151707, test, 5.0e-10);
	test = convert<pound_per_cubic_foot, kilograms_per_cubic_meter>(1.0);
	EXPECT_NEAR(16.01846337, test, 5.0e-9);
	test = convert<pound_per_cubic_inch, kilograms_per_cubic_meter>(1.0);
	EXPECT_NEAR(2.767990471e4, test, 5.0e-6);
	test = convert<pound_per_gallon, kilograms_per_cubic_meter>(1.0);
	EXPECT_NEAR(119.8264273, test, 5.0e-8);
	test = convert<slug_per_cubic_foot, kilograms_per_cubic_meter>(1.0);
	EXPECT_NEAR(515.3788184, test, 5.0e-6);
}

TEST_F(UnitTest, concentrationConversion)
{
	double test;

	test = ppm_t(1.0);
	EXPECT_NEAR(1.0e-6, test, 5.0e-12);
	test = ppb_t(1.0);
	EXPECT_NEAR(1.0e-9, test, 5.0e-12);
	test = ppt_t(1.0);
	EXPECT_NEAR(1.0e-12, test, 5.0e-12);
	test = percent_t(18.0);
	EXPECT_NEAR(0.18, test, 5.0e-12);
}

TEST_F(UnitTest, testConstants)
{
	// scalar constants
	EXPECT_NEAR(3.14159, constants::pi, 5.0e-6);

	// constants with units
	EXPECT_NEAR(299792458, constants::c(), 5.0e-9);
	EXPECT_NEAR(6.67408e-11, constants::G(), 5.0e-17);
	EXPECT_NEAR(6.626070040e-34, constants::h(), 5.0e-44);
	EXPECT_NEAR(1.256637061e-6, constants::mu0(), 5.0e-16);
	EXPECT_NEAR(8.854187817e-12, constants::epsilon0(), 5.0e-21);
	EXPECT_NEAR(376.73031346177, constants::Z0(), 5.0e-12);
	EXPECT_NEAR(8.987551787e9, constants::k_e(), 5.0e-1);
	EXPECT_NEAR(1.602176565e-19, constants::e(), 5.0e-29);
	EXPECT_NEAR(9.10938291e-31, constants::m_e(), 5.0e-40);
	EXPECT_NEAR(1.672621777e-27, constants::m_p(), 5.0e-37);
	EXPECT_NEAR(9.27400968e-24, constants::mu_B(), 5.0e-30);
	EXPECT_NEAR(6.02214129e23, constants::N_A(), 5.0e14);
	EXPECT_NEAR(8.3144621, constants::R(), 5.0e-8);
	EXPECT_NEAR(1.3806488e-23, constants::k_B(), 5.0e-31);
	EXPECT_NEAR(96485.3365, constants::F(), 5.0e-5);
	EXPECT_NEAR(5.670373e-8, constants::sigma(), 5.0e-14);
}

TEST_F(UnitTest, radarRangeEquation)
{

	watt_t			P_t;				// transmit power
	scalar_t		G;					// gain
	meter_t			lambda;				// wavelength
	square_meter_t	sigma;				// radar cross section
	meter_t			R;					// range
	kelvin_t		T_s;				// system noise temp
	hertz_t			B_n;				// bandwidth
	scalar_t		L;					// loss

	P_t = megawatt_t(1.4);
	G = dB_t(33.0);
	lambda = constants::c / megahertz_t(2800);
	sigma = square_meter_t(1.0);
	R = meter_t(111000.0);
	T_s = kelvin_t(950.0);
	B_n = megahertz_t(1.67);
	L = dB_t(8.0);

	scalar_t SNR =	(P_t * units::pow<2>(G) * units::pow<2>(lambda) * sigma) / 
					(units::pow<3>(4 * constants::pi) * units::pow<4>(R) * constants::k_B * T_s * B_n * L);

	EXPECT_NEAR(1.535, SNR(), 5.0e-4);
}
int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}