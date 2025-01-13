#ifdef _MSC_VER
#pragma warning(disable : 4244) // Disable 'conversion from double to const int, possible loss of data'. The tests just make sure
// that such assignments work as expected, we don't want to remove them and we don't care about the warning.
#define _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING // officially, The effect of instantiating the template std::complex for any type other than float,
// double, or long double is unspecified. We don't care though, we want them to work with units in this
// test
#endif

#include <array>
#include <chrono>
#include <complex>
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
	};

	class STDTypeTraits : public ::testing::Test
	{
	};

	class STDSpecializations : public ::testing::Test
	{
	};

	class UnitManipulators : public ::testing::Test
	{
	};

	class UnitType : public ::testing::Test
	{
	};

	class ConversionFactor : public ::testing::Test
	{
	};

	class UnitMath : public ::testing::Test
	{
	};

	class Constexpr : public ::testing::Test
	{
	};

	class UnitLimits : public ::testing::Test
	{
	};

	class CaseStudies : public ::testing::Test
	{
	};

	// Tests that two units have the same conversion ratio to the same dimension.
	constexpr auto has_equivalent_conversion_factor = []<typename T0, typename T1>(const T0&, const T1&)
	{
		using T = std::decay_t<T0>;
		using U = std::decay_t<T1>;
		return units::traits::is_same_dimension_unit_v<T, U> && std::ratio_equal_v<typename T::conversion_factor::conversion_ratio, typename U::conversion_factor::conversion_ratio>;
	};
} // namespace

TEST_F(TypeTraits, sizeOf)
{
	static_assert(sizeof(dimensionless<double>) == sizeof(double));
	static_assert(sizeof(meters<double>) == sizeof(double));
	static_assert(sizeof(degrees_squared<double>) == sizeof(double));
}

TEST_F(TypeTraits, isRatio)
{
	static_assert(traits::is_ratio_v<std::ratio<1>>);
	static_assert(!traits::is_ratio_v<double>);
}

TEST_F(TypeTraits, ratio_sqrt)
{
	using rt2 = ratio_sqrt<std::ratio<2>>;
	EXPECT_LT(std::abs(std::sqrt(2 / 1.0) - rt2::num / static_cast<double>(rt2::den)), 5e-9);

	using rt4 = ratio_sqrt<std::ratio<4>>;
	EXPECT_LT(std::abs(std::sqrt(4 / 1.0) - rt4::num / static_cast<double>(rt4::den)), 5e-9);

	using rt10 = ratio_sqrt<std::ratio<10>>;
	EXPECT_LT(std::abs(std::sqrt(10 / 1.0) - rt10::num / static_cast<double>(rt10::den)), 5e-9);

	using rt30 = ratio_sqrt<std::ratio<30>>;
	EXPECT_LT(std::abs(std::sqrt(30 / 1.0) - rt30::num / static_cast<double>(rt30::den)), 5e-9);

	using rt61 = ratio_sqrt<std::ratio<61>>;
	EXPECT_LT(std::abs(std::sqrt(61 / 1.0) - rt61::num / static_cast<double>(rt61::den)), 5e-9);

	using rt100 = ratio_sqrt<std::ratio<100>>;
	EXPECT_LT(std::abs(std::sqrt(100 / 1.0) - rt100::num / static_cast<double>(rt100::den)), 5e-9);

	using rt1000 = ratio_sqrt<std::ratio<1000>>;
	EXPECT_LT(std::abs(std::sqrt(1000 / 1.0) - rt1000::num / static_cast<double>(rt1000::den)), 5e-9);

	using rt10000 = ratio_sqrt<std::ratio<10000>>;
	EXPECT_LT(std::abs(std::sqrt(10000 / 1.0) - rt10000::num / static_cast<double>(rt10000::den)), 5e-9);
}

TEST_F(TypeTraits, is_conversion_factor)
{
	static_assert(!traits::is_conversion_factor_v<std::ratio<1>>);
	static_assert(!traits::is_conversion_factor_v<double>);
	static_assert(traits::is_conversion_factor_v<feet<double>>);
	static_assert(traits::is_conversion_factor_v<degrees_squared<double>>);
	static_assert(traits::is_conversion_factor_v<meters<double>>);
}

TEST_F(TypeTraits, is_unit)
{
	static_assert(!traits::is_unit_v<std::ratio<1>>);
	static_assert(!traits::is_unit_v<double>);
	static_assert(traits::is_unit_v<meters<double>>);
	static_assert(traits::is_unit_v<feet<double>>);
	static_assert(traits::is_unit_v<degrees_squared<double>>);
}

TEST_F(TypeTraits, replace_underlying)
{
	static_assert(std::is_same_v<traits::replace_underlying_t<dimensionless<int>, int>, dimensionless<int>>);
	static_assert(std::is_same_v<traits::replace_underlying_t<dimensionless<int>, double>, dimensionless<double>>);
}

TEST_F(TypeTraits, conversion_factor_traits)
{
	static_assert(std::is_same_v<void, traits::conversion_factor_traits<double>::conversion_ratio>);
}

TEST_F(TypeTraits, unit_traits)
{
	static_assert(std::is_same_v<double, traits::unit_traits<double>::underlying_type>);
	static_assert(std::is_same_v<double, traits::unit_traits<meters<double>>::underlying_type>);
	static_assert(std::is_same_v<void, traits::unit_traits<double>::value_type>);
	static_assert(std::is_same_v<double, traits::unit_traits<meters<double>>::value_type>);
	static_assert(std::is_same_v<void, traits::unit_traits<double>::value_type>);
	static_assert(std::is_same_v<int, traits::unit_traits<meters<int>>::value_type>);
}

TEST_F(TypeTraits, is_same_dimension_conversion_factor)
{
	static_assert(traits::is_same_dimension_conversion_factor_v<meters<double>::conversion_factor, meters<double>::conversion_factor>);
	static_assert(traits::is_same_dimension_conversion_factor_v<meters<double>::conversion_factor, astronomical_units<double>::conversion_factor>);
	static_assert(traits::is_same_dimension_conversion_factor_v<meters<double>::conversion_factor, parsecs<double>::conversion_factor>);

	static_assert(traits::is_same_dimension_conversion_factor_v<meters<double>::conversion_factor, meters<double>::conversion_factor>);
	static_assert(traits::is_same_dimension_conversion_factor_v<astronomical_units<double>::conversion_factor, meters<double>::conversion_factor>);
	static_assert(traits::is_same_dimension_conversion_factor_v<parsecs<double>::conversion_factor, meters<double>::conversion_factor>);
	static_assert(traits::is_same_dimension_conversion_factor_v<years<double>::conversion_factor, weeks<double>::conversion_factor>);

	static_assert(!traits::is_same_dimension_conversion_factor_v<meters<double>::conversion_factor, seconds<double>::conversion_factor>);
	static_assert(!traits::is_same_dimension_conversion_factor_v<seconds<double>::conversion_factor, meters<double>::conversion_factor>);
	static_assert(!traits::is_same_dimension_conversion_factor_v<years<double>::conversion_factor, meters<double>::conversion_factor>);
}

TEST_F(TypeTraits, inverse)
{
	double test;

	using htz         = traits::strong_t<inverse<seconds<double>>>;
	bool shouldBeTrue = std::is_same_v<htz, hertz<double>::conversion_factor>;
	EXPECT_TRUE(shouldBeTrue);

	test = unit<inverse<fahrenheit<double>::conversion_factor>>(unit<inverse<celsius<double>::conversion_factor>>(1.0)).value();
	EXPECT_NEAR(5.0 / 9.0, test, 5.0e-5);

	test = unit<inverse<fahrenheit<double>::conversion_factor>>(unit<inverse<kelvin<double>::conversion_factor>>(6.0)).value();
	EXPECT_NEAR(10.0 / 3.0, test, 5.0e-5);
}

TEST_F(TypeTraits, strong)
{
	static_assert(std::is_same_v<dimensionless_, traits::strong_t<detail::conversion_factor_base_t<dimensionless_>>>);
	static_assert(std::is_same_v<meters<double>::conversion_factor, traits::strong_t<conversion_factor<std::ratio<1>, dimension::length>>>);
	static_assert(std::is_same_v<kilometers<double>::conversion_factor, traits::strong_t<kilometers<double>::conversion_factor>>);
	static_assert(std::is_same_v<square_meters<double>::conversion_factor, traits::strong_t<squared<meters<double>::conversion_factor>>>);
}

TEST_F(TypeTraits, dimension_of)
{
	using dim = traits::dimension_of_t<years<double>::conversion_factor>;

	static_assert(std::is_same_v<dim, dimension::time>);
	static_assert(!std::is_same_v<dim, dimension::length>);
	static_assert(!std::is_same_v<dim, days<int>>);

	using dim2 = traits::conversion_factor_traits<traits::unit_traits<decltype(meters_per_second<double>(5))>::conversion_factor>::dimension_type;

	static_assert(std::is_same_v<dim2, dimension::velocity>);
	static_assert(!std::is_same_v<dim2, dimension::time>);
	static_assert(!std::is_same_v<dim2, miles_per_hour<int>>);

	using dim = traits::dimension_of_t<years<double>>;

	static_assert(std::is_same_v<dim, dimension::time>);
	static_assert(!std::is_same_v<dim, dimension::length>);
	static_assert(!std::is_same_v<dim, days<int>>);
}

TEST_F(TypeTraits, has_linear_scale)
{
	static_assert(traits::has_linear_scale_v<dimensionless<double>>);
	static_assert(traits::has_linear_scale_v<meters<double>>);
	static_assert(traits::has_linear_scale_v<feet<double>>);
	static_assert(traits::has_linear_scale_v<watts<double>, dimensionless<double>>);
	static_assert(traits::has_linear_scale_v<dimensionless<double>, meters<double>>);
	static_assert(traits::has_linear_scale_v<meters_per_second<double>>);
	static_assert(!traits::has_linear_scale_v<decibels<double>>);
	static_assert(!traits::has_linear_scale_v<decibels<double>, meters_per_second<double>>);
}

TEST_F(TypeTraits, has_decibel_scale)
{
	static_assert(!traits::has_decibel_scale_v<dimensionless<double>>);
	static_assert(!traits::has_decibel_scale_v<meters<double>>);
	static_assert(!traits::has_decibel_scale_v<feet<double>>);
	static_assert(traits::has_decibel_scale_v<decibels<double>>);
	static_assert(traits::has_decibel_scale_v<dBW<double>>);

	static_assert(traits::has_decibel_scale_v<dBW<double>, decibels<double>>);
	static_assert(traits::has_decibel_scale_v<dBW<double>, dBm<double>>);
	static_assert(traits::has_decibel_scale_v<decibels<double>, decibels<double>>);
	static_assert(traits::has_decibel_scale_v<decibels<double>, decibels<double>, decibels<double>>);
	static_assert(!traits::has_decibel_scale_v<decibels<double>, decibels<double>, meters<double>>);
	static_assert(!traits::has_decibel_scale_v<meters<double>, decibels<double>>);
}

TEST_F(TypeTraits, is_dimensionless_unit)
{
	static_assert(traits::is_dimensionless_unit_v<dimensionless<double>>);
	static_assert(traits::is_dimensionless_unit_v<const dimensionless<double>>);
	static_assert(traits::is_dimensionless_unit_v<const dimensionless<double>&>);
	static_assert(traits::is_dimensionless_unit_v<dimensionless<double>>);
	static_assert(traits::is_dimensionless_unit_v<decibels<double>>);
	static_assert(traits::is_dimensionless_unit_v<parts_per_million<double>>);
	static_assert(!traits::is_dimensionless_unit_v<meters<double>>);
	static_assert(!traits::is_dimensionless_unit_v<dBW<double>>);

	static_assert(std::is_arithmetic_v<const double>);
}

TEST_F(TypeTraits, is_length_unit)
{
	static_assert(!traits::is_length_unit_v<double>);
	static_assert(traits::is_length_unit_v<meters<double>>);
	static_assert(traits::is_length_unit_v<const meters<double>>);
	static_assert(traits::is_length_unit_v<const meters<double>&>);
	static_assert(traits::is_length_unit_v<cubits<double>>);
	static_assert(!traits::is_length_unit_v<years<double>>);
}

TEST_F(TypeTraits, is_mass_unit)
{
	static_assert(!traits::is_mass_unit_v<double>);
	static_assert(traits::is_mass_unit_v<kilograms<double>>);
	static_assert(traits::is_mass_unit_v<const kilograms<double>>);
	static_assert(traits::is_mass_unit_v<const kilograms<double>&>);
	static_assert(traits::is_mass_unit_v<stone<double>>);
	static_assert(!traits::is_mass_unit_v<meters<double>>);
}

TEST_F(TypeTraits, is_time_unit)
{
	static_assert(!traits::is_time_unit_v<double>);
	static_assert(traits::is_time_unit_v<seconds<double>>);
	static_assert(traits::is_time_unit_v<const seconds<double>>);
	static_assert(traits::is_time_unit_v<const seconds<double>&>);
	static_assert(traits::is_time_unit_v<years<double>>);
	static_assert(!traits::is_time_unit_v<meters<double>>);
}

TEST_F(TypeTraits, is_angle_unit)
{
	static_assert(!traits::is_angle_unit_v<double>);
	static_assert(traits::is_angle_unit_v<radians<double>>);
	static_assert(traits::is_angle_unit_v<const radians<double>>);
	static_assert(traits::is_angle_unit_v<const radians<double>&>);
	static_assert(traits::is_angle_unit_v<degrees<double>>);
	static_assert(!traits::is_angle_unit_v<watts<double>>);
}

TEST_F(TypeTraits, is_current_unit)
{
	static_assert(!traits::is_current_unit_v<double>);
	static_assert(traits::is_current_unit_v<amperes<double>>);
	static_assert(traits::is_current_unit_v<const amperes<double>>);
	static_assert(traits::is_current_unit_v<const amperes<double>&>);
	static_assert(!traits::is_current_unit_v<volts<double>>);
}

TEST_F(TypeTraits, is_temperature_unit)
{
	static_assert(!traits::is_temperature_unit_v<double>);
	static_assert(traits::is_temperature_unit_v<fahrenheit<double>>);
	static_assert(traits::is_temperature_unit_v<const fahrenheit<double>>);
	static_assert(traits::is_temperature_unit_v<const fahrenheit<double>&>);
	static_assert(traits::is_temperature_unit_v<kelvin<double>>);
	static_assert(!traits::is_temperature_unit_v<cubits<double>>);
}

TEST_F(TypeTraits, is_substance_unit)
{
	static_assert(!traits::is_substance_unit_v<double>);
	static_assert(traits::is_substance_unit_v<mols<double>>);
	static_assert(traits::is_substance_unit_v<const mols<double>>);
	static_assert(traits::is_substance_unit_v<const mols<double>&>);
	static_assert(!traits::is_substance_unit_v<years<double>>);
}

TEST_F(TypeTraits, is_luminous_intensity_unit)
{
	static_assert(!traits::is_luminous_intensity_unit_v<double>);
	static_assert(traits::is_luminous_intensity_unit_v<candelas<double>>);
	static_assert(traits::is_luminous_intensity_unit_v<const candelas<double>>);
	static_assert(traits::is_luminous_intensity_unit_v<const candelas<double>&>);
	static_assert(!traits::is_luminous_intensity_unit_v<radiation_absorbed_dose<double>>);
}

TEST_F(TypeTraits, is_solid_angle_unit)
{
	static_assert(!traits::is_solid_angle_unit_v<double>);
	static_assert(traits::is_solid_angle_unit_v<steradians<double>>);
	static_assert(traits::is_solid_angle_unit_v<const steradians<double>>);
	static_assert(traits::is_solid_angle_unit_v<const degrees_squared<double>&>);
	static_assert(!traits::is_solid_angle_unit_v<degrees<double>>);
}

TEST_F(TypeTraits, is_frequency_unit)
{
	static_assert(!traits::is_frequency_unit_v<double>);
	static_assert(traits::is_frequency_unit_v<hertz<double>>);
	static_assert(traits::is_frequency_unit_v<const hertz<double>>);
	static_assert(traits::is_frequency_unit_v<const hertz<double>&>);
	static_assert(!traits::is_frequency_unit_v<seconds<double>>);
}

TEST_F(TypeTraits, is_velocity_unit)
{
	static_assert(!traits::is_velocity_unit_v<double>);
	static_assert(traits::is_velocity_unit_v<meters_per_second<double>>);
	static_assert(traits::is_velocity_unit_v<const meters_per_second<double>>);
	static_assert(traits::is_velocity_unit_v<const meters_per_second<double>&>);
	static_assert(traits::is_velocity_unit_v<miles_per_hour<double>>);
	static_assert(!traits::is_velocity_unit_v<meters_per_second_squared<double>>);
}

TEST_F(TypeTraits, is_acceleration_unit)
{
	static_assert(!traits::is_acceleration_unit_v<double>);
	static_assert(traits::is_acceleration_unit_v<meters_per_second_squared<double>>);
	static_assert(traits::is_acceleration_unit_v<const meters_per_second_squared<double>>);
	static_assert(traits::is_acceleration_unit_v<const meters_per_second_squared<double>&>);
	static_assert(traits::is_acceleration_unit_v<standard_gravity<double>>);
	static_assert(!traits::is_acceleration_unit_v<inches<double>>);
}

TEST_F(TypeTraits, is_force_unit)
{
	static_assert(!traits::is_force_unit_v<double>);
	static_assert(traits::is_force_unit_v<newtons<double>>);
	static_assert(traits::is_force_unit_v<const newtons<double>>);
	static_assert(traits::is_force_unit_v<const newtons<double>&>);
	static_assert(traits::is_force_unit_v<dynes<double>>);
	static_assert(!traits::is_force_unit_v<watts<double>>);
}

TEST_F(TypeTraits, is_pressure_unit)
{
	static_assert(!traits::is_pressure_unit_v<double>);
	static_assert(traits::is_pressure_unit_v<pascals<double>>);
	static_assert(traits::is_pressure_unit_v<const pascals<double>>);
	static_assert(traits::is_pressure_unit_v<const pascals<double>&>);
	static_assert(traits::is_pressure_unit_v<atmospheres<double>>);
	static_assert(!traits::is_pressure_unit_v<years<double>>);
}

TEST_F(TypeTraits, is_charge_unit)
{
	static_assert(!traits::is_charge_unit_v<double>);
	static_assert(traits::is_charge_unit_v<coulombs<double>>);
	static_assert(traits::is_charge_unit_v<const coulombs<double>>);
	static_assert(traits::is_charge_unit_v<const coulombs<double>&>);
	static_assert(!traits::is_charge_unit_v<watts<double>>);
}

TEST_F(TypeTraits, is_energy_unit)
{
	static_assert(!traits::is_energy_unit_v<double>);
	static_assert(traits::is_energy_unit_v<joules<double>>);
	static_assert(traits::is_energy_unit_v<const joules<double>>);
	static_assert(traits::is_energy_unit_v<const joules<double>&>);
	static_assert(traits::is_energy_unit_v<calories<double>>);
	static_assert(!traits::is_energy_unit_v<watts<double>>);
}

TEST_F(TypeTraits, is_power_unit)
{
	static_assert(!traits::is_power_unit_v<double>);
	static_assert(traits::is_power_unit_v<watts<double>>);
	static_assert(traits::is_power_unit_v<const watts<double>>);
	static_assert(traits::is_power_unit_v<const watts<double>&>);
	static_assert(!traits::is_power_unit_v<henries<double>>);
}

TEST_F(TypeTraits, is_voltage_unit)
{
	static_assert(!traits::is_voltage_unit_v<double>);
	static_assert(traits::is_voltage_unit_v<volts<double>>);
	static_assert(traits::is_voltage_unit_v<const volts<double>>);
	static_assert(traits::is_voltage_unit_v<const volts<double>&>);
	static_assert(!traits::is_voltage_unit_v<henries<double>>);
}

TEST_F(TypeTraits, is_capacitance_unit)
{
	static_assert(!traits::is_capacitance_unit_v<double>);
	static_assert(traits::is_capacitance_unit_v<farads<double>>);
	static_assert(traits::is_capacitance_unit_v<const farads<double>>);
	static_assert(traits::is_capacitance_unit_v<const farads<double>&>);
	static_assert(!traits::is_capacitance_unit_v<ohms<double>>);
}

TEST_F(TypeTraits, is_impedance_unit)
{
	static_assert(!traits::is_impedance_unit_v<double>);
	static_assert(traits::is_impedance_unit_v<ohms<double>>);
	static_assert(traits::is_impedance_unit_v<const ohms<double>>);
	static_assert(traits::is_impedance_unit_v<const ohms<double>&>);
	static_assert(!traits::is_impedance_unit_v<farads<double>>);
}

TEST_F(TypeTraits, is_conductance_unit)
{
	static_assert(!traits::is_conductance_unit_v<double>);
	static_assert(traits::is_conductance_unit_v<siemens<double>>);
	static_assert(traits::is_conductance_unit_v<const siemens<double>>);
	static_assert(traits::is_conductance_unit_v<const siemens<double>&>);
	static_assert(!traits::is_conductance_unit_v<volts<double>>);
}

TEST_F(TypeTraits, is_magnetic_flux_unit)
{
	static_assert(!traits::is_magnetic_flux_unit_v<double>);
	static_assert(traits::is_magnetic_flux_unit_v<webers<double>>);
	static_assert(traits::is_magnetic_flux_unit_v<const webers<double>>);
	static_assert(traits::is_magnetic_flux_unit_v<const webers<double>&>);
	static_assert(traits::is_magnetic_flux_unit_v<maxwells<double>>);
	static_assert(!traits::is_magnetic_flux_unit_v<inches<double>>);
}

TEST_F(TypeTraits, is_magnetic_field_strength_unit)
{
	static_assert(!traits::is_magnetic_field_strength_unit_v<double>);
	static_assert(traits::is_magnetic_field_strength_unit_v<teslas<double>>);
	static_assert(traits::is_magnetic_field_strength_unit_v<const teslas<double>>);
	static_assert(traits::is_magnetic_field_strength_unit_v<const teslas<double>&>);
	static_assert(traits::is_magnetic_field_strength_unit_v<gauss<double>>);
	static_assert(!traits::is_magnetic_field_strength_unit_v<volts<double>>);
}

TEST_F(TypeTraits, is_inductance_unit)
{
	static_assert(!traits::is_inductance_unit_v<double>);
	static_assert(traits::is_inductance_unit_v<henries<double>>);
	static_assert(traits::is_inductance_unit_v<const henries<double>>);
	static_assert(traits::is_inductance_unit_v<const henries<double>&>);
	static_assert(!traits::is_inductance_unit_v<farads<double>>);
}

TEST_F(TypeTraits, is_luminous_flux_unit)
{
	static_assert(!traits::is_luminous_flux_unit_v<double>);
	static_assert(traits::is_luminous_flux_unit_v<lumens<double>>);
	static_assert(traits::is_luminous_flux_unit_v<const lumens<double>>);
	static_assert(traits::is_luminous_flux_unit_v<const lumens<double>&>);
	static_assert(!traits::is_luminous_flux_unit_v<mass::pounds<double>>);
}

TEST_F(TypeTraits, is_illuminance_unit)
{
	static_assert(!traits::is_illuminance_unit_v<double>);
	static_assert(traits::is_illuminance_unit_v<footcandles<double>>);
	static_assert(traits::is_illuminance_unit_v<const footcandles<double>>);
	static_assert(traits::is_illuminance_unit_v<const footcandles<double>&>);
	static_assert(traits::is_illuminance_unit_v<lux<double>>);
	static_assert(!traits::is_illuminance_unit_v<meters<double>>);
}

TEST_F(TypeTraits, is_radioactivity_unit)
{
	static_assert(!traits::is_radioactivity_unit_v<double>);
	static_assert(traits::is_radioactivity_unit_v<sieverts<double>>);
	static_assert(traits::is_radioactivity_unit_v<const sieverts<double>>);
	static_assert(traits::is_radioactivity_unit_v<const sieverts<double>&>);
	static_assert(!traits::is_radioactivity_unit_v<years<double>>);
}

TEST_F(TypeTraits, is_torque_unit)
{
	static_assert(!traits::is_torque_unit_v<double>);
	static_assert(traits::is_torque_unit_v<newton_meters<double>>);
	static_assert(traits::is_torque_unit_v<const newton_meters<double>>);
	static_assert(traits::is_torque_unit_v<const newton_meters<double>&>);
	static_assert(traits::is_torque_unit_v<torque::foot_pounds<double>>);
	static_assert(!traits::is_torque_unit_v<cubic_meters<double>>);
}

TEST_F(TypeTraits, is_area_unit)
{
	static_assert(!traits::is_area_unit_v<double>);
	static_assert(traits::is_area_unit_v<square_meters<double>>);
	static_assert(traits::is_area_unit_v<const square_meters<double>>);
	static_assert(traits::is_area_unit_v<const square_meters<double>&>);
	static_assert(traits::is_area_unit_v<hectares<double>>);
	static_assert(!traits::is_area_unit_v<astronomical_units<double>>);
}

TEST_F(TypeTraits, is_volume_unit)
{
	static_assert(!traits::is_volume_unit_v<double>);
	static_assert(traits::is_volume_unit_v<cubic_meters<double>>);
	static_assert(traits::is_volume_unit_v<const cubic_meters<double>>);
	static_assert(traits::is_volume_unit_v<const cubic_meters<double>&>);
	static_assert(traits::is_volume_unit_v<cubic_inches<double>>);
	static_assert(!traits::is_volume_unit_v<feet<double>>);
}

