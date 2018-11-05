#include <array>
#include <chrono>
#include <gtest/gtest.h>
#include <ratio>
#include <string>
#include <type_traits>
#include <units.h>

using namespace units;
using namespace units::literals;

namespace
{
	class TypeTraits : public ::testing::Test
	{
	protected:
		TypeTraits(){};
		virtual ~TypeTraits(){};
		void SetUp() override{};
		void TearDown() override{};
	};

	class STDTypeTraits : public ::testing::Test
	{
	protected:
		STDTypeTraits(){};
		virtual ~STDTypeTraits(){};
		void SetUp() override{};
		void TearDown() override{};
	};

	class STDSpecializations : public ::testing::Test
	{
	protected:
		STDSpecializations(){};
		virtual ~STDSpecializations(){};
		void SetUp() override{};
		void TearDown() override{};
	};

	class UnitManipulators : public ::testing::Test
	{
	protected:
		UnitManipulators(){};
		virtual ~UnitManipulators(){};
		void SetUp() override{};
		void TearDown() override{};
	};

	class UnitType : public ::testing::Test
	{
	protected:
		UnitType(){};
		virtual ~UnitType(){};
		void SetUp() override{};
		void TearDown() override{};
	};

	class ConversionFactor : public ::testing::Test
	{
	protected:
		ConversionFactor(){};
		virtual ~ConversionFactor(){};
		void SetUp() override{};
		void TearDown() override{};
	};

	class UnitMath : public ::testing::Test
	{
	protected:
		UnitMath(){};
		virtual ~UnitMath(){};
		void SetUp() override{};
		void TearDown() override{};
	};

	class Constexpr : public ::testing::Test
	{
	protected:
		Constexpr(){};
		virtual ~Constexpr(){};
		void SetUp() override{};
		void TearDown() override{};
	};

	class CaseStudies : public ::testing::Test
	{
	protected:
		CaseStudies(){};
		virtual ~CaseStudies(){};
		void SetUp() override{};
		void TearDown() override{};
	};

	// Tests that two units have the same conversion ratio to the same dimension.
	constexpr auto has_equivalent_conversion_factor = [](const auto& t, const auto& u) {
		using T = std::decay_t<decltype(t)>;
		using U = std::decay_t<decltype(u)>;
		return units::traits::is_convertible_unit_v<T, U> &&
			std::ratio_equal_v<typename T::conversion_factor::conversion_ratio,
				typename U::conversion_factor::conversion_ratio>;
	};
} // namespace

TEST_F(TypeTraits, isRatio)
{
	EXPECT_TRUE(traits::is_ratio_v<std::ratio<1>>);
	EXPECT_FALSE(traits::is_ratio_v<double>);
}

TEST_F(TypeTraits, ratio_sqrt)
{
	using rt2 = ratio_sqrt<std::ratio<2>>;
	EXPECT_LT(std::abs(std::sqrt(2 / (double)1) - rt2::num / (double)rt2::den), 5e-9);

	using rt4 = ratio_sqrt<std::ratio<4>>;
	EXPECT_LT(std::abs(std::sqrt(4 / (double)1) - rt4::num / (double)rt4::den), 5e-9);

	using rt10 = ratio_sqrt<std::ratio<10>>;
	EXPECT_LT(std::abs(std::sqrt(10 / (double)1) - rt10::num / (double)rt10::den), 5e-9);

	using rt30 = ratio_sqrt<std::ratio<30>>;
	EXPECT_LT(std::abs(std::sqrt(30 / (double)1) - rt30::num / (double)rt30::den), 5e-9);

	using rt61 = ratio_sqrt<std::ratio<61>>;
	EXPECT_LT(std::abs(std::sqrt(61 / (double)1) - rt61::num / (double)rt61::den), 5e-9);

	using rt100 = ratio_sqrt<std::ratio<100>>;
	EXPECT_LT(std::abs(std::sqrt(100 / (double)1) - rt100::num / (double)rt100::den), 5e-9);

	using rt1000 = ratio_sqrt<std::ratio<1000>>;
	EXPECT_LT(std::abs(std::sqrt(1000 / (double)1) - rt1000::num / (double)rt1000::den), 5e-9);

	using rt10000 = ratio_sqrt<std::ratio<10000>>;
	EXPECT_LT(std::abs(std::sqrt(10000 / (double)1) - rt10000::num / (double)rt10000::den), 5e-9);
}

TEST_F(TypeTraits, is_conversion_factor)
{
	EXPECT_FALSE(traits::is_conversion_factor_v<std::ratio<1>>);
	EXPECT_FALSE(traits::is_conversion_factor_v<double>);
	EXPECT_TRUE(traits::is_conversion_factor_v<meter_conversion_factor>);
	EXPECT_TRUE(traits::is_conversion_factor_v<foot_conversion_factor>);
	EXPECT_TRUE(traits::is_conversion_factor_v<degree_squared_conversion_factor>);
	EXPECT_TRUE(traits::is_conversion_factor_v<meter<double>>);
}

TEST_F(TypeTraits, is_unit)
{
	EXPECT_FALSE(traits::is_unit_v<std::ratio<1>>);
	EXPECT_FALSE(traits::is_unit_v<double>);
	EXPECT_FALSE(traits::is_unit_v<meter_conversion_factor>);
	EXPECT_FALSE(traits::is_unit_v<foot_conversion_factor>);
	EXPECT_FALSE(traits::is_unit_v<degree_squared_conversion_factor>);
	EXPECT_TRUE(traits::is_unit_v<meter<double>>);
}

TEST_F(TypeTraits, unit_base)
{
	EXPECT_TRUE((std::is_same_v<traits::unit_base_t<unit<dimensionless_unit, int>>, unit<dimensionless_unit, int>>));
	EXPECT_TRUE((std::is_same_v<traits::unit_base_t<dimensionless<int>>, unit<dimensionless_unit, int>>));
	EXPECT_TRUE((std::is_same_v<traits::unit_base_t<const volatile unit<dimensionless_unit, int>>,
		unit<dimensionless_unit, int>>));
	EXPECT_TRUE(
		(std::is_same_v<traits::unit_base_t<const volatile dimensionless<int>>, unit<dimensionless_unit, int>>));
	EXPECT_TRUE((std::is_same_v<traits::unit_base_t<meter<double>>, unit<meter_conversion_factor, double>>));
	EXPECT_TRUE(
		(std::is_same_v<traits::unit_base_t<const volatile meter<double>>, unit<meter_conversion_factor, double>>));
}

TEST_F(TypeTraits, replace_underlying)
{
	EXPECT_TRUE((std::is_same_v<traits::replace_underlying_t<unit<dimensionless_unit, int>, int>,
		unit<dimensionless_unit, int>>));
	EXPECT_TRUE((std::is_same_v<traits::replace_underlying_t<unit<dimensionless_unit, int>, double>,
		unit<dimensionless_unit, double>>));
	EXPECT_TRUE((std::is_same_v<traits::replace_underlying_t<dimensionless<int>, int>, dimensionless<int>>));
	EXPECT_TRUE((std::is_same_v<traits::replace_underlying_t<dimensionless<int>, double>, dimensionless<double>>));
}

TEST_F(TypeTraits, conversion_factor_traits)
{
	EXPECT_TRUE((std::is_same_v<void, traits::conversion_factor_traits<double>::conversion_ratio>));
	EXPECT_FALSE((std::is_same_v<void, traits::conversion_factor_traits<meter_conversion_factor>::conversion_ratio>));
}

TEST_F(TypeTraits, unit_traits)
{
	EXPECT_TRUE((std::is_same_v<void, traits::unit_traits<double>::underlying_type>));
	EXPECT_TRUE((std::is_same_v<double, traits::unit_traits<meter<double>>::underlying_type>));
	EXPECT_TRUE((std::is_same_v<void, traits::unit_traits<double>::value_type>));
	EXPECT_TRUE((std::is_same_v<double, traits::unit_traits<meter<double>>::value_type>));
}

TEST_F(TypeTraits, is_same_dimension)
{
	EXPECT_TRUE((traits::is_same_dimension_v<meter_conversion_factor, meter_conversion_factor>));
	EXPECT_TRUE((traits::is_same_dimension_v<meter_conversion_factor, astronomical_unit_conversion_factor>));
	EXPECT_TRUE((traits::is_same_dimension_v<meter_conversion_factor, parsec_conversion_factor>));

	EXPECT_TRUE((traits::is_same_dimension_v<meter_conversion_factor, meter_conversion_factor>));
	EXPECT_TRUE((traits::is_same_dimension_v<astronomical_unit_conversion_factor, meter_conversion_factor>));
	EXPECT_TRUE((traits::is_same_dimension_v<parsec_conversion_factor, meter_conversion_factor>));
	EXPECT_TRUE((traits::is_same_dimension_v<year_conversion_factor, week_conversion_factor>));

	EXPECT_FALSE((traits::is_same_dimension_v<meter_conversion_factor, second_conversion_factor>));
	EXPECT_FALSE((traits::is_same_dimension_v<second_conversion_factor, meter_conversion_factor>));
	EXPECT_FALSE((traits::is_same_dimension_v<year_conversion_factor, meter_conversion_factor>));
}

TEST_F(TypeTraits, inverse)
{
	double test;

	using htz         = traits::strong_t<inverse<second_conversion_factor>>;
	bool shouldBeTrue = std::is_same_v<htz, hertz_conversion_factor>;
	EXPECT_TRUE(shouldBeTrue);

	test = unit<inverse<fahrenheit_t>>(unit<inverse<celsius_t>>(1.0)).value();
	EXPECT_NEAR(5.0 / 9.0, test, 5.0e-5);

	test = unit<inverse<fahrenheit_t>>(unit<inverse<kelvin_t>>(6.0)).value();
	EXPECT_NEAR(10.0 / 3.0, test, 5.0e-5);
}

TEST_F(TypeTraits, strong)
{
	EXPECT_TRUE(
		(std::is_same_v<dimensionless_unit, traits::strong_t<detail::conversion_factor_base_t<dimensionless_unit>>>));
	EXPECT_TRUE((std::is_same_v<meter_conversion_factor,
		traits::strong_t<conversion_factor<std::ratio<1>, dimension::length>>>));
	EXPECT_TRUE((std::is_same_v<kilometer_conversion_factor, traits::strong_t<kilo<meter<int>>>>));
	EXPECT_TRUE((std::is_same_v<square_meter_conversion_factor, traits::strong_t<squared<meter_conversion_factor>>>));
}

TEST_F(TypeTraits, dimension_of)
{
	using dim = traits::dimension_of_t<year_conversion_factor>;

	EXPECT_TRUE((std::is_same_v<dim, dimension::time>));
	EXPECT_FALSE((std::is_same_v<dim, dimension::length>));
	EXPECT_FALSE((std::is_same_v<dim, units::time::day<int>>));

	using dim2 = typename traits::conversion_factor_traits<
		typename traits::unit_traits<decltype(meters_per_second<double>(5))>::conversion_factor>::dimension_type;

	EXPECT_TRUE((std::is_same_v<dim2, dimension::velocity>));
	EXPECT_FALSE((std::is_same_v<dim2, dimension::time>));
	EXPECT_FALSE((std::is_same_v<dim2, units::velocity::miles_per_hour<int>>));
}

TEST_F(TypeTraits, has_linear_scale)
{
	EXPECT_TRUE((traits::has_linear_scale_v<dimensionless<double>>));
	EXPECT_TRUE((traits::has_linear_scale_v<meter<double>>));
	EXPECT_TRUE((traits::has_linear_scale_v<foot<double>>));
	EXPECT_TRUE((traits::has_linear_scale_v<watt<double>, dimensionless<double>>));
	EXPECT_TRUE((traits::has_linear_scale_v<dimensionless<double>, meter<double>>));
	EXPECT_TRUE((traits::has_linear_scale_v<meters_per_second<double>>));
	EXPECT_FALSE((traits::has_linear_scale_v<dB<double>>));
	EXPECT_FALSE((traits::has_linear_scale_v<dB<double>, meters_per_second<double>>));
}

TEST_F(TypeTraits, has_decibel_scale)
{
	EXPECT_FALSE((traits::has_decibel_scale_v<dimensionless<double>>));
	EXPECT_FALSE((traits::has_decibel_scale_v<meter<double>>));
	EXPECT_FALSE((traits::has_decibel_scale_v<foot<double>>));
	EXPECT_TRUE((traits::has_decibel_scale_v<dB<double>>));
	EXPECT_TRUE((traits::has_decibel_scale_v<dBW<double>>));

	EXPECT_TRUE((traits::has_decibel_scale_v<dBW<double>, dB<double>>));
	EXPECT_TRUE((traits::has_decibel_scale_v<dBW<double>, dBm<double>>));
	EXPECT_TRUE((traits::has_decibel_scale_v<dB<double>, dB<double>>));
	EXPECT_TRUE((traits::has_decibel_scale_v<dB<double>, dB<double>, dB<double>>));
	EXPECT_FALSE((traits::has_decibel_scale_v<dB<double>, dB<double>, meter<double>>));
	EXPECT_FALSE((traits::has_decibel_scale_v<meter<double>, dB<double>>));
}

TEST_F(TypeTraits, is_dimensionless_unit)
{
	EXPECT_TRUE((traits::is_dimensionless_unit_v<dimensionless<double>>));
	EXPECT_TRUE((traits::is_dimensionless_unit_v<const dimensionless<double>>));
	EXPECT_TRUE((traits::is_dimensionless_unit_v<const dimensionless<double>&>));
	EXPECT_TRUE((traits::is_dimensionless_unit_v<dimensionless<double>>));
	EXPECT_TRUE((traits::is_dimensionless_unit_v<dB<double>>));
	EXPECT_TRUE((traits::is_dimensionless_unit_v<ppm<double>>));
	EXPECT_FALSE((traits::is_dimensionless_unit_v<meter<double>>));
	EXPECT_FALSE((traits::is_dimensionless_unit_v<dBW<double>>));

	EXPECT_TRUE((std::is_arithmetic_v<const double>));
}

TEST_F(TypeTraits, is_length_unit)
{
	EXPECT_FALSE((traits::is_length_unit_v<double>));
	EXPECT_TRUE((traits::is_length_unit_v<meter<double>>));
	EXPECT_TRUE((traits::is_length_unit_v<const meter<double>>));
	EXPECT_TRUE((traits::is_length_unit_v<const meter<double>&>));
	EXPECT_TRUE((traits::is_length_unit_v<cubit<double>>));
	EXPECT_FALSE((traits::is_length_unit_v<year<double>>));
}

TEST_F(TypeTraits, is_mass_unit)
{
	EXPECT_FALSE((traits::is_mass_unit_v<double>));
	EXPECT_TRUE((traits::is_mass_unit_v<kilogram<double>>));
	EXPECT_TRUE((traits::is_mass_unit_v<const kilogram<double>>));
	EXPECT_TRUE((traits::is_mass_unit_v<const kilogram<double>&>));
	EXPECT_TRUE((traits::is_mass_unit_v<stone<double>>));
	EXPECT_FALSE((traits::is_mass_unit_v<meter<double>>));
}

TEST_F(TypeTraits, is_time_unit)
{
	EXPECT_FALSE((traits::is_time_unit_v<double>));
	EXPECT_TRUE((traits::is_time_unit_v<second<double>>));
	EXPECT_TRUE((traits::is_time_unit_v<const second<double>>));
	EXPECT_TRUE((traits::is_time_unit_v<const second<double>&>));
	EXPECT_TRUE((traits::is_time_unit_v<year<double>>));
	EXPECT_FALSE((traits::is_time_unit_v<meter<double>>));
}

TEST_F(TypeTraits, is_angle_unit)
{
	EXPECT_FALSE((traits::is_angle_unit_v<double>));
	EXPECT_TRUE((traits::is_angle_unit_v<angle::radian<double>>));
	EXPECT_TRUE((traits::is_angle_unit_v<const angle::radian<double>>));
	EXPECT_TRUE((traits::is_angle_unit_v<const angle::radian<double>&>));
	EXPECT_TRUE((traits::is_angle_unit_v<angle::degree<double>>));
	EXPECT_FALSE((traits::is_angle_unit_v<watt<double>>));
}

TEST_F(TypeTraits, is_current_unit)
{
	EXPECT_FALSE((traits::is_current_unit_v<double>));
	EXPECT_TRUE((traits::is_current_unit_v<current::ampere<double>>));
	EXPECT_TRUE((traits::is_current_unit_v<const current::ampere<double>>));
	EXPECT_TRUE((traits::is_current_unit_v<const current::ampere<double>&>));
	EXPECT_FALSE((traits::is_current_unit_v<volt<double>>));
}

TEST_F(TypeTraits, is_temperature_unit)
{
	EXPECT_FALSE((traits::is_temperature_unit_v<double>));
	EXPECT_TRUE((traits::is_temperature_unit_v<fahrenheit<double>>));
	EXPECT_TRUE((traits::is_temperature_unit_v<const fahrenheit<double>>));
	EXPECT_TRUE((traits::is_temperature_unit_v<const fahrenheit<double>&>));
	EXPECT_TRUE((traits::is_temperature_unit_v<kelvin<double>>));
	EXPECT_FALSE((traits::is_temperature_unit_v<cubit<double>>));
}

TEST_F(TypeTraits, is_substance_unit)
{
	EXPECT_FALSE((traits::is_substance_unit_v<double>));
	EXPECT_TRUE((traits::is_substance_unit_v<substance::mol<double>>));
	EXPECT_TRUE((traits::is_substance_unit_v<const substance::mol<double>>));
	EXPECT_TRUE((traits::is_substance_unit_v<const substance::mol<double>&>));
	EXPECT_FALSE((traits::is_substance_unit_v<year<double>>));
}

TEST_F(TypeTraits, is_luminous_intensity_unit)
{
	EXPECT_FALSE((traits::is_luminous_intensity_unit_v<double>));
	EXPECT_TRUE((traits::is_luminous_intensity_unit_v<candela<double>>));
	EXPECT_TRUE((traits::is_luminous_intensity_unit_v<const candela<double>>));
	EXPECT_TRUE((traits::is_luminous_intensity_unit_v<const candela<double>&>));
	EXPECT_FALSE((traits::is_luminous_intensity_unit_v<rad<double>>));
}

TEST_F(TypeTraits, is_solid_angle_unit)
{
	EXPECT_FALSE((traits::is_solid_angle_unit_v<double>));
	EXPECT_TRUE((traits::is_solid_angle_unit_v<steradian<double>>));
	EXPECT_TRUE((traits::is_solid_angle_unit_v<const steradian<double>>));
	EXPECT_TRUE((traits::is_solid_angle_unit_v<const degree_squared<double>&>));
	EXPECT_FALSE((traits::is_solid_angle_unit_v<angle::degree<double>>));
}

TEST_F(TypeTraits, is_frequency_unit)
{
	EXPECT_FALSE((traits::is_frequency_unit_v<double>));
	EXPECT_TRUE((traits::is_frequency_unit_v<hertz<double>>));
	EXPECT_TRUE((traits::is_frequency_unit_v<const hertz<double>>));
	EXPECT_TRUE((traits::is_frequency_unit_v<const hertz<double>&>));
	EXPECT_FALSE((traits::is_frequency_unit_v<second<double>>));
}

TEST_F(TypeTraits, is_velocity_unit)
{
	EXPECT_FALSE((traits::is_velocity_unit_v<double>));
	EXPECT_TRUE((traits::is_velocity_unit_v<meters_per_second<double>>));
	EXPECT_TRUE((traits::is_velocity_unit_v<const meters_per_second<double>>));
	EXPECT_TRUE((traits::is_velocity_unit_v<const meters_per_second<double>&>));
	EXPECT_TRUE((traits::is_velocity_unit_v<miles_per_hour<double>>));
	EXPECT_FALSE((traits::is_velocity_unit_v<meters_per_second_squared<double>>));
}

TEST_F(TypeTraits, is_acceleration_unit)
{
	EXPECT_FALSE((traits::is_acceleration_unit_v<double>));
	EXPECT_TRUE((traits::is_acceleration_unit_v<meters_per_second_squared<double>>));
	EXPECT_TRUE((traits::is_acceleration_unit_v<const meters_per_second_squared<double>>));
	EXPECT_TRUE((traits::is_acceleration_unit_v<const meters_per_second_squared<double>&>));
	EXPECT_TRUE((traits::is_acceleration_unit_v<standard_gravity<double>>));
	EXPECT_FALSE((traits::is_acceleration_unit_v<inch<double>>));
}

TEST_F(TypeTraits, is_force_unit)
{
	EXPECT_FALSE((traits::is_force_unit_v<double>));
	EXPECT_TRUE((traits::is_force_unit_v<units::force::newton<double>>));
	EXPECT_TRUE((traits::is_force_unit_v<const units::force::newton<double>>));
	EXPECT_TRUE((traits::is_force_unit_v<const units::force::newton<double>&>));
	EXPECT_TRUE((traits::is_force_unit_v<units::force::dyne<double>>));
	EXPECT_FALSE((traits::is_force_unit_v<watt<double>>));
}

TEST_F(TypeTraits, is_pressure_unit)
{
	EXPECT_FALSE((traits::is_pressure_unit_v<double>));
	EXPECT_TRUE((traits::is_pressure_unit_v<pascal<double>>));
	EXPECT_TRUE((traits::is_pressure_unit_v<const pascal<double>>));
	EXPECT_TRUE((traits::is_pressure_unit_v<const pascal<double>&>));
	EXPECT_TRUE((traits::is_pressure_unit_v<atmosphere<double>>));
	EXPECT_FALSE((traits::is_pressure_unit_v<year<double>>));
}

TEST_F(TypeTraits, is_charge_unit)
{
	EXPECT_FALSE((traits::is_charge_unit_v<double>));
	EXPECT_TRUE((traits::is_charge_unit_v<coulomb<double>>));
	EXPECT_TRUE((traits::is_charge_unit_v<const coulomb<double>>));
	EXPECT_TRUE((traits::is_charge_unit_v<const coulomb<double>&>));
	EXPECT_FALSE((traits::is_charge_unit_v<watt<double>>));
}

TEST_F(TypeTraits, is_energy_unit)
{
	EXPECT_FALSE((traits::is_energy_unit_v<double>));
	EXPECT_TRUE((traits::is_energy_unit_v<joule<double>>));
	EXPECT_TRUE((traits::is_energy_unit_v<const joule<double>>));
	EXPECT_TRUE((traits::is_energy_unit_v<const joule<double>&>));
	EXPECT_TRUE((traits::is_energy_unit_v<calorie<double>>));
	EXPECT_FALSE((traits::is_energy_unit_v<watt<double>>));
}

TEST_F(TypeTraits, is_power_unit)
{
	EXPECT_FALSE((traits::is_power_unit_v<double>));
	EXPECT_TRUE((traits::is_power_unit_v<watt<double>>));
	EXPECT_TRUE((traits::is_power_unit_v<const watt<double>>));
	EXPECT_TRUE((traits::is_power_unit_v<const watt<double>&>));
	EXPECT_FALSE((traits::is_power_unit_v<henry<double>>));
}

TEST_F(TypeTraits, is_voltage_unit)
{
	EXPECT_FALSE((traits::is_voltage_unit_v<double>));
	EXPECT_TRUE((traits::is_voltage_unit_v<volt<double>>));
	EXPECT_TRUE((traits::is_voltage_unit_v<const volt<double>>));
	EXPECT_TRUE((traits::is_voltage_unit_v<const volt<double>&>));
	EXPECT_FALSE((traits::is_voltage_unit_v<henry<double>>));
}

TEST_F(TypeTraits, is_capacitance_unit)
{
	EXPECT_FALSE((traits::is_capacitance_unit_v<double>));
	EXPECT_TRUE((traits::is_capacitance_unit_v<farad<double>>));
	EXPECT_TRUE((traits::is_capacitance_unit_v<const farad<double>>));
	EXPECT_TRUE((traits::is_capacitance_unit_v<const farad<double>&>));
	EXPECT_FALSE((traits::is_capacitance_unit_v<ohm<double>>));
}

TEST_F(TypeTraits, is_impedance_unit)
{
	EXPECT_FALSE((traits::is_impedance_unit_v<double>));
	EXPECT_TRUE((traits::is_impedance_unit_v<ohm<double>>));
	EXPECT_TRUE((traits::is_impedance_unit_v<const ohm<double>>));
	EXPECT_TRUE((traits::is_impedance_unit_v<const ohm<double>&>));
	EXPECT_FALSE((traits::is_impedance_unit_v<farad<double>>));
}