TEST_F(TypeTraits, is_density_unit)
{
	static_assert(!traits::is_density_unit_v<double>);
	static_assert(traits::is_density_unit_v<kilograms_per_cubic_meter<double>>);
	static_assert(traits::is_density_unit_v<const kilograms_per_cubic_meter<double>>);
	static_assert(traits::is_density_unit_v<const kilograms_per_cubic_meter<double>&>);
	static_assert(traits::is_density_unit_v<ounces_per_cubic_foot<double>>);
	static_assert(!traits::is_density_unit_v<years<double>>);
}

TEST_F(TypeTraits, is_data_unit)
{
	static_assert(!traits::is_data_unit_v<double>);
	static_assert(traits::is_data_unit_v<bits<double>>);
	static_assert(traits::is_data_unit_v<const bits<double>>);
	static_assert(traits::is_data_unit_v<const bits<double>&>);
	static_assert(traits::is_data_unit_v<bytes<double>>);
	static_assert(!traits::is_data_unit_v<years<double>>);
}

TEST_F(TypeTraits, is_data_transfer_rate_unit)
{
	static_assert(!traits::is_data_transfer_rate_unit_v<double>);
	static_assert(traits::is_data_transfer_rate_unit_v<gigabits_per_second<double>>);
	static_assert(traits::is_data_transfer_rate_unit_v<const gigabytes_per_second<double>>);
	static_assert(traits::is_data_transfer_rate_unit_v<const gigabytes_per_second<double>&>);
	static_assert(traits::is_data_transfer_rate_unit_v<gigabytes_per_second<double>>);
	static_assert(!traits::is_data_transfer_rate_unit_v<years<double>>);
}

TEST_F(STDTypeTraits, std_common_type)
{
	static_assert(has_equivalent_conversion_factor(std::common_type_t<meters<double>, meters<double>>(), meters()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<kilometers<double>, kilometers<double>>(), kilometers()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<millimeters<double>, millimeters<double>>(), millimeters()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<meters<double>, kilometers<double>>(), meters()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<kilometers<double>, meters<double>>(), meters()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<meters<double>, millimeters<double>>(), millimeters()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<millimeters<double>, meters<double>>(), millimeters()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<millimeters<double>, kilometers<double>>(), millimeters()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<kilometers<double>, millimeters<double>>(), millimeters()));
	static_assert(std::is_same_v<std::common_type_t<meters<double>, kilometers<double>>, std::common_type_t<kilometers<double>, meters<double>>>);
	static_assert(std::is_same_v<std::common_type_t<meters<double>, millimeters<double>>, std::common_type_t<millimeters<double>, meters<double>>>);
	static_assert(std::is_same_v<std::common_type_t<millimeters<double>, kilometers<double>>, std::common_type_t<kilometers<double>, millimeters<double>>>);

	static_assert(has_equivalent_conversion_factor(std::common_type_t<meters<int>, meters<int>>(), meters<int>()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<kilometers<int>, kilometers<int>>(), kilometers<int>()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<millimeters<int>, millimeters<int>>(), millimeters<int>()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<meters<int>, kilometers<int>>(), meters<int>()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<kilometers<int>, meters<int>>(), meters<int>()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<meters<int>, millimeters<int>>(), millimeters<int>()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<millimeters<int>, meters<int>>(), millimeters<int>()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<millimeters<int>, kilometers<int>>(), millimeters<int>()));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<kilometers<int>, millimeters<int>>(), millimeters<int>()));
	static_assert(std::is_same_v<std::common_type_t<meters<int>, kilometers<int>>, std::common_type_t<kilometers<int>, meters<int>>>);
	static_assert(std::is_same_v<std::common_type_t<meters<int>, millimeters<int>>, std::common_type_t<millimeters<int>, meters<int>>>);
	static_assert(std::is_same_v<std::common_type_t<millimeters<int>, kilometers<int>>, std::common_type_t<kilometers<int>, millimeters<int>>>);

	using half_a_second  = unit<conversion_factor<std::ratio<1, 2>, seconds<double>>, int>;
	using third_a_second = unit<conversion_factor<std::ratio<1, 3>, seconds<double>>, int>;
	using sixth_a_second = unit<conversion_factor<std::ratio<1, 6>, seconds<double>>, int>;

	static_assert(has_equivalent_conversion_factor(std::common_type_t<half_a_second, third_a_second>{}, sixth_a_second{}));
	static_assert(std::is_same_v<std::common_type_t<half_a_second, third_a_second>, std::common_type_t<third_a_second, half_a_second>>);
	static_assert(std::is_same_v<std::common_type_t<half_a_second, third_a_second>::underlying_type, int>);

	static_assert(has_equivalent_conversion_factor(std::common_type_t<kelvin<double>, celsius<double>>{}, celsius{}));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<celsius<double>, kelvin<double>>{}, celsius{}));
	static_assert(std::is_same_v<std::common_type_t<kelvin<double>, celsius<double>>, std::common_type_t<celsius<double>, kelvin<double>>>);

	using half_a_kelvin  = unit<conversion_factor<std::ratio<1, 2>, kelvin<double>>, double>;
	using third_a_kelvin = unit<conversion_factor<std::ratio<1, 3>, kelvin<double>>, int>;
	using sixth_a_kelvin = unit<conversion_factor<std::ratio<1, 6>, kelvin<double>>, int>;

	static_assert(has_equivalent_conversion_factor(std::common_type_t<half_a_kelvin, third_a_kelvin>{}, sixth_a_kelvin{}));
	static_assert(std::is_same_v<std::common_type_t<half_a_kelvin, third_a_kelvin>, std::common_type_t<third_a_kelvin, half_a_kelvin>>);
	static_assert(std::is_same_v<std::common_type_t<half_a_kelvin, third_a_kelvin>::underlying_type, double>);

	static_assert(has_equivalent_conversion_factor(std::common_type_t<radians<double>, degrees<double>>{}, degrees<double>{}));
	static_assert(has_equivalent_conversion_factor(std::common_type_t<degrees<double>, radians<double>>{}, degrees<double>{}));
	static_assert(std::is_same_v<std::common_type_t<radians<double>, degrees<double>>, std::common_type_t<degrees<double>, radians<double>>>);

	using half_a_radian     = unit<conversion_factor<std::ratio<1, 2>, radians<double>>, int>;
	using big_half_a_radian = unit<conversion_factor<std::ratio<2, 4>, radians<double>>, int>;
	using third_a_radian    = unit<conversion_factor<std::ratio<1, 3>, radians<double>>, double>;
	using sixth_a_radian    = unit<conversion_factor<std::ratio<1, 6>, radians<double>>, int>;

	static_assert(has_equivalent_conversion_factor(std::common_type_t<half_a_radian, third_a_radian>{}, sixth_a_radian{}));
	static_assert(std::is_same_v<std::common_type_t<half_a_radian, third_a_radian>, std::common_type_t<third_a_radian, half_a_radian>>);
	static_assert(std::is_same_v<std::common_type_t<half_a_radian, third_a_radian>::underlying_type, double>);

	static_assert(std::is_same_v<std::common_type_t<dimensionless<int>, dimensionless<int>>, dimensionless<int>>);
	static_assert(std::is_same_v<std::common_type_t<dimensionless<int>, dimensionless<double>>, dimensionless<double>>);
	static_assert(std::is_same_v<std::common_type_t<dimensionless<double>, dimensionless<int>>, dimensionless<double>>);

	static_assert(std::is_same_v<traits::conversion_factor_traits<traits::unit_traits<std::common_type_t<half_a_radian, big_half_a_radian>>::conversion_factor>::conversion_ratio, std::ratio<1, 2>>);

	using T = std::common_type_t<percent<double>, double>;
	T a     = 50_pct;
	EXPECT_DOUBLE_EQ(a, 0.5);
	static_assert(std::is_same_v<std::common_type_t<dimensionless<int>, int>, unit<conversion_factor<std::ratio<1>, dimension::dimensionless>, int>>);
	static_assert(std::is_same_v<conversion_factor<std::ratio<1>, dimension::dimensionless>, dimensionless_>);
	static_assert(std::is_same_v<std::common_type_t<dimensionless<int>, int>, unit<dimensionless_, int>>);

	static_assert(std::is_same_v<std::common_type_t<dimensionless<int>, int>, dimensionless<int>>);
	static_assert(std::is_same_v<std::common_type_t<int, dimensionless<int>>, dimensionless<int>>);
	static_assert(std::is_same_v<std::common_type_t<dimensionless<int>, double>, dimensionless<double>>);
	static_assert(std::is_same_v<std::common_type_t<double, dimensionless<int>>, dimensionless<double>>);
	static_assert(std::is_same_v<std::common_type_t<dimensionless<double>, int>, dimensionless<double>>);
	static_assert(std::is_same_v<std::common_type_t<int, dimensionless<double>>, dimensionless<double>>);
	static_assert(std::is_same_v<std::common_type_t<dimensionless<double>, double>, dimensionless<double>>);
	static_assert(std::is_same_v<std::common_type_t<double, dimensionless<double>>, dimensionless<double>>);

	static_assert(std::is_same_v<std::common_type_t<degrees<double>, degrees<double>>, degrees<double>>);
	static_assert(std::is_same_v<std::common_type_t<celsius<double>, celsius<double>>, celsius<double>>);
}

TEST_F(STDSpecializations, hash)
{
	EXPECT_EQ(std::hash<meters<double>>()(3.14_m), std::hash<double>()(3.14));
	EXPECT_EQ(std::hash<millimeters<double>>()(3.14_m), std::hash<double>()(3.14e3));
	EXPECT_EQ(std::hash<millimeters<double>>()(3.14_mm), std::hash<double>()(3.14));
	EXPECT_EQ(std::hash<kilometers<double>>()(3.14_m), std::hash<double>()(3.14e-3));
	EXPECT_EQ(std::hash<kilometers<double>>()(3.14_km), std::hash<double>()(3.14));

	EXPECT_EQ((std::hash<meters<int>>()(meters<int>(42))), 42);
	EXPECT_EQ((std::hash<millimeters<int>>()(meters<int>(42))), 42000);
	EXPECT_EQ((std::hash<millimeters<int>>()(millimeters<int>(42))), 42);
	EXPECT_EQ((std::hash<kilometers<int>>()(kilometers<int>(42))), 42);

	EXPECT_EQ((std::hash<dimensionless<double>>()(3.14)), std::hash<double>()(3.14));
	EXPECT_EQ((std::hash<dimensionless<int>>()(42)), (std::hash<dimensionless<int>>()(42)));

	EXPECT_EQ(std::hash<dBW<double>>()(2.0_dBW), std::hash<double>()(dBW<>(2.0).to_linearized()));
}

TEST_F(UnitManipulators, squared)
{
	double test;

	test = square_feet<double>(unit<squared<meters<double>>>(0.092903)).value();
	EXPECT_NEAR(0.99999956944, test, 5.0e-12);

	using dimensionless_2 = traits::strong_t<squared<units::dimensionless_>>; // this is actually nonsensical, and should also result in
	// a dimensionless.
	bool isSame = std::is_same_v<unit<dimensionless_>, unit<dimensionless_2>>;
	EXPECT_TRUE(isSame);
}

TEST_F(UnitManipulators, cubed)
{
	double test;

	test = cubic_feet<double>(unit<cubed<meters<double>>>(0.0283168)).value();
	EXPECT_NEAR(0.999998354619, test, 5.0e-13);
}

TEST_F(UnitManipulators, square_root)
{
	double test;

	test = meters<double>(unit<square_root<square_kilometers<double>>>(1.0)).value();
	static_assert(traits::is_same_dimension_conversion_factor_v<square_root<square_kilometers<double>>, kilometers<double>>);
	EXPECT_NEAR(1000.0, test, 5.0e-13);
}

TEST_F(UnitManipulators, compound_unit)
{
	using acceleration1 = conversion_factor<std::ratio<1>, dimension::acceleration>;
	using acceleration2 = compound_conversion_factor<meters<double>, inverse<seconds<double>>, inverse<seconds<double>>>;
	using acceleration3 = conversion_factor<std::ratio<1>, make_dimension<dimension::length, std::ratio<1>, dimension::time, std::ratio<-2>>>;
	using acceleration4 = compound_conversion_factor<meters<double>, inverse<squared<seconds<double>>>>;
	using acceleration5 = compound_conversion_factor<meters<double>, squared<inverse<seconds<double>>>>;

	bool areSame12 = std::is_same_v<acceleration1, acceleration2>;
	bool areSame23 = std::is_same_v<acceleration2, acceleration3>;
	bool areSame34 = std::is_same_v<acceleration3, acceleration4>;
	bool areSame45 = std::is_same_v<acceleration4, acceleration5>;

	EXPECT_TRUE(areSame12);
	EXPECT_TRUE(areSame23);
	EXPECT_TRUE(areSame34);
	EXPECT_TRUE(areSame45);

	// test that thing with translations still compile
	using arbitrary1 = compound_conversion_factor<meters<double>, inverse<celsius<double>>>;
	using arbitrary2 = compound_conversion_factor<meters<double>, celsius<double>>;
	using arbitrary3 = traits::strong_t<compound_conversion_factor<arbitrary1, arbitrary2>>;
	static_assert(std::is_same_v<square_meters<double>::conversion_factor, arbitrary3>);
}

TEST_F(UnitManipulators, dimensionalAnalysis)
{
	// these look like 'compound units', but the dimensional analysis can be REALLY handy if the
	// unit types aren't know (i.e. they themselves are template parameters), as you can get the resulting unit of the
	// operation.

	using velocity    = decltype(meters<double>{1.0} / seconds<double>{1.0});
	bool shouldBeTrue = std::is_same_v<meters_per_second<double>, velocity>;
	EXPECT_TRUE(shouldBeTrue);

	using acceleration1 = conversion_factor<std::ratio<1>, dimension::acceleration>;
	using acceleration2 = units::detail::unit_divide<meters<double>, units::detail::unit_multiply<seconds<double>, seconds<double>>>;
	shouldBeTrue        = std::is_same_v<acceleration1, acceleration2>;
	EXPECT_TRUE(shouldBeTrue);
}

TEST_F(UnitType, trivial)
{
	static_assert(std::is_trivial_v<meters<double>>);
	static_assert(std::is_trivially_assignable_v<meters<double>, meters<double>>);
	static_assert(std::is_trivially_constructible_v<meters<double>>);
	static_assert(std::is_trivially_copy_assignable_v<meters<double>>);
	static_assert(std::is_trivially_copy_constructible_v<meters<double>>);
	static_assert(std::is_trivially_copyable_v<meters<double>>);
	static_assert(std::is_trivially_default_constructible_v<meters<double>>);
	static_assert(std::is_trivially_destructible_v<meters<double>>);
	static_assert(std::is_trivially_move_assignable_v<meters<double>>);
	static_assert(std::is_trivially_move_constructible_v<meters<double>>);

	static_assert(std::is_trivial_v<decibels<double>>);
	static_assert(std::is_trivially_assignable_v<decibels<double>, decibels<double>>);
	static_assert(std::is_trivially_constructible_v<decibels<double>>);
	static_assert(std::is_trivially_copy_assignable_v<decibels<double>>);
	static_assert(std::is_trivially_copy_constructible_v<decibels<double>>);
	static_assert(std::is_trivially_copyable_v<decibels<double>>);
	static_assert(std::is_trivially_default_constructible_v<decibels<double>>);
	static_assert(std::is_trivially_destructible_v<decibels<double>>);
	static_assert(std::is_trivially_move_assignable_v<decibels<double>>);
	static_assert(std::is_trivially_move_constructible_v<decibels<double>>);
}

TEST_F(UnitType, complexUnits)
{
	constexpr std::complex<meters<>> x(3_m, 4_m);
	EXPECT_TRUE((std::conj(x) == std::complex{3.0_m, -4.0_m}));
}

TEST_F(UnitType, constructionFromArithmeticType)
{
	constexpr meters a_m(1.0);
	EXPECT_EQ(1.0, a_m.value());

	constexpr meters<double> b_m(1);
	EXPECT_EQ(1, b_m.value());

	constexpr meters c_m(1);
	EXPECT_EQ(1, c_m.value());

	constexpr dimensionless d_dim(1.0);
	EXPECT_EQ(1.0, d_dim.value());

	constexpr dimensionless<double> e_dim(1);
	EXPECT_EQ(1, e_dim.value());

	constexpr dimensionless a_dim(1.0);
	EXPECT_EQ(1.0, a_dim.value());

	constexpr dimensionless<double> b_dim(1);
	EXPECT_EQ(1, b_dim.value());

	constexpr dimensionless c_dim(1);
	EXPECT_EQ(1, c_dim.value());

	constexpr dimensionless f_dim(1);
	EXPECT_EQ(1, f_dim.value());
}

TEST_F(UnitType, constructionFromUnitType)
{
	constexpr meters a_m(1);

	constexpr meters b_m(a_m);
	EXPECT_EQ(1, b_m.value());

	constexpr millimeters a_mm(b_m);
	EXPECT_EQ(1000, a_mm.value());

	constexpr millimeters b_mm(a_mm);
	EXPECT_EQ(1000, b_mm.value());

	constexpr millimeters c_mm(b_mm);
	EXPECT_EQ(1000, c_mm.value());

	constexpr millimeters d_mm(b_m);
	EXPECT_EQ(1000, d_mm.value());

	constexpr meters<double> c_m(b_m);
	EXPECT_EQ(1.0, c_m.value());

	constexpr meters<double> d_m(a_mm);
	EXPECT_EQ(1.0, d_m.value());

	constexpr meters<double> e_m(b_mm);
	EXPECT_EQ(1.0, e_m.value());

	constexpr meters f_m(c_m);
	EXPECT_EQ(1.0, f_m.value());

	constexpr meters<double> g_m(kilometers<int>(1));
	EXPECT_EQ(1000.0, g_m.value());

	constexpr dimensionless a_dim(1);

	constexpr dimensionless b_dim(a_dim);
	EXPECT_EQ(1, b_dim.value());

	constexpr dimensionless c_dim(b_dim);
	EXPECT_EQ(1, c_dim.value());

	constexpr dimensionless d_dim(c_dim);
	EXPECT_EQ(1, d_dim.value());

	constexpr dimensionless e_dim(d_dim);
	EXPECT_EQ(1, e_dim.value());

	constexpr dimensionless f_dim(c_dim);
	EXPECT_EQ(1, f_dim.value());

	constexpr dimensionless g_dim(f_dim);
	EXPECT_EQ(1, g_dim.value());
}