TEST_F(TypeTraits, is_conductance_unit)
{
	EXPECT_FALSE((traits::is_conductance_unit_v<double>));
	EXPECT_TRUE((traits::is_conductance_unit_v<siemens<double>>));
	EXPECT_TRUE((traits::is_conductance_unit_v<const siemens<double>>));
	EXPECT_TRUE((traits::is_conductance_unit_v<const siemens<double>&>));
	EXPECT_FALSE((traits::is_conductance_unit_v<volt<double>>));
}

TEST_F(TypeTraits, is_magnetic_flux_unit)
{
	EXPECT_FALSE((traits::is_magnetic_flux_unit_v<double>));
	EXPECT_TRUE((traits::is_magnetic_flux_unit_v<weber<double>>));
	EXPECT_TRUE((traits::is_magnetic_flux_unit_v<const weber<double>>));
	EXPECT_TRUE((traits::is_magnetic_flux_unit_v<const weber<double>&>));
	EXPECT_TRUE((traits::is_magnetic_flux_unit_v<maxwell<double>>));
	EXPECT_FALSE((traits::is_magnetic_flux_unit_v<inch<double>>));
}

TEST_F(TypeTraits, is_magnetic_field_strength_unit)
{
	EXPECT_FALSE((traits::is_magnetic_field_strength_unit_v<double>));
	EXPECT_TRUE((traits::is_magnetic_field_strength_unit_v<tesla<double>>));
	EXPECT_TRUE((traits::is_magnetic_field_strength_unit_v<const tesla<double>>));
	EXPECT_TRUE((traits::is_magnetic_field_strength_unit_v<const tesla<double>&>));
	EXPECT_TRUE((traits::is_magnetic_field_strength_unit_v<gauss<double>>));
	EXPECT_FALSE((traits::is_magnetic_field_strength_unit_v<volt<double>>));
}

TEST_F(TypeTraits, is_inductance_unit)
{
	EXPECT_FALSE((traits::is_inductance_unit_v<double>));
	EXPECT_TRUE((traits::is_inductance_unit_v<henry<double>>));
	EXPECT_TRUE((traits::is_inductance_unit_v<const henry<double>>));
	EXPECT_TRUE((traits::is_inductance_unit_v<const henry<double>&>));
	EXPECT_FALSE((traits::is_inductance_unit_v<farad<double>>));
}

TEST_F(TypeTraits, is_luminous_flux_unit)
{
	EXPECT_FALSE((traits::is_luminous_flux_unit_v<double>));
	EXPECT_TRUE((traits::is_luminous_flux_unit_v<lumen<double>>));
	EXPECT_TRUE((traits::is_luminous_flux_unit_v<const lumen<double>>));
	EXPECT_TRUE((traits::is_luminous_flux_unit_v<const lumen<double>&>));
	EXPECT_FALSE((traits::is_luminous_flux_unit_v<mass::pound<double>>));
}

TEST_F(TypeTraits, is_illuminance_unit)
{
	EXPECT_FALSE((traits::is_illuminance_unit_v<double>));
	EXPECT_TRUE((traits::is_illuminance_unit_v<footcandle<double>>));
	EXPECT_TRUE((traits::is_illuminance_unit_v<const footcandle<double>>));
	EXPECT_TRUE((traits::is_illuminance_unit_v<const footcandle<double>&>));
	EXPECT_TRUE((traits::is_illuminance_unit_v<lux<double>>));
	EXPECT_FALSE((traits::is_illuminance_unit_v<meter<double>>));
}

TEST_F(TypeTraits, is_radioactivity_unit)
{
	EXPECT_FALSE((traits::is_radioactivity_unit_v<double>));
	EXPECT_TRUE((traits::is_radioactivity_unit_v<sievert<double>>));
	EXPECT_TRUE((traits::is_radioactivity_unit_v<const sievert<double>>));
	EXPECT_TRUE((traits::is_radioactivity_unit_v<const sievert<double>&>));
	EXPECT_FALSE((traits::is_radioactivity_unit_v<year<double>>));
}

TEST_F(TypeTraits, is_torque_unit)
{
	EXPECT_FALSE((traits::is_torque_unit_v<double>));
	EXPECT_TRUE((traits::is_torque_unit_v<torque::newton_meter<double>>));
	EXPECT_TRUE((traits::is_torque_unit_v<const torque::newton_meter<double>>));
	EXPECT_TRUE((traits::is_torque_unit_v<const torque::newton_meter<double>&>));
	EXPECT_TRUE((traits::is_torque_unit_v<torque::foot_pound<double>>));
	EXPECT_FALSE((traits::is_torque_unit_v<volume::cubic_meter<double>>));
}

TEST_F(TypeTraits, is_area_unit)
{
	EXPECT_FALSE((traits::is_area_unit_v<double>));
	EXPECT_TRUE((traits::is_area_unit_v<square_meter<double>>));
	EXPECT_TRUE((traits::is_area_unit_v<const square_meter<double>>));
	EXPECT_TRUE((traits::is_area_unit_v<const square_meter<double>&>));
	EXPECT_TRUE((traits::is_area_unit_v<hectare<double>>));
	EXPECT_FALSE((traits::is_area_unit_v<astronomical_unit<double>>));
}

TEST_F(TypeTraits, is_volume_unit)
{
	EXPECT_FALSE((traits::is_volume_unit_v<double>));
	EXPECT_TRUE((traits::is_volume_unit_v<cubic_meter<double>>));
	EXPECT_TRUE((traits::is_volume_unit_v<const cubic_meter<double>>));
	EXPECT_TRUE((traits::is_volume_unit_v<const cubic_meter<double>&>));
	EXPECT_TRUE((traits::is_volume_unit_v<cubic_inch<double>>));
	EXPECT_FALSE((traits::is_volume_unit_v<foot<double>>));
}

TEST_F(TypeTraits, is_density_unit)
{
	EXPECT_FALSE((traits::is_density_unit_v<double>));
	EXPECT_TRUE((traits::is_density_unit_v<kilograms_per_cubic_meter<double>>));
	EXPECT_TRUE((traits::is_density_unit_v<const kilograms_per_cubic_meter<double>>));
	EXPECT_TRUE((traits::is_density_unit_v<const kilograms_per_cubic_meter<double>&>));
	EXPECT_TRUE((traits::is_density_unit_v<ounces_per_cubic_foot<double>>));
	EXPECT_FALSE((traits::is_density_unit_v<year<double>>));
}

TEST_F(TypeTraits, is_data_unit)
{
	EXPECT_FALSE((traits::is_data_unit_v<double>));
	EXPECT_TRUE((traits::is_data_unit_v<bit<double>>));
	EXPECT_TRUE((traits::is_data_unit_v<const bit<double>>));
	EXPECT_TRUE((traits::is_data_unit_v<const bit<double>&>));
	EXPECT_TRUE((traits::is_data_unit_v<byte<double>>));
	EXPECT_FALSE((traits::is_data_unit_v<year<double>>));
}

TEST_F(TypeTraits, is_data_transfer_rate_unit)
{
	EXPECT_FALSE((traits::is_data_transfer_rate_unit_v<double>));
	EXPECT_TRUE((traits::is_data_transfer_rate_unit_v<gigabits_per_second<double>>));
	EXPECT_TRUE((traits::is_data_transfer_rate_unit_v<const gigabytes_per_second<double>>));
	EXPECT_TRUE((traits::is_data_transfer_rate_unit_v<const gigabytes_per_second<double>&>));
	EXPECT_TRUE((traits::is_data_transfer_rate_unit_v<gigabytes_per_second<double>>));
	EXPECT_FALSE((traits::is_data_transfer_rate_unit_v<year<double>>));
}

TEST_F(STDTypeTraits, std_common_type)
{
	static_assert(
		has_equivalent_conversion_factor(std::common_type_t<meter<double>, meter<double>>(), meter<double>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<kilometer<double>, kilometer<double>>(), kilometer<double>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<millimeter<double>, millimeter<double>>(), millimeter<double>()));
	static_assert(
		has_equivalent_conversion_factor(std::common_type_t<meter<double>, kilometer<double>>(), meter<double>()));
	static_assert(
		has_equivalent_conversion_factor(std::common_type_t<kilometer<double>, meter<double>>(), meter<double>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<meter<double>, millimeter<double>>(), millimeter<double>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<millimeter<double>, meter<double>>(), millimeter<double>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<millimeter<double>, kilometer<double>>(), millimeter<double>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<kilometer<double>, millimeter<double>>(), millimeter<double>()));
	static_assert(std::is_same_v<std::common_type_t<meter<double>, kilometer<double>>,
		std::common_type_t<kilometer<double>, meter<double>>>);
	static_assert(std::is_same_v<std::common_type_t<meter<double>, millimeter<double>>,
		std::common_type_t<millimeter<double>, meter<double>>>);
	static_assert(std::is_same_v<std::common_type_t<millimeter<double>, kilometer<double>>,
		std::common_type_t<kilometer<double>, millimeter<double>>>);

	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<meter<int>, meter<int>>(),
		meter<int>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<kilometer<int>, kilometer<int>>(),
		kilometer<int>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<millimeter<int>, millimeter<int>>(),
		millimeter<int>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<meter<int>, kilometer<int>>(),
		meter<int>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<kilometer<int>, meter<int>>(),
		meter<int>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<meter<int>, millimeter<int>>(),
		millimeter<int>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<millimeter<int>, meter<int>>(),
		millimeter<int>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<millimeter<int>, kilometer<int>>(),
		millimeter<int>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<kilometer<int>, millimeter<int>>(),
		millimeter<int>()));
	static_assert(
		std::is_same_v<std::common_type_t<meter<int>, kilometer<int>>,
			std::common_type_t<kilometer<int>, meter<int>>>);
	static_assert(
		std::is_same_v<std::common_type_t<meter<int>, millimeter<int>>,
			std::common_type_t<millimeter<int>, meter<int>>>);
	static_assert(std::is_same_v<
		std::common_type_t<millimeter<int>, kilometer<int>>,
		std::common_type_t<kilometer<int>, millimeter<int>>>);

	using half_a_second  = unit<conversion_factor<std::ratio<1, 2>, second_conversion_factor>, int>;
	using third_a_second = unit<conversion_factor<std::ratio<1, 3>, second_conversion_factor>, int>;
	using sixth_a_second = unit<conversion_factor<std::ratio<1, 6>, second_conversion_factor>, int>;

	static_assert(
		has_equivalent_conversion_factor(std::common_type_t<half_a_second, third_a_second>{}, sixth_a_second{}));
	static_assert(std::is_same_v<std::common_type_t<half_a_second, third_a_second>,
		std::common_type_t<third_a_second, half_a_second>>);
	static_assert(std::is_same_v<std::common_type_t<half_a_second, third_a_second>::underlying_type, int>);

	static_assert(
		has_equivalent_conversion_factor(std::common_type_t<kelvin<double>, celsius<double>>{}, celsius<double>{}));
	static_assert(
		has_equivalent_conversion_factor(std::common_type_t<celsius<double>, kelvin<double>>{}, celsius<double>{}));
	static_assert(std::is_same_v<std::common_type_t<kelvin<double>, celsius<double>>,
		std::common_type_t<celsius<double>, kelvin<double>>>);

	using half_a_kelvin  = unit<conversion_factor<std::ratio<1, 2>, kelvin_conversion_factor>, double>;
	using third_a_kelvin = unit<conversion_factor<std::ratio<1, 3>, kelvin_conversion_factor>, int>;
	using sixth_a_kelvin = unit<conversion_factor<std::ratio<1, 6>, kelvin_conversion_factor>, int>;

	static_assert(
		has_equivalent_conversion_factor(std::common_type_t<half_a_kelvin, third_a_kelvin>{}, sixth_a_kelvin{}));
	static_assert(std::is_same_v<std::common_type_t<half_a_kelvin, third_a_kelvin>,
		std::common_type_t<third_a_kelvin, half_a_kelvin>>);
	static_assert(std::is_same_v<std::common_type_t<half_a_kelvin, third_a_kelvin>::underlying_type, double>);

	static_assert(
		has_equivalent_conversion_factor(std::common_type_t<radian<double>, degree<double>>{}, degree<double>{}));
	static_assert(
		has_equivalent_conversion_factor(std::common_type_t<degree<double>, radian<double>>{}, degree<double>{}));
	static_assert(std::is_same_v<std::common_type_t<radian<double>, degree<double>>,
		std::common_type_t<degree<double>, radian<double>>>);

	using half_a_radian  = unit<conversion_factor<std::ratio<1, 2>, radian_conversion_factor>, int>;
	using third_a_radian = unit<conversion_factor<std::ratio<1, 3>, radian_conversion_factor>, double>;
	using sixth_a_radian = unit<conversion_factor<std::ratio<1, 6>, radian_conversion_factor>, int>;

	static_assert(
		has_equivalent_conversion_factor(std::common_type_t<half_a_radian, third_a_radian>{}, sixth_a_radian{}));
	static_assert(std::is_same_v<std::common_type_t<half_a_radian, third_a_radian>,
		std::common_type_t<third_a_radian, half_a_radian>>);
	static_assert(std::is_same_v<std::common_type_t<half_a_radian, third_a_radian>::underlying_type, double>);

	static_assert(std::is_same_v<std::common_type_t<dimensionless<int>, dimensionless<int>>, dimensionless<int>>);
	static_assert(std::is_same_v<std::common_type_t<dimensionless<int>, traits::unit_base_t<dimensionless<int>>>,
		dimensionless<int>>);
	static_assert(std::is_same_v<std::common_type_t<traits::unit_base_t<dimensionless<int>>, dimensionless<int>>,
		dimensionless<int>>);
	static_assert(std::is_same_v<
		std::common_type_t<traits::unit_base_t<dimensionless<int>>, traits::unit_base_t<dimensionless<int>>>,
		traits::unit_base_t<dimensionless<int>>>);
	static_assert(std::is_same_v<std::common_type_t<dimensionless<int>, dimensionless<double>>, dimensionless<double>>);
	static_assert(std::is_same_v<std::common_type_t<dimensionless<double>, dimensionless<int>>, dimensionless<double>>);
	static_assert(std::is_same_v<
		std::common_type_t<traits::unit_base_t<dimensionless<int>>, traits::unit_base_t<dimensionless<double>>>,
		traits::unit_base_t<dimensionless<double>>>);

	// static_assert(std::is_same_v<std::common_type_t<dimensionless<int>, int>, dimensionless<int>>);
	// static_assert(std::is_same_v<std::common_type_t<int, dimensionless<int>>, dimensionless<int>>);
	// static_assert(std::is_same_v<std::common_type_t<dimensionless<int>, double>, dimensionless<double>>);
	// static_assert(std::is_same_v<std::common_type_t<double, dimensionless<int>>, dimensionless<double>>);
	// static_assert(std::is_same_v<std::common_type_t<dimensionless<double>, int>, dimensionless<double>>);
	// static_assert(std::is_same_v<std::common_type_t<int, dimensionless<double>>, dimensionless<double>>);
	// static_assert(std::is_same_v<std::common_type_t<dimensionless<double>, double>, dimensionless<double>>);
	// static_assert(std::is_same_v<std::common_type_t<double, dimensionless<double>>, dimensionless<double>>);
	// static_assert(std::is_same_v<std::common_type_t<traits::unit_base_t<dimensionless<int>>, int>,
	// 	traits::unit_base_t<dimensionless<int>>>);
	// static_assert(std::is_same_v<std::common_type_t<int, traits::unit_base_t<dimensionless<int>>>,
	// 	traits::unit_base_t<dimensionless<int>>>);
	// static_assert(std::is_same_v<std::common_type_t<traits::unit_base_t<dimensionless<int>>, double>,
	// 	traits::unit_base_t<dimensionless<double>>>);
	// static_assert(std::is_same_v<std::common_type_t<double, traits::unit_base_t<dimensionless<int>>>,
	// 	traits::unit_base_t<dimensionless<double>>>);
	// static_assert(std::is_same_v<std::common_type_t<traits::unit_base_t<dimensionless<double>>, int>,
	// 	traits::unit_base_t<dimensionless<double>>>);
	// static_assert(std::is_same_v<std::common_type_t<int, traits::unit_base_t<dimensionless<double>>>,
	// 	traits::unit_base_t<dimensionless<double>>>);
	// static_assert(std::is_same_v<std::common_type_t<traits::unit_base_t<dimensionless<double>>, double>,
	// 	traits::unit_base_t<dimensionless<double>>>);
	// static_assert(std::is_same_v<std::common_type_t<double, traits::unit_base_t<dimensionless<double>>>,
	// 	traits::unit_base_t<dimensionless<double>>>);
}

TEST_F(STDSpecializations, hash)
{
	EXPECT_EQ(std::hash<meter<double>>()(3.14_m), std::hash<double>()(3.14));
	EXPECT_EQ(std::hash<millimeter<double>>()(3.14_m), std::hash<double>()(3.14e3));
	EXPECT_EQ(std::hash<millimeter<double>>()(3.14_mm), std::hash<double>()(3.14));
	EXPECT_EQ(std::hash<kilometer<double>>()(3.14_m), std::hash<double>()(3.14e-3));
	EXPECT_EQ(std::hash<kilometer<double>>()(3.14_km), std::hash<double>()(3.14));

	EXPECT_EQ((std::hash<meter<int>>()(meter<int>(42))), 42);
	EXPECT_EQ((std::hash<millimeter<int>>()(meter<int>(42))), 42000);
	EXPECT_EQ((std::hash<millimeter<int>>()(millimeter<int>(42))), 42);
	EXPECT_EQ((std::hash<kilometer<int>>()(kilometer<int>(42))), 42);

	EXPECT_EQ((std::hash<dimensionless<double>>()(3.14)), std::hash<double>()(3.14));
	EXPECT_EQ((std::hash<dimensionless<double>>()(3.14)), std::hash<double>()(3.14));
	EXPECT_EQ((std::hash<dimensionless<int>>()(42)), (std::hash<dimensionless<int>>()(42)));

	EXPECT_EQ(std::hash<dBW<double>>()(2.0_dBW), std::hash<double>()(dBW(2.0).to_linearized()));
}

TEST_F(UnitManipulators, squared)
{
	double test;

	test = unit<square_foot_conversion_factor>(unit<squared<meter_conversion_factor>>(0.092903)).value();
	EXPECT_NEAR(0.99999956944, test, 5.0e-12);

	using dimensionless_2 =
		traits::strong_t<squared<units::dimensionless_unit>>; // this is actually nonsensical, and should also result in
															  // a dimensionless.
	bool isSame = std::is_same_v<unit<dimensionless_unit>, unit<dimensionless_2>>;
	EXPECT_TRUE(isSame);
}

TEST_F(UnitManipulators, cubed)
{
	double test;

	test = unit<cubic_foot_conversion_factor>(unit<cubed<meter_conversion_factor>>(0.0283168)).value();
	EXPECT_NEAR(0.999998354619, test, 5.0e-13);
}

TEST_F(UnitManipulators, square_root)
{
	double test;

	test = meter<double>(unit<square_root<square_kilometer_conversion_factor>>(1.0)).value();
	EXPECT_TRUE(
		(traits::is_convertible_unit_v<square_root<square_kilometer_conversion_factor>, kilometer_conversion_factor>));
	EXPECT_NEAR(1000.0, test, 5.0e-13);
}

TEST_F(UnitManipulators, compound_unit)
{
	using acceleration1 = conversion_factor<std::ratio<1>, dimension::acceleration>;
	using acceleration2 = compound_conversion_factor<meter_conversion_factor, inverse<second_conversion_factor>,
		inverse<second_conversion_factor>>;
	using acceleration3 = conversion_factor<std::ratio<1>,
		make_dimension<dimension::length, std::ratio<1>, dimension::time, std::ratio<-2>>>;
	using acceleration4 =
		compound_conversion_factor<meter_conversion_factor, inverse<squared<second_conversion_factor>>>;
	using acceleration5 =
		compound_conversion_factor<meter_conversion_factor, squared<inverse<second_conversion_factor>>>;

	bool areSame12 = std::is_same_v<acceleration1, acceleration2>;
	bool areSame23 = std::is_same_v<acceleration2, acceleration3>;
	bool areSame34 = std::is_same_v<acceleration3, acceleration4>;
	bool areSame45 = std::is_same_v<acceleration4, acceleration5>;

	EXPECT_TRUE(areSame12);
	EXPECT_TRUE(areSame23);
	EXPECT_TRUE(areSame34);
	EXPECT_TRUE(areSame45);

	// test that thing with translations still compile
	using arbitrary1 = compound_conversion_factor<meter_conversion_factor, inverse<celsius_conversion_factor>>;
	using arbitrary2 = compound_conversion_factor<meter_conversion_factor, celsius_conversion_factor>;
	using arbitrary3 = compound_conversion_factor<arbitrary1, arbitrary2>;
	EXPECT_TRUE((std::is_same_v<square_meter_conversion_factor, traits::strong_t<arbitrary3>>));
}

TEST_F(UnitManipulators, dimensionalAnalysis)
{
	// these look like 'compound units', but the dimensional analysis can be REALLY handy if the
	// unit types aren't know (i.e. they themselves are template parameters), as you can get the resulting unit of the
	// operation.

	using velocity    = traits::strong_t<units::detail::unit_divide<meter_conversion_factor, second_conversion_factor>>;
	bool shouldBeTrue = std::is_same_v<meters_per_second_conversion_factor, velocity>;
	EXPECT_TRUE(shouldBeTrue);

	using acceleration1 = conversion_factor<std::ratio<1>, dimension::acceleration>;
	using acceleration2 = units::detail::unit_divide<meter_conversion_factor,
		units::detail::unit_multiply<second_conversion_factor, second_conversion_factor>>;
	shouldBeTrue        = std::is_same_v<acceleration1, acceleration2>;
	EXPECT_TRUE(shouldBeTrue);
}

TEST_F(UnitType, trivial)
{
	EXPECT_TRUE((std::is_trivial_v<meter<double>>));
	EXPECT_TRUE((std::is_trivially_assignable_v<meter<double>, meter<double>>));
	EXPECT_TRUE((std::is_trivially_constructible_v<meter<double>>));
	EXPECT_TRUE((std::is_trivially_copy_assignable_v<meter<double>>));
	EXPECT_TRUE((std::is_trivially_copy_constructible_v<meter<double>>));
	EXPECT_TRUE((std::is_trivially_copyable_v<meter<double>>));
	EXPECT_TRUE((std::is_trivially_default_constructible_v<meter<double>>));
	EXPECT_TRUE((std::is_trivially_destructible_v<meter<double>>));
	EXPECT_TRUE((std::is_trivially_move_assignable_v<meter<double>>));
	EXPECT_TRUE((std::is_trivially_move_constructible_v<meter<double>>));

	EXPECT_TRUE((std::is_trivial_v<dB<double>>));
	EXPECT_TRUE((std::is_trivially_assignable_v<dB<double>, dB<double>>));
	EXPECT_TRUE((std::is_trivially_constructible_v<dB<double>>));
	EXPECT_TRUE((std::is_trivially_copy_assignable_v<dB<double>>));
	EXPECT_TRUE((std::is_trivially_copy_constructible_v<dB<double>>));
	EXPECT_TRUE((std::is_trivially_copyable_v<dB<double>>));
	EXPECT_TRUE((std::is_trivially_default_constructible_v<dB<double>>));
	EXPECT_TRUE((std::is_trivially_destructible_v<dB<double>>));
	EXPECT_TRUE((std::is_trivially_move_assignable_v<dB<double>>));
	EXPECT_TRUE((std::is_trivially_move_constructible_v<dB<double>>));
}

TEST_F(UnitType, constructionFromArithmeticType)
{
	const meter<double> a_m(1.0);
	EXPECT_EQ(1.0, a_m.value());

	const meter<double> b_m(1);
	EXPECT_EQ(1, b_m.value());

	const meter<int> c_m(1);
	EXPECT_EQ(1, c_m.value());

	const dimensionless<double> d_dim(1.0);
	EXPECT_EQ(1.0, d_dim.value());

	const dimensionless<double> e_dim(1);
	EXPECT_EQ(1, e_dim.value());

	const dimensionless<double> a_dim(1.0);
	EXPECT_EQ(1.0, a_dim.value());

	const dimensionless<double> b_dim(1);
	EXPECT_EQ(1, b_dim.value());

	const dimensionless<int> c_dim(1);
	EXPECT_EQ(1, c_dim.value());

	const dimensionless<int> f_dim(1);
	EXPECT_EQ(1, f_dim.value());
}

TEST_F(UnitType, constructionFromUnitType)
{
	const meter<int> a_m(1);

	const meter<int> b_m(a_m);
	EXPECT_EQ(1, b_m.value());

	const millimeter<int> a_mm(b_m);
	EXPECT_EQ(1000, a_mm.value());

	const millimeter<int> b_mm(a_mm);
	EXPECT_EQ(1000, b_mm.value());

	const millimeter<int> c_mm(b_mm);
	EXPECT_EQ(1000, c_mm.value());

	const millimeter<int> d_mm(b_m);
	EXPECT_EQ(1000, d_mm.value());

	const meter<double> c_m(b_m);
	EXPECT_EQ(1.0, c_m.value());

	const meter<double> d_m(a_mm);
	EXPECT_EQ(1.0, d_m.value());

	const meter<double> e_m(b_mm);
	EXPECT_EQ(1.0, e_m.value());

	const meter<double> f_m(c_m);
	EXPECT_EQ(1.0, f_m.value());

	const meter<double> g_m(kilometer<int>(1));
	EXPECT_EQ(1000.0, g_m.value());

	const dimensionless<int> a_dim(1);

	const dimensionless<int> b_dim(a_dim);
	EXPECT_EQ(1, b_dim.value());

	const dimensionless<int> c_dim(b_dim);
	EXPECT_EQ(1, c_dim.value());

	const dimensionless<int> d_dim(c_dim);
	EXPECT_EQ(1, d_dim.value());

	const dimensionless<double> e_dim(d_dim);
	EXPECT_EQ(1, e_dim.value());

	const dimensionless<double> f_dim(c_dim);
	EXPECT_EQ(1, f_dim.value());

	const dimensionless<double> g_dim(f_dim);
	EXPECT_EQ(1, g_dim.value());
}

TEST_F(UnitType, CTAD)
{
	// Default ctor
	const meter z_m = 1.0_m;
	static_assert(std::is_same_v<std::remove_const_t<decltype(z_m)>, meter<double>>);

	// Underlying type, copy ctor, and same dimensioned units for `int` and `double`.
	const meter a_m(1);
	static_assert(std::is_same_v<std::remove_const_t<decltype(a_m)>, meter<int>>);

	const meter b_m(a_m);
	static_assert(std::is_same_v<std::remove_const_t<decltype(b_m)>, meter<int>>);

	const millimeter a_mm(b_m);
	static_assert(std::is_same_v<std::remove_const_t<decltype(a_mm)>, millimeter<int>>);

	const meter c_m(1.0);
	static_assert(std::is_same_v<std::remove_const_t<decltype(c_m)>, meter<double>>);

	const meter d_m(c_m);
	static_assert(std::is_same_v<std::remove_const_t<decltype(d_m)>, meter<double>>);

	const millimeter b_mm(d_m);
	static_assert(std::is_same_v<std::remove_const_t<decltype(b_mm)>, millimeter<double>>);

	const kilometer a_km(b_mm);
	static_assert(std::is_same_v<std::remove_const_t<decltype(a_km)>, kilometer<double>>);

	// Other underlying types.
	const meter e_m(short(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(e_m)>, meter<short>>);

	const meter f_m(1.0f);
	static_assert(std::is_same_v<std::remove_const_t<decltype(f_m)>, meter<float>>);

	const meter g_m(1LL);
	static_assert(std::is_same_v<std::remove_const_t<decltype(g_m)>, meter<long long>>);

	const meter h_m(1.0L);
	static_assert(std::is_same_v<std::remove_const_t<decltype(h_m)>, meter<long double>>);

	// `unit`.
	const meter i_m(meter<int>(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(i_m)>, meter<int>>);

	const meter j_m(meter<double>(1.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(j_m)>, meter<double>>);

	const meter k_m(kilometer<int>(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(k_m)>, meter<int>>);

	const meter l_m(kilometer<double>(1.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(l_m)>, meter<double>>);

	const meter m_m(millimeter<double>(1.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(m_m)>, meter<double>>);

	// `std::chrono::duration`.
	using namespace std::chrono_literals;

	const second a_s(1_s);
	static_assert(std::is_integral_v<decltype(a_s.value())>);

	const second b_s(1.0_s);
	static_assert(std::is_floating_point_v<decltype(b_s.value())>);

	[[maybe_unused]] const second c_s(1_min);
	[[maybe_unused]] const second d_s(1.0_min);
	[[maybe_unused]] const second e_s(1.0_ms);

	// Dimensionless units.
	const dimensionless z_dim = 1.0;
	static_assert(std::is_same_v<std::remove_const_t<decltype(z_dim)>, dimensionless<double>>);

	const dimensionless a_dim(1);
	static_assert(std::is_same_v<std::remove_const_t<decltype(a_dim)>, dimensionless<int>>);

	const dimensionless b_dim(a_dim);
	static_assert(std::is_same_v<std::remove_const_t<decltype(b_dim)>, dimensionless<int>>);

	const percent a_per(b_dim);
	static_assert(std::is_same_v<std::remove_const_t<decltype(a_per)>, percent<int>>);

	const dimensionless c_dim(1.0);
	static_assert(std::is_same_v<std::remove_const_t<decltype(c_dim)>, dimensionless<double>>);

	const dimensionless d_dim(c_dim);
	static_assert(std::is_same_v<std::remove_const_t<decltype(d_dim)>, dimensionless<double>>);

	const percent b_per(d_dim);
	static_assert(std::is_same_v<std::remove_const_t<decltype(b_per)>, percent<double>>);

	const dimensionless e_dim(short(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(e_dim)>, dimensionless<short>>);

	const dimensionless f_dim(1.0f);
	static_assert(std::is_same_v<std::remove_const_t<decltype(f_dim)>, dimensionless<float>>);

	const dimensionless g_dim(1LL);
	static_assert(std::is_same_v<std::remove_const_t<decltype(g_dim)>, dimensionless<long long>>);

	const dimensionless h_dim(1.0L);
	static_assert(std::is_same_v<std::remove_const_t<decltype(h_dim)>, dimensionless<long double>>);

	const dimensionless i_dim(dimensionless<int>(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(i_dim)>, dimensionless<int>>);

	const dimensionless j_dim(dimensionless<double>(1.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(j_dim)>, dimensionless<double>>);

	const dimensionless k_dim(unit<conversion_factor<std::kilo, dimensionless_unit>, int>(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(k_dim)>, dimensionless<int>>);

	const dimensionless l_dim(unit<conversion_factor<std::kilo, dimensionless_unit>, double>(1.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(l_dim)>, dimensionless<double>>);

	const dimensionless m_dim(unit<conversion_factor<std::milli, dimensionless_unit>, double>(1.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(m_dim)>, dimensionless<double>>);
}

TEST_F(UnitType, assignmentFromArithmeticType)
{
	dimensionless<int> a_dim;
	a_dim = 1;
	EXPECT_EQ(1, a_dim.value());
	a_dim = 1.0;
	EXPECT_EQ(1.0, a_dim.value());

	dimensionless<int> b_dim;
	b_dim = 1;
	EXPECT_EQ(1, b_dim.value());
	b_dim = 1.0;
	EXPECT_EQ(1, b_dim.value());

	dimensionless<double> c_dim;
	c_dim = 1.0;
	EXPECT_EQ(1.0, c_dim.value());
	c_dim = 1;
	EXPECT_EQ(1, c_dim.value());

	dimensionless<double> d_dim;
	d_dim = 1.0;
	EXPECT_EQ(1.0, d_dim.value());
	d_dim = 1;
	EXPECT_EQ(1, d_dim.value());
}

TEST_F(UnitType, assignmentFromUnitType)
{
	meter<int> a_m(1);
	a_m = +a_m;
	EXPECT_EQ(1, a_m.value());

	millimeter<int> a_mm;
	a_mm = a_m;
	EXPECT_EQ(1000, a_mm.value());
	a_mm = +a_mm;
	EXPECT_EQ(1000, a_mm.value());

	millimeter<int> b_mm;
	b_mm = a_m;
	EXPECT_EQ(1000, b_mm.value());
	b_mm = a_mm;
	EXPECT_EQ(1000, b_mm.value());
	b_mm = +b_mm;
	EXPECT_EQ(1000, b_mm.value());

	a_mm = b_mm;
	EXPECT_EQ(1000, a_mm.value());

	meter<double> b_m;
	b_m = a_m;
	EXPECT_EQ(1, b_m.value());
	b_m = a_mm;
	EXPECT_EQ(1, b_m.value());
	b_m = b_mm;
	EXPECT_EQ(1, b_m.value());
	b_m = +b_m;
	EXPECT_EQ(1, b_m.value());
	b_m = kilometer<int>(1);
	EXPECT_EQ(1000, b_m.value());

	dimensionless<int> a_dim(1);
	a_dim = +a_dim;
	EXPECT_EQ(1, a_dim.value());

	dimensionless<int> b_dim;
	b_dim = a_dim;
	EXPECT_EQ(1, b_dim.value());
	b_dim = +b_dim;
	EXPECT_EQ(1, b_dim.value());

	a_dim = b_dim;
	EXPECT_EQ(1, a_dim.value());

	dimensionless<double> c_dim;
	c_dim = a_dim;
	EXPECT_EQ(1, c_dim.value());
	c_dim = b_dim;
	EXPECT_EQ(1, c_dim.value());
	c_dim = +c_dim;
	EXPECT_EQ(1, c_dim.value());
}

TEST_F(UnitType, make_unit)
{
	const auto a_m = make_unit<meter<double>>(5.0);
	EXPECT_EQ(meter<double>(5.0), a_m);

	const auto b_m = make_unit<meter<double>>(5);
	EXPECT_EQ(meter<double>(5), b_m);

	const auto c_m = make_unit<meter<int>>(5);
	EXPECT_EQ((meter<int>(5)), c_m);

	const auto a_dim = make_unit<dimensionless<double>>(5.0);
	EXPECT_EQ(dimensionless<double>(5.0), a_dim);

	const auto b_dim = make_unit<dimensionless<double>>(5);
	EXPECT_EQ(dimensionless<double>(5), b_dim);

	const auto c_dim = make_unit<dimensionless<int>>(5);
	EXPECT_EQ((dimensionless<int>(5)), c_dim);
}

TEST_F(UnitType, unitTypeEquality)
{
	const meter<double> a_m(0);
	const meter<double> b_m(1);

	EXPECT_TRUE(a_m == a_m);
	EXPECT_FALSE(a_m == b_m);
	EXPECT_TRUE(a_m != b_m);
	EXPECT_FALSE(b_m != b_m);

	const meter<int> c_m(0);
	const meter<int> d_m(1);

	EXPECT_TRUE(c_m == c_m);
	EXPECT_FALSE(c_m == d_m);
	EXPECT_TRUE(c_m != d_m);
	EXPECT_FALSE(d_m != d_m);

	EXPECT_TRUE(a_m == c_m);
	EXPECT_TRUE(d_m == b_m);
	EXPECT_FALSE(a_m != c_m);
	EXPECT_FALSE(d_m != b_m);
	EXPECT_TRUE(a_m != d_m);
	EXPECT_TRUE(c_m != b_m);
	EXPECT_FALSE(a_m != c_m);
	EXPECT_FALSE(d_m != b_m);
}

TEST_F(UnitType, unitTypeMixedEquality)
{
	const meter<double> a_m(0);
	const foot<double> a_f(meter<double>(1));

	EXPECT_FALSE(a_m == a_f);
	EXPECT_TRUE(a_m != a_f);

	const foot<int> b_f(0);
	const meter<int> b_m(1);

	EXPECT_FALSE(b_f == b_m);
	EXPECT_TRUE(b_f != b_m);

	EXPECT_TRUE(a_m == b_f);
	EXPECT_TRUE(b_m == a_f);
	EXPECT_FALSE(a_m != b_f);
	EXPECT_FALSE(b_m != a_f);
}

TEST_F(UnitType, unitTypeRelational)
{
	const meter<double> a_m(0);
	const meter<double> b_m(1);

	EXPECT_FALSE(a_m < a_m);
	EXPECT_FALSE(b_m < a_m);
	EXPECT_TRUE(a_m < b_m);
	EXPECT_TRUE(a_m <= a_m);
	EXPECT_FALSE(b_m <= a_m);
	EXPECT_TRUE(a_m <= b_m);
	EXPECT_FALSE(a_m > a_m);
	EXPECT_TRUE(b_m > a_m);
	EXPECT_FALSE(a_m > b_m);
	EXPECT_TRUE(a_m >= a_m);
	EXPECT_TRUE(b_m >= a_m);
	EXPECT_FALSE(a_m >= b_m);

	const meter<int> c_m(0);
	const meter<int> d_m(1);

	EXPECT_FALSE(c_m < c_m);
	EXPECT_FALSE(d_m < c_m);
	EXPECT_TRUE(c_m < d_m);
	EXPECT_TRUE(c_m <= c_m);
	EXPECT_FALSE(d_m <= c_m);
	EXPECT_TRUE(c_m <= d_m);
	EXPECT_FALSE(c_m > c_m);
	EXPECT_TRUE(d_m > c_m);
	EXPECT_FALSE(c_m > d_m);
	EXPECT_TRUE(c_m >= c_m);
	EXPECT_TRUE(d_m >= c_m);
	EXPECT_FALSE(c_m >= d_m);

	EXPECT_FALSE(a_m < c_m);
	EXPECT_FALSE(d_m < a_m);
	EXPECT_TRUE(a_m < d_m);
	EXPECT_TRUE(c_m <= a_m);
	EXPECT_FALSE(d_m <= a_m);
	EXPECT_TRUE(a_m <= d_m);
	EXPECT_FALSE(a_m > c_m);
	EXPECT_TRUE(d_m > a_m);
	EXPECT_FALSE(a_m > d_m);
	EXPECT_TRUE(c_m >= a_m);
	EXPECT_TRUE(d_m >= a_m);
	EXPECT_FALSE(a_m >= d_m);

	const dimensionless<double> a_s(0);
	const dimensionless<int> b_s(1);

	EXPECT_FALSE(a_s < a_s);
	EXPECT_FALSE(b_s < a_s);
	EXPECT_TRUE(a_s < b_s);
	EXPECT_TRUE(a_s <= a_s);
	EXPECT_FALSE(b_s <= a_s);
	EXPECT_TRUE(a_s <= b_s);
	EXPECT_FALSE(a_s > a_s);
	EXPECT_TRUE(b_s > a_s);
	EXPECT_FALSE(a_s > b_s);
	EXPECT_TRUE(a_s >= a_s);
	EXPECT_TRUE(b_s >= a_s);
	EXPECT_FALSE(a_s >= b_s);

	const dimensionless<double> c_s(0);
	const dimensionless<int> d_s(1);

	EXPECT_FALSE(c_s < c_s);
	EXPECT_FALSE(d_s < c_s);
	EXPECT_TRUE(c_s < d_s);
	EXPECT_TRUE(c_s <= c_s);
	EXPECT_FALSE(d_s <= c_s);
	EXPECT_TRUE(c_s <= d_s);
	EXPECT_FALSE(c_s > c_s);
	EXPECT_TRUE(d_s > c_s);
	EXPECT_FALSE(c_s > d_s);
	EXPECT_TRUE(c_s >= c_s);
	EXPECT_TRUE(d_s >= c_s);
	EXPECT_FALSE(c_s >= d_s);

	EXPECT_FALSE(a_s < c_s);
	EXPECT_FALSE(d_s < a_s);
	EXPECT_TRUE(a_s < d_s);
	EXPECT_TRUE(c_s <= a_s);
	EXPECT_FALSE(d_s <= a_s);
	EXPECT_TRUE(a_s <= d_s);
	EXPECT_FALSE(a_s > c_s);
	EXPECT_TRUE(d_s > a_s);
	EXPECT_FALSE(a_s > d_s);
	EXPECT_TRUE(c_s >= a_s);
	EXPECT_TRUE(d_s >= a_s);
	EXPECT_FALSE(a_s >= d_s);
}

TEST_F(UnitType, unitTypeMixedRelational)
{
	const meter<double> a_m(0);
	const foot<double> a_f(meter<double>(1));

	EXPECT_FALSE(a_f < a_m);
	EXPECT_TRUE(a_m < a_f);
	EXPECT_FALSE(a_f <= a_m);
	EXPECT_TRUE(a_m <= a_f);
	EXPECT_TRUE(a_f > a_m);
	EXPECT_FALSE(a_m > a_f);
	EXPECT_TRUE(a_f >= a_m);
	EXPECT_FALSE(a_m >= a_f);

	const foot<int> b_f(0);
	const meter<int> b_m(1);

	EXPECT_FALSE(b_m < b_f);
	EXPECT_TRUE(b_f < b_m);
	EXPECT_FALSE(b_m <= b_f);
	EXPECT_TRUE(b_f <= b_m);
	EXPECT_TRUE(b_m > b_f);
	EXPECT_FALSE(b_f > b_m);
	EXPECT_TRUE(b_m >= b_f);
	EXPECT_FALSE(b_f >= b_m);

	EXPECT_FALSE(a_m < b_f);
	EXPECT_FALSE(a_f < b_m);
	EXPECT_TRUE(b_f <= a_m);
	EXPECT_TRUE(b_m <= a_f);
	EXPECT_FALSE(a_m > b_f);
	EXPECT_FALSE(a_f > b_m);
	EXPECT_TRUE(b_f >= a_m);
	EXPECT_TRUE(b_m >= a_f);
}

TEST_F(UnitType, unitTypeArithmeticOperatorReturnType)
{
	using dimless      = dimensionless<int>;
	using dimless_base = traits::unit_base_t<dimless>;

	dimless dim;
	dimless_base base;

	using meter_conversion_factor = meter<int>;
	using meter_base              = units::meter<int>;

	meter_conversion_factor m;
	meter_base b_m;

	using squared_meter = square_meter<int>;
	using inverse_meter = unit<inverse<units::meter_t>, int>;

	static_assert(std::is_same_v<dimless, decltype(+dim)>);
	static_assert(std::is_same_v<dimless_base, decltype(+base)>);

	static_assert(std::is_same_v<meter_conversion_factor, decltype(+m)>);
	static_assert(std::is_same_v<meter_base, decltype(+b_m)>);

	static_assert(std::is_same_v<dimless, decltype(-dim)>);
	static_assert(std::is_same_v<dimless_base, decltype(-base)>);

	static_assert(std::is_same_v<meter_conversion_factor, decltype(-m)>);
	static_assert(std::is_same_v<meter_base, decltype(-b_m)>);

	static_assert(std::is_same_v<dimless, decltype(dim + 0)>);
	static_assert(std::is_same_v<dimless, decltype(0 + dim)>);
	static_assert(std::is_same_v<dimless, decltype(dim + dim)>);
	static_assert(std::is_same_v<dimless, decltype(dim + base)>);
	static_assert(std::is_same_v<dimless, decltype(base + dim)>);
	static_assert(std::is_same_v<dimless_base, decltype(base + base)>);
	static_assert(std::is_same_v<dimless_base, decltype(base + 0)>);
	static_assert(std::is_same_v<dimless_base, decltype(0 + base)>);

	static_assert(std::is_same_v<meter_conversion_factor, decltype(m + m)>);
	static_assert(std::is_same_v<meter_conversion_factor, decltype(m + b_m)>);
	static_assert(std::is_same_v<meter_conversion_factor, decltype(b_m + m)>);
	static_assert(std::is_same_v<meter_base, decltype(b_m + b_m)>);

	static_assert(std::is_same_v<dimless, decltype(dim - 0)>);
	static_assert(std::is_same_v<dimless, decltype(0 - dim)>);
	static_assert(std::is_same_v<dimless, decltype(dim - dim)>);
	static_assert(std::is_same_v<dimless, decltype(dim - base)>);
	static_assert(std::is_same_v<dimless, decltype(base - dim)>);
	static_assert(std::is_same_v<dimless_base, decltype(base - base)>);
	static_assert(std::is_same_v<dimless_base, decltype(base - 0)>);
	static_assert(std::is_same_v<dimless_base, decltype(0 - base)>);

	static_assert(std::is_same_v<meter_conversion_factor, decltype(m - m)>);
	static_assert(std::is_same_v<meter_conversion_factor, decltype(m - b_m)>);
	static_assert(std::is_same_v<meter_conversion_factor, decltype(b_m - m)>);
	static_assert(std::is_same_v<meter_base, decltype(b_m - b_m)>);

	static_assert(std::is_same_v<dimless, decltype(dim * 1)>);
	static_assert(std::is_same_v<dimless, decltype(1 * dim)>);
	static_assert(std::is_same_v<dimless, decltype(dim * dim)>);
	static_assert(std::is_same_v<dimless, decltype(dim * base)>);
	static_assert(std::is_same_v<dimless, decltype(base * dim)>);
	static_assert(std::is_same_v<dimless, decltype(base * base)>);
	static_assert(std::is_same_v<dimless_base, decltype(base * 1)>);
	static_assert(std::is_same_v<dimless_base, decltype(1 * base)>);

	static_assert(std::is_same_v<meter_conversion_factor, decltype(m * 1)>);
	static_assert(std::is_same_v<meter_conversion_factor, decltype(1 * m)>);
	static_assert(std::is_same_v<meter_conversion_factor, decltype(m * dim)>);
	static_assert(std::is_same_v<meter_conversion_factor, decltype(dim * m)>);
	static_assert(std::is_same_v<meter_conversion_factor, decltype(m * base)>);
	static_assert(std::is_same_v<meter_conversion_factor, decltype(base * m)>);

	static_assert(std::is_same_v<meter_base, decltype(b_m * 1)>);
	static_assert(std::is_same_v<meter_base, decltype(1 * b_m)>);
	static_assert(std::is_same_v<meter_base, decltype(b_m * dim)>);
	static_assert(std::is_same_v<meter_base, decltype(dim * b_m)>);
	static_assert(std::is_same_v<meter_base, decltype(b_m * base)>);
	static_assert(std::is_same_v<meter_base, decltype(base * b_m)>);

	static_assert(std::is_same_v<squared_meter, decltype(m * m)>);
	static_assert(std::is_same_v<squared_meter, decltype(m * b_m)>);
	static_assert(std::is_same_v<squared_meter, decltype(b_m * m)>);
	static_assert(std::is_same_v<squared_meter, decltype(b_m * b_m)>);

	static_assert(std::is_same_v<dimless, decltype(dim / 1)>);
	static_assert(std::is_same_v<dimless_base, decltype(1 / dim)>);
	static_assert(std::is_same_v<dimless, decltype(dim / dim)>);
	static_assert(std::is_same_v<dimless, decltype(dim / base)>);
	static_assert(std::is_same_v<dimless, decltype(base / dim)>);
	static_assert(std::is_same_v<dimless, decltype(base / base)>);
	static_assert(std::is_same_v<dimless_base, decltype(base / 1)>);
	static_assert(std::is_same_v<dimless_base, decltype(1 / base)>);

	static_assert(std::is_same_v<meter_conversion_factor, decltype(m / 1)>);
	static_assert(std::is_same_v<inverse_meter, decltype(1 / m)>);
	static_assert(std::is_same_v<meter_conversion_factor, decltype(m / dim)>);
	static_assert(std::is_same_v<inverse_meter, decltype(dim / m)>);
	static_assert(std::is_same_v<meter_conversion_factor, decltype(m / base)>);
	static_assert(std::is_same_v<inverse_meter, decltype(base / m)>);

	static_assert(std::is_same_v<meter_base, decltype(b_m / 1)>);
	static_assert(std::is_same_v<inverse_meter, decltype(1 / b_m)>);
	static_assert(std::is_same_v<meter_base, decltype(b_m / dim)>);
	static_assert(std::is_same_v<inverse_meter, decltype(dim / b_m)>);
	static_assert(std::is_same_v<meter_base, decltype(b_m / base)>);
	static_assert(std::is_same_v<inverse_meter, decltype(base / b_m)>);

	static_assert(std::is_same_v<dimless, decltype(m / m)>);
	static_assert(std::is_same_v<dimless, decltype(m / b_m)>);
	static_assert(std::is_same_v<dimless, decltype(b_m / m)>);
	static_assert(std::is_same_v<dimless, decltype(b_m / b_m)>);

	static_assert(std::is_same_v<dimless, decltype(dim % 1)>);
	static_assert(std::is_same_v<dimless, decltype(dim % dim)>);
	static_assert(std::is_same_v<dimless, decltype(dim % base)>);
	static_assert(std::is_same_v<dimless_base, decltype(base % dim)>);
	static_assert(std::is_same_v<dimless_base, decltype(base % 1)>);
	static_assert(std::is_same_v<dimless_base, decltype(base % base)>);

	static_assert(std::is_same_v<meter_conversion_factor, decltype(m % 1)>);
	static_assert(std::is_same_v<meter_conversion_factor, decltype(m % dim)>);
	static_assert(std::is_same_v<meter_conversion_factor, decltype(m % base)>);
	static_assert(std::is_same_v<meter_conversion_factor, decltype(m % m)>);
	static_assert(std::is_same_v<meter_conversion_factor, decltype(m % b_m)>);
	static_assert(std::is_same_v<meter_base, decltype(b_m % 1)>);
	static_assert(std::is_same_v<meter_base, decltype(b_m % dim)>);
	static_assert(std::is_same_v<meter_base, decltype(b_m % base)>);
	static_assert(std::is_same_v<meter_base, decltype(b_m % m)>);
	static_assert(std::is_same_v<meter_base, decltype(b_m % b_m)>);
}

TEST_F(UnitType, unitTypeAddition)
{
	// units
	meter<double> a_m(1.0), c_m;
	foot<double> b_ft(3.28084);
	const meter<int> f_m(1);
	const std::common_type_t<meter<int>, foot<int>> g(f_m);

	double d = meter<double>(b_ft).value();
	EXPECT_NEAR(1.0, d, 5.0e-5);
	d = meter<double>(g).value();
	EXPECT_NEAR(1.0, d, 5.0e-5);

	c_m = a_m + b_ft;
	EXPECT_NEAR(2.0, c_m.value(), 5.0e-5);
	c_m = f_m + g;
	EXPECT_NEAR(2.0, c_m.value(), 5.0e-5);
	c_m = a_m + g;
	EXPECT_NEAR(2.0, c_m.value(), 5.0e-5);
	c_m = f_m + b_ft;
	EXPECT_NEAR(2.0, c_m.value(), 5.0e-5);

	c_m = b_ft + meter<double>(3);
	EXPECT_NEAR(4.0, c_m.value(), 5.0e-5);
	c_m = g + meter<int>(3);
	EXPECT_NEAR(4.0, c_m.value(), 5.0e-5);
	c_m = b_ft + meter<int>(3);
	EXPECT_NEAR(4.0, c_m.value(), 5.0e-5);
	c_m = g + meter<double>(3);
	EXPECT_NEAR(4.0, c_m.value(), 5.0e-5);

	foot<double> e_ft = b_ft + meter<double>(3);
	EXPECT_NEAR(13.12336, e_ft.value(), 5.0e-6);
	e_ft = g + meter<int>(3);
	EXPECT_NEAR(13.12336, e_ft.value(), 5.0e-6);
	e_ft = b_ft + meter<int>(3);
	EXPECT_NEAR(13.12336, e_ft.value(), 5.0e-6);
	e_ft = g + meter<double>(3);
	EXPECT_NEAR(13.12336, e_ft.value(), 5.0e-6);

	// dimensionless
	dimensionless<double> sresult = dimensionless<double>(1.0) + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, sresult, 5.0e-6);
	sresult = dimensionless<int>(1) + dimensionless<int>(1);
	EXPECT_NEAR(2.0, sresult, 5.0e-6);
	sresult = dimensionless<double>(1.0) + dimensionless<int>(1);
	EXPECT_NEAR(2.0, sresult, 5.0e-6);
	sresult = dimensionless<int>(1) + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, sresult, 5.0e-6);

	sresult = dimensionless<double>(1.0) + 1.0;
	EXPECT_NEAR(2.0, sresult, 5.0e-6);
	sresult = dimensionless<int>(1) + 1;
	EXPECT_NEAR(2.0, sresult, 5.0e-6);
	sresult = dimensionless<double>(1.0) + 1;
	EXPECT_NEAR(2.0, sresult, 5.0e-6);
	sresult = dimensionless<int>(1) + 1.0;
	EXPECT_NEAR(2.0, sresult, 5.0e-6);

	sresult = 1.0 + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, sresult, 5.0e-6);
	sresult = 1 + dimensionless<int>(1);
	EXPECT_NEAR(2.0, sresult, 5.0e-6);
	sresult = 1.0 + dimensionless<int>(1);
	EXPECT_NEAR(2.0, sresult, 5.0e-6);
	sresult = 1 + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, sresult, 5.0e-6);

	d = dimensionless<double>(1.0) + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, d, 5.0e-6);
	d = dimensionless<int>(1) + dimensionless<int>(1);
	EXPECT_NEAR(2.0, d, 5.0e-6);
	d = dimensionless<double>(1.0) + dimensionless<int>(1);
	EXPECT_NEAR(2.0, d, 5.0e-6);
	d = dimensionless<int>(1) + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, d, 5.0e-6);

	d = dimensionless<double>(1.0) + 1.0;
	EXPECT_NEAR(2.0, d, 5.0e-6);
	d = dimensionless<int>(1) + 1;
	EXPECT_NEAR(2.0, d, 5.0e-6);
	d = dimensionless<double>(1.0) + 1;
	EXPECT_NEAR(2.0, d, 5.0e-6);
	d = dimensionless<int>(1) + 1.0;
	EXPECT_NEAR(2.0, d, 5.0e-6);

	d = 1.0 + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, d, 5.0e-6);
	d = 1. + dimensionless<int>(1);
	EXPECT_NEAR(2.0, d, 5.0e-6);
	d = 1.0 + dimensionless<int>(1);
	EXPECT_NEAR(2.0, d, 5.0e-6);
	d = 1 + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, d, 5.0e-6);
}

TEST_F(UnitType, unitTypeUnaryAddition)
{
	meter<double> a_m(1.0);

	EXPECT_EQ(++a_m, meter<double>(2));
	EXPECT_EQ(a_m++, meter<double>(2));
	EXPECT_EQ(a_m, meter<double>(3));
	EXPECT_EQ(+a_m, meter<double>(3));
	EXPECT_EQ(a_m, meter<double>(3));

	dBW<double> b_dBW(1.0);

	EXPECT_EQ(++b_dBW, dBW<double>(2));
	EXPECT_EQ(b_dBW++, dBW<double>(2));
	EXPECT_EQ(b_dBW, dBW<double>(3));
	EXPECT_EQ(+b_dBW, dBW<double>(3));
	EXPECT_EQ(b_dBW, dBW<double>(3));
}

TEST_F(UnitType, unitTypeSubtraction)
{
	meter<double> a_m(1.0), c_m;
	foot<double> b_ft(3.28084);
	const meter<int> f_m(1);
	const std::common_type_t<meter<int>, foot<int>> g(f_m);

	c_m = a_m - b_ft;
	EXPECT_NEAR(0.0, c_m.value(), 5.0e-5);
	c_m = f_m - g;
	EXPECT_NEAR(0.0, c_m.value(), 5.0e-5);
	c_m = a_m - g;
	EXPECT_NEAR(0.0, c_m.value(), 5.0e-5);
	c_m = f_m - b_ft;
	EXPECT_NEAR(0.0, c_m.value(), 5.0e-5);

	c_m = b_ft - meter<double>(1);
	EXPECT_NEAR(0.0, c_m.value(), 5.0e-5);
	c_m = g - meter<int>(1);
	EXPECT_NEAR(0.0, c_m.value(), 5.0e-5);
	c_m = b_ft - meter<int>(1);
	EXPECT_NEAR(0.0, c_m.value(), 5.0e-5);
	c_m = g - meter<double>(1);
	EXPECT_NEAR(0.0, c_m.value(), 5.0e-5);

	foot<double> e_ft = b_ft - meter<double>(1);
	EXPECT_NEAR(0.0, e_ft.value(), 5.0e-6);
	e_ft = g - meter<int>(1);
	EXPECT_NEAR(0.0, e_ft.value(), 5.0e-6);
	e_ft = b_ft - meter<int>(1);
	EXPECT_NEAR(0.0, e_ft.value(), 5.0e-6);
	e_ft = g - meter<double>(1);
	EXPECT_NEAR(0.0, e_ft.value(), 5.0e-6);

	dimensionless<double> sresult = dimensionless<double>(1.0) - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, sresult, 5.0e-6);
	sresult = dimensionless<int>(1) - dimensionless<int>(1);
	EXPECT_NEAR(0.0, sresult, 5.0e-6);
	sresult = dimensionless<double>(1.0) - dimensionless<int>(1);
	EXPECT_NEAR(0.0, sresult, 5.0e-6);
	sresult = dimensionless<int>(1) - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, sresult, 5.0e-6);

	sresult = dimensionless<double>(1.0) - 1.0;
	EXPECT_NEAR(0.0, sresult, 5.0e-6);
	sresult = dimensionless<int>(1) - 1;
	EXPECT_NEAR(0.0, sresult, 5.0e-6);
	sresult = dimensionless<double>(1.0) - 1;
	EXPECT_NEAR(0.0, sresult, 5.0e-6);
	sresult = dimensionless<int>(1) - 1.0;
	EXPECT_NEAR(0.0, sresult, 5.0e-6);

	sresult = 1.0 - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, sresult, 5.0e-6);
	sresult = 1 - dimensionless<int>(1);
	EXPECT_NEAR(0.0, sresult, 5.0e-6);
	sresult = 1.0 - dimensionless<int>(1);
	EXPECT_NEAR(0.0, sresult, 5.0e-6);
	sresult = 1 - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, sresult, 5.0e-6);

	double d = dimensionless<double>(1.0) - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, d, 5.0e-6);
	d = dimensionless<int>(1) - dimensionless<int>(1);
	EXPECT_NEAR(0.0, d, 5.0e-6);
	d = dimensionless<double>(1.0) - dimensionless<int>(1);
	EXPECT_NEAR(0.0, d, 5.0e-6);
	d = dimensionless<int>(1) - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, d, 5.0e-6);

	d = dimensionless<double>(1.0) - 1.0;
	EXPECT_NEAR(0.0, d, 5.0e-6);
	d = dimensionless<int>(1) - 1;
	EXPECT_NEAR(0.0, d, 5.0e-6);
	d = dimensionless<double>(1.0) - 1;
	EXPECT_NEAR(0.0, d, 5.0e-6);
	d = dimensionless<int>(1) - 1.0;
	EXPECT_NEAR(0.0, d, 5.0e-6);

	d = 1.0 - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, d, 5.0e-6);
	d = 1 - dimensionless<int>(1);
	EXPECT_NEAR(0.0, d, 5.0e-6);
	d = 1.0 - dimensionless<int>(1);
	EXPECT_NEAR(0.0, d, 5.0e-6);
	d = 1 - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, d, 5.0e-6);
}