TEST_F(UnitType, CTAD)
{
	// Default ctor
	constexpr meters z_m{};
	static_assert(std::is_same_v<std::remove_const_t<decltype(z_m)>, meters<double>>);

	// Underlying type, copy ctor, and same dimensioned units for `int` and `double`.
	constexpr meters a_m(1);
	static_assert(std::is_same_v<std::remove_const_t<decltype(a_m)>, meters<int>>);

	constexpr meters b_m(a_m);
	static_assert(std::is_same_v<std::remove_const_t<decltype(b_m)>, meters<int>>);

	constexpr meters b_m2(millimeters(2.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(b_m2)>, meters<double>>);

	constexpr millimeters a_mm(b_m);
	static_assert(std::is_integral_v<decltype(a_mm.value())>);
	static_assert(std::is_same_v<std::remove_const_t<decltype(a_mm)>, millimeters<int>>);
	EXPECT_EQ(a_mm, 1000_mm);

	constexpr meters c_m(1.0);
	static_assert(std::is_same_v<std::remove_const_t<decltype(c_m)>, meters<double>>);

	constexpr meters d_m(c_m);
	static_assert(std::is_same_v<std::remove_const_t<decltype(d_m)>, meters<double>>);

	constexpr millimeters b_mm(d_m);
	static_assert(std::is_same_v<std::remove_const_t<decltype(b_mm)>, millimeters<double>>);

	constexpr kilometers a_km(b_mm);
	static_assert(std::is_same_v<std::remove_const_t<decltype(a_km)>, kilometers<double>>);

	// Other underlying types.
	constexpr meters e_m(static_cast<short>(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(e_m)>, meters<short>>);

	constexpr meters f_m(1.0f);
	static_assert(std::is_same_v<std::remove_const_t<decltype(f_m)>, meters<float>>);

	constexpr meters g_m(1LL);
	static_assert(std::is_same_v<std::remove_const_t<decltype(g_m)>, meters<long long>>);

	constexpr meters h_m(1.0L);
	static_assert(std::is_same_v<std::remove_const_t<decltype(h_m)>, meters<long double>>);

	// `unit`.
	constexpr meters i_m(meters<int>(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(i_m)>, meters<int>>);

	constexpr meters j_m(meters<double>(1.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(j_m)>, meters<double>>);

	constexpr meters k_m(kilometers(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(k_m)>, meters<int>>);

	constexpr meters l_m(kilometers<double>(1.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(l_m)>, meters<double>>);

	constexpr meters m_m(millimeters<double>(1.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(m_m)>, meters<double>>);

	// `std::chrono::duration`.
	using namespace std::chrono_literals;

	constexpr unit a_s(1s);
	static_assert(seconds<int>(1s) == a_s && std::is_integral_v<decltype(a_s.value())>);

	constexpr unit a_min(1.0min);
	static_assert(minutes<double>(1.0) == a_min && std::is_floating_point_v<decltype(a_min.value())>);

	constexpr seconds b_s(1_s);
	static_assert(std::is_integral_v<decltype(b_s.value())>);

	constexpr seconds c_s(1.0_s);
	static_assert(std::is_floating_point_v<decltype(c_s.value())>);

	constexpr seconds d_s(1_min);
	static_assert(std::is_integral_v<decltype(d_s.value())>);

	constexpr seconds e_s(1.0_min);
	static_assert(std::is_floating_point_v<decltype(e_s.value())>);

	constexpr seconds f_s(1.0_ms);
	static_assert(std::is_floating_point_v<decltype(f_s.value())>);

	// Dimensionless units.
	constexpr dimensionless z_dim = 1.0;
	static_assert(std::is_same_v<std::remove_const_t<decltype(z_dim)>, dimensionless<double>>);

	constexpr dimensionless a_dim(1);
	static_assert(std::is_same_v<std::remove_const_t<decltype(a_dim)>, dimensionless<int>>);

	constexpr dimensionless b_dim(a_dim);
	static_assert(std::is_same_v<std::remove_const_t<decltype(b_dim)>, dimensionless<int>>);

	constexpr percent a_per(b_dim);
	static_assert(std::is_same_v<std::remove_const_t<decltype(a_per)>, percent<int>>);

	constexpr dimensionless c_dim(1.0);
	static_assert(std::is_same_v<std::remove_const_t<decltype(c_dim)>, dimensionless<double>>);

	constexpr dimensionless d_dim(c_dim);
	static_assert(std::is_same_v<std::remove_const_t<decltype(d_dim)>, dimensionless<double>>);

	constexpr percent b_per(d_dim);
	static_assert(std::is_same_v<std::remove_const_t<decltype(b_per)>, percent<double>>);

	constexpr dimensionless e_dim(static_cast<short>(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(e_dim)>, dimensionless<short>>);

	constexpr dimensionless f_dim(1.0f);
	static_assert(std::is_same_v<std::remove_const_t<decltype(f_dim)>, dimensionless<float>>);

	constexpr dimensionless g_dim(1LL);
	static_assert(std::is_same_v<std::remove_const_t<decltype(g_dim)>, dimensionless<long long>>);

	constexpr dimensionless h_dim(1.0L);
	static_assert(std::is_same_v<std::remove_const_t<decltype(h_dim)>, dimensionless<long double>>);

	constexpr dimensionless i_dim(dimensionless<int>(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(i_dim)>, dimensionless<int>>);

	constexpr dimensionless j_dim(dimensionless<double>(1.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(j_dim)>, dimensionless<double>>);

	constexpr dimensionless k_dim(unit<conversion_factor<std::kilo, dimensionless_>, int>(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(k_dim)>, dimensionless<int>>);

	constexpr dimensionless l_dim(unit<conversion_factor<std::kilo, dimensionless_>, double>(1.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(l_dim)>, dimensionless<double>>);

	constexpr dimensionless m_dim(unit<conversion_factor<std::milli, dimensionless_>, double>(1.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(m_dim)>, dimensionless<double>>);

	constexpr radians n_dim(degrees{1});
	static_assert(std::is_same_v<std::remove_const_t<decltype(n_dim)>, radians<double>>);

	constexpr radians o_dim(degrees{1.0});
	static_assert(std::is_same_v<std::remove_const_t<decltype(o_dim)>, radians<double>>);
}

TEST_F(UnitType, implicitChronoConversions)
{
	using namespace std::chrono_literals;
	std::chrono::seconds chronoSec(1);
	const seconds<int>   unitsSec = chronoSec;
	chronoSec                     = unitsSec;
	EXPECT_EQ(unitsSec, 1_s);
	EXPECT_EQ(chronoSec, 1s);
}

TEST_F(UnitType, negativeConstexprLiterals)
{
	static constexpr radians kAngularValue{-30.0_deg};
	EXPECT_EQ(-30.0_deg, kAngularValue);
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

	percent<double> p_dim;
	p_dim = 0.30;
	EXPECT_EQ(30_pct, p_dim);
	EXPECT_DOUBLE_EQ(0.3, p_dim.value());
	EXPECT_DOUBLE_EQ(30, p_dim.raw());
	p_dim = 1;
	EXPECT_EQ(100_pct, p_dim);
	EXPECT_DOUBLE_EQ(1, p_dim.value());
	EXPECT_DOUBLE_EQ(100, p_dim.raw());
}

TEST_F(UnitType, assignmentFromUnitType)
{
	meters<int> a_m(1);
	a_m = +a_m;
	EXPECT_EQ(1, a_m.value());

	millimeters<int> a_mm;
	a_mm = a_m;
	EXPECT_EQ(1000, a_mm.value());
	a_mm = +a_mm;
	EXPECT_EQ(1000, a_mm.value());

	millimeters<int> b_mm;
	b_mm = a_m;
	EXPECT_EQ(1000, b_mm.value());
	b_mm = a_mm;
	EXPECT_EQ(1000, b_mm.value());
	b_mm = +b_mm;
	EXPECT_EQ(1000, b_mm.value());

	a_mm = b_mm;
	EXPECT_EQ(1000, a_mm.value());

	meters<double> b_m;
	b_m = a_m;
	EXPECT_EQ(1, b_m.value());
	b_m = a_mm;
	EXPECT_EQ(1, b_m.value());
	b_m = b_mm;
	EXPECT_EQ(1, b_m.value());
	b_m = +b_m;
	EXPECT_EQ(1, b_m.value());
	b_m = kilometers<int>(1);
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

	percent<double> d_dim;
	d_dim = dimensionless<double>(0.75);
	EXPECT_EQ(75_pct, d_dim);
	EXPECT_EQ(0.75, d_dim.value());
	d_dim = parts_per_million<double>(8);
	EXPECT_EQ(0.0008_pct, d_dim);
	EXPECT_EQ(0.000008, d_dim.value());
	d_dim = +dimensionless<double>(0.75);
	EXPECT_EQ(75_pct, d_dim);
	EXPECT_EQ(0.75, d_dim.value());
}

TEST_F(UnitType, make_unit)
{
	constexpr auto a_m = make_unit<meters<double>>(5.0);
	EXPECT_EQ(meters<double>(5.0), a_m);

	constexpr auto b_m = make_unit<meters<double>>(5);
	EXPECT_EQ(meters<double>(5), b_m);

	constexpr auto c_m = make_unit<meters<int>>(5);
	EXPECT_EQ((meters<int>(5)), c_m);

	constexpr auto a_dim = make_unit<dimensionless<double>>(5.0);
	EXPECT_EQ(dimensionless<double>(5.0), a_dim);

	constexpr auto b_dim = make_unit<dimensionless<double>>(5);
	EXPECT_EQ(dimensionless<double>(5), b_dim);

	constexpr auto c_dim = make_unit<dimensionless<int>>(5);
	EXPECT_EQ((dimensionless<int>(5)), c_dim);
}

TEST_F(UnitType, unitTypeEquality)
{
	constexpr meters a_m(0.0);
	constexpr meters b_m(1.0);

	EXPECT_TRUE(a_m == a_m);
	EXPECT_FALSE(a_m == b_m);
	EXPECT_TRUE(a_m != b_m);
	EXPECT_FALSE(b_m != b_m);

	constexpr meters c_m(0);
	constexpr meters d_m(1);

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

	constexpr percent w_m(100.0);
	constexpr percent x_m(1.0);

	EXPECT_TRUE(w_m == w_m);
	EXPECT_FALSE(w_m == x_m);
	EXPECT_TRUE(w_m != x_m);
	EXPECT_FALSE(x_m != x_m);

	constexpr percent y_m(100);
	constexpr percent z_m(1);

	EXPECT_TRUE(y_m == y_m);
	EXPECT_FALSE(y_m == z_m);
	EXPECT_TRUE(y_m != z_m);
	EXPECT_FALSE(z_m != z_m);

	EXPECT_TRUE(w_m == y_m);
	EXPECT_TRUE(z_m == x_m);
	EXPECT_FALSE(w_m != y_m);
	EXPECT_FALSE(z_m != x_m);
	EXPECT_TRUE(w_m != z_m);
	EXPECT_TRUE(y_m != x_m);
	EXPECT_FALSE(w_m != y_m);
	EXPECT_FALSE(z_m != x_m);
}

TEST_F(UnitType, unitTypeMixedEquality)
{
	constexpr meters<double> a_m(0);
	constexpr feet           a_f(meters<double>(1));

	EXPECT_FALSE(a_m == a_f);
	EXPECT_TRUE(a_m != a_f);

	constexpr feet   b_f(0);
	constexpr meters b_m(1);

	EXPECT_FALSE(b_f == b_m);
	EXPECT_TRUE(b_f != b_m);

	EXPECT_TRUE(a_m == b_f);
	EXPECT_TRUE(b_m == a_f);
	EXPECT_FALSE(a_m != b_f);
	EXPECT_FALSE(b_m != a_f);

	constexpr percent           a_pct(1.0);
	constexpr parts_per_million b_ppm(20000.0);

	EXPECT_FALSE(a_pct == b_ppm);
	EXPECT_TRUE(a_pct != b_ppm);

	constexpr percent           c_pct(1);
	constexpr parts_per_million d_ppm(20000);

	EXPECT_FALSE(c_pct == d_ppm);
	EXPECT_TRUE(c_pct != d_ppm);

	EXPECT_TRUE(a_pct == c_pct);
	EXPECT_TRUE(b_ppm == d_ppm);
	EXPECT_FALSE(a_pct != c_pct);
	EXPECT_FALSE(b_ppm != d_ppm);
}

TEST_F(UnitType, unitTypeRelational)
{
	constexpr meters a_m(0.0);
	constexpr meters b_m(1.0);

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

	constexpr meters c_m(0);
	constexpr meters d_m(1);

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

	constexpr dimensionless a_s(0.0);
	constexpr dimensionless b_s(1);

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

	constexpr dimensionless c_s(0.0);
	constexpr dimensionless d_s(1);

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

	constexpr percent p(2.0);
	constexpr percent pp(5);

	EXPECT_FALSE(p < p);
	EXPECT_FALSE(pp < p);
	EXPECT_TRUE(p < pp);
	EXPECT_TRUE(p <= p);
	EXPECT_FALSE(pp <= p);
	EXPECT_TRUE(p <= pp);
	EXPECT_FALSE(p > p);
	EXPECT_TRUE(pp > p);
	EXPECT_FALSE(p > pp);
	EXPECT_TRUE(p >= p);
	EXPECT_TRUE(pp >= p);
	EXPECT_FALSE(p >= pp);
}

TEST_F(UnitType, unitTypeMixedRelational)
{
	constexpr meters a_m(0.0);
	constexpr feet   a_f(meters{1.0});

	EXPECT_FALSE(a_f < a_m);
	EXPECT_TRUE(a_m < a_f);
	EXPECT_FALSE(a_f <= a_m);
	EXPECT_TRUE(a_m <= a_f);
	EXPECT_TRUE(a_f > a_m);
	EXPECT_FALSE(a_m > a_f);
	EXPECT_TRUE(a_f >= a_m);
	EXPECT_FALSE(a_m >= a_f);

	constexpr feet   b_f(0);
	constexpr meters b_m(1);

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

	constexpr percent           p(1.0);
	constexpr parts_per_million pp(20000);

	EXPECT_FALSE(p < p);
	EXPECT_FALSE(pp < p);
	EXPECT_TRUE(p < pp);
	EXPECT_TRUE(p <= p);
	EXPECT_FALSE(pp <= p);
	EXPECT_TRUE(p <= pp);
	EXPECT_FALSE(p > p);
	EXPECT_TRUE(pp > p);
	EXPECT_FALSE(p > pp);
	EXPECT_TRUE(p >= p);
	EXPECT_TRUE(pp >= p);
	EXPECT_FALSE(p >= pp);
}

TEST_F(UnitType, unitTypeArithmeticOperatorReturnType)
{
	percent<int> pcnt;
	meters<int>  length;

	constexpr dimensionless dim{1};
	constexpr auto          test = dim - 0;
	static_assert(detail::has_dimension_of<decltype(dim - 0), dimension::dimensionless>::value);
	EXPECT_EQ(1, dim);
	EXPECT_EQ(test, dim);

	static_assert(std::is_same_v<dimensionless<int>, decltype(+dim)>);
	static_assert(std::is_same_v<percent<int>, decltype(+pcnt)>);
	static_assert(std::is_same_v<meters<int>, decltype(+length)>);

	static_assert(std::is_same_v<dimensionless<int>, decltype(-dim)>);
	static_assert(std::is_same_v<percent<int>, decltype(-pcnt)>);
	static_assert(std::is_same_v<meters<int>, decltype(-length)>);

	static_assert(std::is_same_v<dimensionless<int>, decltype(dim + 0)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(0 + dim)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(dim + dim)>);
	static_assert(std::is_same_v<percent<int>, decltype(pcnt + 0)>);
	static_assert(std::is_same_v<percent<int>, decltype(0 + pcnt)>);
	static_assert(std::is_same_v<percent<int>, decltype(pcnt + pcnt)>);
	static_assert(std::is_same_v<meters<int>, decltype(length + length)>);

	static_assert(std::is_same_v<dimensionless<int>, decltype(dim - 0)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(0 - dim)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(dim - dim)>);
	static_assert(std::is_same_v<percent<int>, decltype(pcnt - 0)>);
	static_assert(std::is_same_v<percent<int>, decltype(0 - pcnt)>);
	static_assert(std::is_same_v<percent<int>, decltype(pcnt - pcnt)>);
	static_assert(std::is_same_v<meters<int>, decltype(length - length)>);

	static_assert(std::is_same_v<dimensionless<int>, decltype(dim * 1)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(1 * dim)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(dim * dim)>);
	static_assert(std::is_same_v<percent<int>, decltype(pcnt * 1)>);
	static_assert(std::is_same_v<percent<int>, decltype(1 * pcnt)>);
	static_assert(std::is_same_v<unit<conversion_factor<std::ratio<1, 10000>, units::dimension::dimensionless>, int>, decltype(pcnt * pcnt)>);

	static_assert(std::is_same_v<meters<int>, decltype(length * 1)>);
	static_assert(std::is_same_v<meters<int>, decltype(1 * length)>);
	static_assert(std::is_same_v<meters<int>, decltype(length * dim)>);
	static_assert(std::is_same_v<meters<int>, decltype(dim * length)>);
	static_assert(std::is_same_v<meters<int>, decltype(length * pcnt)>);
	static_assert(std::is_same_v<meters<int>, decltype(pcnt * length)>);
	static_assert(std::is_same_v<square_meters<int>, decltype(length * length)>);

	static_assert(std::is_same_v<dimensionless<int>, decltype(dim / 1)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(1 / dim)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(dim / dim)>);
	static_assert(std::is_same_v<percent<int>, decltype(pcnt / 1)>);
	static_assert(std::is_same_v<unit<inverse<percent<>>, int>, decltype(1 / pcnt)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(pcnt / pcnt)>);

	static_assert(std::is_same_v<meters<int>, decltype(length / 1)>);
	static_assert(std::is_same_v<unit<inverse<meters<>>, int>, decltype(1 / length)>);
	static_assert(std::is_same_v<meters<int>, decltype(length / dim)>);
	static_assert(std::is_same_v<unit<inverse<meters<>>, int>, decltype(dim / length)>);
	static_assert(std::is_same_v<meters<int>, decltype(length / pcnt)>);
	static_assert(std::is_same_v<unit<inverse<meters<>>, int>, decltype(pcnt / length)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(length / length)>);

	static_assert(std::is_same_v<dimensionless<int>, decltype(dim % 1)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(dim % dim)>);
	static_assert(std::is_same_v<percent<int>, decltype(pcnt % 1)>);
	static_assert(std::is_same_v<percent<int>, decltype(pcnt % pcnt)>);

	static_assert(std::is_same_v<meters<int>, decltype(length % 1)>);
	static_assert(std::is_same_v<meters<int>, decltype(length % dim)>);
	static_assert(std::is_same_v<meters<int>, decltype(length % pcnt)>);
	static_assert(std::is_same_v<meters<int>, decltype(length % length)>);
}

TEST_F(UnitType, unitTypeAddition)
{
	// units
	constexpr meters                                     a_m(1.0);
	constexpr feet                                       b_ft(3.28084);
	meters                                               c_m{0.0};
	constexpr meters                                     f_m(1);
	constexpr std::common_type_t<meters<int>, feet<int>> g_m(f_m);

	double d_m = meters(b_ft).value();
	EXPECT_NEAR(1.0, d_m, 5.0e-5);
	d_m = meters(g_m).value();
	EXPECT_NEAR(1.0, d_m, 5.0e-5);

	c_m = a_m + b_ft;
	EXPECT_NEAR(2.0, c_m.value(), 5.0e-5);
	c_m = f_m + g_m;
	EXPECT_NEAR(2.0, c_m.value(), 5.0e-5);
	c_m = a_m + g_m;
	EXPECT_NEAR(2.0, c_m.value(), 5.0e-5);
	c_m = f_m + b_ft;
	EXPECT_NEAR(2.0, c_m.value(), 5.0e-5);

	c_m = b_ft + meters<double>(3);
	EXPECT_NEAR(4.0, c_m.value(), 5.0e-5);
	c_m = g_m + meters<int>(3);
	EXPECT_NEAR(4.0, c_m.value(), 5.0e-5);
	c_m = b_ft + meters<int>(3);
	EXPECT_NEAR(4.0, c_m.value(), 5.0e-5);
	c_m = g_m + meters<double>(3);
	EXPECT_NEAR(4.0, c_m.value(), 5.0e-5);

	feet<double> e_ft = b_ft + meters<double>(3);
	EXPECT_NEAR(13.12336, e_ft.value(), 5.0e-6);
	e_ft = g_m + meters<int>(3);
	EXPECT_NEAR(13.12336, e_ft.value(), 5.0e-6);
	e_ft = b_ft + meters<int>(3);
	EXPECT_NEAR(13.12336, e_ft.value(), 5.0e-6);
	e_ft = g_m + meters<double>(3);
	EXPECT_NEAR(13.12336, e_ft.value(), 5.0e-6);

	// dimensionless
	dimensionless<double> result = dimensionless<double>(1.0) + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, result, 5.0e-6);
	result = dimensionless<int>(1) + dimensionless<int>(1);
	EXPECT_NEAR(2.0, result, 5.0e-6);
	result = dimensionless<double>(1.0) + dimensionless<int>(1);
	EXPECT_NEAR(2.0, result, 5.0e-6);
	result = dimensionless<int>(1) + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, result, 5.0e-6);

	result = dimensionless<double>(1.0) + 1.0;
	EXPECT_NEAR(2.0, result, 5.0e-6);
	result = dimensionless<int>(1) + 1;
	EXPECT_NEAR(2.0, result, 5.0e-6);
	result = dimensionless<double>(1.0) + 1;
	EXPECT_NEAR(2.0, result, 5.0e-6);
	result = dimensionless<int>(1) + 1.0;
	EXPECT_NEAR(2.0, result, 5.0e-6);

	result = 1.0 + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, result, 5.0e-6);
	result = 1 + dimensionless<int>(1);
	EXPECT_NEAR(2.0, result, 5.0e-6);
	result = 1.0 + dimensionless<int>(1);
	EXPECT_NEAR(2.0, result, 5.0e-6);
	result = 1 + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, result, 5.0e-6);

	d_m = dimensionless<double>(1.0) + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, d_m, 5.0e-6);
	d_m = dimensionless<int>(1) + dimensionless<int>(1);
	EXPECT_NEAR(2.0, d_m, 5.0e-6);
	d_m = dimensionless<double>(1.0) + dimensionless<int>(1);
	EXPECT_NEAR(2.0, d_m, 5.0e-6);
	d_m = dimensionless<int>(1) + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, d_m, 5.0e-6);

	d_m = dimensionless<double>(1.0) + 1.0;
	EXPECT_NEAR(2.0, d_m, 5.0e-6);
	d_m = dimensionless<int>(1) + 1;
	EXPECT_NEAR(2.0, d_m, 5.0e-6);
	d_m = dimensionless<double>(1.0) + 1;
	EXPECT_NEAR(2.0, d_m, 5.0e-6);
	d_m = dimensionless<int>(1) + 1.0;
	EXPECT_NEAR(2.0, d_m, 5.0e-6);

	d_m = 1.0 + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, d_m, 5.0e-6);
	d_m = 1. + dimensionless<int>(1);
	EXPECT_NEAR(2.0, d_m, 5.0e-6);
	d_m = 1.0 + dimensionless<int>(1);
	EXPECT_NEAR(2.0, d_m, 5.0e-6);
	d_m = 1 + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, d_m, 5.0e-6);

	// concentration
	percent<double> pResult = percent<double>(1.0) + percent<double>(1.0);
	EXPECT_NEAR(0.02, pResult, 5.0e-6);
	EXPECT_EQ(2_pct, pResult);
	pResult = percent<int>(1) + percent<int>(1);
	EXPECT_NEAR(0.02, pResult, 5.0e-6);
	EXPECT_EQ(2_pct, pResult);
	pResult = percent<double>(1.0) + percent<int>(1);
	EXPECT_NEAR(0.02, pResult, 5.0e-6);
	EXPECT_EQ(2_pct, pResult);
	pResult = percent<int>(1) + percent<double>(1.0);
	EXPECT_NEAR(0.02, pResult, 5.0e-6);
	EXPECT_EQ(2_pct, pResult);

	pResult = percent<>(1.0) + 1.0;
	EXPECT_NEAR(1.01, pResult, 5.0e-6);
	EXPECT_EQ(101_pct, pResult);
	pResult = percent<int>(1) + 1;
	EXPECT_NEAR(1.01, pResult, 5.0e-6);
	EXPECT_EQ(101_pct, pResult);
	pResult = percent<double>(1.0) + 1;
	EXPECT_NEAR(1.01, pResult, 5.0e-6);
	EXPECT_EQ(101_pct, pResult);
	pResult = percent<int>(1) + 1.0;
	EXPECT_NEAR(1.01, pResult, 5.0e-6);
	EXPECT_EQ(101_pct, pResult);

	pResult = 1.0 + percent<double>(1.0);
	EXPECT_NEAR(1.01, pResult, 5.0e-6);
	EXPECT_EQ(101_pct, pResult);
	pResult = 1 + percent<int>(1);
	EXPECT_NEAR(1.01, pResult, 5.0e-6);
	EXPECT_EQ(101_pct, pResult);
	pResult = 1.0 + percent<int>(1);
	EXPECT_NEAR(1.01, pResult, 5.0e-6);
	EXPECT_EQ(101_pct, pResult);
	pResult = 1 + percent<double>(1.0);
	EXPECT_NEAR(1.01, pResult, 5.0e-6);
	EXPECT_EQ(101_pct, pResult);

	d_m = percent<double>(1.0) + percent<double>(1.0);
	EXPECT_NEAR(0.02, d_m, 5.0e-6);
	d_m = percent<int>(1) + percent<int>(1);
	EXPECT_NEAR(0.02, d_m, 5.0e-6);
	d_m = percent<double>(1.0) + percent<int>(1);
	EXPECT_NEAR(0.02, d_m, 5.0e-6);
	d_m = percent<int>(1) + percent<double>(1.0);
	EXPECT_NEAR(0.02, d_m, 5.0e-6);

	d_m = percent<double>(1.0) + 1.0;
	EXPECT_NEAR(1.01, d_m, 5.0e-6);
	d_m = percent<int>(1) + 1;
	EXPECT_NEAR(1.01, d_m, 5.0e-6);
	d_m = percent<double>(1.0) + 1;
	EXPECT_NEAR(1.01, d_m, 5.0e-6);
	d_m = percent<int>(1) + 1.0;
	EXPECT_NEAR(1.01, d_m, 5.0e-6);

	d_m = 1.0 + percent<double>(1.0);
	EXPECT_NEAR(1.01, d_m, 5.0e-6);
	d_m = 1. + percent<int>(1);
	EXPECT_NEAR(1.01, d_m, 5.0e-6);
	d_m = 1.0 + percent<int>(1);
	EXPECT_NEAR(1.01, d_m, 5.0e-6);
	d_m = 1 + percent<double>(1.0);
	EXPECT_NEAR(1.01, d_m, 5.0e-6);

	percent<double> p = 5_pct + 25_pct;
	EXPECT_EQ(30_pct, p);
	EXPECT_NEAR(0.3, p.value(), 5.0e-6);
	EXPECT_DOUBLE_EQ(30, p.raw());

	percent<double> z = 5_pct + 10000_ppm;
	EXPECT_EQ(6_pct, z);
	EXPECT_NEAR(0.06, z.value(), 5.0e-6);
	EXPECT_DOUBLE_EQ(6, z.raw());
}

TEST_F(UnitType, unitTypeUnaryAddition)
{
	meters<double> a_m(1.0);

	EXPECT_EQ(++a_m, meters<double>(2));
	EXPECT_EQ(a_m++, meters<double>(2));
	EXPECT_EQ(a_m, meters<double>(3));
	EXPECT_EQ(+a_m, meters<double>(3));
	EXPECT_EQ(a_m, meters<double>(3));

	dBW<double> b_dBW(1.0);

	EXPECT_EQ(++b_dBW, dBW<double>(2));
	EXPECT_EQ(b_dBW++, dBW<double>(2));
	EXPECT_EQ(b_dBW, dBW<double>(3));
	EXPECT_EQ(+b_dBW, dBW<double>(3));
	EXPECT_EQ(b_dBW, dBW<double>(3));

	percent<double> c_pct(1.0);

	EXPECT_EQ(++c_pct, percent<double>(2));
	EXPECT_EQ(c_pct++, percent<double>(2));
	EXPECT_EQ(c_pct, percent<double>(3));
	EXPECT_EQ(+c_pct, percent<double>(3));
	EXPECT_EQ(c_pct, percent<double>(3));
}

TEST_F(UnitType, unitTypeSubtraction)
{
	constexpr meters                                     a_m(1.0);
	constexpr feet                                       b_ft(3.28084);
	meters                                               c_m{0.0};
	constexpr meters                                     f_m(1);
	constexpr std::common_type_t<meters<int>, feet<int>> g_m(f_m);

	c_m = a_m - b_ft;
	EXPECT_NEAR(0.0, c_m.value(), 5.0e-5);
	c_m = f_m - g_m;
	EXPECT_NEAR(0.0, c_m.value(), 5.0e-5);
	c_m = a_m - g_m;
	EXPECT_NEAR(0.0, c_m.value(), 5.0e-5);
	c_m = f_m - b_ft;
	EXPECT_NEAR(0.0, c_m.value(), 5.0e-5);

	c_m = b_ft - meters<double>(1);
	EXPECT_NEAR(0.0, c_m.value(), 5.0e-5);
	c_m = g_m - meters<int>(1);
	EXPECT_NEAR(0.0, c_m.value(), 5.0e-5);
	c_m = b_ft - meters<int>(1);
	EXPECT_NEAR(0.0, c_m.value(), 5.0e-5);
	c_m = g_m - meters<double>(1);
	EXPECT_NEAR(0.0, c_m.value(), 5.0e-5);

	feet<double> e_ft = b_ft - meters<double>(1);
	EXPECT_NEAR(0.0, e_ft.value(), 5.0e-6);
	e_ft = g_m - meters<int>(1);
	EXPECT_NEAR(0.0, e_ft.value(), 5.0e-6);
	e_ft = b_ft - meters<int>(1);
	EXPECT_NEAR(0.0, e_ft.value(), 5.0e-6);
	e_ft = g_m - meters<double>(1);
	EXPECT_NEAR(0.0, e_ft.value(), 5.0e-6);

	// dimensionless
	dimensionless<double> sResult = dimensionless<double>(1.0) - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, sResult, 5.0e-6);
	sResult = dimensionless<int>(1) - dimensionless<int>(1);
	EXPECT_NEAR(0.0, sResult, 5.0e-6);
	sResult = dimensionless<double>(1.0) - dimensionless<int>(1);
	EXPECT_NEAR(0.0, sResult, 5.0e-6);
	sResult = dimensionless<int>(1) - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, sResult, 5.0e-6);

	sResult = dimensionless<double>(1.0) - 1.0;
	EXPECT_NEAR(0.0, sResult, 5.0e-6);
	sResult = dimensionless<int>(1) - 1;
	EXPECT_NEAR(0.0, sResult, 5.0e-6);
	sResult = dimensionless<double>(1.0) - 1;
	EXPECT_NEAR(0.0, sResult, 5.0e-6);
	sResult = dimensionless<int>(1) - 1.0;
	EXPECT_NEAR(0.0, sResult, 5.0e-6);

	sResult = 1.0 - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, sResult, 5.0e-6);
	sResult = 1 - dimensionless<int>(1);
	EXPECT_NEAR(0.0, sResult, 5.0e-6);
	sResult = 1.0 - dimensionless<int>(1);
	EXPECT_NEAR(0.0, sResult, 5.0e-6);
	sResult = 1 - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, sResult, 5.0e-6);

	double dim = dimensionless<double>(1.0) - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, dim, 5.0e-6);
	dim = dimensionless<int>(1) - dimensionless<int>(1);
	EXPECT_NEAR(0.0, dim, 5.0e-6);
	dim = dimensionless<double>(1.0) - dimensionless<int>(1);
	EXPECT_NEAR(0.0, dim, 5.0e-6);
	dim = dimensionless<int>(1) - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, dim, 5.0e-6);

	dim = dimensionless<double>(1.0) - 1.0;
	EXPECT_NEAR(0.0, dim, 5.0e-6);
	dim = dimensionless<int>(1) - 1;
	EXPECT_NEAR(0.0, dim, 5.0e-6);
	dim = dimensionless<double>(1.0) - 1;
	EXPECT_NEAR(0.0, dim, 5.0e-6);
	dim = dimensionless<int>(1) - 1.0;
	EXPECT_NEAR(0.0, dim, 5.0e-6);

	dim = 1.0 - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, dim, 5.0e-6);
	dim = 1 - dimensionless<int>(1);
	EXPECT_NEAR(0.0, dim, 5.0e-6);
	dim = 1.0 - dimensionless<int>(1);
	EXPECT_NEAR(0.0, dim, 5.0e-6);
	dim = 1 - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, dim, 5.0e-6);

	// concentration
	percent<double> pResult = percent<double>(5) - percent<double>(1.0);
	EXPECT_NEAR(0.04, pResult, 5.0e-6);
	pResult = percent<int>(5) - percent<int>(1);
	EXPECT_NEAR(4_pct, pResult, 5.0e-6);
	pResult = percent<double>(5.0) - percent<int>(1);
	EXPECT_NEAR(0.04, pResult, 5.0e-6);
	pResult = percent<int>(5) - percent<double>(1.0);
	EXPECT_NEAR(4_pct, pResult, 5.0e-6);

	pResult = percent<double>(100.0) - 1.0;
	EXPECT_NEAR(0.0, pResult, 5.0e-6);
	pResult = percent<int>(100) - 1;
	EXPECT_NEAR(0.0, pResult, 5.0e-6);
	pResult = percent<double>(100.0) - 1;
	EXPECT_NEAR(0.0, pResult, 5.0e-6);
	pResult = percent<int>(100) - 1.0;
	EXPECT_NEAR(0.0, pResult, 5.0e-6);

	pResult = 1.0 - percent<double>(100.0);
	EXPECT_NEAR(0.0, pResult, 5.0e-6);
	pResult = 1 - percent<int>(100);
	EXPECT_NEAR(0.0, pResult, 5.0e-6);
	pResult = 1.0 - percent<int>(100);
	EXPECT_NEAR(0.0, pResult, 5.0e-6);
	pResult = 1 - percent<double>(100.0);
	EXPECT_NEAR(0.0, pResult, 5.0e-6);

	dim = percent<double>(100.0) - percent<double>(10.0);
	EXPECT_NEAR(0.9, dim, 5.0e-6);
	dim = percent<int>(100) - percent<int>(10);
	EXPECT_NEAR(0.9, dim, 5.0e-6);
	dim = percent<double>(100.0) - percent<int>(10);
	EXPECT_NEAR(0.9, dim, 5.0e-6);
	dim = percent<int>(100) - percent<double>(10.0);
	EXPECT_NEAR(0.9, dim, 5.0e-6);

	dim = percent<double>(100.0) - 1.0;
	EXPECT_NEAR(0.0, dim, 5.0e-6);
	dim = percent<int>(100) - 1;
	EXPECT_NEAR(0.0, dim, 5.0e-6);
	dim = percent<double>(100.0) - 1;
	EXPECT_NEAR(0.0, dim, 5.0e-6);
	dim = percent<int>(100) - 1.0;
	EXPECT_NEAR(0.0, dim, 5.0e-6);

	dim = 1.0 - percent<double>(100.0);
	EXPECT_NEAR(0.0, dim, 5.0e-6);
	dim = 1 - percent<int>(100);
	EXPECT_NEAR(0.0, dim, 5.0e-6);
	dim = 1.0 - percent<int>(100);
	EXPECT_NEAR(0.0, dim, 5.0e-6);
	dim = 1 - percent<double>(100.0);
	EXPECT_NEAR(0.0, dim, 5.0e-6);
}