TEST_F(UnitType, unitTypeUnarySubtraction)
{
	meter<double> a_m(4.0);

	EXPECT_EQ(--a_m, meter<double>(3));
	EXPECT_EQ(a_m--, meter<double>(3));
	EXPECT_EQ(a_m, meter<double>(2));
	EXPECT_EQ(-a_m, meter<double>(-2));
	EXPECT_EQ(a_m, meter<double>(2));

	dBW<double> b_dBW(4.0);

	EXPECT_EQ(--b_dBW, dBW<double>(3));
	EXPECT_EQ(b_dBW--, dBW<double>(3));
	EXPECT_EQ(b_dBW, dBW<double>(2));
	EXPECT_EQ(-b_dBW, dBW<double>(-2));
	EXPECT_EQ(b_dBW, dBW<double>(2));
}

TEST_F(UnitType, unitTypeMultiplication)
{
	meter<double> a_m(1.0), b_m(2.0);
	foot<double> a_ft(3.28084);
	const meter<int> d_m(1), e_m(2);
	const std::common_type_t<meter<int>, foot<int>> f(d_m);

	auto c_m2 = a_m * b_m;
	EXPECT_NEAR(2.0, c_m2.value(), 5.0e-5);
	c_m2 = d_m * e_m;
	EXPECT_NEAR(2.0, c_m2.value(), 5.0e-5);
	c_m2 = a_m * e_m;
	EXPECT_NEAR(2.0, c_m2.value(), 5.0e-5);
	c_m2 = d_m * b_m;
	EXPECT_NEAR(2.0, c_m2.value(), 5.0e-5);

	c_m2 = b_m * meter<double>(2);
	EXPECT_NEAR(4.0, c_m2.value(), 5.0e-5);
	c_m2 = e_m * meter<int>(2);
	EXPECT_NEAR(4.0, c_m2.value(), 5.0e-5);
	c_m2 = b_m * meter<int>(2);
	EXPECT_NEAR(4.0, c_m2.value(), 5.0e-5);
	c_m2 = e_m * meter<double>(2);
	EXPECT_NEAR(4.0, c_m2.value(), 5.0e-5);

	c_m2 = b_m * a_ft;
	EXPECT_NEAR(2.0, c_m2.value(), 5.0e-5);
	c_m2 = e_m * f;
	EXPECT_NEAR(2.0, c_m2.value(), 5.0e-5);
	c_m2 = b_m * f;
	EXPECT_NEAR(2.0, c_m2.value(), 5.0e-5);
	c_m2 = e_m * a_ft;
	EXPECT_NEAR(2.0, c_m2.value(), 5.0e-5);

	auto c_m = b_m * 2.0;
	EXPECT_NEAR(4.0, c_m.value(), 5.0e-5);
	c_m = e_m * 2;
	EXPECT_NEAR(4.0, c_m.value(), 5.0e-5);
	c_m = b_m * 2;
	EXPECT_NEAR(4.0, c_m.value(), 5.0e-5);
	c_m = e_m * 2.0;
	EXPECT_NEAR(4.0, c_m.value(), 5.0e-5);

	c_m = 2.0 * b_m;
	EXPECT_NEAR(4.0, c_m.value(), 5.0e-5);
	c_m = 2 * e_m;
	EXPECT_NEAR(4.0, c_m.value(), 5.0e-5);
	c_m = 2.0 * e_m;
	EXPECT_NEAR(4.0, c_m.value(), 5.0e-5);
	c_m = 2 * b_m;
	EXPECT_NEAR(4.0, c_m.value(), 5.0e-5);

	double convert = dimensionless<double>(3.14);
	EXPECT_NEAR(3.14, convert, 5.0e-5);
	convert = dimensionless<int>(3);
	EXPECT_NEAR(3, convert, 5.0e-5);

	dimensionless<double> sresult = dimensionless<double>(5.0) * dimensionless<double>(4.0);
	EXPECT_NEAR(20.0, sresult.value(), 5.0e-5);
	sresult = dimensionless<int>(5) * dimensionless<int>(4);
	EXPECT_NEAR(20.0, sresult.value(), 5.0e-5);
	sresult = dimensionless<double>(5.0) * dimensionless<int>(4);
	EXPECT_NEAR(20.0, sresult.value(), 5.0e-5);
	sresult = dimensionless<int>(5) * dimensionless<double>(4.0);
	EXPECT_NEAR(20.0, sresult.value(), 5.0e-5);

	sresult = dimensionless<double>(5.0) * 4.0;
	EXPECT_NEAR(20.0, sresult.value(), 5.0e-5);
	sresult = dimensionless<int>(5) * 4;
	EXPECT_NEAR(20.0, sresult.value(), 5.0e-5);
	sresult = dimensionless<double>(5.0) * 4;
	EXPECT_NEAR(20.0, sresult.value(), 5.0e-5);
	sresult = dimensionless<int>(5) * 4.0;
	EXPECT_NEAR(20.0, sresult.value(), 5.0e-5);

	sresult = 4.0 * dimensionless<double>(5.0);
	EXPECT_NEAR(20.0, sresult.value(), 5.0e-5);
	sresult = 4 * dimensionless<int>(5);
	EXPECT_NEAR(20.0, sresult.value(), 5.0e-5);
	sresult = 4.0 * dimensionless<int>(5);
	EXPECT_NEAR(20.0, sresult.value(), 5.0e-5);
	sresult = 4 * dimensionless<double>(5.0);
	EXPECT_NEAR(20.0, sresult.value(), 5.0e-5);

	double result = dimensionless<double>(5.0) * dimensionless<double>(4.0);
	EXPECT_NEAR(20.0, result, 5.0e-5);
	result = dimensionless<int>(5) * dimensionless<int>(4);
	EXPECT_NEAR(20.0, result, 5.0e-5);
	result = dimensionless<double>(5.0) * dimensionless<int>(4);
	EXPECT_NEAR(20.0, result, 5.0e-5);
	result = dimensionless<int>(5) * dimensionless<double>(4.0);
	EXPECT_NEAR(20.0, result, 5.0e-5);

	result = dimensionless<double>(5.0) * 4.0;
	EXPECT_NEAR(20.0, result, 5.0e-5);
	result = dimensionless<int>(5) * 4;
	EXPECT_NEAR(20.0, result, 5.0e-5);
	result = dimensionless<double>(5.0) * 4;
	EXPECT_NEAR(20.0, result, 5.0e-5);
	result = dimensionless<int>(5) * 4.0;
	EXPECT_NEAR(20.0, result, 5.0e-5);

	result = 4.0 * dimensionless<double>(5.0);
	EXPECT_NEAR(20.0, result, 5.0e-5);
	result = 4 * dimensionless<int>(5);
	EXPECT_NEAR(20.0, result, 5.0e-5);
	result = 4.0 * dimensionless<int>(5);
	EXPECT_NEAR(20.0, result, 5.0e-5);
	result = 4 * dimensionless<double>(5.0);
	EXPECT_NEAR(20.0, result, 5.0e-5);
}

TEST_F(UnitType, unitTypeMixedUnitMultiplication)
{
	meter<double> a_m(1.0);
	foot<double> b_ft(3.28084);
	unit<inverse<meter_conversion_factor>> i_m(2.0);
	const meter<int> b_m(1);
	const std::common_type_t<meter<int>, foot<int>> f(b_m);
	const unit<inverse<meter_t>, int> i_i_m(2);

	// resultant unit is square of the common type unit
	// you can get whatever (compatible) type you want if you ask explicitly
	unit<squared<meter_conversion_factor>> c_m2 = a_m * b_ft;
	EXPECT_NEAR(1.0, c_m2.value(), 5.0e-5);
	c_m2 = b_m * f;
	EXPECT_NEAR(1.0, c_m2.value(), 5.0e-5);
	c_m2 = a_m * f;
	EXPECT_NEAR(1.0, c_m2.value(), 5.0e-5);
	c_m2 = b_m * b_ft;
	EXPECT_NEAR(1.0, c_m2.value(), 5.0e-5);

	unit<squared<foot_conversion_factor>> c_ft2 = b_ft * a_m;
	EXPECT_NEAR(10.7639111056, c_ft2.value(), 5.0e-7);
	c_ft2 = f * b_m;
	EXPECT_NEAR(10.7639111056, c_ft2.value(), 5.0e-6);
	c_ft2 = b_ft * b_m;
	EXPECT_NEAR(10.7639111056, c_ft2.value(), 5.0e-7);
	c_ft2 = f * a_m;
	EXPECT_NEAR(10.7639111056, c_ft2.value(), 5.0e-6);

	square_meter<double> d_m2 = b_ft * a_m;
	EXPECT_NEAR(1.0, d_m2.value(), 5.0e-5);
	d_m2 = f * b_m;
	EXPECT_NEAR(1.0, d_m2.value(), 5.0e-5);
	d_m2 = b_ft * b_m;
	EXPECT_NEAR(1.0, d_m2.value(), 5.0e-5);
	d_m2 = f * a_m;
	EXPECT_NEAR(1.0, d_m2.value(), 5.0e-5);

	// a unit times a sclar ends up with the same units.
	meter<double> e_m = a_m * dimensionless<double>(3.0);
	EXPECT_NEAR(3.0, e_m.value(), 5.0e-5);
	e_m = b_m * dimensionless<int>(3);
	EXPECT_NEAR(3.0, e_m.value(), 5.0e-5);
	e_m = a_m * dimensionless<int>(3);
	EXPECT_NEAR(3.0, e_m.value(), 5.0e-5);
	e_m = b_m * dimensionless<double>(3.0);
	EXPECT_NEAR(3.0, e_m.value(), 5.0e-5);

	e_m = dimensionless<double>(4.0) * a_m;
	EXPECT_NEAR(4.0, e_m.value(), 5.0e-5);
	e_m = dimensionless<int>(4) * b_m;
	EXPECT_NEAR(4.0, e_m.value(), 5.0e-5);
	e_m = dimensionless<double>(4) * b_m;
	EXPECT_NEAR(4.0, e_m.value(), 5.0e-5);
	e_m = dimensionless<int>(4) * a_m;
	EXPECT_NEAR(4.0, e_m.value(), 5.0e-5);

	// unit times its inverse results in a dimensionless
	dimensionless<double> s = a_m * i_m;
	EXPECT_NEAR(2.0, s, 5.0e-5);
	s = b_m * i_i_m;
	EXPECT_NEAR(2.0, s, 5.0e-5);
	s = a_m * i_i_m;
	EXPECT_NEAR(2.0, s, 5.0e-5);
	s = b_m * i_m;
	EXPECT_NEAR(2.0, s, 5.0e-5);

	c_m2 = b_ft * meter<double>(2);
	EXPECT_NEAR(2.0, c_m2.value(), 5.0e-5);
	c_m2 = f * meter<int>(2);
	EXPECT_NEAR(2.0, c_m2.value(), 5.0e-5);
	c_m2 = b_ft * meter<int>(2);
	EXPECT_NEAR(2.0, c_m2.value(), 5.0e-5);
	c_m2 = f * meter<double>(2);
	EXPECT_NEAR(2.0, c_m2.value(), 5.0e-5);

	unit<squared<foot_conversion_factor>> e_ft2 = b_ft * meter<double>(3);
	EXPECT_NEAR(32.2917333168, e_ft2.value(), 5.0e-6);
	e_ft2 = f * meter<int>(3);
	EXPECT_NEAR(32.2917333168, e_ft2.value(), 5.0e-6);
	e_ft2 = b_ft * meter<int>(3);
	EXPECT_NEAR(32.2917333168, e_ft2.value(), 5.0e-6);
	e_ft2 = f * meter<double>(3);
	EXPECT_NEAR(32.2917333168, e_ft2.value(), 5.0e-6);

	auto mps = meter<double>(10.0) * unit<inverse<second_conversion_factor>>(1.0);
	EXPECT_EQ(mps, meters_per_second<double>(10));
	mps = meter<int>(10) * unit<inverse<second_t>, int>(1);
	EXPECT_EQ(mps, meters_per_second<double>(10));
	mps = meter<double>(10.0) * unit<inverse<second_t>, int>(1);
	EXPECT_EQ(mps, meters_per_second<double>(10));
	mps = meter<int>(10) * unit<inverse<second_t>>(1.0);
	EXPECT_EQ(mps, meters_per_second<double>(10));
}

TEST_F(UnitType, unitTypedimensionlessMultiplication)
{
	meter<double> a_m(1.0);

	auto result_m = dimensionless<double>(3.0) * a_m;
	EXPECT_NEAR(3.0, result_m.value(), 5.0e-5);

	result_m = a_m * dimensionless<double>(4.0);
	EXPECT_NEAR(4.0, result_m.value(), 5.0e-5);

	result_m = 3.0 * a_m;
	EXPECT_NEAR(3.0, result_m.value(), 5.0e-5);

	result_m = a_m * 4.0;
	EXPECT_NEAR(4.0, result_m.value(), 5.0e-5);

	bool isSame = std::is_same_v<decltype(result_m), meter<double>>;
	EXPECT_TRUE(isSame);
}

TEST_F(UnitType, unitTypeDivision)
{
	meter<double> a_m(1.0), b_m(2.0);
	foot<double> a_ft(3.28084);
	second<double> a_sec(10.0);
	const meter<int> d_m(1), e_m(2);
	const std::common_type_t<meter<int>, foot<int>> j(d_m);
	const second<int> b_sec(10);
	bool isSame;

	auto c = a_m / a_ft;
	EXPECT_NEAR(1.0, c, 5.0e-5);
	c = d_m / j;
	EXPECT_NEAR(1.0, c, 5.0e-5);
	c = a_m / j;
	EXPECT_NEAR(1.0, c, 5.0e-5);
	c = d_m / a_ft;
	EXPECT_NEAR(1.0, c, 5.0e-5);
	isSame = std::is_same_v<decltype(c), dimensionless<double>>;
	EXPECT_TRUE(isSame);

	c = a_m / b_m;
	EXPECT_NEAR(0.5, c, 5.0e-5);
	c = d_m / e_m;
	EXPECT_EQ(0, c);
	c = a_m / e_m;
	EXPECT_NEAR(0.5, c, 5.0e-5);
	c = d_m / b_m;
	EXPECT_NEAR(0.5, c, 5.0e-5);
	isSame = std::is_same_v<decltype(c), dimensionless<double>>;
	EXPECT_TRUE(isSame);

	c = a_ft / a_m;
	EXPECT_NEAR(1.0, c, 5.0e-5);
	c = j / d_m;
	EXPECT_NEAR(1.0, c, 5.0e-5);
	c = a_ft / d_m;
	EXPECT_NEAR(1.0, c, 5.0e-5);
	c = j / a_m;
	EXPECT_NEAR(1.0, c, 5.0e-5);
	isSame = std::is_same_v<decltype(c), dimensionless<double>>;
	EXPECT_TRUE(isSame);

	c = dimensionless<double>(1.0) / 2.0;
	EXPECT_NEAR(0.5, c, 5.0e-5);
	c = dimensionless<int>(1) / 2;
	EXPECT_EQ(0, c);
	c = dimensionless<double>(1.0) / 2;
	EXPECT_NEAR(0.5, c, 5.0e-5);
	c = dimensionless<int>(1) / 2.0;
	EXPECT_NEAR(0.5, c, 5.0e-5);
	isSame = std::is_same_v<decltype(c), dimensionless<double>>;
	EXPECT_TRUE(isSame);

	c = 1.0 / dimensionless<double>(2.0);
	EXPECT_NEAR(0.5, c, 5.0e-5);
	c = 1 / dimensionless<int>(2);
	EXPECT_EQ(0, c);
	c = 1.0 / dimensionless<int>(2);
	EXPECT_NEAR(0.5, c, 5.0e-5);
	c = 1 / dimensionless<double>(2.0);
	EXPECT_NEAR(0.5, c, 5.0e-5);
	isSame = std::is_same_v<decltype(c), dimensionless<double>>;
	EXPECT_TRUE(isSame);

	double d = dimensionless<double>(1.0) / 2.0;
	EXPECT_NEAR(0.5, d, 5.0e-5);

	auto e = a_m / a_sec;
	EXPECT_NEAR(0.1, e.value(), 5.0e-5);
	e = d_m / b_sec;
	EXPECT_EQ(0, e.value());
	e = a_m / b_sec;
	EXPECT_NEAR(0.1, e.value(), 5.0e-5);
	e = d_m / a_sec;
	EXPECT_NEAR(0.1, e.value(), 5.0e-5);
	isSame = std::is_same_v<decltype(e), meters_per_second<double>>;
	EXPECT_TRUE(isSame);

	auto f = a_m / 8.0;
	EXPECT_NEAR(0.125, f.value(), 5.0e-5);
	f = d_m / 8;
	EXPECT_EQ(0, f.value());
	f = a_m / 8;
	EXPECT_NEAR(0.125, f.value(), 5.0e-5);
	f = d_m / 8.0;
	EXPECT_NEAR(0.125, f.value(), 5.0e-5);
	isSame = std::is_same_v<decltype(f), meter<double>>;
	EXPECT_TRUE(isSame);

	auto g = 4.0 / b_m;
	EXPECT_NEAR(2.0, g.value(), 5.0e-5);
	g = 4 / e_m;
	EXPECT_NEAR(2.0, g.value(), 5.0e-5);
	g = 4.0 / e_m;
	EXPECT_NEAR(2.0, g.value(), 5.0e-5);
	g = 4 / b_m;
	EXPECT_NEAR(2.0, g.value(), 5.0e-5);
	isSame = std::is_same_v<decltype(g), unit<inverse<meter_conversion_factor>>>;
	EXPECT_TRUE(isSame);

	auto mph                      = mile<double>(60.0) / hour<double>(1.0);
	meters_per_second<double> mps = mph;
	EXPECT_NEAR(26.8224, mps.value(), 5.0e-5);
	mps = mile<int>(60) / hour<int>(1);
	EXPECT_NEAR(26.8224, mps.value(), 5.0e-5);
	mps = mile<double>(60.0) / hour<int>(1);
	EXPECT_NEAR(26.8224, mps.value(), 5.0e-5);
	mps = mile<int>(60) / hour<double>(1.0);
	EXPECT_NEAR(26.8224, mps.value(), 5.0e-5);

	auto h = 10.0_rad / 2.0_rad;
	EXPECT_NEAR(5, h, 5.0e-5);
	h = radian<int>(10) / radian<int>(2);
	EXPECT_NEAR(5, h, 5.0e-5);
	h = 10.0_rad / radian<int>(2);
	EXPECT_NEAR(5, h, 5.0e-5);
	h = radian<int>(10) / 2.0_rad;
	EXPECT_NEAR(5, h, 5.0e-5);
	isSame = std::is_same_v<decltype(h), dimensionless<double>>;
	EXPECT_TRUE(isSame);

	auto i = (3.0_N * 2.0_m) / 6.0_J;
	EXPECT_NEAR(1, i, 5.0e-5);
	i = (force::newton<int>(3) * meter<int>(2)) /
		joule<int>(6);
	EXPECT_NEAR(1, i, 5.0e-5);
	i = (3.0_N * meter<int>(2)) / joule<int>(6);
	EXPECT_NEAR(1, i, 5.0e-5);
	i = (force::newton<int>(3) * meter<int>(2)) / 6.0_J;
	EXPECT_NEAR(1, i, 5.0e-5);
	isSame = std::is_same_v<decltype(i), dimensionless<double>>;
	EXPECT_TRUE(isSame);
}

TEST_F(UnitType, unitTypeModulo)
{
	const meter<int> a_m(2200);
	const meter<int> b_m(1800);
	const kilometer<int> a_km(2);

	const auto c_m = a_m % b_m;
	EXPECT_EQ(400, c_m.value());
	static_assert(has_equivalent_conversion_factor(c_m, a_m));

	const auto d_m = a_m % a_km;
	EXPECT_EQ(200, d_m.value());
	static_assert(has_equivalent_conversion_factor(d_m, a_m));

	const auto b_km = a_km % dimensionless<int>(3);
	EXPECT_EQ(2, b_km.value());
	static_assert(has_equivalent_conversion_factor(b_km, a_km));

	const auto e_m = a_m % 2000;
	EXPECT_EQ(200, e_m.value());
	static_assert(has_equivalent_conversion_factor(e_m, a_m));

	const dimensionless<int> a_s(12);
	const dimensionless<int> b_s(5);

	const auto c_s = a_s % b_s;
	EXPECT_EQ(2, c_s.value());
	static_assert(has_equivalent_conversion_factor(c_s, a_s));

	const auto d_s = a_s % 20;
	EXPECT_EQ(12, d_s.value());
	static_assert(has_equivalent_conversion_factor(d_s, a_s));
}

TEST_F(UnitType, compoundAssignmentAddition)
{
	// units
	meter<double> a(0.0);
	a += meter<double>(1.0);

	EXPECT_EQ(meter<double>(1.0), a);

	a += foot<double>(meter<double>(1));

	EXPECT_EQ(meter<double>(2.0), a);

	a += meter<int>(1);

	EXPECT_EQ(meter<double>(3.0), a);

	a += std::common_type_t<meter<int>, foot<int>>(
		meter<int>(1));

	EXPECT_EQ(meter<double>(4.0), a);

	meter<int> c(0);
	c += meter<int>(1);

	EXPECT_EQ((meter<int>(1)), c);

	c += kilometer<int>(1);

	EXPECT_EQ((meter<int>(1001)), c);

	// dimensionlesss
	dimensionless<double> b(0);
	b += dimensionless<double>(1.0);

	EXPECT_EQ(dimensionless<double>(1.0), b);

	b += 1.0;

	EXPECT_EQ(dimensionless<double>(2.0), b);

	b += dimensionless<int>(1);

	EXPECT_EQ(dimensionless<double>(3.0), b);

	b += 1;

	EXPECT_EQ(dimensionless<double>(4.0), b);

	dimensionless<int> d(0);
	d += dimensionless<int>(1);

	EXPECT_EQ((dimensionless<int>(1)), d);

	d += 1;

	EXPECT_EQ((dimensionless<int>(2)), d);
}

TEST_F(UnitType, compoundAssignmentSubtraction)
{
	// units
	meter<double> a(2.0);
	a -= meter<double>(1.0);

	EXPECT_EQ(meter<double>(1.0), a);

	a -= foot<double>(meter<double>(1));

	EXPECT_EQ(meter<double>(0.0), a);

	a -= meter<int>(1);

	EXPECT_EQ(meter<double>(-1.0), a);

	a -= std::common_type_t<meter<int>, foot<int>>(
		meter<int>(1));

	EXPECT_EQ(meter<double>(-2.0), a);

	meter<int> c(1);
	c -= meter<int>(1);

	EXPECT_EQ((meter<int>(0)), c);

	c -= kilometer<int>(1);

	EXPECT_EQ((meter<int>(-1000)), c);

	// dimensionlesss
	dimensionless<double> b(2);
	b -= dimensionless<double>(1.0);

	EXPECT_EQ(dimensionless<double>(1.0), b);

	b -= 1.0;

	EXPECT_EQ(dimensionless<double>(0), b);

	b -= dimensionless<int>(1);

	EXPECT_EQ(dimensionless<double>(-1.0), b);

	b -= 1;

	EXPECT_EQ(dimensionless<double>(-2.0), b);

	dimensionless<int> d(2);
	d -= dimensionless<int>(1);

	EXPECT_EQ((dimensionless<int>(1)), d);

	d -= 1;

	EXPECT_EQ((dimensionless<int>(0)), d);
}

TEST_F(UnitType, compoundAssignmentMultiplication)
{
	// units
	meter<double> a(2.0);
	a *= dimensionless<double>(2.0);

	EXPECT_EQ(meter<double>(4.0), a);

	a *= 2.0;

	EXPECT_EQ(meter<double>(8.0), a);

	a *= dimensionless<int>(2);

	EXPECT_EQ(meter<double>(16), a);

	a *= 2;

	EXPECT_EQ(meter<double>(32), a);

	meter<int> c(2);
	c *= dimensionless<int>(2);

	EXPECT_EQ((meter<int>(4)), c);

	c *= dimensionless<double>(2.0);

	EXPECT_EQ((meter<int>(8)), c);

	c *= 2;

	EXPECT_EQ((meter<int>(16)), c);

	c *= 2.0;

	EXPECT_EQ((meter<int>(32)), c);

	// dimensionlesss
	dimensionless<double> b(2);
	b *= dimensionless<double>(2.0);

	EXPECT_EQ(dimensionless<double>(4.0), b);

	b *= 2.0;

	EXPECT_EQ(dimensionless<double>(8.0), b);

	b *= dimensionless<int>(2);

	EXPECT_EQ(dimensionless<double>(16.0), b);

	b *= 2;

	EXPECT_EQ(dimensionless<double>(32.0), b);

	dimensionless<int> d(2);
	d *= dimensionless<int>(2);

	EXPECT_EQ((dimensionless<int>(4)), d);

	d *= dimensionless<double>(2.0);

	EXPECT_EQ((dimensionless<int>(8)), d);

	d *= 2;

	EXPECT_EQ((dimensionless<int>(16)), d);

	d *= 2.0;

	EXPECT_EQ((dimensionless<int>(32)), d);
}

TEST_F(UnitType, compoundAssignmentDivision)
{
	// units
	meter<double> a(8.0);
	a /= dimensionless<double>(2.0);

	EXPECT_EQ(meter<double>(4.0), a);

	a /= 2.0;

	EXPECT_EQ(meter<double>(2.0), a);

	a /= dimensionless<int>(2);

	EXPECT_EQ(meter<double>(1), a);

	a /= 2;

	EXPECT_EQ(meter<double>(0.5), a);

	meter<int> c(32);
	c /= dimensionless<int>(2);

	EXPECT_EQ((meter<int>(16)), c);

	c /= dimensionless<double>(2.0);

	EXPECT_EQ((meter<int>(8)), c);

	c /= 2;

	EXPECT_EQ((meter<int>(4)), c);

	c /= 2.0;

	EXPECT_EQ((meter<int>(2)), c);

	// dimensionlesss
	dimensionless<double> b(8);
	b /= dimensionless<double>(2.0);

	EXPECT_EQ(dimensionless<double>(4.0), b);

	b /= 2.0;

	EXPECT_EQ(dimensionless<double>(2.0), b);

	b /= dimensionless<int>(2);

	EXPECT_EQ(dimensionless<double>(1.0), b);

	b /= 2;

	EXPECT_EQ(dimensionless<double>(0.5), b);

	dimensionless<int> d(32);
	d /= dimensionless<int>(2);

	EXPECT_EQ((dimensionless<int>(16)), d);

	d /= dimensionless<double>(2.0);

	EXPECT_EQ((dimensionless<int>(8)), d);

	d /= 2;

	EXPECT_EQ((dimensionless<int>(4)), d);

	d /= 2.0;

	EXPECT_EQ((dimensionless<int>(2)), d);
}

TEST_F(UnitType, compoundAssignmentModulo)
{
	// units
	meter<int> a_m(2200);

	a_m %= meter<int>(2000);
	EXPECT_EQ(200, a_m.value());

	a_m %= kilometer<int>(1);
	EXPECT_EQ(200, a_m.value());

	a_m %= dimensionless<int>(180);
	EXPECT_EQ(20, a_m.value());

	a_m %= dimensionless<double>(15.0);
	EXPECT_EQ(5, a_m.value());

	a_m %= 6;
	EXPECT_EQ(5, a_m.value());

	a_m %= 3.0;
	EXPECT_EQ(2, a_m.value());

	// dimensionless
	dimensionless<int> a_s(12);

	a_s %= dimensionless<int>(20);
	EXPECT_EQ(12, a_s.value());

	a_s %= dimensionless<double>(7.0);
	EXPECT_EQ(5, a_s.value());

	a_s %= 3;
	EXPECT_EQ(2, a_s.value());

	a_s %= 3.0;
	EXPECT_EQ(2, a_s.value());
}

TEST_F(UnitType, dimensionlessTypeImplicitConversion)
{
	double test = dimensionless<double>(3.0);
	EXPECT_DOUBLE_EQ(3.0, test);

	dimensionless<double> testS = 3.0;
	EXPECT_DOUBLE_EQ(3.0, testS);

	dimensionless<double> test3(ppm<double>(10));
	EXPECT_DOUBLE_EQ(0.00001, test3);

	dimensionless<double> test4;
	test4 = ppm<double>(1);
	EXPECT_DOUBLE_EQ(0.000001, test4);
}

TEST_F(UnitType, valueMethod)
{
	double test = meter<double>(3.0).to<double>();
	EXPECT_DOUBLE_EQ(3.0, test);

	auto test2 = meter<double>(4.0).value();
	EXPECT_DOUBLE_EQ(4.0, test2);
	EXPECT_TRUE((std::is_same_v<decltype(test2), double>));
}

TEST_F(UnitType, convertMethod)
{
	double test = meter<double>(3.0).convert<foot_conversion_factor>().to<double>();
	EXPECT_NEAR(9.84252, test, 5.0e-6);
}

TEST_F(UnitType, unit_tSyntax)
{
	EXPECT_TRUE((std::is_same_v<meter_t, meter<double>>));
	EXPECT_FALSE((std::is_same_v<meter_t, meter<int>>));
	EXPECT_FALSE((std::is_same_v<meter_t, foot<int>>));
}