TEST_F(UnitType, unitTypeUnarySubtraction)
{
	meters<double> a_m(4.0);

	EXPECT_EQ(--a_m, meters<double>(3));
	EXPECT_EQ(a_m--, meters<double>(3));
	EXPECT_EQ(a_m, meters<double>(2));
	EXPECT_EQ(-a_m, meters<double>(-2));
	EXPECT_EQ(a_m, meters<double>(2));

	dBW<double> b_dBW(4.0);

	EXPECT_EQ(--b_dBW, dBW<double>(3));
	EXPECT_EQ(b_dBW--, dBW<double>(3));
	EXPECT_EQ(b_dBW, dBW<double>(2));
	EXPECT_EQ(-b_dBW, dBW<double>(-2));
	EXPECT_EQ(b_dBW, dBW<double>(2));

	percent<double> c_pct(4.0);

	EXPECT_EQ(--c_pct, percent<double>(3));
	EXPECT_EQ(c_pct--, percent<double>(3));
	EXPECT_EQ(c_pct, percent<double>(2));
	EXPECT_EQ(-c_pct, percent<double>(-2));
	EXPECT_EQ(c_pct, percent<double>(2));
}

TEST_F(UnitType, unitTypeMultiplication)
{
	constexpr meters                                     a_m(1.0);
	constexpr meters                                     b_m(2.0);
	constexpr feet                                       a_ft(3.28084);
	constexpr meters<int>                                d_m(1), e_m(2);
	constexpr std::common_type_t<meters<int>, feet<int>> f(d_m);

	auto c_m2 = a_m * b_m;
	EXPECT_NEAR(2.0, c_m2.value(), 5.0e-5);
	c_m2 = d_m * e_m;
	EXPECT_NEAR(2.0, c_m2.value(), 5.0e-5);
	c_m2 = a_m * e_m;
	EXPECT_NEAR(2.0, c_m2.value(), 5.0e-5);
	c_m2 = d_m * b_m;
	EXPECT_NEAR(2.0, c_m2.value(), 5.0e-5);

	c_m2 = b_m * meters<double>(2);
	EXPECT_NEAR(4.0, c_m2.value(), 5.0e-5);
	c_m2 = e_m * meters<int>(2);
	EXPECT_NEAR(4.0, c_m2.value(), 5.0e-5);
	c_m2 = b_m * meters<int>(2);
	EXPECT_NEAR(4.0, c_m2.value(), 5.0e-5);
	c_m2 = e_m * meters<double>(2);
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

	// dimensionless
	dimensionless<double> sResult = dimensionless<double>(5.0) * dimensionless<double>(4.0);
	EXPECT_NEAR(20.0, sResult.value(), 5.0e-5);
	sResult = dimensionless<int>(5) * dimensionless<int>(4);
	EXPECT_NEAR(20.0, sResult.value(), 5.0e-5);
	sResult = dimensionless<double>(5.0) * dimensionless<int>(4);
	EXPECT_NEAR(20.0, sResult.value(), 5.0e-5);
	sResult = dimensionless<int>(5) * dimensionless<double>(4.0);
	EXPECT_NEAR(20.0, sResult.value(), 5.0e-5);

	sResult = dimensionless<double>(5.0) * 4.0;
	EXPECT_NEAR(20.0, sResult.value(), 5.0e-5);
	sResult = dimensionless<int>(5) * 4;
	EXPECT_NEAR(20.0, sResult.value(), 5.0e-5);
	sResult = dimensionless<double>(5.0) * 4;
	EXPECT_NEAR(20.0, sResult.value(), 5.0e-5);
	sResult = dimensionless<int>(5) * 4.0;
	EXPECT_NEAR(20.0, sResult.value(), 5.0e-5);

	sResult = 4.0 * dimensionless<double>(5.0);
	EXPECT_NEAR(20.0, sResult.value(), 5.0e-5);
	sResult = 4 * dimensionless<int>(5);
	EXPECT_NEAR(20.0, sResult.value(), 5.0e-5);
	sResult = 4.0 * dimensionless<int>(5);
	EXPECT_NEAR(20.0, sResult.value(), 5.0e-5);
	sResult = 4 * dimensionless<double>(5.0);
	EXPECT_NEAR(20.0, sResult.value(), 5.0e-5);

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

	// dimensionless result
	result = 60_km / 400_mm;
	EXPECT_EQ(150'000, result);

	// concentration
	percent<double> pResult = percent<double>(5.0) * percent<double>(4.0);
	EXPECT_DOUBLE_EQ(0.2_pct, pResult);
	EXPECT_DOUBLE_EQ(0.002, pResult.value());
	pResult = percent<int>(5) * percent<int>(4);
	EXPECT_DOUBLE_EQ(0.2_pct, pResult);
	EXPECT_DOUBLE_EQ(0.002, pResult.value());
	pResult = percent<double>(5.0) * percent<int>(4);
	EXPECT_DOUBLE_EQ(0.2_pct, pResult);
	EXPECT_DOUBLE_EQ(0.002, pResult.value());
	pResult = percent<int>(5) * percent<double>(4.0);
	EXPECT_DOUBLE_EQ(0.2_pct, pResult);
	EXPECT_DOUBLE_EQ(0.002, pResult.value());

	pResult = percent<double>(5.0) * 4.0;
	EXPECT_DOUBLE_EQ(20.0_pct, pResult);
	EXPECT_DOUBLE_EQ(0.2, pResult.value());
	pResult = percent<int>(5) * 4;
	EXPECT_DOUBLE_EQ(20.0_pct, pResult);
	EXPECT_DOUBLE_EQ(0.2, pResult.value());
	pResult = percent<double>(5.0) * 4;
	EXPECT_DOUBLE_EQ(20.0_pct, pResult);
	EXPECT_DOUBLE_EQ(0.2, pResult.value());
	pResult = percent<int>(5) * 4.0;
	EXPECT_DOUBLE_EQ(20.0_pct, pResult);
	EXPECT_DOUBLE_EQ(0.2, pResult.value());

	pResult = 4.0 * percent<double>(5.0);
	EXPECT_DOUBLE_EQ(20.0_pct, pResult);
	EXPECT_DOUBLE_EQ(0.2, pResult.value());
	pResult = 4 * percent<int>(5);
	EXPECT_DOUBLE_EQ(20.0_pct, pResult);
	EXPECT_DOUBLE_EQ(0.2, pResult.value());
	pResult = 4.0 * percent<int>(5);
	EXPECT_DOUBLE_EQ(20.0_pct, pResult);
	EXPECT_DOUBLE_EQ(0.2, pResult.value());
	pResult = 4 * percent<double>(5.0);
	EXPECT_DOUBLE_EQ(20.0_pct, pResult);
	EXPECT_DOUBLE_EQ(0.2, pResult.value());

	result = percent<double>(5.0) * percent<double>(4.0);
	EXPECT_NEAR(0.002, result, 5.0e-5);
	result = percent<int>(5) * percent<int>(4);
	EXPECT_NEAR(0.002, result, 5.0e-5);
	result = percent<double>(5.0) * percent<int>(4);
	EXPECT_NEAR(0.002, result, 5.0e-5);
	result = percent<int>(5) * percent<double>(4.0);
	EXPECT_NEAR(0.002, result, 5.0e-5);

	result = percent<double>(5.0) * 4.0;
	EXPECT_NEAR(0.2, result, 5.0e-5);
	result = percent<int>(5) * 4;
	EXPECT_NEAR(0.2, result, 5.0e-5);
	result = percent<double>(5.0) * 4;
	EXPECT_NEAR(0.2, result, 5.0e-5);
	result = percent<int>(5) * 4.0;
	EXPECT_NEAR(0.2, result, 5.0e-5);

	result = 4.0 * percent<double>(5.0);
	EXPECT_NEAR(0.2, result, 5.0e-5);
	result = 4 * percent<int>(5);
	EXPECT_NEAR(0.2, result, 5.0e-5);
	result = 4.0 * percent<int>(5);
	EXPECT_NEAR(0.2, result, 5.0e-5);
	result = 4 * percent<double>(5.0);
	EXPECT_NEAR(0.2, result, 5.0e-5);

	auto value = 10.0_pct * 100.0_m;
	EXPECT_EQ(value, 10.0_m);
}

TEST_F(UnitType, unitTypeMixedUnitMultiplication)
{
	constexpr meters                                     a_m(1.0);
	constexpr feet                                       b_ft(3.28084);
	constexpr unit<inverse<meters<double>>>              i_m(2.0);
	constexpr meters                                     b_m(1);
	constexpr std::common_type_t<meters<int>, feet<int>> f(b_m);
	constexpr unit<inverse<meters<double>>, int>         i_i_m(2);

	// resultant unit is square of the common type unit
	// you can get whatever (compatible) type you want if you ask explicitly
	unit<squared<meters<double>>> c_m2 = a_m * b_ft;
	EXPECT_NEAR(1.0, c_m2.value(), 5.0e-5);
	c_m2 = b_m * f;
	EXPECT_NEAR(1.0, c_m2.value(), 5.0e-5);
	c_m2 = a_m * f;
	EXPECT_NEAR(1.0, c_m2.value(), 5.0e-5);
	c_m2 = b_m * b_ft;
	EXPECT_NEAR(1.0, c_m2.value(), 5.0e-5);

	unit<squared<feet<double>>> c_ft2 = b_ft * a_m;
	EXPECT_NEAR(10.7639111056, c_ft2.value(), 5.0e-7);
	c_ft2 = f * b_m;
	EXPECT_NEAR(10.7639111056, c_ft2.value(), 5.0e-6);
	c_ft2 = b_ft * b_m;
	EXPECT_NEAR(10.7639111056, c_ft2.value(), 5.0e-7);
	c_ft2 = f * a_m;
	EXPECT_NEAR(10.7639111056, c_ft2.value(), 5.0e-6);

	square_meters<double> d_m2 = b_ft * a_m;
	EXPECT_NEAR(1.0, d_m2.value(), 5.0e-5);
	d_m2 = f * b_m;
	EXPECT_NEAR(1.0, d_m2.value(), 5.0e-5);
	d_m2 = b_ft * b_m;
	EXPECT_NEAR(1.0, d_m2.value(), 5.0e-5);
	d_m2 = f * a_m;
	EXPECT_NEAR(1.0, d_m2.value(), 5.0e-5);

	// a unit times a scalar ends up with the same units.
	meters<double> e_m = a_m * dimensionless<double>(3.0);
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
	dimensionless<double> dim = a_m * i_m;
	EXPECT_NEAR(2.0, dim, 5.0e-5);
	dim = b_m * i_i_m;
	EXPECT_NEAR(2.0, dim, 5.0e-5);
	dim = a_m * i_i_m;
	EXPECT_NEAR(2.0, dim, 5.0e-5);
	dim = b_m * i_m;
	EXPECT_NEAR(2.0, dim, 5.0e-5);

	c_m2 = b_ft * meters<double>(2);
	EXPECT_NEAR(2.0, c_m2.value(), 5.0e-5);
	c_m2 = f * meters<int>(2);
	EXPECT_NEAR(2.0, c_m2.value(), 5.0e-5);
	c_m2 = b_ft * meters<int>(2);
	EXPECT_NEAR(2.0, c_m2.value(), 5.0e-5);
	c_m2 = f * meters<double>(2);
	EXPECT_NEAR(2.0, c_m2.value(), 5.0e-5);

	unit<squared<feet<double>>> e_ft2 = b_ft * meters<double>(3);
	EXPECT_NEAR(32.2917333168, e_ft2.value(), 5.0e-6);
	e_ft2 = f * meters<int>(3);
	EXPECT_NEAR(32.2917333168, e_ft2.value(), 5.0e-6);
	e_ft2 = b_ft * meters<int>(3);
	EXPECT_NEAR(32.2917333168, e_ft2.value(), 5.0e-6);
	e_ft2 = f * meters<double>(3);
	EXPECT_NEAR(32.2917333168, e_ft2.value(), 5.0e-6);

	auto metersPerSecond = meters<double>(10.0) * unit<inverse<seconds<double>>>(1.0);
	EXPECT_EQ(metersPerSecond, meters_per_second<double>(10));
	metersPerSecond = meters<int>(10) * unit<inverse<seconds<double>>, int>(1);
	EXPECT_EQ(metersPerSecond, meters_per_second<double>(10));
	metersPerSecond = meters<double>(10.0) * unit<inverse<seconds<double>>, int>(1);
	EXPECT_EQ(metersPerSecond, meters_per_second<double>(10));
	metersPerSecond = meters<int>(10) * unit<inverse<seconds<double>>>(1.0);
	EXPECT_EQ(metersPerSecond, meters_per_second<double>(10));
}

TEST_F(UnitType, unitTypeDimensionlessMultiplication)
{
	meters<double> a_m(1.0);

	auto result_m = dimensionless<double>(3.0) * a_m;
	EXPECT_NEAR(3.0, result_m.value(), 5.0e-5);

	result_m = a_m * dimensionless<double>(4.0);
	EXPECT_NEAR(4.0, result_m.value(), 5.0e-5);

	result_m = 3.0 * a_m;
	EXPECT_NEAR(3.0, result_m.value(), 5.0e-5);

	result_m = a_m * 4.0;
	EXPECT_NEAR(4.0, result_m.value(), 5.0e-5);

	bool isSame = std::is_same_v<decltype(result_m), meters<double>>;
	EXPECT_TRUE(isSame);

	kilometers<double> dist = 50.0_pct * 2000_m;
	EXPECT_EQ(dist, 1_km);
}

TEST_F(UnitType, unitTypeDivision)
{
	constexpr meters<double>                             a_m(1.0), b_m(2.0);
	constexpr feet                                       a_ft(3.28084);
	constexpr seconds                                    a_sec(10.0);
	constexpr meters<int>                                d_m(1), e_m(2);
	constexpr std::common_type_t<meters<int>, feet<int>> j(d_m);
	constexpr seconds                                    b_sec(10);
	bool                                                 isSame;

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

	double d_dim = dimensionless<double>(1.0) / 2.0;
	EXPECT_NEAR(0.5, d_dim, 5.0e-5);

	auto e_mps = a_m / a_sec;
	EXPECT_NEAR(0.1, e_mps.value(), 5.0e-5);
	e_mps = d_m / b_sec;
	EXPECT_EQ(0, e_mps.value());
	e_mps = a_m / b_sec;
	EXPECT_NEAR(0.1, e_mps.value(), 5.0e-5);
	e_mps = d_m / a_sec;
	EXPECT_NEAR(0.1, e_mps.value(), 5.0e-5);
	isSame = std::is_same_v<decltype(e_mps), meters_per_second<double>>;
	EXPECT_TRUE(isSame);

	auto f = a_m / 8.0;
	EXPECT_NEAR(0.125, f.value(), 5.0e-5);
	f = d_m / 8;
	EXPECT_EQ(0, f.value());
	f = a_m / 8;
	EXPECT_NEAR(0.125, f.value(), 5.0e-5);
	f = d_m / 8.0;
	EXPECT_NEAR(0.125, f.value(), 5.0e-5);
	isSame = std::is_same_v<decltype(f), meters<double>>;
	EXPECT_TRUE(isSame);

	auto invMeters = 4.0 / b_m;
	EXPECT_NEAR(2.0, invMeters.value(), 5.0e-5);
	invMeters = 4 / e_m;
	EXPECT_NEAR(2.0, invMeters.value(), 5.0e-5);
	invMeters = 4.0 / e_m;
	EXPECT_NEAR(2.0, invMeters.value(), 5.0e-5);
	invMeters = 4 / b_m;
	EXPECT_NEAR(2.0, invMeters.value(), 5.0e-5);
	isSame = std::is_same_v<decltype(invMeters), unit<inverse<meters<double>>>>;
	EXPECT_TRUE(isSame);

	auto                      miles_speed = miles<double>(60.0) / hours<double>(1.0);
	meters_per_second<double> speed       = miles_speed;
	EXPECT_NEAR(26.8224, speed.value(), 5.0e-5);
	speed = miles<int>(60) / hours<int>(1);
	EXPECT_NEAR(26.8224, speed.value(), 5.0e-5);
	speed = miles<double>(60.0) / hours<int>(1);
	EXPECT_NEAR(26.8224, speed.value(), 5.0e-5);
	speed = miles<int>(60) / hours<double>(1.0);
	EXPECT_NEAR(26.8224, speed.value(), 5.0e-5);

	auto h_dim = 10.0_rad / 2.0_rad;
	EXPECT_NEAR(5, h_dim, 5.0e-5);
	h_dim = radians<int>(10) / radians<int>(2);
	EXPECT_NEAR(5, h_dim, 5.0e-5);
	h_dim = 10.0_rad / radians<int>(2);
	EXPECT_NEAR(5, h_dim, 5.0e-5);
	h_dim = radians<int>(10) / 2.0_rad;
	EXPECT_NEAR(5, h_dim, 5.0e-5);
	isSame = std::is_same_v<decltype(h_dim), dimensionless<double>>;
	EXPECT_TRUE(isSame);

	auto i = (3.0_N * 2.0_m) / 6.0_J;
	EXPECT_NEAR(1, i, 5.0e-5);
	i = (force::newtons<int>(3) * meters<int>(2)) / joules<int>(6);
	EXPECT_NEAR(1, i, 5.0e-5);
	i = (3.0_N * meters<int>(2)) / joules<int>(6);
	EXPECT_NEAR(1, i, 5.0e-5);
	i = (force::newtons<int>(3) * meters<int>(2)) / 6.0_J;
	EXPECT_NEAR(1, i, 5.0e-5);
	isSame = std::is_same_v<decltype(i), dimensionless<double>>;
	EXPECT_TRUE(isSame);

	unit<inverse<meters<double>>> k = 50.0_pct / 1.0_m;
	EXPECT_DOUBLE_EQ(k.value(), 0.5);
	meters<double> l = 10.0_km / 25.0_pct;
	EXPECT_EQ(l, 40'000.0_m);
	dimensionless<double> m_dim = 5.0_pct / 4.0_pct;
	EXPECT_EQ(m_dim, 1.25);
	dimensionless<int> n = 5_pct / 4_pct;
	EXPECT_EQ(n, 1);

	double o = 5.0 / 20.0_pct;
	EXPECT_DOUBLE_EQ(o, 25.0);
	o = 20.0_pct / 5.0;
	EXPECT_DOUBLE_EQ(o, 0.04);

	double p = 1.0 / pi;
	EXPECT_NEAR(0.3183098862, p, 5.0e-11);
}

TEST_F(UnitType, unitTypeModulo)
{
	constexpr meters     a_m(2200);
	constexpr meters     b_m(1800);
	constexpr kilometers a_km(2);

	constexpr auto c_m = a_m % b_m;
	EXPECT_EQ(400, c_m.value());
	static_assert(has_equivalent_conversion_factor(c_m, a_m));

	constexpr auto d_m = a_m % a_km;
	EXPECT_EQ(200, d_m.value());
	static_assert(has_equivalent_conversion_factor(d_m, a_m));

	constexpr auto b_km = a_km % dimensionless<int>(3);
	EXPECT_EQ(2, b_km.value());
	static_assert(has_equivalent_conversion_factor(b_km, a_km));

	constexpr auto e_m = a_m % 2000;
	EXPECT_EQ(200, e_m.value());
	static_assert(has_equivalent_conversion_factor(e_m, a_m));

	constexpr dimensionless a_s(12);
	constexpr dimensionless b_s(5);

	constexpr auto c_s = a_s % b_s;
	EXPECT_EQ(2, c_s.value());
	static_assert(has_equivalent_conversion_factor(c_s, a_s));

	constexpr auto d_s = a_s % 20;
	EXPECT_EQ(12, d_s.value());
	static_assert(has_equivalent_conversion_factor(d_s, a_s));

	auto y = percent<int>(12) % percent<int>(5);
	EXPECT_EQ(2_pct, y);
	static_assert(has_equivalent_conversion_factor(y, percent<int>(5)));

	y = percent<int>(5) % percent<int>(12);
	EXPECT_EQ(5_pct, y);
	static_assert(has_equivalent_conversion_factor(y, percent<int>(5)));

	auto y2 = dimensionless<int>(15) % percent<int>(5);
	EXPECT_EQ(0_pct, y2);

	auto y3 = percent<int>(29) % percent<int>(3);
	EXPECT_EQ(2_pct, y3);

	auto z = percent<int>(12) % 20;
	EXPECT_EQ(12_pct, z);
	static_assert(has_equivalent_conversion_factor(z, percent<int>(12)));

	z = percent<int>(12) % 5;
	EXPECT_EQ(2_pct, z);
	static_assert(has_equivalent_conversion_factor(z, percent<int>(12)));

	z = percent<int>(12) % dimensionless<int>(5);
	EXPECT_EQ(2_pct, z);
	static_assert(has_equivalent_conversion_factor(z, percent<int>(12)));

	std::vector<percent<int>> vec;
	for (percent<int> i = 1_pct; i <= 100_pct; ++i)
	{
		if (i % 10_pct == 0_pct)
			vec.push_back(i);
	}
	EXPECT_EQ(vec.size(), 10);
	EXPECT_EQ(vec[0], 10_pct);
	EXPECT_EQ(vec[1], 20_pct);
	EXPECT_EQ(vec[2], 30_pct);
	EXPECT_EQ(vec[3], 40_pct);
	EXPECT_EQ(vec[4], 50_pct);
	EXPECT_EQ(vec[5], 60_pct);
	EXPECT_EQ(vec[6], 70_pct);
	EXPECT_EQ(vec[7], 80_pct);
	EXPECT_EQ(vec[8], 90_pct);
	EXPECT_EQ(vec[9], 100_pct);
}

TEST_F(UnitType, compoundAssignmentAddition)
{
	// units
	meters<double> a(0.0);
	a += meters<double>(1.0);

	EXPECT_EQ(meters<double>(1.0), a);

	a += feet<double>(meters<double>(1));

	EXPECT_EQ(meters<double>(2.0), a);

	a += meters<int>(1);

	EXPECT_EQ(meters<double>(3.0), a);

	a += std::common_type_t<meters<int>, feet<int>>(meters<int>(1));

	EXPECT_EQ(meters<double>(4.0), a);

	meters<int> c(0);
	c += meters<int>(1);

	EXPECT_EQ((meters<int>(1)), c);

	c += kilometers<int>(1);

	EXPECT_EQ((meters<int>(1001)), c);

	// dimensionless
	dimensionless<double> b_dim(0);
	b_dim += dimensionless<double>(1.0);

	EXPECT_EQ(dimensionless<double>(1.0), b_dim);

	b_dim += 1.0;

	EXPECT_EQ(dimensionless<double>(2.0), b_dim);

	b_dim += dimensionless<int>(1);

	EXPECT_EQ(dimensionless<double>(3.0), b_dim);

	b_dim += 1;

	EXPECT_EQ(dimensionless<double>(4.0), b_dim);

	dimensionless<int> e_dim(0);
	e_dim += dimensionless<int>(1);

	EXPECT_EQ((dimensionless<int>(1)), e_dim);

	e_dim += 1;

	EXPECT_EQ((dimensionless<int>(2)), e_dim);

	// concentration
	percent<double> e_pct(0);
	e_pct += percent<double>(1.0);

	EXPECT_EQ(percent<double>(1.0), e_pct);

	e_pct += 1.0;

	EXPECT_EQ(percent<double>(101.0), e_pct);

	e_pct += percent<int>(1);

	EXPECT_EQ(percent<double>(102.0), e_pct);

	e_pct += 1;

	EXPECT_EQ(percent<double>(202.0), e_pct);

	percent<int> f(0);
	f += percent<int>(1);

	EXPECT_EQ((percent<int>(1)), f);

	f += 1;

	EXPECT_EQ((percent<int>(101)), f);

	f += dimensionless<int>(1);

	EXPECT_EQ((percent<int>(201)), f);
}

TEST_F(UnitType, compoundAssignmentSubtraction)
{
	// units
	meters<double> a(2.0);
	a -= meters<double>(1.0);

	EXPECT_EQ(meters<double>(1.0), a);

	a -= feet<double>(meters<double>(1));

	EXPECT_EQ(meters<double>(0.0), a);

	a -= meters<int>(1);

	EXPECT_EQ(meters<double>(-1.0), a);

	a -= std::common_type_t<meters<int>, feet<int>>(meters<int>(1));

	EXPECT_EQ(meters<double>(-2.0), a);

	meters<int> c(1);
	c -= meters<int>(1);

	EXPECT_EQ((meters<int>(0)), c);

	c -= kilometers<int>(1);

	EXPECT_EQ((meters<int>(-1000)), c);

	// dimensionless
	dimensionless<double> b_dim(2);
	b_dim -= dimensionless<double>(1.0);

	EXPECT_EQ(dimensionless<double>(1.0), b_dim);

	b_dim -= 1.0;

	EXPECT_EQ(dimensionless<double>(0), b_dim);

	b_dim -= dimensionless<int>(1);

	EXPECT_EQ(dimensionless<double>(-1.0), b_dim);

	b_dim -= 1;

	EXPECT_EQ(dimensionless<double>(-2.0), b_dim);

	dimensionless<int> d_dim(2);
	d_dim -= dimensionless<int>(1);

	EXPECT_EQ((dimensionless<int>(1)), d_dim);

	d_dim -= 1;

	EXPECT_EQ((dimensionless<int>(0)), d_dim);

	// concentration
	percent<double> e_pct(200);
	e_pct -= percent<double>(1.0);

	EXPECT_EQ(percent<double>(199.0), e_pct);

	e_pct -= 1.0;

	EXPECT_EQ(percent<double>(99.0), e_pct);

	e_pct -= percent<int>(100);

	EXPECT_EQ(percent<double>(-1.0), e_pct);

	e_pct -= 1;

	EXPECT_EQ(percent<double>(-101.0), e_pct);

	percent<int> f(2);
	f -= percent<int>(1);

	EXPECT_EQ((percent<int>(1)), f);

	f -= 1;

	EXPECT_EQ(percent<int>(-99), f);

	f -= dimensionless<int>(1);

	EXPECT_EQ(percent<int>(-199), f);
}

TEST_F(UnitType, compoundAssignmentMultiplication)
{
	// units
	meters<double> a(2.0);
	a *= dimensionless<double>(2.0);

	EXPECT_EQ(meters<double>(4.0), a);

	a *= 2.0;

	EXPECT_EQ(meters<double>(8.0), a);

	a *= dimensionless<int>(2);

	EXPECT_EQ(meters<double>(16), a);

	a *= 2;

	EXPECT_EQ(meters<double>(32), a);

	meters<int> c(2);
	c *= dimensionless<int>(2);

	EXPECT_EQ((meters<int>(4)), c);

	c *= dimensionless<double>(2.0);

	EXPECT_EQ((meters<int>(8)), c);

	c *= 2;

	EXPECT_EQ((meters<int>(16)), c);

	c *= 2.0;

	EXPECT_EQ((meters<int>(32)), c);

	c *= 200.0_pct;

	EXPECT_EQ((meters<int>(64)), c);

	// dimensionless
	dimensionless<double> b_dim(2);
	b_dim *= dimensionless<double>(2.0);

	EXPECT_EQ(dimensionless<double>(4.0), b_dim);

	b_dim *= 2.0;

	EXPECT_EQ(dimensionless<double>(8.0), b_dim);

	b_dim *= dimensionless<int>(2);

	EXPECT_EQ(dimensionless<double>(16.0), b_dim);

	b_dim *= 2;

	EXPECT_EQ(dimensionless<double>(32.0), b_dim);

	dimensionless<int> d_dim(2);
	d_dim *= dimensionless<int>(2);

	EXPECT_EQ((dimensionless<int>(4)), d_dim);

	d_dim *= dimensionless<double>(2.0);

	EXPECT_EQ((dimensionless<int>(8)), d_dim);

	d_dim *= 2;

	EXPECT_EQ((dimensionless<int>(16)), d_dim);

	d_dim *= 2.0;

	EXPECT_EQ((dimensionless<int>(32)), d_dim);

	// concentration
	percent<double> e_pct(2);
	e_pct *= percent<double>(2.0);

	EXPECT_EQ(percent<double>(0.04), e_pct);
	EXPECT_EQ(0.0004, e_pct.value());

	e_pct *= 2.0;

	EXPECT_EQ(percent<double>(0.08), e_pct);
	EXPECT_EQ(0.0008, e_pct.value());

	e_pct *= percent<int>(2);

	EXPECT_EQ(percent<double>(0.0016), e_pct);
	EXPECT_EQ(0.000016, e_pct.value());

	e_pct *= 2;

	EXPECT_EQ(percent<double>(0.0032), e_pct);
	EXPECT_EQ(0.000032, e_pct.value());

	percent<int> f(2);
	f *= percent<int>(200);

	EXPECT_EQ((percent<int>(4)), f);

	f *= percent<double>(200.0);

	EXPECT_EQ((percent<int>(8)), f);

	f *= 2;

	EXPECT_EQ((percent<int>(16)), f);

	f *= 2.0;

	EXPECT_EQ((percent<int>(32)), f);

	f *= dimensionless<int>(2);

	EXPECT_EQ((percent<int>(64)), f);
}

TEST_F(UnitType, compoundAssignmentDivision)
{
	// units
	meters<double> a(8.0);
	a /= dimensionless<double>(2.0);

	EXPECT_EQ(meters<double>(4.0), a);

	a /= 2.0;

	EXPECT_EQ(meters<double>(2.0), a);

	a /= dimensionless<int>(2);

	EXPECT_EQ(meters<double>(1), a);

	a /= 2;

	EXPECT_EQ(meters<double>(0.5), a);

	a /= percent<double>(50);

	EXPECT_EQ(1_m, a);

	meters<int> c(32);
	c /= dimensionless<int>(2);

	EXPECT_EQ((meters<int>(16)), c);

	c /= dimensionless<double>(2.0);

	EXPECT_EQ((meters<int>(8)), c);

	c /= 2;

	EXPECT_EQ((meters<int>(4)), c);

	c /= 2.0;

	EXPECT_EQ((meters<int>(2)), c);

	// dimensionless
	dimensionless<double> b_dim(8);
	b_dim /= dimensionless<double>(2.0);

	EXPECT_EQ(dimensionless<double>(4.0), b_dim);

	b_dim /= 2.0;

	EXPECT_EQ(dimensionless<double>(2.0), b_dim);

	b_dim /= dimensionless<int>(2);

	EXPECT_EQ(dimensionless<double>(1.0), b_dim);

	b_dim /= 2;

	EXPECT_EQ(dimensionless<double>(0.5), b_dim);

	dimensionless<int> d_dim(32);
	d_dim /= dimensionless<int>(2);

	EXPECT_EQ((dimensionless<int>(16)), d_dim);

	d_dim /= dimensionless<double>(2.0);

	EXPECT_EQ((dimensionless<int>(8)), d_dim);

	d_dim /= 2;

	EXPECT_EQ((dimensionless<int>(4)), d_dim);

	d_dim /= 2.0;

	EXPECT_EQ((dimensionless<int>(2)), d_dim);

	// concentration
	percent<double> e_pct(8);
	e_pct /= percent<double>(50.0);

	EXPECT_EQ(percent<double>(16.0), e_pct);
	EXPECT_EQ(0.16, e_pct.value());

	e_pct /= 2.0;

	EXPECT_EQ(percent<double>(8.0), e_pct);
	EXPECT_EQ(0.08, e_pct.value());

	e_pct /= percent<int>(2);

	EXPECT_EQ(percent<double>(400.0), e_pct);
	EXPECT_EQ(4, e_pct.value());

	e_pct /= 2;

	EXPECT_EQ(percent<double>(200.0), e_pct);
	EXPECT_EQ(2, e_pct.value());
}

TEST_F(UnitType, compoundAssignmentModulo)
{
	// units
	meters<int> a_m(2200);

	a_m %= meters<int>(2000);
	EXPECT_EQ(200, a_m.value());

	a_m %= kilometers<int>(1);
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

	a_s %= 5;
	EXPECT_EQ(2, a_s.value());

	// concentration
	percent<int> b_s(12);

	b_s %= percent<int>(20);
	EXPECT_EQ(12_pct, b_s);

	b_s %= percent<int>(5);
	EXPECT_EQ(2_pct, b_s);

	b_s %= 5;
	EXPECT_EQ(0, b_s.value());

	b_s %= dimensionless<int>(5);
	EXPECT_EQ(0, b_s.value());
}

TEST_F(UnitType, dimensionlessTypeImplicitConversion)
{
	double test = dimensionless<double>(3.0);
	EXPECT_DOUBLE_EQ(3.0, test);

	dimensionless<double> testS = 3.0;
	EXPECT_DOUBLE_EQ(3.0, testS);

	dimensionless<double> test3(parts_per_million<double>(10));
	EXPECT_DOUBLE_EQ(0.00001, test3);

	dimensionless<double> test4;
	test4 = parts_per_million<double>(1);
	EXPECT_DOUBLE_EQ(0.000001, test4);

	dimensionless<double> test5;
	test5 = percent<double>(10);
	EXPECT_DOUBLE_EQ(0.1, test5);
}

TEST_F(UnitType, valueMethod)
{
	constexpr auto test = meters(3.0).to<double>();
	EXPECT_DOUBLE_EQ(3.0, test);

	constexpr auto test2 = meters(4.0).value();
	EXPECT_DOUBLE_EQ(4.0, test2);
	static_assert(std::is_same_v<std::decay_t<decltype(test2)>, double>);

	constexpr miles_per_hour speed = 100.0_ft / 1.0_min;
	EXPECT_DOUBLE_EQ(speed.value(), 1.1363636363636365);

	constexpr auto test3 = 5.0_m * (2.0 / 1000.0_mm);
	EXPECT_DOUBLE_EQ(test3.value(), test3.to<double>());

	constexpr auto test4 = (2.0 / 1000.0_mm) / 5_hr;
	EXPECT_DOUBLE_EQ(test4.value(), test4.to<double>());

	constexpr percent test5(85.1);
	EXPECT_DOUBLE_EQ(test5.to<double>(), 0.851);

	constexpr percent test6(1.0_m / 2000.0_mm);
	EXPECT_EQ(test6, 50_pct);
	EXPECT_DOUBLE_EQ(test6.to<double>(), 0.5);

	constexpr percent test7(1000.0_mm / 2.0_m);
	EXPECT_EQ(test7, 50_pct);
	EXPECT_DOUBLE_EQ(test7.to<double>(), 0.5);

	constexpr percent test8 = 1000.0_mm / 2.0_m;
	EXPECT_EQ(test8, 50_pct);
	EXPECT_DOUBLE_EQ(test8.to<double>(), 0.5);
}

TEST_F(UnitType, convertMethod)
{
	constexpr auto test = meters(3.0).convert<feet<double>::conversion_factor>().to<double>();
	EXPECT_NEAR(9.84252, test, 5.0e-6);

	constexpr auto unit2 = meters<double>(3.0).convert<feet>();
	constexpr auto test2 = unit2.to<double>();
	EXPECT_NEAR(9.84252, test2, 5.0e-6);
}

#ifndef UNIT_LIB_DISABLE_IOSTREAM
TEST_F(UnitType, cout)
{
	testing::internal::CaptureStdout();
	std::cout << meters_per_second<double>(5);
	std::string output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("5 mps", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << degrees<double>(349.87);
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("349.87 deg", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << meters<double>(1.0);
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("1 m", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << decibels<double>(31.0);
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("31 dB", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << volts<double>(21.79);
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
	std::cout << pow<4>(meters<double>(2));
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("16 m^4", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << pow<3>(feet<double>(2));
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("8 cu_ft", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << std::setprecision(9) << pow<4>(feet<double>(2));
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("0.138095597 m^4", output.c_str());

	// constants
	testing::internal::CaptureStdout();
	std::cout << std::setprecision(8) << constants::k_B;
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("1.380649e-23 m^2 kg K^-1 s^-2", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << std::setprecision(11) << constants::mu_B;
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("9.2740100783e-24 A m^2", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << std::setprecision(10) << constants::sigma;
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("5.670374419e-08 kg K^-4 s^-3", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << std::setprecision(7) << 51_pct;
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("51 pct", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << std::setprecision(7) << 15_ppb;
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("15 ppb", output.c_str());
}

TEST_F(UnitType, to_string)
{
	feet<double> a_ft(3.5);
	EXPECT_STREQ("3.5 ft", to_string(a_ft).c_str());

	meters<double> b_m(8);
	EXPECT_STREQ("8 m", to_string(b_m).c_str());

	percent<double> c_pct(25.1);
	EXPECT_STREQ("25.1 pct", to_string(c_pct).c_str());
}

TEST_F(UnitType, to_string_locale)
{
	struct lconv*     lc;
	std::string       output;
	std::stringstream os1;
	std::stringstream os2;

	// German locale
#if defined(_MSC_VER)
	setlocale(LC_ALL, "de-DE");
	os1.imbue(std::locale("de-DE"));
#else
	EXPECT_STREQ("de_DE.utf8", setlocale(LC_ALL, "de_DE.utf8")) << "For this test to work, you need a german locale installed: `sudo locale-gen de_DE.UTF-8`";
	os1.imbue(std::locale("de_DE.utf8"));
#endif

	lc            = localeconv();
	char point_de = *lc->decimal_point;
	EXPECT_EQ(point_de, ',');

	kilometers<double> de = 2.0_km;
	EXPECT_STREQ("2 km", to_string(de).c_str());

	de = 2.5_km;
	EXPECT_STREQ("2,5 km", to_string(de).c_str());

	os1 << std::setprecision(11) << constants::mu_B;
	output = os1.str();
	EXPECT_STREQ("9,2740100783e-24 A m^2", output.c_str());

	// US locale
#if defined(_MSC_VER)
	setlocale(LC_ALL, "en-US");
	os2.imbue(std::locale("en-US"));
#else
	EXPECT_STREQ("en_US.utf8", setlocale(LC_ALL, "en_US.utf8")) << "For this test to work, you need a USA locale installed: `sudo locale-gen en_US.UTF-8`";
	os2.imbue(std::locale("en_US.utf8"));
#endif

	lc            = localeconv();
	char point_us = *lc->decimal_point;
	EXPECT_EQ(point_us, '.');

	miles<double> dist = 2.0_mi;
	EXPECT_STREQ("2 mi", to_string(dist).c_str());

	dist = 2.5_mi;
	EXPECT_STREQ("2.5 mi", to_string(dist).c_str());

	os2 << std::setprecision(11) << constants::mu_B;
	output = os2.str();
	EXPECT_STREQ("9.2740100783e-24 A m^2", output.c_str());
}

TEST_F(UnitType, nameAndAbbreviation)
{
	feet a_ft(3.5);
	EXPECT_STREQ("ft", unit_abbreviation_v<decltype(a_ft)>);
	EXPECT_STREQ("ft", a_ft.abbreviation());
	EXPECT_STREQ("feet", a_ft.name());

	meters b_m(8.0);
	EXPECT_STREQ("m", unit_abbreviation_v<decltype(b_m)>);
	EXPECT_STREQ("m", b_m.abbreviation());
	EXPECT_STREQ("meters", b_m.name());
}
#endif

TEST(Consistency, recovers_input_values)
{
	for (int i = 0; i <= 100; ++i)
	{
		EXPECT_DOUBLE_EQ(i, units::concentration::percent<double>(i).value() * 100);
	}
}

TEST(Consistency, percent)
{
	percent<double> a_pct(50);
	percent<double> b_pct = 50;

	EXPECT_DOUBLE_EQ(a_pct, 50_pct);
	EXPECT_DOUBLE_EQ(b_pct, 50_pct);
}

TEST_F(UnitType, identity)
{
	auto            a_pct = percent<double>(50);
	percent<double> b_pct = a_pct;
	percent<double> d_pct = 1.0 * a_pct;
	percent<double> f_pct = a_pct * 1.0;
	EXPECT_EQ(a_pct, percent<double>(50));
	EXPECT_EQ(b_pct, percent<double>(50));
	EXPECT_EQ(d_pct, percent<double>(50));
	EXPECT_EQ(f_pct, percent<double>(50));
	EXPECT_NEAR(0.5, a_pct, 5.0e-10);
	EXPECT_NEAR(0.5, b_pct, 5.0e-10);
	EXPECT_NEAR(0.5, d_pct, 5.0e-10);
	EXPECT_NEAR(0.5, f_pct, 5.0e-10);
	EXPECT_NEAR(0.5, a_pct.value(), 5.0e-10);
	EXPECT_NEAR(0.5, b_pct.value(), 5.0e-10);
	EXPECT_NEAR(0.5, d_pct.value(), 5.0e-10);
	EXPECT_NEAR(0.5, f_pct.value(), 5.0e-10);

	a_pct = percent<double>(50);
	b_pct = a_pct / 1;
	EXPECT_EQ(a_pct, b_pct);
	EXPECT_EQ(b_pct, percent<double>(50));

	a_pct = percent<double>(50);
	a_pct *= 1;
	EXPECT_EQ(a_pct, percent<double>(50));

	a_pct = percent<double>(50);
	a_pct /= 1;
	EXPECT_EQ(a_pct, percent<double>(50));

	meters<double> w(5.3);
	meters<double> x = w;
	meters<double> y = 1 * w;
	meters<double> z = w * 1;
	EXPECT_EQ(w, meters<double>(5.3));
	EXPECT_EQ(x, meters<double>(5.3));
	EXPECT_EQ(y, meters<double>(5.3));
	EXPECT_EQ(z, meters<double>(5.3));
	EXPECT_NEAR(5.3, w.value(), 5.0e-10);
	EXPECT_NEAR(5.3, x.value(), 5.0e-10);
	EXPECT_NEAR(5.3, y.value(), 5.0e-10);
	EXPECT_NEAR(5.3, z.value(), 5.0e-10);

	double v = 1.0 * pi;
	EXPECT_EQ(v, pi.to<double>());
	v = pi * 1.0;
	EXPECT_EQ(v, pi.to<double>());
}

TEST_F(UnitType, negative)
{
	meters<double> a_m(5.3);
	meters<double> b_m(-5.3);
	EXPECT_NEAR(a_m.to<double>(), -b_m.to<double>(), 5.0e-320);
	EXPECT_NEAR(b_m.to<double>(), -a_m.to<double>(), 5.0e-320);

	decibels<double> c_dB(2.87);
	decibels<double> d_dB(-2.87);
	EXPECT_NEAR(c_dB.to<double>(), -d_dB.to<double>(), 5.0e-320);
	EXPECT_NEAR(d_dB.to<double>(), -c_dB.to<double>(), 5.0e-320);

	parts_per_million<double> e_ppm = -1 * parts_per_million<double>(10);
	EXPECT_EQ(e_ppm, -parts_per_million<double>(10));
	EXPECT_NEAR(-0.00001, e_ppm, 5.0e-10);

	percent<double> f = -1 * percent<double>(50);
	EXPECT_EQ(f, -percent<double>(50));
	EXPECT_NEAR(-0.5, f, 5.0e-10);

	percent<double> g_pct = percent<double>(50) * -1;
	EXPECT_EQ(g_pct, -percent<double>(50));
	EXPECT_NEAR(-0.5, g_pct, 5.0e-10);
}

TEST_F(UnitType, concentration)
{
	parts_per_billion<double> a_ppb(parts_per_million<double>(1));
	EXPECT_EQ(parts_per_billion<double>(1000), a_ppb);
	EXPECT_EQ(0.000001, a_ppb);
	EXPECT_EQ(0.000001, a_ppb.to<double>());

	dimensionless<double> b_ppm(parts_per_million<double>(1));
	EXPECT_EQ(0.000001, b_ppm);

	dimensionless<double> c_dim = parts_per_billion<double>(1);
	EXPECT_EQ(0.000000001, c_dim);

	static_assert(traits::is_same_dimension_unit_v<percent<double>, dimensionless<double>>);
}

TEST_F(UnitType, dBConversion)
{
	dBW<double>   a_dbw(23.1);
	watts<double> a_w   = a_dbw;
	dBm<double>   a_dbm = a_dbw;

	EXPECT_NEAR(204.173794, a_w.value(), 5.0e-7);
	EXPECT_NEAR(53.1, a_dbm.value(), 5.0e-7);

	milliwatts<double> b_mw(100000.0);
	watts<double>      b_w   = b_mw;
	dBm<double>        b_dbm = b_mw;
	dBW<double>        b_dbw = b_mw;

	EXPECT_NEAR(100.0, b_w.value(), 5.0e-7);
	EXPECT_NEAR(50.0, b_dbm.value(), 5.0e-7);
	EXPECT_NEAR(20.0, b_dbw.value(), 5.0e-7);
}

TEST_F(UnitType, dBAddition)
{
	bool isSame;

	auto result_dbw = dBW<double>(10.0) + decibels<double>(30.0);
	EXPECT_NEAR(40.0, result_dbw.value(), 5.0e-5);
	result_dbw = dBW<int>(10) + decibels<int>(30);
	EXPECT_NEAR(40.0, result_dbw.value(), 5.0e-5);
	result_dbw = decibels<double>(12.0) + dBW<double>(30.0);
	EXPECT_NEAR(42.0, result_dbw.value(), 5.0e-5);
	result_dbw = decibels<int>(12) + dBW<int>(30);
	EXPECT_NEAR(42.0, result_dbw.value(), 2);
	isSame = std::is_same_v<decltype(result_dbw), dBW<double>>;
	EXPECT_TRUE(isSame);

	auto result_dbm = decibels<double>(30.0) + dBm<double>(20.0);
	EXPECT_NEAR(50.0, result_dbm.value(), 5.0e-5);
	result_dbm = decibels<int>(30) + dBm<int>(20);
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

	auto result_dbw = dBW<double>(10.0) - decibels<double>(30.0);
	EXPECT_NEAR(-20.0, result_dbw.value(), 5.0e-5);
	isSame = std::is_same_v<decltype(result_dbw), dBW<double>>;
	EXPECT_TRUE(isSame);

	auto result_dbm = dBm<double>(100.0) - decibels<double>(30.0);
	EXPECT_NEAR(70.0, result_dbm.value(), 5.0e-5);
	isSame = std::is_same_v<decltype(result_dbm), dBm<double>>;
	EXPECT_TRUE(isSame);

	auto result_db = dBW<double>(100.0) - dBW<double>(80.0);
	EXPECT_NEAR(20.0, result_db.value(), 5.0e-5);
	isSame = std::is_same_v<decltype(result_db), decibels<double>>;
	EXPECT_TRUE(isSame);

	result_db = decibels<double>(100.0) - decibels<double>(80.0);
	EXPECT_NEAR(20.0, result_db.value(), 5.0e-5);
	isSame = std::is_same_v<decltype(result_db), decibels<double>>;
	EXPECT_TRUE(isSame);
}

TEST_F(UnitType, unit_cast)
{
	meters<double>   test1(5.7);
	hectares<double> test2(16);

	double dResult1 = 5.7;

	double dResult2 = 16;
	int    iResult2 = 16;

	EXPECT_EQ(dResult1, unit_cast<double>(test1));
	EXPECT_EQ(dResult2, unit_cast<double>(test2));
	EXPECT_EQ(iResult2, unit_cast<int>(test2));

	static_assert(std::is_same_v<double, decltype(unit_cast<double>(test1))>);
	static_assert(std::is_same_v<int, decltype(unit_cast<int>(test2))>);
}

// literal syntax is only supported in GCC 4.7+ and MSVC2015+
TEST_F(UnitType, literals)
{
	// basic functionality testing
	static_assert(std::is_same_v<decltype(16.2_m), meters<double>>);
	static_assert(std::is_same_v<decltype(16_m), meters<int>>);
	EXPECT_TRUE(meters<double>(16.2) == 16.2_m);
	EXPECT_TRUE(meters<double>(16) == 16.0_m);
	EXPECT_TRUE(meters<int>(16) == 16_m);

	static_assert(std::is_same_v<decltype(11.2_ft), feet<double>>);
	static_assert(std::is_same_v<decltype(11_ft), feet<int>>);
	EXPECT_TRUE(feet<double>(11.2) == 11.2_ft);
	EXPECT_TRUE(feet<double>(11) == 11.0_ft);
	EXPECT_TRUE(feet<int>(11) == 11_ft);

	// auto using literal syntax
	auto x = 10.0_m;
	static_assert(std::is_same_v<decltype(x), meters<double>>);
	EXPECT_TRUE(meters<double>(10) == x);

	// conversion using literal syntax
	feet<double> y = 0.3048_m;
	EXPECT_TRUE(1.0_ft == y);

	// Pythagorean theorem
	meters<double> a_m = 3.0_m;
	meters<double> b_m = 4.0_m;
	meters<double> c_m = sqrt(pow<2>(a_m) + pow<2>(b_m));
	EXPECT_TRUE(c_m == 5.0_m);
}

TEST_F(UnitType, Constants)
{
	// simple numeric operations
	static_assert(10 * km / 2 == 5 * km);

	// conversions to common units
	static_assert(1 * hr == 3600 * s);
	static_assert(1 * km + 1 * m == 1001 * m);

	// derived quantities
	static_assert(1 * km / (1 * s) == 1000 * m / s);
	static_assert(2 * km / hr * (2 * hr) == 4 * km);
	static_assert(2 * km / (2 * km / hr) == 1 * hr);

	static_assert(2 * m * (3 * m) == 6 * sq_m);

	static_assert(10 * km / (5 * km) == 2);

	static_assert(1000 / (1 * s) == 1 * kHz);
}

TEST_F(ConversionFactor, length)
{
	double test;
	test = nanometers<double>(0.000000001_m).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = micrometers<double>(meters<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = millimeters<double>(meters<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = centimeters<double>(meters<double>(0.01)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = kilometers<double>(meters<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = meters<double>(meters<double>(1.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = feet<double>(meters<double>(0.3048)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = miles<double>(meters<double>(1609.344)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = inches<double>(meters<double>(0.0254)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = nautical_miles<double>(meters<double>(1852.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = astronomical_units<double>(meters<double>(149597870700.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = lightyears<double>(meters<double>(9460730472580800.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = parsecs<double>(meters<double>(3.08567758e16)).value();
	EXPECT_NEAR(1.0, test, 5.0e7);

	test = feet<double>(feet<double>(6.3)).value();
	EXPECT_NEAR(6.3, test, 5.0e-5);
	test = inches<double>(feet<double>(6.0)).value();
	EXPECT_NEAR(72.0, test, 5.0e-5);
	test = feet<double>(inches<double>(6.0)).value();
	EXPECT_NEAR(0.5, test, 5.0e-5);
	test = feet<double>(meters<double>(1.0)).value();
	EXPECT_NEAR(3.28084, test, 5.0e-5);
	test = nautical_miles<double>(miles<double>(6.3)).value();
	EXPECT_NEAR(5.47455, test, 5.0e-6);
	test = meters<double>(miles<double>(11.0)).value();
	EXPECT_NEAR(17702.8, test, 5.0e-2);
	test = chains<double>(meters<double>(1.0)).value();
	EXPECT_NEAR(0.0497097, test, 5.0e-7);
	test = inches<double>(mils<double>(1.0)).value();
	EXPECT_NEAR(0.001, test, 5.0e-7);
	test = mils<double>(inches<double>(1.0)).value();
	EXPECT_NEAR(1000, test, 5.0e-7);

	EXPECT_EQ(metres<double>(1), meters<double>(1));

	EXPECT_EQ(1_m, 1 * m);
	EXPECT_EQ(2_ft, 2 * ft);
}

TEST_F(ConversionFactor, mass)
{
	double test;

	test = grams<double>(kilograms<double>(1.0e-3)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = micrograms<double>(kilograms<double>(1.0e-9)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = milligrams<double>(kilograms<double>(1.0e-6)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = kilograms<double>(kilograms<double>(1.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = tonnes<double>(kilograms<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = mass::pounds<double>(kilograms<double>(0.453592)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = long_tons<double>(kilograms<double>(1016.05)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = short_tons<double>(kilograms<double>(907.185)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = mass::ounces<double>(kilograms<double>(0.0283495)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = carats<double>(kilograms<double>(0.0002)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = kilograms<double>(slugs<double>(1.0)).value();
	EXPECT_NEAR(14.593903, test, 5.0e-7);

	test = carats<double>(mass::pounds<double>(6.3)).value();
	EXPECT_NEAR(14288.2, test, 5.0e-2);
}

TEST_F(ConversionFactor, time)
{
	double result = 0;

	constexpr double daysPerYear = 365;
	constexpr double hoursPerDay = 24;
	constexpr double minsPerHour = 60;
	constexpr double secsPerMin  = 60;
	constexpr double daysPerWeek = 7;

	result = 2 * daysPerYear * hoursPerDay * minsPerHour * secsPerMin * (1 / minsPerHour) * (1 / secsPerMin) * (1 / hoursPerDay) * (1 / daysPerWeek);
	EXPECT_NEAR(104.286, result, 5.0e-4);

	years<double> twoYears(2.0);
	weeks<double> twoYearsInWeeks = twoYears;
	EXPECT_NEAR(weeks<double>(104.286).to<double>(), twoYearsInWeeks.to<double>(), 5.0e-4);

	double test;

	test = seconds<double>(seconds<double>(1.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = nanoseconds<double>(seconds<double>(1.0e-9)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = microseconds<double>(seconds<double>(1.0e-6)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = milliseconds<double>(seconds<double>(1.0e-3)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = minutes<double>(seconds<double>(60.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = hours<double>(seconds<double>(3600.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = days<double>(seconds<double>(86400.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = weeks<double>(seconds<double>(604800.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = years<double>(seconds<double>(3.154e7)).value();
	EXPECT_NEAR(1.0, test, 5.0e3);

	test = weeks<double>(years<double>(2.0)).value();
	EXPECT_NEAR(104.2857142857143, test, 5.0e-14);
	test = minutes<double>(hours<double>(4.0)).value();
	EXPECT_NEAR(240.0, test, 5.0e-14);
	test = days<double>(julian_years<double>(1.0)).value();
	EXPECT_NEAR(365.25, test, 5.0e-14);
	test = days<double>(gregorian_years<double>(1.0)).value();
	EXPECT_NEAR(365.2425, test, 5.0e-14);
}

TEST_F(ConversionFactor, angle)
{
	angle::degrees<double> quarterCircleDeg(90.0);
	angle::radians<double> quarterCircleRad = quarterCircleDeg;
	EXPECT_NEAR(angle::radians<double>(detail::PI_VAL / 2.0).to<double>(), quarterCircleRad.to<double>(), 5.0e-12);

	double test;

	test = angle::radians<double>(angle::radians<double>(1.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = angle::milliradians<double>(angle::radians<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-4);
	test = angle::degrees<double>(angle::radians<double>(0.0174533)).value();
	EXPECT_NEAR(1.0, test, 5.0e-7);
	test = angle::arcminutes<double>(angle::radians<double>(0.000290888)).value();
	EXPECT_NEAR(0.99999928265913, test, 5.0e-8);
	test = angle::arcseconds<double>(angle::radians<double>(4.8481e-6)).value();
	EXPECT_NEAR(0.999992407, test, 5.0e-10);
	test = angle::turns<double>(angle::radians<double>(6.28319)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = angle::gradians<double>(angle::radians<double>(0.015708)).value();
	EXPECT_NEAR(1.0, test, 5.0e-6);

	test = angle::radians<double>(angle::radians<double>(2.1)).value();
	EXPECT_NEAR(2.1, test, 5.0e-6);
	test = angle::gradians<double>(angle::arcseconds<double>(2.1)).value();
	EXPECT_NEAR(0.000648148, test, 5.0e-6);
	test = angle::degrees<double>(angle::radians<double>(detail::PI_VAL)).value();
	EXPECT_NEAR(180.0, test, 5.0e-6);
	test = angle::radians<double>(angle::degrees<double>(90.0)).value();
	EXPECT_NEAR(detail::PI_VAL / 2, test, 5.0e-6);
}

TEST_F(ConversionFactor, current)
{
	double test;

	test = current::milliamperes<double>(current::amperes<double>(2.1)).value();
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

	test = millicandelas<double>(candelas<double>(72.0)).value();
	EXPECT_NEAR(72000.0, test, 5.0e-5);
	test = candelas<double>(millicandelas<double>(376.0)).value();
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
	bool   same;

	same = std::is_same_v<traits::dimension_of_t<steradians<double>>, traits::dimension_of_t<degrees_squared<double>>>;
	EXPECT_TRUE(same);

	test = steradians<double>(steradians<double>(72.0)).value();
	EXPECT_NEAR(72.0, test, 5.0e-5);
	test = degrees_squared<double>(steradians<double>(1.0)).value();
	EXPECT_NEAR(3282.8, test, 5.0e-2);
	test = spats<double>(steradians<double>(8.0)).value();
	EXPECT_NEAR(0.636619772367582, test, 5.0e-14);
	test = steradians<double>(degrees_squared<double>(3282.8)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = degrees_squared<double>(degrees_squared<double>(72.0)).value();
	EXPECT_NEAR(72.0, test, 5.0e-5);
	test = spats<double>(degrees_squared<double>(3282.8)).value();
	EXPECT_NEAR(1.0 / (4 * detail::PI_VAL), test, 5.0e-5);
	test = steradians<double>(spats<double>(1.0 / (4 * detail::PI_VAL))).value();
	EXPECT_NEAR(1.0, test, 5.0e-14);
	test = degrees_squared<double>(spats<double>(1.0 / (4 * detail::PI_VAL))).value();
	EXPECT_NEAR(3282.8, test, 5.0e-2);
	test = spats<double>(spats<double>(72.0)).value();
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
	bool   same;

	same = std::is_same_v<meters_per_second<double>::conversion_factor, traits::strong_t<conversion_factor<std::ratio<1>, dimension::velocity>>>;
	EXPECT_TRUE(same);

	same = traits::is_same_dimension_unit_v<miles_per_hour<double>, meters_per_second<double>>;
	EXPECT_TRUE(same);

	test = miles_per_hour<double>(meters_per_second<double>(1250.0)).value();
	EXPECT_NEAR(2796.17, test, 5.0e-3);
	test = kilometers_per_hour<double>(feet_per_second<double>(2796.17)).value();
	EXPECT_NEAR(3068.181418, test, 5.0e-7);
	test = miles_per_hour<double>(knots<double>(600.0)).value();
	EXPECT_NEAR(690.468, test, 5.0e-4);
	test = feet_per_second<double>(miles_per_hour<double>(120.0)).value();
	EXPECT_NEAR(176.0, test, 5.0e-5);
	test = meters_per_second<double>(feet_per_second<double>(10.0)).value();
	EXPECT_NEAR(3.048, test, 5.0e-5);

	EXPECT_EQ(10_mps, 10 * m / s);
}

TEST_F(ConversionFactor, angular_velocity)
{
	double test;
	bool   same;

	same = std::is_same_v<radians_per_second<double>::conversion_factor, traits::strong_t<conversion_factor<std::ratio<1>, dimension::angular_velocity>>>;
	EXPECT_TRUE(same);

	same = traits::is_same_dimension_conversion_factor_v<revolutions_per_minute<double>, radians_per_second<double>>;
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
	test = standard_gravity<double>(feet_per_second_squared<double>(1.0)).value();
	EXPECT_NEAR(0.0310810, test, 5.0e-6);
	test = feet_per_second_squared<double>(gals<double>(1.0)).value();
	EXPECT_NEAR(0.0328084, test, 5.0e-6);
}

TEST_F(ConversionFactor, force)
{
	double test;

	test = units::force::newtons<double>(units::force::newtons<double>(1.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = units::force::pounds<double>(units::force::newtons<double>(6.3)).value();
	EXPECT_NEAR(1.4163, test, 5.0e-5);
	test = units::force::dynes<double>(units::force::newtons<double>(5.0)).value();
	EXPECT_NEAR(500000.0, test, 5.0e-5);
	test = units::force::poundals<double>(units::force::newtons<double>(2.1)).value();
	EXPECT_NEAR(15.1893, test, 5.0e-5);
	test = units::force::kiloponds<double>(units::force::newtons<double>(173.0)).value();
	EXPECT_NEAR(17.6411, test, 5.0e-5);
	test = units::force::kiloponds<double>(units::force::poundals<double>(21.879)).value();
	EXPECT_NEAR(0.308451933, test, 5.0e-10);
}

TEST_F(ConversionFactor, area)
{
	double test;

	test = acres<double>(hectares<double>(6.3)).value();
	EXPECT_NEAR(15.5676, test, 5.0e-5);
	test = square_kilometers<double>(square_miles<double>(10.0)).value();
	EXPECT_NEAR(25.8999, test, 5.0e-5);
	test = square_meters<double>(square_inches<double>(4.0)).value();
	EXPECT_NEAR(0.00258064, test, 5.0e-9);
	test = square_feet<double>(acres<double>(5.0)).value();
	EXPECT_NEAR(217800.0, test, 5.0e-5);
	test = square_feet<double>(square_meters<double>(1.0)).value();
	EXPECT_NEAR(10.7639, test, 5.0e-5);
}

TEST_F(ConversionFactor, pressure)
{
	double test;

	test = torrs<double>(pascals<double>(1.0)).value();
	EXPECT_NEAR(0.00750062, test, 5.0e-5);
	test = pounds_per_square_inch<double>(bars<double>(2.2)).value();
	EXPECT_NEAR(31.9083, test, 5.0e-5);
	test = bars<double>(atmospheres<double>(4.0)).value();
	EXPECT_NEAR(4.053, test, 5.0e-5);
	test = pascals<double>(torrs<double>(800.0)).value();
	EXPECT_NEAR(106657.89474, test, 5.0e-5);
	test = atmospheres<double>(pounds_per_square_inch<double>(38.0)).value();
	EXPECT_NEAR(2.58575, test, 5.0e-5);
	test = pascals<double>(pounds_per_square_inch<double>(1.0)).value();
	EXPECT_NEAR(6894.76, test, 5.0e-3);
	test = bars<double>(pascals<double>(0.25)).value();
	EXPECT_NEAR(2.5e-6, test, 5.0e-5);
	test = atmospheres<double>(torrs<double>(9.0)).value();
	EXPECT_NEAR(0.0118421, test, 5.0e-8);
	test = torrs<double>(bars<double>(12.0)).value();
	EXPECT_NEAR(9000.74, test, 5.0e-3);
	test = pounds_per_square_inch<double>(atmospheres<double>(1.0)).value();
	EXPECT_NEAR(14.6959, test, 5.0e-5);
	test = millimeters_of_mercury(inches_of_mercury{1.0}).value();
	EXPECT_NEAR(25.4, test, 5.0e-5);
	test = pascals(inches_of_mercury{1.0}).value();
	EXPECT_NEAR(3386.38864, test, 5.0e-5);

	EXPECT_EQ(133.322387415_Pa, 1.0_mmHg);
}

TEST_F(ConversionFactor, charge)
{
	double test;

	test = ampere_hours<double>(coulombs<double>(4.0)).value();
	EXPECT_NEAR(0.00111111, test, 5.0e-9);
	test = coulombs<double>(ampere_hours<double>(1.0)).value();
	EXPECT_NEAR(3600.0, test, 5.0e-6);
}

TEST_F(ConversionFactor, energy)
{
	double test;

	test = calories<double>(joules<double>(8000.000464)).value();
	EXPECT_NEAR(1912.046, test, 5.0e-4);
	test = joules<double>(therms<double>(12.0)).value();
	EXPECT_NEAR(1.266e+9, test, 5.0e5);
	test = watt_hours<double>(megajoules<double>(100.0)).value();
	EXPECT_NEAR(27777.778, test, 5.0e-4);
	test = megajoules<double>(kilocalories<double>(56.0)).value();
	EXPECT_NEAR(0.234304, test, 5.0e-7);
	test = therms<double>(kilojoules<double>(56.0)).value();
	EXPECT_NEAR(0.000530904, test, 5.0e-5);
	test = kilojoules<double>(british_thermal_units<double>(18.56399995447)).value();
	EXPECT_NEAR(19.5860568, test, 5.0e-5);
	test = energy::foot_pounds<double>(calories<double>(18.56399995447)).value();
	EXPECT_NEAR(57.28776190423856, test, 5.0e-5);
	test = calories<double>(megajoules<double>(1.0)).value();
	EXPECT_NEAR(239006.0, test, 5.0e-1);
	test = kilowatt_hours<double>(kilocalories<double>(2.0)).value();
	EXPECT_NEAR(0.00232444, test, 5.0e-9);
	test = kilocalories<double>(therms<double>(0.1)).value();
	EXPECT_NEAR(2521.04, test, 5.0e-3);
	test = megajoules<double>(watt_hours<double>(67.0)).value();
	EXPECT_NEAR(0.2412, test, 5.0e-5);
	test = watt_hours<double>(british_thermal_units<double>(100.0)).value();
	EXPECT_NEAR(29.3071, test, 5.0e-5);
	test = british_thermal_units<double>(calories<double>(100.0)).value();
	EXPECT_NEAR(0.396567, test, 5.0e-5);
}

TEST_F(ConversionFactor, power)
{
	double test;

	test = watts<double>(unit<compound_conversion_factor<energy::foot_pounds<double>, inverse<seconds<double>>>>(550.0)).value();
	EXPECT_NEAR(745.7, test, 5.0e-2);
	test = gigawatts<double>(watts<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-4);
	test = watts<double>(microwatts<double>(200000.0)).value();
	EXPECT_NEAR(0.2, test, 5.0e-4);
	test = watts<double>(horsepower<double>(100.0)).value();
	EXPECT_NEAR(74570.0, test, 5.0e-1);
	test = megawatts<double>(horsepower<double>(5.0)).value();
	EXPECT_NEAR(0.0037284994, test, 5.0e-7);
	test = horsepower<double>(kilowatts<double>(232.0)).value();
	EXPECT_NEAR(311.117, test, 5.0e-4);
	test = horsepower<double>(milliwatts<double>(1001.0)).value();
	EXPECT_NEAR(0.001342363, test, 5.0e-9);
}

TEST_F(ConversionFactor, voltage)
{
	double test;

	test = millivolts<double>(volts<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = volts<double>(picovolts<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = volts<double>(nanovolts<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = volts<double>(microvolts<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = volts<double>(millivolts<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = volts<double>(kilovolts<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = volts<double>(megavolts<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = volts<double>(gigavolts<double>(0.000000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = volts<double>(statvolts<double>(299.792458)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = statvolts<double>(millivolts<double>(1000.0)).value();
	EXPECT_NEAR(299.792458, test, 5.0e-5);
	test = nanovolts<double>(abvolts<double>(0.1)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = abvolts<double>(microvolts<double>(0.01)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, capacitance)
{
	double test;

	test = millifarads<double>(farads<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = farads<double>(picofarads<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = farads<double>(nanofarads<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = farads<double>(microfarads<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = farads<double>(millifarads<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = farads<double>(kilofarads<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = farads<double>(megafarads<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = farads<double>(gigafarads<double>(0.000000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);

	auto                            f  = coulombs<double>(1) / volts<double>(1);
	[[maybe_unused]] farads<double> f2 = coulombs<double>(1) / volts<double>(1);
	static_assert(std::is_convertible_v<decltype(f), farads<double>>);

	auto one_farad = []() -> farads<double> { return coulombs<double>(1) / volts<double>(1); };

	EXPECT_EQ(1.0_F, one_farad());
}

TEST_F(ConversionFactor, impedance)
{
	double test;

	test = milliohms<double>(ohms<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = ohms<double>(picoohms<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = ohms<double>(nanoohms<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = ohms<double>(microohms<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = ohms<double>(milliohms<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = ohms<double>(kiloohms<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = ohms<double>(megaohms<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = ohms<double>(gigaohms<double>(0.000000001)).value();
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

	test = milliwebers<double>(webers<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = webers<double>(picowebers<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = webers<double>(nanowebers<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = webers<double>(microwebers<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = webers<double>(milliwebers<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = webers<double>(kilowebers<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = webers<double>(megawebers<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = webers<double>(gigawebers<double>(0.000000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = webers<double>(maxwells<double>(100000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = maxwells<double>(nanowebers<double>(10.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, magnetic_field_strength)
{
	double test;

	test = milliteslas<double>(teslas<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = teslas<double>(picoteslas<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = teslas<double>(nanoteslas<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = teslas<double>(microteslas<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = teslas<double>(milliteslas<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = teslas<double>(kiloteslas<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = teslas<double>(megateslas<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = teslas<double>(gigateslas<double>(0.000000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = teslas<double>(gauss<double>(10000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = gauss<double>(nanoteslas<double>(100000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, inductance)
{
	double test;

	test = millihenries<double>(henries<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = henries<double>(picohenries<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = henries<double>(nanohenries<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = henries<double>(microhenries<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = henries<double>(millihenries<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = henries<double>(kilohenries<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = henries<double>(megahenries<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = henries<double>(gigahenries<double>(0.000000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, jerk)
{
	double test;
	test = meters_per_second_cubed(feet_per_second_cubed{3.280839895}).value();
	EXPECT_NEAR(test, 1.0, 5.0e-5);
}

TEST_F(ConversionFactor, luminous_flux)
{
	double test;

	test = millilumens<double>(lumens<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = lumens<double>(picolumens<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lumens<double>(nanolumens<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lumens<double>(microlumens<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lumens<double>(millilumens<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lumens<double>(kilolumens<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lumens<double>(megalumens<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lumens<double>(gigalumens<double>(0.000000001)).value();
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

	test = lumens_per_square_inch<double>(lux<double>(1550.0031000062)).value();
	EXPECT_NEAR(1.0, test, 5.0e-13);
	test = lux<double>(phots<double>(0.0001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, luminance)
{
	candelas_per_square_meter<double> test;

	test = stilbs<double>(1.0);
	EXPECT_DOUBLE_EQ(test.value(), pow<4>(10.0));
	test = apostilbs<double>(2.0);
	EXPECT_DOUBLE_EQ(test.value(), 0.63661977236758138);
	test = brils<double>(1.0);
	EXPECT_DOUBLE_EQ(test.value(), 3.1830988618379068e-08);
	test = skots<double>(1.0);
	EXPECT_DOUBLE_EQ(test.value(), 0.0003183098861837907);
	test = lamberts<double>(1.0);
	EXPECT_DOUBLE_EQ(test.value(), 3183.098861837907);
	test = foot_lamberts<double>(1.0);
	EXPECT_DOUBLE_EQ(test.value(), 3.4262590996353905);

	brils<double> test2 = foot_lamberts<double>(89.46);
	EXPECT_DOUBLE_EQ(test2.value(), 9629394258.788517);

	millilamberts<double> test3 = blondels<double>(89.46);
	EXPECT_DOUBLE_EQ(test3.value(), 8.946);
}

TEST_F(ConversionFactor, radiation)
{
	double test;

	test = millibecquerels<double>(becquerels<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = becquerels<double>(picobecquerels<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = becquerels<double>(nanobecquerels<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = becquerels<double>(microbecquerels<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = becquerels<double>(millibecquerels<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = becquerels<double>(kilobecquerels<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = becquerels<double>(megabecquerels<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = becquerels<double>(gigabecquerels<double>(0.000000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);

	test = milligrays<double>(grays<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = grays<double>(picograys<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = grays<double>(nanograys<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = grays<double>(micrograys<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = grays<double>(milligrays<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = grays<double>(kilograys<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = grays<double>(megagrays<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = grays<double>(gigagrays<double>(0.000000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);

	test = millisieverts<double>(sieverts<double>(10.0)).value();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = sieverts<double>(picosieverts<double>(1000000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = sieverts<double>(nanosieverts<double>(1000000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = sieverts<double>(microsieverts<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = sieverts<double>(millisieverts<double>(1000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = sieverts<double>(kilosieverts<double>(0.001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = sieverts<double>(megasieverts<double>(0.000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = sieverts<double>(gigasieverts<double>(0.000000001)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);

	test = curies<double>(becquerels<double>(37.0e9)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = rutherfords<double>(becquerels<double>(1000000.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = grays<double>(radiation_absorbed_dose<double>(100.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, torque)
{
	double test;

	test = newton_meters<double>(torque::foot_pounds<double>(1.0)).value();
	EXPECT_NEAR(1.355817948, test, 5.0e-5);
	test = newton_meters<double>(inch_pounds<double>(1.0)).value();
	EXPECT_NEAR(0.112984829, test, 5.0e-5);
	test = newton_meters<double>(foot_poundals<double>(1.0)).value();
	EXPECT_NEAR(4.214011009e-2, test, 5.0e-5);
	test = newton_meters<double>(meter_kilograms<double>(1.0)).value();
	EXPECT_NEAR(9.80665, test, 5.0e-5);
	test = meter_kilograms<double>(inch_pounds<double>(86.79616930855788)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = inch_pounds<double>(foot_poundals<double>(2.681170713)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, volume)
{
	double test;

	test = cubic_meters<double>(cubic_meters<double>(1.0)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = cubic_meters<double>(cubic_millimeters<double>(1.0)).value();
	EXPECT_NEAR(1.0e-9, test, 5.0e-5);
	test = cubic_meters<double>(cubic_kilometers<double>(1.0)).value();
	EXPECT_NEAR(1.0e9, test, 5.0e-5);
	test = cubic_meters<double>(liters<double>(1.0)).value();
	EXPECT_NEAR(0.001, test, 5.0e-5);
	test = cubic_meters<double>(milliliters<double>(1.0)).value();
	EXPECT_NEAR(1.0e-6, test, 5.0e-5);
	test = cubic_meters<double>(cubic_inches<double>(1.0)).value();
	EXPECT_NEAR(1.6387e-5, test, 5.0e-10);
	test = cubic_meters<double>(cubic_feet<double>(1.0)).value();
	EXPECT_NEAR(0.0283168, test, 5.0e-8);
	test = cubic_meters<double>(cubic_yards<double>(1.0)).value();
	EXPECT_NEAR(0.764555, test, 5.0e-7);
	test = cubic_meters<double>(cubic_miles<double>(1.0)).value();
	EXPECT_NEAR(4.168e+9, test, 5.0e5);
	test = cubic_meters<double>(gallons<double>(1.0)).value();
	EXPECT_NEAR(0.00378541, test, 5.0e-8);
	test = cubic_meters<double>(quarts<double>(1.0)).value();
	EXPECT_NEAR(0.000946353, test, 5.0e-10);
	test = cubic_meters<double>(pints<double>(1.0)).value();
	EXPECT_NEAR(0.000473176, test, 5.0e-10);
	test = cubic_meters<double>(cups<double>(1.0)).value();
	EXPECT_NEAR(0.00024, test, 5.0e-6);
	test = cubic_meters<double>(volume::fluid_ounces<double>(1.0)).value();
	EXPECT_NEAR(2.9574e-5, test, 5.0e-5);
	test = cubic_meters<double>(barrels<double>(1.0)).value();
	EXPECT_NEAR(0.158987294928, test, 5.0e-13);
	test = cubic_meters<double>(bushels<double>(1.0)).value();
	EXPECT_NEAR(0.0352391, test, 5.0e-8);
	test = cubic_meters<double>(cords<double>(1.0)).value();
	EXPECT_NEAR(3.62456, test, 5.0e-6);
	test = cubic_meters<double>(cubic_fathoms<double>(1.0)).value();
	EXPECT_NEAR(6.11644, test, 5.0e-6);
	test = cubic_meters<double>(tablespoons<double>(1.0)).value();
	EXPECT_NEAR(1.4787e-5, test, 5.0e-10);
	test = cubic_meters<double>(teaspoons<double>(1.0)).value();
	EXPECT_NEAR(4.9289e-6, test, 5.0e-11);
	test = cubic_meters<double>(pinches<double>(1.0)).value();
	EXPECT_NEAR(616.11519921875e-9, test, 5.0e-20);
	test = cubic_meters<double>(dashes<double>(1.0)).value();
	EXPECT_NEAR(308.057599609375e-9, test, 5.0e-20);
	test = cubic_meters<double>(drops<double>(1.0)).value();
	EXPECT_NEAR(82.14869322916e-9, test, 5.0e-9);
	test = cubic_meters<double>(fifths<double>(1.0)).value();
	EXPECT_NEAR(0.00075708236, test, 5.0e-12);
	test = cubic_meters<double>(drams<double>(1.0)).value();
	EXPECT_NEAR(3.69669e-6, test, 5.0e-12);
	test = cubic_meters<double>(gills<double>(1.0)).value();
	EXPECT_NEAR(0.000118294, test, 5.0e-10);
	test = cubic_meters<double>(pecks<double>(1.0)).value();
	EXPECT_NEAR(0.00880977, test, 5.0e-9);
	test = cubic_meters<double>(sacks<double>(9.4591978)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = cubic_meters<double>(shots<double>(1.0)).value();
	EXPECT_NEAR(4.43603e-5, test, 5.0e-11);
	test = cubic_meters<double>(strikes<double>(1.0)).value();
	EXPECT_NEAR(0.07047814033376, test, 5.0e-5);
	test = milliliters<double>(volume::fluid_ounces<double>(1.0)).value();
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

	test = parts_per_million<double>(1.0);
	EXPECT_NEAR(1.0e-6, test, 5.0e-12);
	test = parts_per_billion<double>(1.0);
	EXPECT_NEAR(1.0e-9, test, 5.0e-12);
	test = parts_per_trillion<double>(1.0);
	EXPECT_NEAR(1.0e-12, test, 5.0e-12);
	test = percent<double>(18.0);
	EXPECT_NEAR(0.18, test, 5.0e-12);

	auto test2 = percent(18.0);
	EXPECT_DOUBLE_EQ(test2.to<double>(), 0.18);
	EXPECT_DOUBLE_EQ(test2.value(), 0.18);
	test2 = 0.5;
	EXPECT_EQ(test2, 50_pct);
	EXPECT_DOUBLE_EQ(test2.value(), 0.5);
	EXPECT_DOUBLE_EQ(test2.to<double>(), 0.5);
}

TEST_F(ConversionFactor, data)
{
	EXPECT_EQ(8, (bits<double>(bytes<double>(1)).value()));

	EXPECT_EQ(1000, (bytes<double>(kilobytes<double>(1)).value()));
	EXPECT_EQ(1000, (kilobytes<double>(megabytes<double>(1)).value()));
	EXPECT_EQ(1000, (megabytes<double>(gigabytes<double>(1)).value()));
	EXPECT_EQ(1000, (gigabytes<double>(terabytes<double>(1)).value()));
	EXPECT_EQ(1000, (terabytes<double>(petabytes<double>(1)).value()));
	EXPECT_EQ(1000, (petabytes<double>(exabytes<double>(1)).value()));

	EXPECT_EQ(1024, (bytes<double>(kibibytes<double>(1)).value()));
	EXPECT_EQ(1024, (kibibytes<double>(mebibytes<double>(1)).value()));
	EXPECT_EQ(1024, (mebibytes<double>(gibibytes<double>(1)).value()));
	EXPECT_EQ(1024, (gibibytes<double>(tebibytes<double>(1)).value()));
	EXPECT_EQ(1024, (tebibytes<double>(pebibytes<double>(1)).value()));
	EXPECT_EQ(1024, (pebibytes<double>(exbibytes<double>(1)).value()));

	EXPECT_EQ(93750000, (kibibits<double>(gigabytes<double>(12)).value()));

	EXPECT_EQ(1000, (bits<double>(kilobits<double>(1)).value()));
	EXPECT_EQ(1000, (kilobits<double>(megabits<double>(1)).value()));
	EXPECT_EQ(1000, (megabits<double>(gigabits<double>(1)).value()));
	EXPECT_EQ(1000, (gigabits<double>(terabits<double>(1)).value()));
	EXPECT_EQ(1000, (terabits<double>(petabits<double>(1)).value()));
	EXPECT_EQ(1000, (petabits<double>(exabits<double>(1)).value()));

	EXPECT_EQ(1024, (bits<double>(kibibits<double>(1)).value()));
	EXPECT_EQ(1024, (kibibits<double>(mebibits<double>(1)).value()));
	EXPECT_EQ(1024, (mebibits<double>(gibibits<double>(1)).value()));
	EXPECT_EQ(1024, (gibibits<double>(tebibits<double>(1)).value()));
	EXPECT_EQ(1024, (tebibits<double>(pebibits<double>(1)).value()));
	EXPECT_EQ(1024, (pebibits<double>(exbibits<double>(1)).value()));

	// Source: https://en.wikipedia.org/wiki/Binary_prefix
	EXPECT_NEAR(percent<double>(2.4), kibibytes<double>(1) / kilobytes<double>(1) - 1, 0.005);
	EXPECT_NEAR(percent<double>(4.9), mebibytes<double>(1) / megabytes<double>(1) - 1, 0.005);
	EXPECT_NEAR(percent<double>(7.4), gibibytes<double>(1) / gigabytes<double>(1) - 1, 0.005);
	EXPECT_NEAR(percent<double>(10.0), tebibytes<double>(1) / terabytes<double>(1) - 1, 0.005);
	EXPECT_NEAR(percent<double>(12.6), pebibytes<double>(1) / petabytes<double>(1) - 1, 0.005);
	EXPECT_NEAR(percent<double>(15.3), exbibytes<double>(1) / exabytes<double>(1) - 1, 0.005);
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

TEST_F(ConversionFactor, energy_density)
{
	static_assert(units::traits::is_same_dimension_unit_v<joules_per_meter_cubed<double>, decltype(J / (m * m * m))>);
	static_assert(traits::is_energy_density_unit_v<decltype(J / cu_m)>);
	static_assert(traits::is_pressure_unit_v<decltype(J / cu_m)>); // energy density IS pressure

	constexpr joules_per_meter_cubed test = 64.0 * J / 2.0_cu_m;
	EXPECT_EQ(test, 32.0_Jpm3);
}

TEST_F(ConversionFactor, irradiance)
{
	static_assert(std::is_same_v<watts_per_meter_squared<double>, decltype(W / sq_m)>);
	static_assert(traits::is_irradiance_unit_v<decltype(W / sq_m)>);

	constexpr watts_per_meter_squared test = 42.0 * W / 2.0_sq_m;
	EXPECT_EQ(test, 21.0_Wpm2);
}

TEST_F(ConversionFactor, radiance)
{
	static_assert(std::is_same_v<watts_per_steradian_per_meter_squared<double>, decltype(W / sr / sq_m)>);
	static_assert(traits::is_radiance_unit_v<decltype(W / sr / sq_m)>);

	constexpr watts_per_steradian_per_meter_squared test = 44.0 * W / 2.0_sr / 2.0_sq_m;
	EXPECT_EQ(test, 11.0_Wpsrm2);
}

TEST_F(ConversionFactor, radiant_intensity)
{
	static_assert(std::is_same_v<watts_per_steradian<double>, decltype(W / sr)>);
	static_assert(traits::is_radiant_intensity_unit_v<decltype(W / sr)>);

	constexpr watts_per_steradian test = 44.0 * W / 2.0_sr;
	EXPECT_EQ(test, 22.0_Wpsr);
}

TEST_F(ConversionFactor, spectral_flux)
{
	static_assert(std::is_same_v<watts_per_meter<double>, decltype(W / m)>);
	static_assert(traits::is_spectral_flux_unit_v<decltype(W / m)>);

	constexpr watts_per_meter test = 44.0 * W / 4.0_m;
	EXPECT_EQ(test, 11.0_Wpm);
}

TEST_F(ConversionFactor, spectral_intensity)
{
	static_assert(std::is_same_v<watts_per_steradian_per_meter<double>, decltype(W / sr / m)>);
	static_assert(traits::is_spectral_intensity_unit_v<decltype(W / sr / m)>);

	constexpr watts_per_steradian_per_meter test = 44.0 * W / 2.0_sr / 4.0_m;
	EXPECT_EQ(test, 5.5_Wpsrm);
}

TEST_F(ConversionFactor, spectral_irradiance)
{
	static_assert(std::is_same_v<watts_per_meter_cubed<double>, decltype(W / cu_m)>);
	static_assert(traits::is_spectral_irradiance_unit_v<decltype(W / cu_m)>);

	constexpr watts_per_meter_cubed test = 44.0 * W / 2.0_cu_m;
	EXPECT_EQ(test, 22_Wpm3);
}

TEST_F(ConversionFactor, spectral_radiance)
{
	static_assert(std::is_same_v<watts_per_steradian_per_meter_cubed<double>, decltype(W / sr / cu_m)>);
	static_assert(traits::is_spectral_radiance_unit_v<decltype(W / sr / cu_m)>);

	constexpr watts_per_steradian_per_meter_cubed test = 44.0 * W / 2.0_sr / 2.0_cu_m;
	EXPECT_EQ(test, 11_Wpsrm3);
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
	static_assert(std::is_same_v<meters<double>, decltype(constants::pi * meters<double>(1))>);
	static_assert(std::is_same_v<meters<double>, decltype(meters<double>(1) * constants::pi)>);

	EXPECT_NEAR(detail::PI_VAL, (constants::pi * meters<double>(1)).to<double>(), 5.0e-10);
	EXPECT_NEAR(detail::PI_VAL, (meters<double>(1) * constants::pi).to<double>(), 5.0e-10);

	// explicit multiplication
	meters<double> a_m = pi * meters(1);
	meters<double> b_m = meters(1) * pi;

	EXPECT_NEAR(detail::PI_VAL, a_m.to<double>(), 5.0e-10);
	EXPECT_NEAR(detail::PI_VAL, b_m.to<double>(), 5.0e-10);

	// auto division
	static_assert(std::is_same_v<hertz<double>, decltype(constants::pi / seconds<double>(1))>);
	static_assert(std::is_same_v<seconds<double>, decltype(seconds<double>(1) / constants::pi)>);

	EXPECT_NEAR(detail::PI_VAL, (constants::pi / seconds<double>(1)).to<double>(), 5.0e-10);
	EXPECT_NEAR(1.0 / detail::PI_VAL, (seconds<double>(1) / constants::pi).to<double>(), 5.0e-10);

	// explicit
	hertz<double>   c_Hz = constants::pi / seconds<double>(1);
	seconds<double> d_s  = seconds<double>(1) / constants::pi;

	EXPECT_NEAR(detail::PI_VAL, c_Hz.to<double>(), 5.0e-10);
	EXPECT_NEAR(1.0 / detail::PI_VAL, d_s.to<double>(), 5.0e-10);
}

TEST_F(ConversionFactor, constants)
{
	// Source: https://physics.nist.gov/cuu/Constants/index.html
	EXPECT_DOUBLE_EQ(3.141592653589793, constants::pi);
	EXPECT_DOUBLE_EQ(299792458.0, constants::c.value());
	EXPECT_DOUBLE_EQ(6.67430e-11, constants::G.value());
	EXPECT_DOUBLE_EQ(6.62607015e-34, constants::h.value());
	EXPECT_DOUBLE_EQ(1.054571817e-34, constants::h_bar.value());
	EXPECT_DOUBLE_EQ(1.25663706212e-6, constants::mu0.value());
	EXPECT_DOUBLE_EQ(8.8541878128e-12, constants::epsilon0.value());
	EXPECT_DOUBLE_EQ(376.730313668, constants::Z0.value());
	EXPECT_DOUBLE_EQ(8.9875517923e9, constants::k_e.value());
	EXPECT_DOUBLE_EQ(1.602176634e-19, constants::e.value());
	EXPECT_DOUBLE_EQ(9.1093837015e-31, constants::m_e.value());
	EXPECT_DOUBLE_EQ(1.67262192369e-27, constants::m_p.value());
	EXPECT_DOUBLE_EQ(9.2740100783e-24, constants::mu_B.value());
	EXPECT_DOUBLE_EQ(6.02214076e23, constants::N_A.value());
	EXPECT_DOUBLE_EQ(8.314462618, constants::R.value());
	EXPECT_DOUBLE_EQ(1.380649e-23, constants::k_B.value());
	EXPECT_DOUBLE_EQ(96485.33212, constants::F.value());
	EXPECT_DOUBLE_EQ(5.670374419e-8, constants::sigma.value());
}

TEST_F(ConversionFactor, std_chrono)
{
	nanoseconds a_ns(std::chrono::nanoseconds(10));
	EXPECT_EQ(nanoseconds(10), a_ns);
	microseconds b_us(std::chrono::microseconds(10));

	EXPECT_EQ(microseconds(10), b_us);
	milliseconds c_ms = std::chrono::milliseconds(10);
	EXPECT_EQ(milliseconds(10), c_ms);
	seconds d_s = std::chrono::seconds(1);
	EXPECT_EQ(seconds(1), d_s);
	minutes e_min = std::chrono::minutes(120);
	EXPECT_EQ(minutes(120), e_min);
	hours f_hr = std::chrono::hours(2);
	EXPECT_EQ(hours(2), f_hr);

	std::chrono::nanoseconds g_ns = nanoseconds<int>(100);
	EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(g_ns).count(), 100);
	std::chrono::nanoseconds h_ns = microseconds<int>(2);
	EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(h_ns).count(), 2000);
	std::chrono::nanoseconds i_ns = milliseconds<int>(1);
	EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(i_ns).count(), 1000000);
	std::chrono::nanoseconds j_ns = seconds<int>(1);
	EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(j_ns).count(), 1000000000);
	std::chrono::nanoseconds k_ns = minutes<int>(1);
	EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(k_ns).count(), 60000000000);
	std::chrono::nanoseconds l_ns = hours<int>(1);
	EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(l_ns).count(), 3600000000000);
}

TEST_F(ConversionFactor, squaredTemperature)
{
	using squared_celsius   = compound_conversion_factor<squared<celsius<double>>>;
	using squared_celsius_t = unit<squared_celsius>;
	constexpr squared_celsius_t right(100);
	constexpr celsius           rootRight = sqrt(right);
	EXPECT_EQ(celsius<double>(10), rootRight);
}

TEST_F(UnitMath, min)
{
	meters a_m(1.0);
	feet   c_ft(1.0);
	EXPECT_EQ(c_ft, units::min(a_m, c_ft));

	constexpr meters      d_m(1);
	constexpr centimeters e_cm(99);
	EXPECT_EQ(e_cm, units::min(d_m, e_cm));
}

TEST_F(UnitMath, max)
{
	meters<double> a_m(1);
	feet<double>   c_ft(1);
	EXPECT_EQ(a_m, max(a_m, c_ft));

	meters<int>      d_m(1);
	centimeters<int> e_cm(101);
	EXPECT_EQ(e_cm, max(d_m, e_cm));
}

TEST_F(UnitMath, ternaryOperator)
{
	degrees val1 = 10_deg;
	degrees val2 = 90_deg;

	bool value   = true;
	auto new_val = value ? val1 - val2 : val2;
	EXPECT_EQ(new_val, -80_deg);

	value   = false;
	new_val = value ? val1 - val2 : val2;
	EXPECT_EQ(new_val, 90_deg);
}

TEST_F(UnitMath, cos)
{
	static_assert(std::is_same_v<dimensionless<double>, decltype(cos(angle::radians<double>(0)))>);
	static_assert(std::is_same_v<dimensionless<double>, decltype(cos(degrees<int>(0)))>);
	EXPECT_NEAR(dimensionless<double>(-0.41614683654), cos(angle::radians<double>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(-0.41614683654), cos(radians<int>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(-0.70710678118), cos(angle::degrees<double>(135)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(-0.70710678118), cos(degrees<int>(135)), 5.0e-11);
}

TEST_F(UnitMath, sin)
{
	static_assert(std::is_same_v<dimensionless<double>, decltype(sin(angle::radians<double>(0)))>);
	static_assert(std::is_same_v<dimensionless<double>, decltype(sin(degrees<int>(0)))>);
	EXPECT_NEAR(dimensionless<double>(0.90929742682), sin(angle::radians<double>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(0.90929742682), sin(radians<int>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(0.70710678118), sin(angle::degrees<double>(135)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(0.70710678118), sin(degrees<int>(135)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(0), sin(1.0_rad * units::constants::pi), 5.0e-16);
}

TEST_F(UnitMath, tan)
{
	static_assert(std::is_same_v<dimensionless<double>, decltype(tan(angle::radians<double>(0)))>);
	static_assert(std::is_same_v<dimensionless<double>, decltype(tan(degrees<int>(0)))>);
	EXPECT_NEAR(dimensionless<double>(-2.18503986326), tan(angle::radians<double>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(-2.18503986326), tan(radians<int>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(-1.0), tan(angle::degrees<double>(135)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(-1.0), tan(degrees<int>(135)), 5.0e-11);
}

TEST_F(UnitMath, acos)
{
	static_assert(std::is_same_v<angle::radians<double>, decltype(acos(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<angle::radians<double>, decltype(acos(dimensionless<int>(0)))>);
	auto in1  = -0.41614683654;
	auto in2  = 0;
	auto in3  = -0.70710678118654752440084436210485;
	auto in4  = 0;
	auto out1 = 2;
	auto out2 = 1.570796326795;
	auto out3 = 135;
	auto out4 = 90;
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), acos(dimensionless<double>(in1)).to<double>(), 5.0e-11);
	EXPECT_NEAR(angle::radians<double>(out2).to<double>(), acos(dimensionless<int>(in2)).to<double>(), 5.0e-11);
	EXPECT_NEAR(angle::degrees<double>(out3).to<double>(), angle::degrees<double>(acos(dimensionless<double>(in3))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degrees<double>(out4).to<double>(), angle::degrees<double>(acos(dimensionless<int>(in4))).to<double>(), 5.0e-12);
	auto uin1 = in1 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin2 = in2 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin3 = in3 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin4 = in4 * 1.0_m * (1.0 / (1000.0_mm));
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), acos(uin1).to<double>(), 5.0e-11);
	EXPECT_NEAR(angle::radians<double>(out2).to<double>(), acos(uin2).to<double>(), 5.0e-11);
	EXPECT_NEAR(angle::degrees<double>(out3).to<double>(), angle::degrees<double>(acos(uin3)).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degrees<double>(out4).to<double>(), angle::degrees<double>(acos(uin4)).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, asin)
{
	static_assert(std::is_same_v<angle::radians<double>, decltype(asin(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<angle::radians<double>, decltype(asin(dimensionless<int>(0)))>);
	auto in1  = 0.90929742682;
	auto in2  = 1;
	auto in3  = 0.70710678118654752440084436210485;
	auto in4  = 1;
	auto out1 = 1.14159265;
	auto out2 = 1.570796326795;
	auto out3 = 45;
	auto out4 = 90;
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), asin(dimensionless<double>(in1)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::radians<double>(out2).to<double>(), asin(dimensionless<int>(in2)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::degrees<double>(out3).to<double>(), angle::degrees<double>(asin(dimensionless<double>(in3))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degrees<double>(out4).to<double>(), angle::degrees<double>(asin(dimensionless<int>(in4))).to<double>(), 5.0e-12);
	auto uin1 = in1 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin2 = in2 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin3 = in3 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin4 = in4 * 1.0_m * (1.0 / (1000.0_mm));
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), asin(uin1).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::radians<double>(out2).to<double>(), asin(uin2).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::degrees<double>(out3).to<double>(), angle::degrees<double>(asin(uin3)).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degrees<double>(out4).to<double>(), angle::degrees<double>(asin(uin4)).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, atan)
{
	static_assert(std::is_same_v<angle::radians<double>, decltype(atan(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<angle::radians<double>, decltype(atan(dimensionless<int>(0)))>);
	auto in1  = -2.18503986326;
	auto in2  = 1;
	auto in3  = -1;
	auto in4  = 1;
	auto out1 = -1.14159265;
	auto out2 = 0.785398163397;
	auto out3 = -45;
	auto out4 = 45;
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), atan(dimensionless<double>(in1)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::radians<double>(out2).to<double>(), atan(dimensionless<int>(in2)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::degrees<double>(out3).to<double>(), angle::degrees<double>(atan(dimensionless<double>(in3))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degrees<double>(out4).to<double>(), angle::degrees<double>(atan(dimensionless<int>(in4))).to<double>(), 5.0e-12);
	auto uin1 = in1 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin2 = in2 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin3 = in3 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin4 = in4 * 1.0_m * (1.0 / (1000.0_mm));
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), atan(uin1).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::radians<double>(out2).to<double>(), atan(uin2).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::degrees<double>(out3).to<double>(), angle::degrees<double>(atan(uin3)).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degrees<double>(out4).to<double>(), angle::degrees<double>(atan(uin4)).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, atan2)
{
	static_assert(std::is_same_v<angle::radians<double>, decltype(atan2(dimensionless<double>(1), dimensionless<double>(1)))>);
	static_assert(std::is_same_v<angle::radians<double>, decltype(atan2(dimensionless<int>(1), dimensionless<int>(1)))>);
	EXPECT_NEAR(angle::radians<double>(detail::PI_VAL / 4).to<double>(), atan2(dimensionless<double>(2), dimensionless<double>(2)).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::radians<double>(detail::PI_VAL / 4).to<double>(), atan2(dimensionless<int>(2), dimensionless<int>(2)).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degrees<double>(45).to<double>(), angle::degrees<double>(atan2(dimensionless<double>(2), dimensionless<double>(2))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degrees<double>(45).to<double>(), angle::degrees<double>(atan2(dimensionless<int>(2), dimensionless<int>(2))).to<double>(), 5.0e-12);

	static_assert(std::is_same_v<angle::radians<double>, decltype(atan2(dimensionless<double>(1), dimensionless<double>(1)))>);
	EXPECT_NEAR(angle::radians<double>(detail::PI_VAL / 6).to<double>(), atan2(dimensionless<double>(1), sqrt(dimensionless<double>(3))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::radians<double>(detail::PI_VAL / 6).to<double>(), atan2(dimensionless<int>(1), sqrt(dimensionless<int>(3))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degrees<double>(30).to<double>(), angle::degrees<double>(atan2(dimensionless<double>(1), sqrt(dimensionless<double>(3)))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degrees<double>(30).to<double>(), angle::degrees<double>(atan2(dimensionless<int>(1), sqrt(dimensionless<int>(3)))).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, cosh)
{
	static_assert(std::is_same_v<dimensionless<double>, decltype(cosh(angle::radians<double>(0)))>);
	static_assert(std::is_same_v<dimensionless<double>, decltype(cosh(degrees<int>(0)))>);
	EXPECT_NEAR(dimensionless<double>(3.76219569108), cosh(angle::radians<double>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(3.76219569108), cosh(radians<int>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(5.32275215), cosh(angle::degrees<double>(135)), 5.0e-9);
	EXPECT_NEAR(dimensionless<double>(5.32275215), cosh(degrees<int>(135)), 5.0e-9);
}

TEST_F(UnitMath, sinh)
{
	static_assert(std::is_same_v<dimensionless<double>, decltype(sinh(angle::radians<double>(0)))>);
	static_assert(std::is_same_v<dimensionless<double>, decltype(sinh(degrees<int>(0)))>);
	EXPECT_NEAR(dimensionless<double>(3.62686040785), sinh(angle::radians<double>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(3.62686040785), sinh(radians<int>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(5.22797192), sinh(angle::degrees<double>(135)), 5.0e-9);
	EXPECT_NEAR(dimensionless<double>(5.22797192), sinh(degrees<int>(135)), 5.0e-9);
}

TEST_F(UnitMath, tanh)
{
	static_assert(std::is_same_v<dimensionless<double>, decltype(tanh(angle::radians<double>(0)))>);
	static_assert(std::is_same_v<dimensionless<double>, decltype(tanh(degrees<int>(0)))>);
	EXPECT_NEAR(dimensionless<double>(0.96402758007), tanh(angle::radians<double>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(0.96402758007), tanh(radians<int>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(0.98219338), tanh(angle::degrees<double>(135)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(0.98219338), tanh(degrees<int>(135)), 5.0e-11);
}

TEST_F(UnitMath, acosh)
{
	static_assert(std::is_same_v<angle::radians<double>, decltype(acosh(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<angle::radians<double>, decltype(acosh(dimensionless<int>(0)))>);
	auto ins  = 2;
	auto out1 = 1.316957896924817;
	auto out2 = 75.456129290216893;
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), acosh(dimensionless<double>(ins)).to<double>(), 5.0e-11);
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), acosh(dimensionless<int>(ins)).to<double>(), 5.0e-11);
	EXPECT_NEAR(angle::degrees<double>(out2).to<double>(), angle::degrees<double>(acosh(dimensionless<double>(ins))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degrees<double>(out2).to<double>(), angle::degrees<double>(acosh(dimensionless<int>(ins))).to<double>(), 5.0e-12);
	auto uins = ins * 1.0_m * (1.0 / (1000.0_mm));
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), acosh(uins).to<double>(), 5.0e-11);
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), acosh(uins).to<double>(), 5.0e-11);
	EXPECT_NEAR(angle::degrees<double>(out2).to<double>(), angle::degrees<double>(acosh(uins)).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degrees<double>(out2).to<double>(), angle::degrees<double>(acosh(uins)).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, asinh)
{
	static_assert(std::is_same_v<angle::radians<double>, decltype(asinh(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<angle::radians<double>, decltype(asinh(dimensionless<int>(0)))>);
	auto ins  = 2;
	auto out1 = 1.443635475178810;
	auto out2 = 82.714219883108939;
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), asinh(dimensionless<double>(ins)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), asinh(dimensionless<int>(ins)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::degrees<double>(out2).to<double>(), angle::degrees<double>(asinh(dimensionless<double>(ins))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degrees<double>(out2).to<double>(), angle::degrees<double>(asinh(dimensionless<int>(ins))).to<double>(), 5.0e-12);
	auto uins = ins * 1.0_m * (1.0 / (1000.0_mm));
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), asinh(uins).to<double>(), 5.0e-11);
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), asinh(uins).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::degrees<double>(out2).to<double>(), angle::degrees<double>(asinh(uins)).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degrees<double>(out2).to<double>(), angle::degrees<double>(asinh(uins)).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, atanh)
{
	static_assert(std::is_same_v<angle::radians<double>, decltype(atanh(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<angle::radians<double>, decltype(atanh(dimensionless<int>(0)))>);
	auto ins  = 0.5;
	auto out1 = 0.549306144334055;
	auto out2 = 31.472923730945389;
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), atanh(dimensionless<double>(ins)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::radians<double>(0).to<double>(), atanh(dimensionless<int>(0)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::degrees<double>(out2).to<double>(), angle::degrees<double>(atanh(dimensionless<double>(ins))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degrees<double>(0).to<double>(), angle::degrees<double>(atanh(dimensionless<int>(0))).to<double>(), 5.0e-12);
	auto uins = ins * 1.0_m * (1.0 / (1000.0_mm));
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), atanh(uins).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), atanh(uins).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::degrees<double>(out2).to<double>(), angle::degrees<double>(atanh(uins)).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degrees<double>(out2).to<double>(), angle::degrees<double>(atanh(uins)).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, exp)
{
	double val = 10.0;
	EXPECT_EQ(std::exp(val), exp(dimensionless<double>(val)));
	auto uval = 5.0_m * (2.0 / 1000.0_mm);
	EXPECT_EQ(static_cast<double>(uval), static_cast<typename decltype(uval)::underlying_type>(uval));
	EXPECT_EQ(std::exp(uval.to<double>()), units::exp(uval));
}

TEST_F(UnitMath, log)
{
	double val = 100.0;
	EXPECT_EQ(std::log(val), log(dimensionless<double>(val)));
	auto uval = 5.0_m * (2.0 / 1000.0_mm);
	EXPECT_EQ(std::log(uval.to<double>()), units::log(uval));
}

TEST_F(UnitMath, log10)
{
	double val = 100.0;
	EXPECT_EQ(std::log10(val), log10(dimensionless<double>(val)));
	auto uval = 5.0_m * (2.0 / 1000.0_mm);
	EXPECT_EQ(std::log10(uval.to<double>()), units::log10(uval));
}

TEST_F(UnitMath, modf)
{
	double                val = 100.0;
	double                modfr1;
	dimensionless<double> modfr2;
	EXPECT_EQ(std::modf(val, &modfr1), modf(dimensionless<double>(val), &modfr2));
	EXPECT_EQ(modfr1, modfr2);
	auto           uval = 5.0_m * (2.0 / 1000.0_mm);
	double         umodfr1;
	decltype(uval) umodfr2;
	EXPECT_EQ(std::modf(uval.to<double>(), &umodfr1), units::modf(uval, &umodfr2));
}

TEST_F(UnitMath, exp2)
{
	double val = 10.0;
	EXPECT_EQ(std::exp2(val), exp2(dimensionless<double>(val)));
	auto uval = 5.0_m * (2.0 / 1000.0_mm);
	EXPECT_EQ(std::exp2(uval.to<double>()), units::exp2(uval));
}

TEST_F(UnitMath, expm1)
{
	double val = 10.0;
	EXPECT_EQ(std::expm1(val), expm1(dimensionless<double>(val)));
	auto uval = 5.0_m * (2.0 / 1000.0_mm);
	EXPECT_EQ(std::expm1(uval.to<double>()), units::expm1(uval));
}

TEST_F(UnitMath, log1p)
{
	double val = 10.0;
	EXPECT_EQ(std::log1p(val), log1p(dimensionless<double>(val)));
	auto uval = 5.0_m * (2.0 / 1000.0_mm);
	EXPECT_EQ(std::log1p(uval.to<double>()), units::log1p(uval));
}

TEST_F(UnitMath, log2)
{
	double val = 10.0;
	EXPECT_EQ(std::log2(val), log2(dimensionless<double>(val)));
	auto uval = 5.0_m * (2.0 / 1000.0_mm);
	EXPECT_EQ(std::log2(uval.to<double>()), units::log2(uval));
}

TEST_F(UnitMath, pow)
{
	constexpr meters value(10.0);

	auto inv_sq = pow<-2>(value);
	EXPECT_NEAR(0.01, inv_sq.value(), 5.0e-5);
	static_assert(std::is_same_v<decltype(inv_sq), unit<inverse<square_meters<>>>>);

	auto inv = pow<-1>(value);
	EXPECT_NEAR(0.1, inv.value(), 5.0e-4);
	static_assert(std::is_same_v<decltype(inv), unit<inverse<meters<>>>>);

	auto scalar = pow<0>(value);
	EXPECT_NEAR(1, scalar.value(), 5.0e-3);
	static_assert(std::is_same_v<decltype(scalar), dimensionless<>>);

	auto sq = pow<2>(value);
	EXPECT_NEAR(100.0, sq.value(), 5.0e-2);
	static_assert(std::is_same_v<decltype(sq), square_meters<double>>);

	auto cube = pow<3>(value);
	EXPECT_NEAR(1000.0, cube.value(), 5.0e-2);
	static_assert(std::is_same_v<decltype(cube), unit<traits::strong_t<cubed<meters<double>>>>>);

	auto fourth = pow<4>(value);
	EXPECT_NEAR(10000.0, fourth.value(), 5.0e-2);
	static_assert(std::is_same_v<decltype(fourth), unit<compound_conversion_factor<squared<meters<double>>, squared<meters<double>>>>>);
}

TEST_F(UnitMath, sqrt)
{
	static_assert(std::is_same_v<meters<double>, decltype(sqrt(square_meters<double>(4.0)))>);
	EXPECT_NEAR(meters<double>(2.0).to<double>(), sqrt(square_meters<double>(4.0)).to<double>(), 5.0e-9);

	static_assert(std::is_same_v<angle::radians<double>, decltype(sqrt(steradians<double>(16.0)))>);
	EXPECT_NEAR(angle::radians<double>(4.0).to<double>(), sqrt(steradians<double>(16.0)).to<double>(), 5.0e-9);

	static_assert(std::is_convertible_v<feet<double>, decltype(sqrt(square_feet<double>(10.0)))>);

	// for rational conversion (i.e. no integral root) let's check a bunch of different ways this could go wrong
	feet<double> resultFt = sqrt(square_feet<double>(10.0));
	EXPECT_NEAR(feet<double>(3.16227766017).to<double>(), sqrt(square_feet<double>(10.0)).to<double>(), 5.0e-9);
	EXPECT_NEAR(feet<double>(3.16227766017).to<double>(), resultFt.to<double>(), 5.0e-9);
	EXPECT_EQ(resultFt, sqrt(square_feet<double>(10.0)));

	percent resultPct = sqrt(16.0_pct);
	EXPECT_EQ(resultPct, 4.0_pct);
	EXPECT_EQ(0.04, resultPct);
}

TEST_F(UnitMath, hypot)
{
	static_assert(std::is_same_v<meters<double>, decltype(hypot(meters<double>(3.0), meters<double>(4.0)))>);
	EXPECT_NEAR(meters<double>(5.0).to<double>(), (hypot(meters<double>(3.0), meters<double>(4.0))).to<double>(), 5.0e-9);

	static_assert(traits::is_same_dimension_unit_v<feet<double>, decltype(hypot(feet<double>(3.0), meters<double>(1.2192)))>);
	EXPECT_NEAR(feet<double>(5.0).to<double>(), feet<double>(hypot(feet<double>(3.0), meters<double>(1.2192))).to<double>(), 5.0e-9);
}

TEST_F(UnitMath, ceil)
{
	double val = 101.1;
	EXPECT_EQ(ceil(val), ceil(meters<double>(val)).to<double>());
	static_assert(std::is_same_v<meters<double>, decltype(ceil(meters<double>(val)))>);
}

TEST_F(UnitMath, floor)
{
	double val = 101.1;
	EXPECT_EQ(floor(val), floor(dimensionless<double>(val)));
}

TEST_F(UnitMath, fmod)
{
	EXPECT_EQ(fmod(100.0, 101.2), fmod(meters<double>(100.0), meters<double>(101.2)).to<double>());
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
	double         sign = -1;
	meters<double> val(5.0);
	EXPECT_EQ(meters<double>(-5.0), copysign(val, sign));
	EXPECT_EQ(meters<double>(-5.0), copysign(val, angle::radians<double>(sign)));
}

TEST_F(UnitMath, fdim)
{
	EXPECT_EQ(meters<double>(0.0), fdim(meters<double>(8.0), meters<double>(10.0)));
	EXPECT_EQ(meters<double>(2.0), fdim(meters<double>(10.0), meters<double>(8.0)));
	EXPECT_NEAR(meters<double>(9.3904).to<double>(), meters<double>(fdim(meters<double>(10.0), feet<double>(2.0))).to<double>(),
		5.0e-320); // not sure why they aren't comparing exactly equal, but clearly they are.
}

TEST_F(UnitMath, fmin)
{
	EXPECT_EQ(meters<double>(8.0), fmin(meters<double>(8.0), meters<double>(10.0)));
	EXPECT_EQ(meters<double>(8.0), fmin(meters<double>(10.0), meters<double>(8.0)));
	EXPECT_EQ(feet<double>(2.0), fmin(meters<double>(10.0), feet<double>(2.0)));
}

TEST_F(UnitMath, fmax)
{
	EXPECT_EQ(meters<double>(10.0), fmax(meters<double>(8.0), meters<double>(10.0)));
	EXPECT_EQ(meters<double>(10.0), fmax(meters<double>(10.0), meters<double>(8.0)));
	EXPECT_EQ(meters<double>(10.0), fmax(meters<double>(10.0), feet<double>(2.0)));
}

TEST_F(UnitMath, fabs)
{
	EXPECT_EQ(meters<double>(10.0), fabs(meters<double>(-10.0)));
	EXPECT_EQ(meters<double>(10.0), fabs(meters<double>(10.0)));
}

TEST_F(UnitMath, abs)
{
	EXPECT_EQ(meters<double>(10.0), abs(meters<double>(-10.0)));
	EXPECT_EQ(meters<double>(10.0), abs(meters<double>(10.0)));
}

TEST_F(UnitMath, fma)
{
	meters<double>        x(2.0);
	meters<double>        y(3.0);
	square_meters<double> z(1.0);
	EXPECT_EQ(square_meters<double>(7.0), (units::fma(x, y, z)));
}

TEST_F(UnitMath, isnan)
{
	meters<> zero(0.0);
	meters<> nan(NAN);
	meters<> inf(INFINITY);

	EXPECT_TRUE(units::isnan(nan));
	EXPECT_FALSE(units::isnan(inf));
	EXPECT_FALSE(units::isnan(0.0_m));
	EXPECT_FALSE(units::isnan(DBL_MIN / 2.0 * 1_m));
	EXPECT_TRUE(units::isnan(zero / zero));
	EXPECT_TRUE(units::isnan(inf - inf));
}

TEST_F(UnitMath, isinf)
{
	constexpr meters zero(0.0);
	constexpr meters nan(NAN);
	constexpr meters inf(INFINITY);

	EXPECT_FALSE(units::isnan(zero));
	EXPECT_FALSE(units::isinf(nan));
	EXPECT_TRUE(units::isinf(inf));
	EXPECT_FALSE(units::isinf(0.0_m));
	EXPECT_TRUE(units::isinf(exp(1600_rad / 2_rad)));
	EXPECT_FALSE(units::isinf(DBL_MIN / 2.0 * 1_m));
}

TEST_F(UnitMath, isfinite)
{
	meters zero(0.0);
	meters nan(NAN);
	meters inf(INFINITY);

	EXPECT_TRUE(units::isfinite(zero));
	EXPECT_FALSE(units::isfinite(nan));
	EXPECT_FALSE(units::isfinite(inf));
	EXPECT_TRUE(units::isfinite(0.0_m));
	EXPECT_FALSE(units::isfinite(exp(1600_rad / 2_rad)));
	EXPECT_TRUE(units::isfinite(DBL_MIN / 2.0 * 1_m));
}

TEST_F(UnitMath, isnormal)
{
	meters zero(0.0);
	meters nan(NAN);
	meters inf(INFINITY);

	EXPECT_FALSE(units::isnormal(zero));
	EXPECT_FALSE(units::isnormal(nan));
	EXPECT_FALSE(units::isnormal(inf));
	EXPECT_FALSE(units::isnormal(0.0_m));
	EXPECT_TRUE(units::isnormal(1.0_m));
}

TEST_F(UnitMath, isunordered)
{
	meters zero(0.0);
	meters nan(NAN);

	EXPECT_TRUE(units::isunordered(nan, zero));
	EXPECT_TRUE(units::isunordered(zero, nan));
	EXPECT_FALSE(units::isunordered(zero, zero));
}

// Constexpr
TEST_F(Constexpr, construction)
{
	constexpr meters<double> result0(0);
	constexpr auto           result1 = make_unit<meters<double>>(1);
	constexpr auto           result2 = meters<double>(2);

	EXPECT_EQ(meters<double>(0), result0);
	EXPECT_EQ(meters<double>(1), result1);
	EXPECT_EQ(meters<double>(2), result2);

	EXPECT_TRUE(noexcept(meters<double>(0)));
	EXPECT_TRUE(noexcept(make_unit<meters<double>>(1)));
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
	[[maybe_unused]] constexpr auto result4(meters<int>(1) + meters<int>(1));
	[[maybe_unused]] constexpr auto result5(meters<int>(1) - meters<int>(1));
	[[maybe_unused]] constexpr auto result6(meters<int>(1) * meters<int>(1));
	[[maybe_unused]] constexpr auto result7(meters<int>(1) / meters<int>(1));
	[[maybe_unused]] constexpr auto result8(pow<2>(meters<int>(2)));
	constexpr auto                  result9  = pow<3>(2.0_m);
	constexpr auto                  result10 = 2.0_m * 2.0_m;

	EXPECT_TRUE(noexcept(1.0_m + 1.0_m));
	EXPECT_TRUE(noexcept(1.0_m - 1.0_m));
	EXPECT_TRUE(noexcept(1.0_m * 1.0_m));
	EXPECT_TRUE(noexcept(1.0_m / 1.0_m));
	EXPECT_TRUE(noexcept(meters<int>(1) + meters<int>(1)));
	EXPECT_TRUE(noexcept(meters<int>(1) - meters<int>(1)));
	EXPECT_TRUE(noexcept(meters<int>(1) * meters<int>(1)));
	EXPECT_TRUE(noexcept(meters<int>(1) / meters<int>(1)));
	EXPECT_TRUE(noexcept(pow<2>(meters<double>(2))));
	EXPECT_TRUE(noexcept(pow<3>(2.0_m)));
	EXPECT_TRUE(noexcept(2.0_m * 2.0_m));

	meters<double> length{42};
	EXPECT_TRUE(noexcept(+length));
	EXPECT_TRUE(noexcept(-length));
	EXPECT_TRUE(noexcept(++length));
	EXPECT_TRUE(noexcept(--length));
	EXPECT_TRUE(noexcept(length++));
	EXPECT_TRUE(noexcept(length--));

	EXPECT_EQ(8.0_cu_m, result9);
	EXPECT_EQ(4.0_sq_m, result10);
}

TEST_F(Constexpr, assignment)
{
	auto testConstexpr = []() constexpr noexcept
	{
		meters<double> length{42.};
		+length;
		-length;
		++length;
		--length;
		length++;
		length--;
		length += 2.0_m;
		length -= 2.0_m;
		length *= 2;
		length /= 2;
		return length;
	};

	[[maybe_unused]] constexpr auto length = testConstexpr();
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
	constexpr std::array<meters<double>, 5> arr{{0.0_m, 1.0_m, 2.0_m, 3.0_m, 4.0_m}};
	constexpr bool                          equal = (arr[3] == 3.0_m);
	EXPECT_TRUE(equal);
}

TEST_F(UnitLimits, UnitMin)
{
	EXPECT_EQ(meters(std::numeric_limits<double>::min()), std::numeric_limits<meters<double>>::min());
	EXPECT_EQ(seconds(std::numeric_limits<double>::min()), std::numeric_limits<seconds<double>>::min());
}

TEST_F(UnitLimits, UnitDenormMin)
{
	EXPECT_EQ(meters(std::numeric_limits<double>::denorm_min()), std::numeric_limits<meters<double>>::denorm_min());
	EXPECT_EQ(seconds(std::numeric_limits<double>::denorm_min()), std::numeric_limits<seconds<double>>::denorm_min());
}

TEST_F(UnitLimits, UnitMax)
{
	EXPECT_EQ(meters(std::numeric_limits<double>::max()), std::numeric_limits<meters<double>>::max());
	EXPECT_EQ(seconds(std::numeric_limits<double>::max()), std::numeric_limits<seconds<double>>::max());
}

TEST_F(UnitLimits, UnitLowest)
{
	EXPECT_EQ(meters(std::numeric_limits<double>::lowest()), std::numeric_limits<meters<double>>::lowest());
	EXPECT_EQ(seconds(std::numeric_limits<double>::lowest()), std::numeric_limits<seconds<double>>::lowest());
}

TEST_F(UnitLimits, UnitEpsilon)
{
	EXPECT_EQ(meters(std::numeric_limits<double>::epsilon()), std::numeric_limits<meters<double>>::epsilon());
	EXPECT_EQ(seconds(std::numeric_limits<double>::epsilon()), std::numeric_limits<seconds<double>>::epsilon());
}

TEST_F(UnitLimits, UnitRoundError)
{
	EXPECT_EQ(meters(std::numeric_limits<double>::round_error()), std::numeric_limits<meters<double>>::round_error());
	EXPECT_EQ(seconds(std::numeric_limits<double>::round_error()), std::numeric_limits<seconds<double>>::round_error());
}

TEST_F(UnitLimits, UnitInfinity)
{
	EXPECT_TRUE(std::numeric_limits<meters<double>>::has_infinity);
	EXPECT_TRUE(std::numeric_limits<meters<double>>::infinity() > std::numeric_limits<meters<double>>::max());
	EXPECT_FALSE(std::numeric_limits<seconds<int>>::has_infinity);
}

TEST_F(UnitLimits, UnitQuietNaN)
{
	EXPECT_NE(meters(std::numeric_limits<double>::quiet_NaN()), std::numeric_limits<meters<double>>::quiet_NaN());
	EXPECT_NE(seconds(std::numeric_limits<double>::quiet_NaN()), std::numeric_limits<seconds<double>>::quiet_NaN());
	EXPECT_TRUE(units::isnan(std::numeric_limits<meters<double>>::quiet_NaN()));
}

TEST_F(UnitLimits, UnitSignalingNaN)
{
	EXPECT_NE(meters(std::numeric_limits<double>::signaling_NaN()), std::numeric_limits<meters<double>>::signaling_NaN());
	EXPECT_NE(seconds(std::numeric_limits<double>::signaling_NaN()), std::numeric_limits<seconds<double>>::signaling_NaN());
	EXPECT_TRUE(units::isnan(std::numeric_limits<meters<double>>::signaling_NaN()));
}

TEST_F(UnitLimits, UnitIsSpecialized)
{
	EXPECT_TRUE(std::numeric_limits<double>::is_specialized == std::numeric_limits<meters<double>>::is_specialized);
	EXPECT_TRUE(std::numeric_limits<double>::is_specialized == std::numeric_limits<seconds<double>>::is_specialized);
}

TEST_F(UnitLimits, UnitIsSigned)
{
	EXPECT_TRUE(std::numeric_limits<double>::is_signed == std::numeric_limits<meters<double>>::is_signed);
	EXPECT_TRUE(std::numeric_limits<double>::is_signed == std::numeric_limits<seconds<double>>::is_signed);
}

TEST_F(UnitLimits, UnitIsInteger)
{
	EXPECT_TRUE(std::numeric_limits<double>::is_integer == std::numeric_limits<meters<double>>::is_integer);
	EXPECT_TRUE(std::numeric_limits<double>::is_integer == std::numeric_limits<seconds<double>>::is_integer);
}

TEST_F(UnitLimits, UnitIsExact)
{
	EXPECT_TRUE(std::numeric_limits<double>::is_exact == std::numeric_limits<meters<double>>::is_exact);
	EXPECT_TRUE(std::numeric_limits<double>::is_exact == std::numeric_limits<seconds<double>>::is_exact);
}

TEST_F(UnitLimits, UnitHasInifinity)
{
	EXPECT_TRUE(std::numeric_limits<double>::has_infinity == std::numeric_limits<meters<double>>::has_infinity);
	EXPECT_TRUE(std::numeric_limits<double>::has_infinity == std::numeric_limits<seconds<double>>::has_infinity);
}

TEST_F(UnitLimits, UnitHasQuietNaN)
{
	EXPECT_TRUE(std::numeric_limits<double>::has_quiet_NaN == std::numeric_limits<meters<double>>::has_quiet_NaN);
	EXPECT_TRUE(std::numeric_limits<double>::has_quiet_NaN == std::numeric_limits<seconds<double>>::has_quiet_NaN);
}

TEST_F(UnitLimits, UnitHasSignalingNaN)
{
	EXPECT_TRUE(std::numeric_limits<double>::has_signaling_NaN == std::numeric_limits<meters<double>>::has_signaling_NaN);
	EXPECT_TRUE(std::numeric_limits<double>::has_signaling_NaN == std::numeric_limits<seconds<double>>::has_signaling_NaN);
}

TEST_F(CaseStudies, radarRangeEquation)
{
	watts<>         P_t;    // transmit power
	dimensionless<> gain;   // gain
	meters<>        lambda; // wavelength
	square_meters<> rcs;    // radar cross-section
	meters<>        range;  // range
	kelvin<>        T_s;    // system noise temp
	hertz<>         B_n;    // bandwidth
	dimensionless<> loss;   // loss

	P_t    = megawatts<>(1.4);
	gain   = decibels<>(33.0);
	lambda = constants::c / megahertz<>(2800.0);
	rcs    = square_meters<>(1.0);
	range  = meters<>(111000.0);
	T_s    = kelvin<>(950.0);
	B_n    = megahertz<>(1.67);
	loss   = decibels<>(8.0);

	const dimensionless<double> SNR = (P_t * pow<2>(gain) * pow<2>(lambda) * rcs) / (pow<3>(4 * pi) * pow<4>(range) * k_B * T_s * B_n * loss);

	EXPECT_NEAR(1.535, SNR.value(), 5.0e-4);
}

TEST_F(CaseStudies, rightTriangle)
{
	constexpr auto a_m = 3.0_m;
	constexpr auto b_m = 4.0_m;
	constexpr auto c_m = sqrt(pow<2>(a_m) + pow<2>(b_m));
	EXPECT_EQ(5.0_m, c_m);
}

TEST_F(CaseStudies, dataReadSimulation)
{
	constexpr auto data_size     = 100_MB;
	constexpr auto read_rate     = 2_MBps;
	bytes          read_progress = 10_MB;

	auto advance_simulation = [&](auto time) { read_progress = units::min(read_progress + time * read_rate, data_size); };

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

	liters_per_second  lps(5);
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