#ifndef UNIT_LIB_DISABLE_IOSTREAM
TEST_F(UnitType, cout)
{
	testing::internal::CaptureStdout();
	std::cout << meters_per_second<double>(5);
	std::string output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("5 mps", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << degree<double>(349.87);
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("349.87 deg", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << meter<double>(1.0);
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("1 m", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << dB<double>(31.0);
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("31 dB", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << volt<double>(21.79);
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("21.79 V", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << dBW<double>(12.0);
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("12 dBW", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << dBm<double>(120.0);
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("120 dBm", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << miles_per_hour<double>(72.1);
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("72.1 mph", output.c_str());

	// undefined unit
	testing::internal::CaptureStdout();
	std::cout << pow<4>(meter<double>(2));
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("16 m^4", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << pow<3>(foot<double>(2));
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("8 cu_ft", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << std::setprecision(9) << pow<4>(foot<double>(2));
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("0.138095597 m^4", output.c_str());

	// constants
	testing::internal::CaptureStdout();
	std::cout << std::setprecision(8) << constants::k_B;
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("1.3806485e-23 m^2 kg K^-1 s^-2", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << std::setprecision(9) << constants::mu_B;
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("9.27400999e-24 A m^2", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << std::setprecision(7) << constants::sigma;
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("5.670367e-08 kg K^-4 s^-3", output.c_str());
}

TEST_F(UnitType, to_string)
{
	foot<double> a(3.5);
	EXPECT_STREQ("3.5 ft", units::length::to_string(a).c_str());

	meter<double> b(8);
	EXPECT_STREQ("8 m", units::length::to_string(b).c_str());
}

TEST_F(UnitType, to_string_locale)
{
	struct lconv* lc;

	// German locale
#if defined(_MSC_VER)
	setlocale(LC_ALL, "de-DE");
#else
	EXPECT_STREQ("de_DE.utf8", setlocale(LC_ALL, "de_DE.utf8"));
#endif

	lc            = localeconv();
	char point_de = *lc->decimal_point;
	EXPECT_EQ(point_de, ',');

	kilometer<double> de = 2.0_km;
	EXPECT_STREQ("2 km", units::length::to_string(de).c_str());

	de = 2.5_km;
	EXPECT_STREQ("2,5 km", units::length::to_string(de).c_str());

	// US locale
#if defined(_MSC_VER)
	setlocale(LC_ALL, "en-US");
#else
	EXPECT_STREQ("en_US.utf8", setlocale(LC_ALL, "en_US.utf8"));
#endif

	lc            = localeconv();
	char point_us = *lc->decimal_point;
	EXPECT_EQ(point_us, '.');

	mile<double> us = 2.0_mi;
	EXPECT_STREQ("2 mi", units::length::to_string(us).c_str());

	us = 2.5_mi;
	EXPECT_STREQ("2.5 mi", units::length::to_string(us).c_str());
}

TEST_F(UnitType, nameAndAbbreviation)
{
	foot<double> a(3.5);
	EXPECT_STREQ("ft", unit_abbreviation_v<decltype(a)>);
	EXPECT_STREQ("ft", a.abbreviation());
	EXPECT_STREQ("foot", a.name());

	meter<double> b(8);
	EXPECT_STREQ("m", unit_abbreviation_v<decltype(b)>);
	EXPECT_STREQ("m", b.abbreviation());
	EXPECT_STREQ("meter", b.name());
}
#endif

TEST_F(UnitType, negative)
{
	meter<double> a(5.3);
	meter<double> b(-5.3);
	EXPECT_NEAR(a.to<double>(), -b.to<double>(), 5.0e-320);
	EXPECT_NEAR(b.to<double>(), -a.to<double>(), 5.0e-320);

	dB<double> c(2.87);
	dB<double> d(-2.87);
	EXPECT_NEAR(c.to<double>(), -d.to<double>(), 5.0e-320);
	EXPECT_NEAR(d.to<double>(), -c.to<double>(), 5.0e-320);

	ppm<double> e = -1 * ppm<double>(10);
	EXPECT_EQ(e, -ppm<double>(10));
	EXPECT_NEAR(-0.00001, e, 5.0e-10);
}

TEST_F(UnitType, concentration)
{
	ppb<double> a(ppm<double>(1));
	EXPECT_EQ(ppb<double>(1000), a);
	EXPECT_EQ(0.000001, a);
	EXPECT_EQ(0.000001, a.to<double>());

	dimensionless<double> b(ppm<double>(1));
	EXPECT_EQ(0.000001, b);

	dimensionless<double> c = ppb<double>(1);
	EXPECT_EQ(0.000000001, c);
}

TEST_F(UnitType, dBConversion)
{
	dBW<double> a_dbw(23.1);
	watt<double> a_w  = a_dbw;
	dBm<double> a_dbm = a_dbw;

	EXPECT_NEAR(204.173794, a_w.value(), 5.0e-7);
	EXPECT_NEAR(53.1, a_dbm.value(), 5.0e-7);

	milliwatt<double> b_mw(100000.0);
	watt<double> b_w  = b_mw;
	dBm<double> b_dbm = b_mw;
	dBW<double> b_dbw = b_mw;

	EXPECT_NEAR(100.0, b_w.value(), 5.0e-7);
	EXPECT_NEAR(50.0, b_dbm.value(), 5.0e-7);
	EXPECT_NEAR(20.0, b_dbw.value(), 5.0e-7);
}

TEST_F(UnitType, dBAddition)
{
	bool isSame;

	auto result_dbw = dBW<double>(10.0) + dB<double>(30.0);
	EXPECT_NEAR(40.0, result_dbw.value(), 5.0e-5);
	result_dbw =
		dBW<int>(10) + dB<int>(30);
	EXPECT_NEAR(40.0, result_dbw.value(), 5.0e-5);
	result_dbw = dB<double>(12.0) + dBW<double>(30.0);
	EXPECT_NEAR(42.0, result_dbw.value(), 5.0e-5);
	result_dbw =
		dB<int>(12) + dBW<int>(30);
	EXPECT_NEAR(42.0, result_dbw.value(), 2);
	isSame = std::is_same_v<decltype(result_dbw), dBW<double>>;
	EXPECT_TRUE(isSame);

	auto result_dbm = dB<double>(30.0) + dBm<double>(20.0);
	EXPECT_NEAR(50.0, result_dbm.value(), 5.0e-5);
	result_dbm =
		dB<int>(30) + dBm<int>(20);
	EXPECT_NEAR(50.0, result_dbm.value(), 5.0e-5);

	// adding dBW to dBW is something you probably shouldn't do, but let's see if it works...
	unit<squared<dBW<double>>> result_dBW2 = ::units::power::dBW<double>(10.0) + dBm<double>(40.0);
	EXPECT_NEAR(100.0, result_dBW2.to_linearized(), 5.0e-5);
	unit<squared<dBW<int>>> result_dBW3 = dBW<int>(10) + dBm<int>(40);
	EXPECT_NEAR(100.0, result_dBW3.to_linearized(), 5.0e-5);
}

TEST_F(UnitType, dBSubtraction)
{
	bool isSame;

	auto result_dbw = dBW<double>(10.0) - dB<double>(30.0);
	EXPECT_NEAR(-20.0, result_dbw.value(), 5.0e-5);
	isSame = std::is_same_v<decltype(result_dbw), dBW<double>>;
	EXPECT_TRUE(isSame);

	auto result_dbm = dBm<double>(100.0) - dB<double>(30.0);
	EXPECT_NEAR(70.0, result_dbm.value(), 5.0e-5);
	isSame = std::is_same_v<decltype(result_dbm), dBm<double>>;
	EXPECT_TRUE(isSame);

	auto result_db = dBW<double>(100.0) - dBW<double>(80.0);
	EXPECT_NEAR(20.0, result_db.value(), 5.0e-5);
	isSame = std::is_same_v<decltype(result_db), dB<double>>;
	EXPECT_TRUE(isSame);

	result_db = dB<double>(100.0) - dB<double>(80.0);
	EXPECT_NEAR(20.0, result_db.value(), 5.0e-5);
	isSame = std::is_same_v<decltype(result_db), dB<double>>;
	EXPECT_TRUE(isSame);
}

TEST_F(UnitType, unit_cast)
{
	meter<double> test1(5.7);
	hectare<double> test2(16);

	double dResult1 = 5.7;

	double dResult2 = 16;
	int iResult2    = 16;

	EXPECT_EQ(dResult1, unit_cast<double>(test1));
	EXPECT_EQ(dResult2, unit_cast<double>(test2));
	EXPECT_EQ(iResult2, unit_cast<int>(test2));

	EXPECT_TRUE((std::is_same_v<double, decltype(unit_cast<double>(test1))>));
	EXPECT_TRUE((std::is_same_v<int, decltype(unit_cast<int>(test2))>));
}

// literal syntax is only supported in GCC 4.7+ and MSVC2015+
TEST_F(UnitType, literals)
{
	// basic functionality testing
	EXPECT_TRUE((std::is_same_v<decltype(16.2_m), meter<double>>));
	EXPECT_TRUE((std::is_same_v<decltype(16_m), meter<int>>));
	EXPECT_TRUE(meter<double>(16.2) == 16.2_m);
	EXPECT_TRUE(meter<double>(16) == 16.0_m);
	EXPECT_TRUE(meter<int>(16) == 16_m);

	EXPECT_TRUE((std::is_same_v<decltype(11.2_ft), foot<double>>));
	EXPECT_TRUE((std::is_same_v<decltype(11_ft), foot<int>>));
	EXPECT_TRUE(foot<double>(11.2) == 11.2_ft);
	EXPECT_TRUE(foot<double>(11) == 11.0_ft);
	EXPECT_TRUE(foot<int>(11) == 11_ft);

	// auto using literal syntax
	auto x = 10.0_m;
	EXPECT_TRUE((std::is_same_v<decltype(x), meter<double>>));
	EXPECT_TRUE(meter<double>(10) == x);

	// conversion using literal syntax
	foot<double> y = 0.3048_m;
	EXPECT_TRUE(1.0_ft == y);

	// Pythagorean theorem
	meter<double> a = 3.0_m;
	meter<double> b = 4.0_m;
	meter<double> c = sqrt(pow<2>(a) + pow<2>(b));
	EXPECT_TRUE(c == 5.0_m);
}

TEST_F(ConversionFactor, length)
{
	double test;
	test = nanometer<double>(0.000000001_m).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = micrometer<double>(meter<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = millimeter<double>(meter<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = centimeter<double>(meter<double>(0.01)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = kilometer<double>(meter<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = meter<double>(meter<double>(1.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = foot<double>(meter<double>(0.3048)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = mile<double>(meter<double>(1609.344)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = inch<double>(meter<double>(0.0254)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = nautical_mile<double>(meter<double>(1852.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = astronomical_unit<double>(meter<double>(149597870700.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = lightyear<double>(meter<double>(9460730472580800.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = parsec<double>(meter<double>(3.08567758e16)).value();
	EXPECT_NEAR(1.0, test, 5.0e7);

	test = foot<double>(foot<double>(6.3)).value();
	EXPECT_NEAR(6.3, test, 5.0e-5);
	test = inch<double>(foot<double>(6.0)).value();
	EXPECT_NEAR(72.0, test, 5.0e-5);
	test = foot<double>(inch<double>(6.0)).value();
	EXPECT_NEAR(0.5, test, 5.0e-5);
	test = foot<double>(meter<double>(1.0)).value();
	EXPECT_NEAR(3.28084, test, 5.0e-5);
	test = nautical_mile<double>(mile<double>(6.3)).value();
	EXPECT_NEAR(5.47455, test, 5.0e-6);
	test = meter<double>(mile<double>(11.0)).value();
	EXPECT_NEAR(17702.8, test, 5.0e-2);
	test = chain<double>(meter<double>(1.0)).value();
	EXPECT_NEAR(0.0497097, test, 5.0e-7);

	EXPECT_EQ(metre<double>(1), meter<double>(1));
}

TEST_F(ConversionFactor, mass)
{
	double test;

	test = gram<double>(kilogram<double>(1.0e-3)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = microgram<double>(kilogram<double>(1.0e-9)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = milligram<double>(kilogram<double>(1.0e-6)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = kilogram<double>(kilogram<double>(1.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = metric_ton<double>(kilogram<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = mass::pound<double>(kilogram<double>(0.453592)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = long_ton<double>(kilogram<double>(1016.05)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = short_ton<double>(kilogram<double>(907.185)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = mass::ounce<double>(kilogram<double>(0.0283495)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = carat<double>(kilogram<double>(0.0002)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = kilogram<double>(slug<double>(1.0)).value();
	EXPECT_NEAR(14.593903, test, 5.0e-7);

	test = carat<double>(mass::pound<double>(6.3)).value();
	EXPECT_NEAR(14288.2, test, 5.0e-2);
}

TEST_F(ConversionFactor, time)
{
	double result      = 0;
	double daysPerYear = 365;
	double hoursPerDay = 24;
	double minsPerHour = 60;
	double secsPerMin  = 60;
	double daysPerWeek = 7;

	result = 2 * daysPerYear * hoursPerDay * minsPerHour * secsPerMin * (1 / minsPerHour) * (1 / secsPerMin) *
		(1 / hoursPerDay) * (1 / daysPerWeek);
	EXPECT_NEAR(104.286, result, 5.0e-4);

	year<double> twoYears(2.0);
	week<double> twoYearsInWeeks = twoYears;
	EXPECT_NEAR(week<double>(104.286).to<double>(), twoYearsInWeeks.to<double>(), 5.0e-4);

	double test;

	test = second<double>(second<double>(1.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = nanosecond<double>(second<double>(1.0e-9)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = microsecond<double>(second<double>(1.0e-6)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = millisecond<double>(second<double>(1.0e-3)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = minute<double>(second<double>(60.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = hour<double>(second<double>(3600.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = day<double>(second<double>(86400.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = week<double>(second<double>(604800.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = year<double>(second<double>(3.154e7)).value();
	EXPECT_NEAR(1.0, test, 5.0e3);

	test = week<double>(year<double>(2.0)).value();
	EXPECT_NEAR(104.2857142857143, test, 5.0e-14);
	test = minute<double>(hour<double>(4.0)).value();
	EXPECT_NEAR(240.0, test, 5.0e-14);
	test = day<double>(julian_year<double>(1.0)).value();
	EXPECT_NEAR(365.25, test, 5.0e-14);
	test = day<double>(gregorian_year<double>(1.0)).value();
	EXPECT_NEAR(365.2425, test, 5.0e-14);
}

TEST_F(ConversionFactor, angle)
{
	angle::degree<double> quarterCircleDeg(90.0);
	angle::radian<double> quarterCircleRad = quarterCircleDeg;
	EXPECT_NEAR(angle::radian<double>(detail::PI_VAL / 2.0).to<double>(), quarterCircleRad.to<double>(), 5.0e-12);

	double test;

	test = angle::radian<double>(angle::radian<double>(1.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = angle::milliradian<double>(angle::radian<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-4);
	test = angle::degree<double>(angle::radian<double>(0.0174533)).value();
	EXPECT_NEAR(1.0, test, 5.0e-7);
	test = angle::arcminute<double>(angle::radian<double>(0.000290888)).value();
	EXPECT_NEAR(0.99999928265913, test, 5.0e-8);
	test = angle::arcsecond<double>(angle::radian<double>(4.8481e-6)).value();
	EXPECT_NEAR(0.999992407, test, 5.0e-10);
	test = angle::turn<double>(angle::radian<double>(6.28319)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = angle::gradian<double>(angle::radian<double>(0.015708)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);

	test = angle::radian<double>(angle::radian<double>(2.1)).value();
	EXPECT_NEAR(2.1, test, 5.0e-6);
	test = angle::gradian<double>(angle::arcsecond<double>(2.1)).value();
	EXPECT_NEAR(0.000648148, test, 5.0e-6);
	test = angle::degree<double>(angle::radian<double>(detail::PI_VAL)).value();
	EXPECT_NEAR(180.0, test, 5.0e-6);
	test = angle::radian<double>(angle::degree<double>(90.0)).value();
	EXPECT_NEAR(detail::PI_VAL / 2, test, 5.0e-6);
}

TEST_F(ConversionFactor, current)
{
	double test;

	test = current::milliampere<double>(current::ampere<double>(2.1)).value();
	EXPECT_NEAR(2100.0, test, 5.0e-6);
}

TEST_F(ConversionFactor, temperature)
{
	// temp conversion are weird/hard since they involve translations AND scaling.
	double test;

	test = kelvin<double>(kelvin<double>(72.0)).value();
	EXPECT_NEAR(72.0, test, 5.0e-5);
	test = fahrenheit<double>(fahrenheit<double>(72.0)).value();
	EXPECT_NEAR(72.0, test, 5.0e-5);
	test = fahrenheit<double>(kelvin<double>(300.0)).value();
	EXPECT_NEAR(80.33, test, 5.0e-5);
	test = kelvin<double>(fahrenheit<double>(451.0)).value();
	EXPECT_NEAR(505.928, test, 5.0e-4);
	test = celsius<double>(kelvin<double>(300.0)).value();
	EXPECT_NEAR(26.85, test, 5.0e-3);
	test = kelvin<double>(celsius<double>(451.0)).value();
	EXPECT_NEAR(724.15, test, 5.0e-3);
	test = celsius<double>(fahrenheit<double>(72.0)).value();
	EXPECT_NEAR(22.2222, test, 5.0e-5);
	test = fahrenheit<double>(celsius<double>(100.0)).value();
	EXPECT_NEAR(212.0, test, 5.0e-5);
	test = celsius<double>(fahrenheit<double>(32.0)).value();
	EXPECT_NEAR(0.0, test, 5.0e-5);
	test = fahrenheit<double>(celsius<double>(0.0)).value();
	EXPECT_NEAR(32.0, test, 5.0e-5);
	test = kelvin<double>(rankine<double>(100.0)).value();
	EXPECT_NEAR(55.5556, test, 5.0e-5);
	test = rankine<double>(kelvin<double>(100.0)).value();
	EXPECT_NEAR(180.0, test, 5.0e-5);
	test = rankine<double>(fahrenheit<double>(100.0)).value();
	EXPECT_NEAR(559.67, test, 5.0e-5);
	test = fahrenheit<double>(rankine<double>(72.0)).value();
	EXPECT_NEAR(-387.67, test, 5.0e-5);
	test = kelvin<double>(reaumur<double>(100.0)).value();
	EXPECT_NEAR(398.0, test, 5.0e-1);
	test = celsius<double>(reaumur<double>(80.0)).value();
	EXPECT_NEAR(100.0, test, 5.0e-5);
	test = reaumur<double>(celsius<double>(212.0)).value();
	EXPECT_NEAR(169.6, test, 5.0e-2);
	test = fahrenheit<double>(reaumur<double>(80.0)).value();
	EXPECT_NEAR(212.0, test, 5.0e-5);
	test = reaumur<double>(fahrenheit<double>(37.0)).value();
	EXPECT_NEAR(2.222, test, 5.0e-3);
}

TEST_F(ConversionFactor, luminous_intensity)
{
	double test;

	test = millicandela<double>(candela<double>(72.0)).value();
	EXPECT_NEAR(72000.0, test, 5.0e-5);
	test = candela<double>(millicandela<double>(376.0)).value();
	EXPECT_NEAR(0.376, test, 5.0e-5);
}

TEST_F(ConversionFactor, substance)
{
	static_assert(1_g / 1_mol == 1_g_per_mol);
	static_assert(1_mol / 1_g == 1_M);
}

TEST_F(ConversionFactor, solid_angle)
{
	double test;
	bool same;

	same = std::is_same_v<traits::dimension_of_t<steradian_conversion_factor>,
		traits::dimension_of_t<degree_squared_conversion_factor>>;
	EXPECT_TRUE(same);

	test = steradian<double>(steradian<double>(72.0)).value();
	EXPECT_NEAR(72.0, test, 5.0e-5);
	test = degree_squared<double>(steradian<double>(1.0)).value();
	EXPECT_NEAR(3282.8, test, 5.0e-2);
	test = spat<double>(steradian<double>(8.0)).value();
	EXPECT_NEAR(0.636619772367582, test, 5.0e-14);
	test = steradian<double>(degree_squared<double>(3282.8)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = degree_squared<double>(degree_squared<double>(72.0)).value();
	EXPECT_NEAR(72.0, test, 5.0e-5);
	test = spat<double>(degree_squared<double>(3282.8)).value();
	EXPECT_NEAR(1.0 / (4 * detail::PI_VAL), test, 5.0e-5);
	test = steradian<double>(spat<double>(1.0 / (4 * detail::PI_VAL))).value();
	EXPECT_NEAR(1.0, test, 5.0e-14);
	test = degree_squared<double>(spat<double>(1.0 / (4 * detail::PI_VAL))).value();
	EXPECT_NEAR(3282.8, test, 5.0e-2);
	test = spat<double>(spat<double>(72.0)).value();
	EXPECT_NEAR(72.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, frequency)
{
	double test;

	test = kilohertz<double>(hertz<double>(63000.0)).value();
	EXPECT_NEAR(63.0, test, 5.0e-5);
	test = hertz<double>(hertz<double>(6.3)).value();
	EXPECT_NEAR(6.3, test, 5.0e-5);
	test = hertz<double>(kilohertz<double>(5.0)).value();
	EXPECT_NEAR(5000.0, test, 5.0e-5);
	test = hertz<double>(megahertz<double>(1.0)).value();
	EXPECT_NEAR(1.0e6, test, 5.0e-5);
}

TEST_F(ConversionFactor, velocity)
{
	double test;
	bool same;

	same = std::is_same_v<meters_per_second_conversion_factor,
		traits::strong_t<conversion_factor<std::ratio<1>, dimension::velocity>>>;
	EXPECT_TRUE(same);
	same = traits::is_convertible_unit_v<miles_per_hour_conversion_factor, meters_per_second_conversion_factor>;
	EXPECT_TRUE(same);

	test = miles_per_hour<double>(meters_per_second<double>(1250.0)).value();
	EXPECT_NEAR(2796.17, test, 5.0e-3);
	test = kilometers_per_hour<double>(feet_per_second<double>(2796.17)).value();
	EXPECT_NEAR(3068.181418, test, 5.0e-7);
	test = miles_per_hour<double>(knot<double>(600.0)).value();
	EXPECT_NEAR(690.468, test, 5.0e-4);
	test = feet_per_second<double>(miles_per_hour<double>(120.0)).value();
	EXPECT_NEAR(176.0, test, 5.0e-5);
	test = meters_per_second<double>(feet_per_second<double>(10.0)).value();
	EXPECT_NEAR(3.048, test, 5.0e-5);
}

TEST_F(ConversionFactor, angular_velocity)
{
	double test;
	bool same;

	same = std::is_same_v<radians_per_second_conversion_factor,
		traits::strong_t<conversion_factor<std::ratio<1>, dimension::angular_velocity>>>;
	EXPECT_TRUE(same);
	same =
		traits::is_convertible_unit_v<revolutions_per_minute_conversion_factor, radians_per_second_conversion_factor>;
	EXPECT_TRUE(same);

	test = milliarcseconds_per_year<double>(radians_per_second<double>(1.0)).value();
	EXPECT_NEAR(6.504e15, test, 1.0e12);
	test = radians_per_second<double>(degrees_per_second<double>(1.0)).value();
	EXPECT_NEAR(0.0174533, test, 5.0e-8);
	test = radians_per_second<double>(revolutions_per_minute<double>(1.0)).value();
	EXPECT_NEAR(0.10471975512, test, 5.0e-13);
	test = radians_per_second<double>(milliarcseconds_per_year<double>(1.0)).value();
	EXPECT_NEAR(1.537e-16, test, 5.0e-20);
}

TEST_F(ConversionFactor, acceleration)
{
	double test;

	test = meters_per_second_squared<double>(standard_gravity<double>(1.0)).value();
	EXPECT_NEAR(9.80665, test, 5.0e-10);
}

TEST_F(ConversionFactor, force)
{
	double test;

	test = units::force::newton<double>(units::force::newton<double>(1.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = units::force::pound<double>(units::force::newton<double>(6.3)).value();
	EXPECT_NEAR(1.4163, test, 5.0e-5);
	test = units::force::dyne<double>(units::force::newton<double>(5.0)).value();
	EXPECT_NEAR(500000.0, test, 5.0e-5);
	test = units::force::poundal<double>(units::force::newton<double>(2.1)).value();
	EXPECT_NEAR(15.1893, test, 5.0e-5);
	test = units::force::kilopond<double>(units::force::newton<double>(173.0)).value();
	EXPECT_NEAR(17.6411, test, 5.0e-5);
	test = units::force::kilopond<double>(units::force::poundal<double>(21.879)).value();
	EXPECT_NEAR(0.308451933, test, 5.0e-10);
}

TEST_F(ConversionFactor, area)
{
	double test;

	test = acre<double>(hectare<double>(6.3)).value();
	EXPECT_NEAR(15.5676, test, 5.0e-5);
	test = square_kilometer<double>(square_mile<double>(10.0)).value();
	EXPECT_NEAR(25.8999, test, 5.0e-5);
	test = square_meter<double>(square_inch<double>(4.0)).value();
	EXPECT_NEAR(0.00258064, test, 5.0e-9);
	test = square_foot<double>(acre<double>(5.0)).value();
	EXPECT_NEAR(217800.0, test, 5.0e-5);
	test = square_foot<double>(square_meter<double>(1.0)).value();
	EXPECT_NEAR(10.7639, test, 5.0e-5);
}

TEST_F(ConversionFactor, pressure)
{
	double test;

	test = torr<double>(pascal<double>(1.0)).value();
	EXPECT_NEAR(0.00750062, test, 5.0e-5);
	test = pounds_per_square_inch<double>(bar<double>(2.2)).value();
	EXPECT_NEAR(31.9083, test, 5.0e-5);
	test = bar<double>(atmosphere<double>(4.0)).value();
	EXPECT_NEAR(4.053, test, 5.0e-5);
	test = pascal<double>(torr<double>(800.0)).value();
	EXPECT_NEAR(106657.89474, test, 5.0e-5);
	test = atmosphere<double>(pounds_per_square_inch<double>(38.0)).value();
	EXPECT_NEAR(2.58575, test, 5.0e-5);
	test = pascal<double>(pounds_per_square_inch<double>(1.0)).value();
	EXPECT_NEAR(6894.76, test, 5.0e-3);
	test = bar<double>(pascal<double>(0.25)).value();
	EXPECT_NEAR(2.5e-6, test, 5.0e-5);
	test = atmosphere<double>(torr<double>(9.0)).value();
	EXPECT_NEAR(0.0118421, test, 5.0e-8);
	test = torr<double>(bar<double>(12.0)).value();
	EXPECT_NEAR(9000.74, test, 5.0e-3);
	test = pounds_per_square_inch<double>(atmosphere<double>(1.0)).value();
	EXPECT_NEAR(14.6959, test, 5.0e-5);

	EXPECT_EQ(133.322387415_Pa, 1.0_mmHg);
}

TEST_F(ConversionFactor, charge)
{
	double test;

	test = ampere_hour<double>(coulomb<double>(4.0)).value();
	EXPECT_NEAR(0.00111111, test, 5.0e-9);
	test = coulomb<double>(ampere_hour<double>(1.0)).value();
	EXPECT_NEAR(3600.0, test, 5.0e-6);
}

TEST_F(ConversionFactor, energy)
{
	double test;

	test = calorie<double>(joule<double>(8000.000464)).value();
	EXPECT_NEAR(1912.046, test, 5.0e-4);
	test = joule<double>(therm<double>(12.0)).value();
	EXPECT_NEAR(1.266e+9, test, 5.0e5);
	test = watt_hour<double>(megajoule<double>(100.0)).value();
	EXPECT_NEAR(27777.778, test, 5.0e-4);
	test = megajoule<double>(kilocalorie<double>(56.0)).value();
	EXPECT_NEAR(0.234304, test, 5.0e-7);
	test = therm<double>(kilojoule<double>(56.0)).value();
	EXPECT_NEAR(0.000530904, test, 5.0e-5);
	test = kilojoule<double>(british_thermal_unit<double>(18.56399995447)).value();
	EXPECT_NEAR(19.5860568, test, 5.0e-5);
	test = energy::foot_pound<double>(calorie<double>(18.56399995447)).value();
	EXPECT_NEAR(57.28776190423856, test, 5.0e-5);
	test = calorie<double>(megajoule<double>(1.0)).value();
	EXPECT_NEAR(239006.0, test, 5.0e-1);
	test = kilowatt_hour<double>(kilocalorie<double>(2.0)).value();
	EXPECT_NEAR(0.00232444, test, 5.0e-9);
	test = kilocalorie<double>(therm<double>(0.1)).value();
	EXPECT_NEAR(2521.04, test, 5.0e-3);
	test = megajoule<double>(watt_hour<double>(67.0)).value();
	EXPECT_NEAR(0.2412, test, 5.0e-5);
	test = watt_hour<double>(british_thermal_unit<double>(100.0)).value();
	EXPECT_NEAR(29.3071, test, 5.0e-5);
	test = british_thermal_unit<double>(calorie<double>(100.0)).value();
	EXPECT_NEAR(0.396567, test, 5.0e-5);
}

TEST_F(ConversionFactor, power)
{
	double test;

	test = watt<double>(
		unit<compound_conversion_factor<energy::foot_pound_conversion_factor, inverse<second_conversion_factor>>>(
			550.0)).value();
	EXPECT_NEAR(745.7, test, 5.0e-2);
	test = gigawatt<double>(watt<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-4);
	test = watt<double>(microwatt<double>(200000.0)).value();
	EXPECT_NEAR(0.2, test, 5.0e-4);
	test = watt<double>(horsepower<double>(100.0)).value();
	EXPECT_NEAR(74570.0, test, 5.0e-1);
	test = megawatt<double>(horsepower<double>(5.0)).value();
	EXPECT_NEAR(0.0037284994, test, 5.0e-7);
	test = horsepower<double>(kilowatt<double>(232.0)).value();
	EXPECT_NEAR(311.117, test, 5.0e-4);
	test = horsepower<double>(milliwatt<double>(1001.0)).value();
	EXPECT_NEAR(0.001342363, test, 5.0e-9);
}

TEST_F(ConversionFactor, voltage)
{
	double test;

	test = millivolt<double>(volt<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = volt<double>(picovolt<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = volt<double>(nanovolt<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = volt<double>(microvolt<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = volt<double>(millivolt<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = volt<double>(kilovolt<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = volt<double>(megavolt<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = volt<double>(gigavolt<double>(0.000000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = volt<double>(statvolt<double>(299.792458)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = statvolt<double>(millivolt<double>(1000.0)).value();
	EXPECT_NEAR(299.792458, test, 5.0e-5);
	test = nanovolt<double>(abvolt<double>(0.1)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = abvolt<double>(microvolt<double>(0.01)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, capacitance)
{
	double test;

	test = millifarad<double>(farad<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = farad<double>(picofarad<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = farad<double>(nanofarad<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = farad<double>(microfarad<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = farad<double>(millifarad<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = farad<double>(kilofarad<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = farad<double>(megafarad<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = farad<double>(gigafarad<double>(0.000000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);

	auto f           = coulomb<double>(1) / volt<double>(1);
	farad<double> f2 = coulomb<double>(1) / volt<double>(1);
	EXPECT_TRUE((std::is_convertible_v<decltype(f), farad<double>>));

	auto one_farad = []() -> farad<double> { return coulomb<double>(1) / volt<double>(1); };

	EXPECT_EQ(1.0_F, one_farad());
}

TEST_F(ConversionFactor, impedance)
{
	double test;

	test = milliohm<double>(ohm<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = ohm<double>(picoohm<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = ohm<double>(nanoohm<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = ohm<double>(microohm<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = ohm<double>(milliohm<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = ohm<double>(kiloohm<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = ohm<double>(megaohm<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = ohm<double>(gigaohm<double>(0.000000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, conductance)
{
	double test;

	test = millisiemens<double>(siemens<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = siemens<double>(picosiemens<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = siemens<double>(nanosiemens<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = siemens<double>(microsiemens<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = siemens<double>(millisiemens<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = siemens<double>(kilosiemens<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = siemens<double>(megasiemens<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = siemens<double>(gigasiemens<double>(0.000000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, magnetic_flux)
{
	double test;

	test = milliweber<double>(weber<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = weber<double>(picoweber<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = weber<double>(nanoweber<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = weber<double>(microweber<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = weber<double>(milliweber<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = weber<double>(kiloweber<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = weber<double>(megaweber<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = weber<double>(gigaweber<double>(0.000000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = weber<double>(maxwell<double>(100000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = maxwell<double>(nanoweber<double>(10.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, magnetic_field_strength)
{
	double test;

	test = millitesla<double>(tesla<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = tesla<double>(picotesla<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = tesla<double>(nanotesla<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = tesla<double>(microtesla<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = tesla<double>(millitesla<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = tesla<double>(kilotesla<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = tesla<double>(megatesla<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = tesla<double>(gigatesla<double>(0.000000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = tesla<double>(gauss<double>(10000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = gauss<double>(nanotesla<double>(100000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, inductance)
{
	double test;

	test = millihenry<double>(henry<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = henry<double>(picohenry<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = henry<double>(nanohenry<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = henry<double>(microhenry<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = henry<double>(millihenry<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = henry<double>(kilohenry<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = henry<double>(megahenry<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = henry<double>(gigahenry<double>(0.000000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, luminous_flux)
{
	double test;

	test = millilumen<double>(lumen<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = lumen<double>(picolumen<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lumen<double>(nanolumen<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lumen<double>(microlumen<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lumen<double>(millilumen<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lumen<double>(kilolumen<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lumen<double>(megalumen<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lumen<double>(gigalumen<double>(0.000000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, illuminance)
{
	double test;

	test = millilux<double>(lux<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = lux<double>(picolux<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lux<double>(nanolux<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lux<double>(microlux<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lux<double>(millilux<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lux<double>(kilolux<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lux<double>(megalux<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lux<double>(gigalux<double>(0.000000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);

	// 	test = lux<double>(footcandle<double>(0.092903)).value();
	// 	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lumens_per_square_inch<double>(lux<double>(1550.0031000062)).value();
	EXPECT_NEAR(1.0, test, 5.0e-13);
	test = lux<double>(phot<double>(0.0001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, radiation)
{
	double test;

	test = millibecquerel<double>(becquerel<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = becquerel<double>(picobecquerel<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = becquerel<double>(nanobecquerel<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = becquerel<double>(microbecquerel<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = becquerel<double>(millibecquerel<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = becquerel<double>(kilobecquerel<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = becquerel<double>(megabecquerel<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = becquerel<double>(gigabecquerel<double>(0.000000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);

	test = milligray<double>(gray<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = gray<double>(picogray<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = gray<double>(nanogray<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = gray<double>(microgray<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = gray<double>(milligray<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = gray<double>(kilogray<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = gray<double>(megagray<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = gray<double>(gigagray<double>(0.000000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);

	test = millisievert<double>(sievert<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = sievert<double>(picosievert<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = sievert<double>(nanosievert<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = sievert<double>(microsievert<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = sievert<double>(millisievert<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = sievert<double>(kilosievert<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = sievert<double>(megasievert<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = sievert<double>(gigasievert<double>(0.000000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);

	test = curie<double>(becquerel<double>(37.0e9)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = rutherford<double>(becquerel<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = gray<double>(rad<double>(100.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, torque)
{
	double test;

	test = newton_meter<double>(torque::foot_pound<double>(1.0)).value();
	EXPECT_NEAR(1.355817948, test, 5.0e-5);
	test = newton_meter<double>(inch_pound<double>(1.0)).value();
	EXPECT_NEAR(0.112984829, test, 5.0e-5);
	test = newton_meter<double>(foot_poundal<double>(1.0)).value();
	EXPECT_NEAR(4.214011009e-2, test, 5.0e-5);
	test = newton_meter<double>(meter_kilogram<double>(1.0)).value();
	EXPECT_NEAR(9.80665, test, 5.0e-5);
	test = meter_kilogram<double>(inch_pound<double>(86.79616930855788)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = inch_pound<double>(foot_poundal<double>(2.681170713)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, volume)
{
	double test;

	test = cubic_meter<double>(cubic_meter<double>(1.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = cubic_meter<double>(cubic_millimeter<double>(1.0)).value();
	EXPECT_NEAR(1.0e-9, test, 5.0e-5);
	test = cubic_meter<double>(cubic_kilometer<double>(1.0)).value();
	EXPECT_NEAR(1.0e9, test, 5.0e-5);
	test = cubic_meter<double>(liter<double>(1.0)).value();
	EXPECT_NEAR(0.001, test, 5.0e-5);
	test = cubic_meter<double>(milliliter<double>(1.0)).value();
	EXPECT_NEAR(1.0e-6, test, 5.0e-5);
	test = cubic_meter<double>(cubic_inch<double>(1.0)).value();
	EXPECT_NEAR(1.6387e-5, test, 5.0e-10);
	test = cubic_meter<double>(cubic_foot<double>(1.0)).value();
	EXPECT_NEAR(0.0283168, test, 5.0e-8);
	test = cubic_meter<double>(cubic_yard<double>(1.0)).value();
	EXPECT_NEAR(0.764555, test, 5.0e-7);
	test = cubic_meter<double>(cubic_mile<double>(1.0)).value();
	EXPECT_NEAR(4.168e+9, test, 5.0e5);
	test = cubic_meter<double>(gallon<double>(1.0)).value();
	EXPECT_NEAR(0.00378541, test, 5.0e-8);
	test = cubic_meter<double>(quart<double>(1.0)).value();
	EXPECT_NEAR(0.000946353, test, 5.0e-10);
	test = cubic_meter<double>(pint<double>(1.0)).value();
	EXPECT_NEAR(0.000473176, test, 5.0e-10);
	test = cubic_meter<double>(cup<double>(1.0)).value();
	EXPECT_NEAR(0.00024, test, 5.0e-6);
	test = cubic_meter<double>(volume::fluid_ounce<double>(1.0)).value();
	EXPECT_NEAR(2.9574e-5, test, 5.0e-5);
	test = cubic_meter<double>(barrel<double>(1.0)).value();
	EXPECT_NEAR(0.158987294928, test, 5.0e-13);
	test = cubic_meter<double>(bushel<double>(1.0)).value();
	EXPECT_NEAR(0.0352391, test, 5.0e-8);
	test = cubic_meter<double>(cord<double>(1.0)).value();
	EXPECT_NEAR(3.62456, test, 5.0e-6);
	test = cubic_meter<double>(cubic_fathom<double>(1.0)).value();
	EXPECT_NEAR(6.11644, test, 5.0e-6);
	test = cubic_meter<double>(tablespoon<double>(1.0)).value();
	EXPECT_NEAR(1.4787e-5, test, 5.0e-10);
	test = cubic_meter<double>(teaspoon<double>(1.0)).value();
	EXPECT_NEAR(4.9289e-6, test, 5.0e-11);
	test = cubic_meter<double>(pinch<double>(1.0)).value();
	EXPECT_NEAR(616.11519921875e-9, test, 5.0e-20);
	test = cubic_meter<double>(dash<double>(1.0)).value();
	EXPECT_NEAR(308.057599609375e-9, test, 5.0e-20);
	test = cubic_meter<double>(drop<double>(1.0)).value();
	EXPECT_NEAR(82.14869322916e-9, test, 5.0e-9);
	test = cubic_meter<double>(fifth<double>(1.0)).value();
	EXPECT_NEAR(0.00075708236, test, 5.0e-12);
	test = cubic_meter<double>(dram<double>(1.0)).value();
	EXPECT_NEAR(3.69669e-6, test, 5.0e-12);
	test = cubic_meter<double>(gill<double>(1.0)).value();
	EXPECT_NEAR(0.000118294, test, 5.0e-10);
	test = cubic_meter<double>(peck<double>(1.0)).value();
	EXPECT_NEAR(0.00880977, test, 5.0e-9);
	test = cubic_meter<double>(sack<double>(9.4591978)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = cubic_meter<double>(shot<double>(1.0)).value();
	EXPECT_NEAR(4.43603e-5, test, 5.0e-11);
	test = cubic_meter<double>(strike<double>(1.0)).value();
	EXPECT_NEAR(0.07047814033376, test, 5.0e-5);
	test = milliliter<double>(volume::fluid_ounce<double>(1.0)).value();
	EXPECT_NEAR(29.5735, test, 5.0e-5);
}

TEST_F(ConversionFactor, density)
{
	double test;

	test = kilograms_per_cubic_meter<double>(kilograms_per_cubic_meter<double>(1.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = kilograms_per_cubic_meter<double>(grams_per_milliliter<double>(1.0)).value();
	EXPECT_NEAR(1000.0, test, 5.0e-5);
	test = kilograms_per_cubic_meter<double>(kilograms_per_liter<double>(1.0)).value();
	EXPECT_NEAR(1000.0, test, 5.0e-5);
	test = kilograms_per_cubic_meter<double>(ounces_per_cubic_foot<double>(1.0)).value();
	EXPECT_NEAR(1.001153961, test, 5.0e-10);
	test = kilograms_per_cubic_meter<double>(ounces_per_cubic_inch<double>(1.0)).value();
	EXPECT_NEAR(1.729994044e3, test, 5.0e-7);
	test = kilograms_per_cubic_meter<double>(ounces_per_gallon<double>(1.0)).value();
	EXPECT_NEAR(7.489151707, test, 5.0e-10);
	test = kilograms_per_cubic_meter<double>(pounds_per_cubic_foot<double>(1.0)).value();
	EXPECT_NEAR(16.01846337, test, 5.0e-9);
	test = kilograms_per_cubic_meter<double>(pounds_per_cubic_inch<double>(1.0)).value();
	EXPECT_NEAR(2.767990471e4, test, 5.0e-6);
	test = kilograms_per_cubic_meter<double>(pounds_per_gallon<double>(1.0)).value();
	EXPECT_NEAR(119.8264273, test, 5.0e-8);
	test = kilograms_per_cubic_meter<double>(slugs_per_cubic_foot<double>(1.0)).value();
	EXPECT_NEAR(515.3788184, test, 5.0e-6);
}

TEST_F(ConversionFactor, concentration)
{
	double test;

	test = ppm<double>(1.0);
	EXPECT_NEAR(1.0e-6, test, 5.0e-12);
	test = ppb<double>(1.0);
	EXPECT_NEAR(1.0e-9, test, 5.0e-12);
	test = ppt<double>(1.0);
	EXPECT_NEAR(1.0e-12, test, 5.0e-12);
	test = percent<double>(18.0);
	EXPECT_NEAR(0.18, test, 5.0e-12);
}

TEST_F(ConversionFactor, data)
{
	EXPECT_EQ(8, (bit<double>(byte<double>(1)).value()));

	EXPECT_EQ(1000, (byte<double>(kilobyte<double>(1)).value()));
	EXPECT_EQ(1000, (kilobyte<double>(megabyte<double>(1)).value()));
	EXPECT_EQ(1000, (megabyte<double>(gigabyte<double>(1)).value()));
	EXPECT_EQ(1000, (gigabyte<double>(terabyte<double>(1)).value()));
	EXPECT_EQ(1000, (terabyte<double>(petabyte<double>(1)).value()));
	EXPECT_EQ(1000, (petabyte<double>(exabyte<double>(1)).value()));

	EXPECT_EQ(1024, (byte<double>(kibibyte<double>(1)).value()));
	EXPECT_EQ(1024, (kibibyte<double>(mebibyte<double>(1)).value()));
	EXPECT_EQ(1024, (mebibyte<double>(gibibyte<double>(1)).value()));
	EXPECT_EQ(1024, (gibibyte<double>(tebibyte<double>(1)).value()));
	EXPECT_EQ(1024, (tebibyte<double>(pebibyte<double>(1)).value()));
	EXPECT_EQ(1024, (pebibyte<double>(exbibyte<double>(1)).value()));

	EXPECT_EQ(93750000, (kibibit<double>(gigabyte<double>(12)).value()));

	EXPECT_EQ(1000, (bit<double>(kilobit<double>(1)).value()));
	EXPECT_EQ(1000, (kilobit<double>(megabit<double>(1)).value()));
	EXPECT_EQ(1000, (megabit<double>(gigabit<double>(1)).value()));
	EXPECT_EQ(1000, (gigabit<double>(terabit<double>(1)).value()));
	EXPECT_EQ(1000, (terabit<double>(petabit<double>(1)).value()));
	EXPECT_EQ(1000, (petabit<double>(exabit<double>(1)).value()));

	EXPECT_EQ(1024, (bit<double>(kibibit<double>(1)).value()));
	EXPECT_EQ(1024, (kibibit<double>(mebibit<double>(1)).value()));
	EXPECT_EQ(1024, (mebibit<double>(gibibit<double>(1)).value()));
	EXPECT_EQ(1024, (gibibit<double>(tebibit<double>(1)).value()));
	EXPECT_EQ(1024, (tebibit<double>(pebibit<double>(1)).value()));
	EXPECT_EQ(1024, (pebibit<double>(exbibit<double>(1)).value()));

	// Source: https://en.wikipedia.org/wiki/Binary_prefix
	EXPECT_NEAR(percent<double>(2.4), kibibyte<double>(1) / kilobyte<double>(1) - 1, 0.005);
	EXPECT_NEAR(percent<double>(4.9), mebibyte<double>(1) / megabyte<double>(1) - 1, 0.005);
	EXPECT_NEAR(percent<double>(7.4), gibibyte<double>(1) / gigabyte<double>(1) - 1, 0.005);
	EXPECT_NEAR(percent<double>(10.0), tebibyte<double>(1) / terabyte<double>(1) - 1, 0.005);
	EXPECT_NEAR(percent<double>(12.6), pebibyte<double>(1) / petabyte<double>(1) - 1, 0.005);
	EXPECT_NEAR(percent<double>(15.3), exbibyte<double>(1) / exabyte<double>(1) - 1, 0.005);
}

TEST_F(ConversionFactor, data_transfer_rate)
{
	EXPECT_EQ(8, (bits_per_second<double>(bytes_per_second<double>(1)).value()));

	EXPECT_EQ(1000, (bytes_per_second<double>(kilobytes_per_second<double>(1)).value()));
	EXPECT_EQ(1000, (kilobytes_per_second<double>(megabytes_per_second<double>(1)).value()));
	EXPECT_EQ(1000, (megabytes_per_second<double>(gigabytes_per_second<double>(1)).value()));
	EXPECT_EQ(1000, (gigabytes_per_second<double>(terabytes_per_second<double>(1)).value()));
	EXPECT_EQ(1000, (terabytes_per_second<double>(petabytes_per_second<double>(1)).value()));
	EXPECT_EQ(1000, (petabytes_per_second<double>(exabytes_per_second<double>(1)).value()));

	EXPECT_EQ(1024, (bytes_per_second<double>(kibibytes_per_second<double>(1)).value()));
	EXPECT_EQ(1024, (kibibytes_per_second<double>(mebibytes_per_second<double>(1)).value()));
	EXPECT_EQ(1024, (mebibytes_per_second<double>(gibibytes_per_second<double>(1)).value()));
	EXPECT_EQ(1024, (gibibytes_per_second<double>(tebibytes_per_second<double>(1)).value()));
	EXPECT_EQ(1024, (tebibytes_per_second<double>(pebibytes_per_second<double>(1)).value()));
	EXPECT_EQ(1024, (pebibytes_per_second<double>(exbibytes_per_second<double>(1)).value()));

	EXPECT_EQ(93750000, (kibibits_per_second<double>(gigabytes_per_second<double>(12)).value()));

	EXPECT_EQ(1000, (bits_per_second<double>(kilobits_per_second<double>(1)).value()));
	EXPECT_EQ(1000, (kilobits_per_second<double>(megabits_per_second<double>(1)).value()));
	EXPECT_EQ(1000, (megabits_per_second<double>(gigabits_per_second<double>(1)).value()));
	EXPECT_EQ(1000, (gigabits_per_second<double>(terabits_per_second<double>(1)).value()));
	EXPECT_EQ(1000, (terabits_per_second<double>(petabits_per_second<double>(1)).value()));
	EXPECT_EQ(1000, (petabits_per_second<double>(exabits_per_second<double>(1)).value()));

	EXPECT_EQ(1024, (bits_per_second<double>(kibibits_per_second<double>(1)).value()));
	EXPECT_EQ(1024, (kibibits_per_second<double>(mebibits_per_second<double>(1)).value()));
	EXPECT_EQ(1024, (mebibits_per_second<double>(gibibits_per_second<double>(1)).value()));
	EXPECT_EQ(1024, (gibibits_per_second<double>(tebibits_per_second<double>(1)).value()));
	EXPECT_EQ(1024, (tebibits_per_second<double>(pebibits_per_second<double>(1)).value()));
	EXPECT_EQ(1024, (pebibits_per_second<double>(exbibits_per_second<double>(1)).value()));

	// Source: https://en.wikipedia.org/wiki/Binary_prefix
	EXPECT_NEAR(percent<double>(2.4), kibibytes_per_second<double>(1) / kilobytes_per_second<double>(1) - 1, 0.005);
	EXPECT_NEAR(percent<double>(4.9), mebibytes_per_second<double>(1) / megabytes_per_second<double>(1) - 1, 0.005);
	EXPECT_NEAR(percent<double>(7.4), gibibytes_per_second<double>(1) / gigabytes_per_second<double>(1) - 1, 0.005);
	EXPECT_NEAR(percent<double>(10.0), tebibytes_per_second<double>(1) / terabytes_per_second<double>(1) - 1, 0.005);
	EXPECT_NEAR(percent<double>(12.6), pebibytes_per_second<double>(1) / petabytes_per_second<double>(1) - 1, 0.005);
	EXPECT_NEAR(percent<double>(15.3), exbibytes_per_second<double>(1) / exabytes_per_second<double>(1) - 1, 0.005);
}

TEST_F(ConversionFactor, pi)
{
	EXPECT_TRUE(units::traits::is_dimensionless_unit_v<decltype(constants::pi)>);
	EXPECT_TRUE(units::traits::is_dimensionless_unit_v<detail::PI>);

	// implicit conversion/arithmetic
	EXPECT_NEAR(3.14159, constants::pi, 5.0e-6);
	EXPECT_NEAR(6.28318531, (2 * constants::pi), 5.0e-9);
	EXPECT_NEAR(6.28318531, (constants::pi + constants::pi), 5.0e-9);
	EXPECT_NEAR(0.0, (constants::pi - constants::pi), 5.0e-9);
	EXPECT_NEAR(31.00627668, pow<3>(constants::pi), 5.0e-10);
	EXPECT_NEAR(0.0322515344, (1.0 / pow<3>(constants::pi)), 5.0e-11);
	EXPECT_TRUE(detail::PI_VAL == constants::pi);
	EXPECT_TRUE(1.0 != constants::pi);
	EXPECT_TRUE(4.0 > constants::pi);
	EXPECT_TRUE(3.0 < constants::pi);
	EXPECT_TRUE(constants::pi > 3.0);
	EXPECT_TRUE(constants::pi < 4.0);

	// explicit conversion
	EXPECT_NEAR(3.14159, constants::pi.to<double>(), 5.0e-6);

	// auto multiplication
	EXPECT_TRUE((std::is_same_v<meter<double>, decltype(constants::pi * meter<double>(1))>));
	EXPECT_TRUE((std::is_same_v<meter<double>, decltype(meter<double>(1) * constants::pi)>));

	EXPECT_NEAR(detail::PI_VAL, (constants::pi * meter<double>(1)).to<double>(), 5.0e-10);
	EXPECT_NEAR(detail::PI_VAL, (meter<double>(1) * constants::pi).to<double>(), 5.0e-10);

	// explicit multiplication
	meter<double> a = constants::pi * meter<double>(1);
	meter<double> b = meter<double>(1) * constants::pi;

	EXPECT_NEAR(detail::PI_VAL, a.to<double>(), 5.0e-10);
	EXPECT_NEAR(detail::PI_VAL, b.to<double>(), 5.0e-10);

	// auto division
	EXPECT_TRUE((std::is_same_v<hertz<double>, decltype(constants::pi / second<double>(1))>));
	EXPECT_TRUE((std::is_same_v<second<double>, decltype(second<double>(1) / constants::pi)>));

	EXPECT_NEAR(detail::PI_VAL, (constants::pi / second<double>(1)).to<double>(), 5.0e-10);
	EXPECT_NEAR(1.0 / detail::PI_VAL, (second<double>(1) / constants::pi).to<double>(), 5.0e-10);

	// explicit
	hertz<double> c  = constants::pi / second<double>(1);
	second<double> d = second<double>(1) / constants::pi;

	EXPECT_NEAR(detail::PI_VAL, c.to<double>(), 5.0e-10);
	EXPECT_NEAR(1.0 / detail::PI_VAL, d.to<double>(), 5.0e-10);
}

TEST_F(ConversionFactor, constants)
{
	// Source: NIST "2014 CODATA recommended values"
	EXPECT_NEAR(299792458, constants::c.value(), 5.0e-9);
	EXPECT_NEAR(6.67408e-11, constants::G.value(), 5.0e-17);
	EXPECT_NEAR(6.626070040e-34, constants::h.value(), 5.0e-44);
	EXPECT_NEAR(1.2566370614e-6, constants::mu0.value(), 5.0e-17);
	EXPECT_NEAR(8.854187817e-12, constants::epsilon0.value(), 5.0e-21);
	EXPECT_NEAR(376.73031346177, constants::Z0.value(), 5.0e-12);
	EXPECT_NEAR(8987551787.3681764, constants::k_e.value(), 5.0e-6);
	EXPECT_NEAR(1.6021766208e-19, constants::e.value(), 5.0e-29);
	EXPECT_NEAR(9.10938356e-31, constants::m_e.value(), 5.0e-40);
	EXPECT_NEAR(1.672621898e-27, constants::m_p.value(), 5.0e-37);
	EXPECT_NEAR(9.274009994e-24, constants::mu_B.value(), 5.0e-32);
	EXPECT_NEAR(6.022140857e23, constants::N_A.value(), 5.0e14);
	EXPECT_NEAR(8.3144598, constants::R.value(), 5.0e-8);
	EXPECT_NEAR(1.38064852e-23, constants::k_B.value(), 5.0e-31);
	EXPECT_NEAR(96485.33289, constants::F.value(), 5.0e-5);
	EXPECT_NEAR(5.670367e-8, constants::sigma.value(), 5.0e-14);
}

TEST_F(ConversionFactor, std_chrono)
{
	nanosecond<double> a = std::chrono::nanoseconds(10);
	EXPECT_EQ(nanosecond<double>(10), a);
	microsecond<double> b = std::chrono::microseconds(10);
	EXPECT_EQ(microsecond<double>(10), b);
	millisecond<double> c = std::chrono::milliseconds(10);
	EXPECT_EQ(millisecond<double>(10), c);
	second<double> d = std::chrono::seconds(1);
	EXPECT_EQ(second<double>(1), d);
	minute<double> e = std::chrono::minutes(120);
	EXPECT_EQ(minute<double>(120), e);
	hour<double> f = std::chrono::hours(2);
	EXPECT_EQ(hour<double>(2), f);

	std::chrono::nanoseconds g = nanosecond<int>(100);
	EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(g).count(), 100);
	std::chrono::nanoseconds h = microsecond<int>(2);
	EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(h).count(), 2000);
	std::chrono::nanoseconds i = millisecond<int>(1);
	EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(i).count(), 1000000);
	std::chrono::nanoseconds j = second<int>(1);
	EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(j).count(), 1000000000);
	std::chrono::nanoseconds k = minute<int>(1);
	EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(k).count(), 60000000000);
	std::chrono::nanoseconds l = hour<int>(1);
	EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(l).count(), 3600000000000);
}

TEST_F(ConversionFactor, squaredTemperature)
{
	using squared_celsius   = units::compound_conversion_factor<squared<celsius_conversion_factor>>;
	using squared_celsius_t = units::unit<squared_celsius>;
	const squared_celsius_t right(100);
	const celsius<double> rootRight = sqrt(right);
	EXPECT_EQ(celsius<double>(10), rootRight);
}

TEST_F(UnitMath, min)
{
	meter<double> a(1);
	foot<double> c(1);
	EXPECT_EQ(c, units::min(a, c));

	const unit<meter_conversion_factor> d(1);
	const unit<centimeter_conversion_factor> e(99);
	EXPECT_EQ(e, units::min(d, e));
}

TEST_F(UnitMath, max)
{
	meter<double> a(1);
	foot<double> c(1);
	EXPECT_EQ(a, max(a, c));

	const unit<meter_conversion_factor> d(1);
	const unit<centimeter_conversion_factor> e(101);
	EXPECT_EQ(e, max(d, e));
}

TEST_F(UnitMath, cos)
{
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(cos(angle::radian<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(cos(degree<int>(0)))>));
	EXPECT_NEAR(dimensionless<double>(-0.41614683654), cos(angle::radian<double>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(-0.41614683654), cos(radian<int>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(-0.70710678118), cos(angle::degree<double>(135)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(-0.70710678118), cos(degree<int>(135)), 5.0e-11);
}

TEST_F(UnitMath, sin)
{
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(sin(angle::radian<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(sin(degree<int>(0)))>));
	EXPECT_NEAR(dimensionless<double>(0.90929742682), sin(angle::radian<double>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(0.90929742682), sin(radian<int>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(0.70710678118), sin(angle::degree<double>(135)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(0.70710678118), sin(degree<int>(135)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(0), sin(1.0_rad * units::constants::pi), 5.0e-16);
}

TEST_F(UnitMath, tan)
{
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(tan(angle::radian<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(tan(degree<int>(0)))>));
	EXPECT_NEAR(dimensionless<double>(-2.18503986326), tan(angle::radian<double>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(-2.18503986326), tan(radian<int>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(-1.0), tan(angle::degree<double>(135)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(-1.0), tan(degree<int>(135)), 5.0e-11);
}

TEST_F(UnitMath, acos)
{
	EXPECT_TRUE((std::is_same_v<angle::radian<double>, decltype(acos(dimensionless<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<angle::radian<double>, decltype(acos(dimensionless<int>(0)))>));
	EXPECT_NEAR(
		angle::radian<double>(2).to<double>(), acos(dimensionless<double>(-0.41614683654)).to<double>(), 5.0e-11);
	EXPECT_NEAR(angle::radian<double>(1.570796326795).to<double>(), acos(dimensionless<int>(0)).to<double>(),
		5.0e-11);
	EXPECT_NEAR(angle::degree<double>(135).to<double>(),
		angle::degree<double>(acos(dimensionless<double>(-0.70710678118654752440084436210485))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degree<double>(90).to<double>(),
		angle::degree<double>(acos(dimensionless<int>(0))).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, asin)
{
	EXPECT_TRUE((std::is_same_v<angle::radian<double>, decltype(asin(dimensionless<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<angle::radian<double>, decltype(asin(dimensionless<int>(0)))>));
	EXPECT_NEAR(angle::radian<double>(1.14159265).to<double>(), asin(dimensionless<double>(0.90929742682)).to<double>(),
		5.0e-9);
	EXPECT_NEAR(angle::radian<double>(1.570796326795).to<double>(), asin(dimensionless<int>(1)).to<double>(),
		5.0e-9);
	EXPECT_NEAR(angle::degree<double>(45).to<double>(),
		angle::degree<double>(asin(dimensionless<double>(0.70710678118654752440084436210485))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degree<double>(90).to<double>(),
		angle::degree<double>(asin(dimensionless<int>(1))).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, atan)
{
	EXPECT_TRUE((std::is_same_v<angle::radian<double>, decltype(atan(dimensionless<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<angle::radian<double>, decltype(atan(dimensionless<int>(0)))>));
	EXPECT_NEAR(angle::radian<double>(-1.14159265).to<double>(),
		atan(dimensionless<double>(-2.18503986326)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::radian<double>(0.785398163397).to<double>(), atan(dimensionless<int>(1)).to<double>(),
		5.0e-9);
	EXPECT_NEAR(angle::degree<double>(-45).to<double>(),
		angle::degree<double>(atan(dimensionless<double>(-1.0))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degree<double>(45).to<double>(),
		angle::degree<double>(atan(dimensionless<int>(1))).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, atan2)
{
	EXPECT_TRUE(
		(std::is_same_v<angle::radian<double>, decltype(atan2(dimensionless<double>(1), dimensionless<double>(1)))>));
	EXPECT_TRUE((std::is_same_v<angle::radian<double>,
		decltype(atan2(dimensionless<int>(1), dimensionless<int>(1)))>));
	EXPECT_NEAR(angle::radian<double>(detail::PI_VAL / 4).to<double>(),
		atan2(dimensionless<double>(2), dimensionless<double>(2)).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::radian<double>(detail::PI_VAL / 4).to<double>(),
		atan2(dimensionless<int>(2), dimensionless<int>(2)).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degree<double>(45).to<double>(),
		angle::degree<double>(atan2(dimensionless<double>(2), dimensionless<double>(2))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degree<double>(45).to<double>(),
		angle::degree<double>(atan2(dimensionless<int>(2), dimensionless<int>(2))).to<double>(),
		5.0e-12);

	EXPECT_TRUE(
		(std::is_same_v<angle::radian<double>, decltype(atan2(dimensionless<double>(1), dimensionless<double>(1)))>));
	EXPECT_NEAR(angle::radian<double>(detail::PI_VAL / 6).to<double>(),
		atan2(dimensionless<double>(1), sqrt(dimensionless<double>(3))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::radian<double>(detail::PI_VAL / 6).to<double>(),
		atan2(dimensionless<int>(1), sqrt(dimensionless<int>(3))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degree<double>(30).to<double>(),
		angle::degree<double>(atan2(dimensionless<double>(1), sqrt(dimensionless<double>(3)))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degree<double>(30).to<double>(),
		angle::degree<double>(atan2(dimensionless<int>(1), sqrt(dimensionless<int>(3))))
			.to<double>(),
		5.0e-12);
}

TEST_F(UnitMath, cosh)
{
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(cosh(angle::radian<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(cosh(degree<int>(0)))>));
	EXPECT_NEAR(dimensionless<double>(3.76219569108), cosh(angle::radian<double>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(3.76219569108), cosh(radian<int>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(5.32275215), cosh(angle::degree<double>(135)), 5.0e-9);
	EXPECT_NEAR(dimensionless<double>(5.32275215), cosh(degree<int>(135)), 5.0e-9);
}

TEST_F(UnitMath, sinh)
{
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(sinh(angle::radian<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(sinh(degree<int>(0)))>));
	EXPECT_NEAR(dimensionless<double>(3.62686040785), sinh(angle::radian<double>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(3.62686040785), sinh(radian<int>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(5.22797192), sinh(angle::degree<double>(135)), 5.0e-9);
	EXPECT_NEAR(dimensionless<double>(5.22797192), sinh(degree<int>(135)), 5.0e-9);
}

TEST_F(UnitMath, tanh)
{
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(tanh(angle::radian<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(tanh(degree<int>(0)))>));
	EXPECT_NEAR(dimensionless<double>(0.96402758007), tanh(angle::radian<double>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(0.96402758007), tanh(radian<int>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(0.98219338), tanh(angle::degree<double>(135)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(0.98219338), tanh(degree<int>(135)), 5.0e-11);
}

TEST_F(UnitMath, acosh)
{
	EXPECT_TRUE((std::is_same_v<angle::radian<double>, decltype(acosh(dimensionless<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<angle::radian<double>, decltype(acosh(dimensionless<int>(0)))>));
	EXPECT_NEAR(
		angle::radian<double>(1.316957896924817).to<double>(), acosh(dimensionless<double>(2.0)).to<double>(), 5.0e-11);
	EXPECT_NEAR(angle::radian<double>(1.316957896924817).to<double>(),
		acosh(dimensionless<int>(2)).to<double>(), 5.0e-11);
	EXPECT_NEAR(angle::degree<double>(75.456129290216893).to<double>(),
		angle::degree<double>(acosh(dimensionless<double>(2.0))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degree<double>(75.456129290216893).to<double>(),
		angle::degree<double>(acosh(dimensionless<int>(2))).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, asinh)
{
	EXPECT_TRUE((std::is_same_v<angle::radian<double>, decltype(asinh(dimensionless<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<angle::radian<double>, decltype(asinh(dimensionless<int>(0)))>));
	EXPECT_NEAR(
		angle::radian<double>(1.443635475178810).to<double>(), asinh(dimensionless<double>(2)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::radian<double>(1.443635475178810).to<double>(),
		asinh(dimensionless<int>(2)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::degree<double>(82.714219883108939).to<double>(),
		angle::degree<double>(asinh(dimensionless<double>(2))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degree<double>(82.714219883108939).to<double>(),
		angle::degree<double>(asinh(dimensionless<int>(2))).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, atanh)
{
	EXPECT_TRUE((std::is_same_v<angle::radian<double>, decltype(atanh(dimensionless<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<angle::radian<double>, decltype(atanh(dimensionless<int>(0)))>));
	EXPECT_NEAR(
		angle::radian<double>(0.549306144334055).to<double>(), atanh(dimensionless<double>(0.5)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::radian<double>(0).to<double>(), atanh(dimensionless<int>(0)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::degree<double>(31.472923730945389).to<double>(),
		angle::degree<double>(atanh(dimensionless<double>(0.5))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degree<double>(0).to<double>(),
		angle::degree<double>(atanh(dimensionless<int>(0))).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, exp)
{
	double val = 10.0;
	EXPECT_EQ(std::exp(val), exp(dimensionless<double>(val)));
}

TEST_F(UnitMath, log)
{
	double val = 100.0;
	EXPECT_EQ(std::log(val), log(dimensionless<double>(val)));
}

TEST_F(UnitMath, log10)
{
	double val = 100.0;
	EXPECT_EQ(std::log10(val), log10(dimensionless<double>(val)));
}

TEST_F(UnitMath, modf)
{
	double val = 100.0;
	double modfr1;
	dimensionless<double> modfr2;
	EXPECT_EQ(std::modf(val, &modfr1), modf(dimensionless<double>(val), &modfr2));
	EXPECT_EQ(modfr1, modfr2);
}

TEST_F(UnitMath, exp2)
{
	double val = 10.0;
	EXPECT_EQ(std::exp2(val), exp2(dimensionless<double>(val)));
}

TEST_F(UnitMath, expm1)
{
	double val = 10.0;
	EXPECT_EQ(std::expm1(val), expm1(dimensionless<double>(val)));
}

TEST_F(UnitMath, log1p)
{
	double val = 10.0;
	EXPECT_EQ(std::log1p(val), log1p(dimensionless<double>(val)));
}

TEST_F(UnitMath, log2)
{
	double val = 10.0;
	EXPECT_EQ(std::log2(val), log2(dimensionless<double>(val)));
}

TEST_F(UnitMath, pow)
{
	bool isSame;
	meter<double> value(10.0);

	auto sq = pow<2>(value);
	EXPECT_NEAR(100.0, sq.value(), 5.0e-2);
	isSame = std::is_same_v<decltype(sq), square_meter<double>>;
	EXPECT_TRUE(isSame);

	auto cube = pow<3>(value);
	EXPECT_NEAR(1000.0, cube.value(), 5.0e-2);
	isSame = std::is_same_v<decltype(cube), traits::strong_t<unit<traits::strong_t<cubed<meter_conversion_factor>>>>>;
	EXPECT_TRUE(isSame);

	auto fourth = pow<4>(value);
	EXPECT_NEAR(10000.0, fourth.value(), 5.0e-2);
	isSame = std::is_same_v<decltype(fourth),
		unit<compound_conversion_factor<squared<meter_conversion_factor>, squared<meter_conversion_factor>>>>;
	EXPECT_TRUE(isSame);
}

TEST_F(UnitMath, sqrt)
{
	EXPECT_TRUE((std::is_same_v<meter<double>, decltype(sqrt(square_meter<double>(4.0)))>));
	EXPECT_NEAR(meter<double>(2.0).to<double>(), sqrt(square_meter<double>(4.0)).to<double>(), 5.0e-9);

	EXPECT_TRUE((std::is_same_v<angle::radian<double>, decltype(sqrt(steradian<double>(16.0)))>));
	EXPECT_NEAR(angle::radian<double>(4.0).to<double>(), sqrt(steradian<double>(16.0)).to<double>(), 5.0e-9);

	EXPECT_TRUE((std::is_convertible_v<foot<double>, decltype(sqrt(square_foot<double>(10.0)))>));

	// for rational conversion (i.e. no integral root) let's check a bunch of different ways this could go wrong
	foot<double> resultFt = sqrt(square_foot<double>(10.0));
	EXPECT_NEAR(foot<double>(3.16227766017).to<double>(), sqrt(square_foot<double>(10.0)).to<double>(), 5.0e-9);
	EXPECT_NEAR(foot<double>(3.16227766017).to<double>(), resultFt.to<double>(), 5.0e-9);
	EXPECT_EQ(resultFt, sqrt(square_foot<double>(10.0)));
}

TEST_F(UnitMath, hypot)
{
	EXPECT_TRUE((std::is_same_v<meter<double>, decltype(hypot(meter<double>(3.0), meter<double>(4.0)))>));
	EXPECT_NEAR(meter<double>(5.0).to<double>(), (hypot(meter<double>(3.0), meter<double>(4.0))).to<double>(), 5.0e-9);

	static_assert(
		traits::is_convertible_unit_v<foot<double>, decltype(hypot(foot<double>(3.0), meter<double>(1.2192)))>);
	EXPECT_NEAR(foot<double>(5.0).to<double>(),
		foot<double>(hypot(foot<double>(3.0), meter<double>(1.2192))).to<double>(), 5.0e-9);
}

TEST_F(UnitMath, ceil)
{
	double val = 101.1;
	EXPECT_EQ(ceil(val), ceil(meter<double>(val)).to<double>());
	EXPECT_TRUE((std::is_same_v<meter<double>, decltype(ceil(meter<double>(val)))>));
}

TEST_F(UnitMath, floor)
{
	double val = 101.1;
	EXPECT_EQ(floor(val), floor(dimensionless<double>(val)));
}

TEST_F(UnitMath, fmod)
{
	EXPECT_EQ(fmod(100.0, 101.2), fmod(meter<double>(100.0), meter<double>(101.2)).to<double>());
}

TEST_F(UnitMath, trunc)
{
	double val = 101.1;
	EXPECT_EQ(trunc(val), trunc(dimensionless<double>(val)));
}

TEST_F(UnitMath, round)
{
	double val = 101.1;
	EXPECT_EQ(round(val), round(dimensionless<double>(val)));
}

TEST_F(UnitMath, copysign)
{
	double sign = -1;
	meter<double> val(5.0);
	EXPECT_EQ(meter<double>(-5.0), copysign(val, sign));
	EXPECT_EQ(meter<double>(-5.0), copysign(val, angle::radian<double>(sign)));
}

TEST_F(UnitMath, fdim)
{
	EXPECT_EQ(meter<double>(0.0), fdim(meter<double>(8.0), meter<double>(10.0)));
	EXPECT_EQ(meter<double>(2.0), fdim(meter<double>(10.0), meter<double>(8.0)));
	EXPECT_NEAR(meter<double>(9.3904).to<double>(),
		meter<double>(fdim(meter<double>(10.0), foot<double>(2.0))).to<double>(),
		5.0e-320); // not sure why they aren't comparing exactly equal, but clearly they are.
}

TEST_F(UnitMath, fmin)
{
	EXPECT_EQ(meter<double>(8.0), fmin(meter<double>(8.0), meter<double>(10.0)));
	EXPECT_EQ(meter<double>(8.0), fmin(meter<double>(10.0), meter<double>(8.0)));
	EXPECT_EQ(foot<double>(2.0), fmin(meter<double>(10.0), foot<double>(2.0)));
}

TEST_F(UnitMath, fmax)
{
	EXPECT_EQ(meter<double>(10.0), fmax(meter<double>(8.0), meter<double>(10.0)));
	EXPECT_EQ(meter<double>(10.0), fmax(meter<double>(10.0), meter<double>(8.0)));
	EXPECT_EQ(meter<double>(10.0), fmax(meter<double>(10.0), foot<double>(2.0)));
}

TEST_F(UnitMath, fabs)
{
	EXPECT_EQ(meter<double>(10.0), fabs(meter<double>(-10.0)));
	EXPECT_EQ(meter<double>(10.0), fabs(meter<double>(10.0)));
}

TEST_F(UnitMath, abs)
{
	EXPECT_EQ(meter<double>(10.0), abs(meter<double>(-10.0)));
	EXPECT_EQ(meter<double>(10.0), abs(meter<double>(10.0)));
}

TEST_F(UnitMath, fma)
{
	meter<double> x(2.0);
	meter<double> y(3.0);
	square_meter<double> z(1.0);
	EXPECT_EQ(square_meter<double>(7.0), (units::fma(x, y, z)));
}

// Constexpr
TEST_F(Constexpr, construction)
{
	constexpr meter<double> result0(0);
	constexpr auto result1 = make_unit<meter<double>>(1);
	constexpr auto result2 = meter<double>(2);

	EXPECT_EQ(meter<double>(0), result0);
	EXPECT_EQ(meter<double>(1), result1);
	EXPECT_EQ(meter<double>(2), result2);

	EXPECT_TRUE(noexcept(meter<double>(0)));
	EXPECT_TRUE(noexcept(make_unit<meter<double>>(1)));
}

TEST_F(Constexpr, constants)
{
	EXPECT_TRUE(noexcept(constants::c.value()));
	EXPECT_TRUE(noexcept(constants::G.value()));
	EXPECT_TRUE(noexcept(constants::h.value()));
	EXPECT_TRUE(noexcept(constants::mu0.value()));
	EXPECT_TRUE(noexcept(constants::epsilon0.value()));
	EXPECT_TRUE(noexcept(constants::Z0.value()));
	EXPECT_TRUE(noexcept(constants::k_e.value()));
	EXPECT_TRUE(noexcept(constants::e.value()));
	EXPECT_TRUE(noexcept(constants::m_e.value()));
	EXPECT_TRUE(noexcept(constants::m_p.value()));
	EXPECT_TRUE(noexcept(constants::mu_B.value()));
	EXPECT_TRUE(noexcept(constants::N_A.value()));
	EXPECT_TRUE(noexcept(constants::R.value()));
	EXPECT_TRUE(noexcept(constants::k_B.value()));
	EXPECT_TRUE(noexcept(constants::F.value()));
	EXPECT_TRUE(noexcept(constants::sigma.value()));
}

TEST_F(Constexpr, arithmetic)
{
	[[maybe_unused]] constexpr auto result0(1.0_m + 1.0_m);
	[[maybe_unused]] constexpr auto result1(1.0_m - 1.0_m);
	[[maybe_unused]] constexpr auto result2(1.0_m * 1.0_m);
	[[maybe_unused]] constexpr auto result3(1.0_m / 1.0_m);
	[[maybe_unused]] constexpr auto result4(meter<double>(1) + meter<double>(1));
	[[maybe_unused]] constexpr auto result5(meter<double>(1) - meter<double>(1));
	[[maybe_unused]] constexpr auto result6(meter<double>(1) * meter<double>(1));
	[[maybe_unused]] constexpr auto result7(meter<double>(1) / meter<double>(1));
	[[maybe_unused]] constexpr auto result8(pow<2>(meter<double>(2)));
	constexpr auto result9  = pow<3>(2.0_m);
	constexpr auto result10 = 2.0_m * 2.0_m;

	EXPECT_TRUE(noexcept(1.0_m + 1.0_m));
	EXPECT_TRUE(noexcept(1.0_m - 1.0_m));
	EXPECT_TRUE(noexcept(1.0_m * 1.0_m));
	EXPECT_TRUE(noexcept(1.0_m / 1.0_m));
	EXPECT_TRUE(noexcept(meter<double>(1) + meter<double>(1)));
	EXPECT_TRUE(noexcept(meter<double>(1) - meter<double>(1)));
	EXPECT_TRUE(noexcept(meter<double>(1) * meter<double>(1)));
	EXPECT_TRUE(noexcept(meter<double>(1) / meter<double>(1)));
	EXPECT_TRUE(noexcept(pow<2>(meter<double>(2))));
	EXPECT_TRUE(noexcept(pow<3>(2.0_m)));
	EXPECT_TRUE(noexcept(2.0_m * 2.0_m));

	meter<double> m{42};
	EXPECT_TRUE(noexcept(+m));
	EXPECT_TRUE(noexcept(-m));
	EXPECT_TRUE(noexcept(++m));
	EXPECT_TRUE(noexcept(--m));
	EXPECT_TRUE(noexcept(m++));
	EXPECT_TRUE(noexcept(m--));

	EXPECT_EQ(8.0_cu_m, result9);
	EXPECT_EQ(4.0_sq_m, result10);
}

TEST_F(Constexpr, assignment)
{
	auto testConstexpr = []() constexpr noexcept
	{
		meter<double> m{42};
		+m;
		-m;
		++m;
		--m;
		m++;
		m--;
		m += 2.0_m;
		m -= 2.0_m;
		m *= 2;
		m /= 2;
		return m;
	};

	[[maybe_unused]] constexpr auto m = testConstexpr();
}

TEST_F(Constexpr, realtional)
{
	constexpr bool equalityTrue          = (1.0_m == 1.0_m);
	constexpr bool equalityFalse         = (1.0_m == 2.0_m);
	constexpr bool lessThanTrue          = (1.0_m < 2.0_m);
	constexpr bool lessThanFalse         = (1.0_m < 1.0_m);
	constexpr bool lessThanEqualTrue1    = (1.0_m <= 1.0_m);
	constexpr bool lessThanEqualTrue2    = (1.0_m <= 2.0_m);
	constexpr bool lessThanEqualFalse    = (1.0_m < 0.0_m);
	constexpr bool greaterThanTrue       = (2.0_m > 1.0_m);
	constexpr bool greaterThanFalse      = (2.0_m > 2.0_m);
	constexpr bool greaterThanEqualTrue1 = (2.0_m >= 1.0_m);
	constexpr bool greaterThanEqualTrue2 = (2.0_m >= 2.0_m);
	constexpr bool greaterThanEqualFalse = (2.0_m > 3.0_m);

	EXPECT_TRUE(equalityTrue);
	EXPECT_TRUE(lessThanTrue);
	EXPECT_TRUE(lessThanEqualTrue1);
	EXPECT_TRUE(lessThanEqualTrue2);
	EXPECT_TRUE(greaterThanTrue);
	EXPECT_TRUE(greaterThanEqualTrue1);
	EXPECT_TRUE(greaterThanEqualTrue2);
	EXPECT_FALSE(equalityFalse);
	EXPECT_FALSE(lessThanFalse);
	EXPECT_FALSE(lessThanEqualFalse);
	EXPECT_FALSE(greaterThanFalse);
	EXPECT_FALSE(greaterThanEqualFalse);
}

TEST_F(Constexpr, stdArray)
{
	constexpr std::array<meter<double>, 5> arr{{0.0_m, 1.0_m, 2.0_m, 3.0_m, 4.0_m}};
	constexpr bool equal = (arr[3] == 3.0_m);
	EXPECT_TRUE(equal);
}

TEST_F(CaseStudies, radarRangeEquation)
{
	watt<double> P_t;           // transmit power
	dimensionless<double> G;    // gain
	meter<double> lambda;       // wavelength
	square_meter<double> sigma; // radar cross section
	meter<double> R;            // range
	kelvin<double> T_s;         // system noise temp
	hertz<double> B_n;          // bandwidth
	dimensionless<double> L;    // loss

	P_t    = megawatt<double>(1.4);
	G      = dB<double>(33.0);
	lambda = constants::c / megahertz<double>(2800);
	sigma  = square_meter<double>(1.0);
	R      = meter<double>(111000.0);
	T_s    = kelvin<double>(950.0);
	B_n    = megahertz<double>(1.67);
	L      = dB<double>(8.0);

	dimensionless<double> SNR = (P_t * pow<2>(G) * pow<2>(lambda) * sigma) /
		(pow<3>(4 * constants::pi) * pow<4>(R) * constants::k_B * T_s * B_n * L);

	EXPECT_NEAR(1.535, SNR.value(), 5.0e-4);
}

TEST_F(CaseStudies, rightTriangle)
{
	constexpr auto a = 3.0_m;
	constexpr auto b = 4.0_m;
	constexpr auto c = sqrt(pow<2>(a) + pow<2>(b));
	EXPECT_EQ(5.0_m, c);
}

TEST_F(CaseStudies, dataReadSimulation)
{
	const megabyte<int> data_size             = 100_MB;
	const megabytes_per_second<int> read_rate = 2_MBps;
	byte<int> read_progress                   = 10_MB;

	auto advance_simulation = [&](auto time) {
		read_progress = units::min(read_progress + time * read_rate, data_size);
	};

	advance_simulation(10_s);
	EXPECT_EQ(read_progress, 30_MB);

	advance_simulation(25_s);
	EXPECT_EQ(read_progress, 80_MB);

	advance_simulation(500_ms);
	EXPECT_EQ(read_progress, 81_MB);

	advance_simulation(25_s);
	EXPECT_EQ(read_progress, data_size);
}

TEST_F(CaseStudies, selfDefinedUnits)
{
	using liters_per_second  = decltype(1.0_L / 1.0_s);
	using gallons_per_minute = decltype(1.0_gal / 1.0_min);

	liters_per_second lps(5);
	gallons_per_minute gpm = lps;

	EXPECT_NEAR(79.2516157, gpm.to<double>(), 0.5e-7);

	testing::internal::CaptureStdout();
	std::cout << lps;
	std::string output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("0.005 m^3 s^-1", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << gpm;
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("0.005 m^3 s^-1", output.c_str());
}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
