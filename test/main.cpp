#ifdef _MSC_VER
#pragma warning(disable : 4244) // Disable 'conversion from double to const int, possible loss of data'. The tests just make sure
// that such assignments work as expected, we don't want to remove them and we don't care about the warning.
#define _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING // officially, The effect of instantiating the template std::complex for any type other than float,
// double, or long double is unspecified. We don't care though, we want them to work with units in this
// test
#endif

#include <array>
#include <atomic>
#include <chrono>
#include <compare>
#include <complex>
#include <filesystem>
#include <format>
#include <fstream>
#include <gtest/gtest.h>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <locale>
#include <ratio>
#include <sstream>
#include <string>
#include <type_traits>
#include <units.h>
#include <unordered_map>
#include <units/serialization.h>

using namespace units;
using namespace units::literals;

// #378 ODR-audit regression guards: dimension-keyed physical-quantity concepts (Velocity/Force/...) and the
// value/dimension/layout/serialization safety invariants that make the cross-TU type-identity split value-safe.
#include "odrDimensionConcept.h"

// A user-defined base dimension + unit, declared outside the library, to prove serialization is extensible to
// dimensions the library has never seen (no central table, no fixed ceiling).
namespace units
{
	namespace dimension
	{
		struct pixels_tag
		{
			static constexpr auto name         = "pixels";
			static constexpr auto abbreviation = "px";
		};
		using pixels = make_dimension<pixels_tag>;
		// a dimension with a FRACTIONAL exponent (length^(1/2)), to exercise the fractional-exponent serialization path
		using root_length = dimension_pow<length, std::ratio<1, 2>>;
	} // namespace dimension
	UNIT_ADD(screen, dots, px, conversion_factor<std::ratio<1>, dimension::pixels>)
	UNIT_ADD(root_length, root_meters, rt_m, conversion_factor<std::ratio<1>, dimension::root_length>)
} // namespace units

namespace
{
	// Detector op for the SFINAE-safety guards: is std::common_type_t<A, B> well-formed? Used with the library's
	// detection idiom (detail::is_detected_v) to assert common_type is SFINAE-empty (not a hard error) where expected.
	template<class A, class B>
	using common_type_of = std::common_type_t<A, B>;

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

	class ConcentrationSemantics : public ::testing::Test
	{
	};

	class UnitLimits : public ::testing::Test
	{
	};

	class CaseStudies : public ::testing::Test
	{
	};

	class Serialization : public ::testing::Test
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

// TEST NTTP SUPPORT
template<meters<double> m>
class NTTPTestClass {}; // If this fails to compile, you've broken NTTP support (public inheritence, public members)

TEST_F(TypeTraits, NTTPTest)
{
	// Force an actual NTTP instantiation:
	static_assert(requires { typename NTTPTestClass<1.0_m>; },
				  "Units failed NTTP support (class-type NTTP broken).");
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

TEST_F(TypeTraits, is_volume_flow_rate_unit)
{
	static_assert(!traits::is_volume_flow_rate_unit_v<double>);
	static_assert(traits::is_volume_flow_rate_unit_v<cubic_meters_per_second<double>>);
	static_assert(traits::is_volume_flow_rate_unit_v<const liters_per_second<double>>);
	static_assert(traits::is_volume_flow_rate_unit_v<const gallons_per_minute<double>&>);
	static_assert(!traits::is_volume_flow_rate_unit_v<cubic_meters<double>>);
	static_assert(!traits::is_volume_flow_rate_unit_v<meters_per_second<double>>);
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
	static_assert(traits::is_torque_unit_v<torque::pound_feet<double>>);
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

// Documents the intended relationship between std::hash and operator== for floating units (issue #397).
// operator== is deliberately tolerant (relative epsilon); std::hash hashes the exact linearized value, which is
// the standard and correct behavior for floating-point keys. The exact-value guarantee that container use relies
// on holds: values that are EXACTLY equal always hash equal. Two values that differ by one ULP compare equal
// under the tolerant operator== yet may hash to different buckets -- exactly as they would with std::hash<double>
// -- which is harmless: it degrades to ordinary float-keyed-container behavior, never a wrong result. A hash that
// were "consistent" with the relative-tolerant, non-transitive operator== is impossible without being constant,
// so exact-value hashing is the only coherent choice. This is working as designed, not a defect.
TEST_F(STDSpecializations, hashIsExactValueNotTolerant)
{
	const meters<double> a(1.0);
	const meters<double> b(std::nextafter(1.0, 2.0)); // one ULP above a

	// operator== is tolerant: a and b are "equal" quantities.
	EXPECT_TRUE(a == b);

	// The guarantee that matters: exactly-equal values hash equal (deterministic, representation-independent).
	EXPECT_EQ(std::hash<meters<double>>()(a), std::hash<meters<double>>()(meters<double>(1.0)));
	EXPECT_EQ(std::hash<meters<double>>()(a), std::hash<double>()(1.0));

	// std::hash reflects the exact stored value; the one-ULP neighbour is a distinct hash input, just as it is for
	// std::hash<double>. This mirror is the documented, intentional behavior.
	EXPECT_EQ(std::hash<meters<double>>()(b), std::hash<double>()(std::nextafter(1.0, 2.0)));
}

// General coverage (not tied to a specific change): units must work END-TO-END as STL associative-container
// keys — an ordered container exercises operator<, an unordered one exercises std::hash + operator== together.
// The hash test above only calls std::hash directly; this proves the real use case, and that a scaled key
// (kilometers vs meters) compares/hashes by magnitude, not by stored representation.
TEST_F(STDSpecializations, unitsAsContainerKeys)
{
	// std::map — ordered by value via operator<.
	std::map<meters<double>, std::string> byValue;
	byValue[meters<double>(1.0)] = "one";
	byValue[meters<double>(2.0)] = "two";
	byValue[kilometers<double>(0.003)] = "three-m"; // 3 m, distinct key from 1 m / 2 m
	EXPECT_EQ(byValue.size(), 3u);
	EXPECT_EQ(byValue[meters<double>(1.0)], "one");
	EXPECT_EQ(byValue.begin()->second, "one"); // smallest key first

	// std::unordered_map — needs both std::hash<meters<double>> AND operator==.
	std::unordered_map<meters<double>, int> byHash;
	byHash[meters<double>(5.0)] = 50;
	EXPECT_EQ(byHash.at(meters<double>(5.0)), 50);
	// a scaled-but-equal key resolves to the SAME bucket (kilometers<double>(0.005) == 5 m). Look it up after
	// converting to the map's key type, since a heterogeneous [] would insert a different key type.
	EXPECT_EQ(byHash.at(meters<double>(kilometers<double>(0.005))), 50);

	// std::set — membership by value.
	std::set<seconds<double>> timeSet{seconds<double>(1.0), seconds<double>(2.0), seconds<double>(1.0)};
	EXPECT_EQ(timeSet.size(), 2u); // the duplicate 1 s collapses
	EXPECT_TRUE(timeSet.count(seconds<double>(2.0)) == 1);
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

	// A unit literal is floating-point (1_s is seconds<double>, the same as 1.0_s), so a quantity deduced from
	// one is floating-point. An integer-backed quantity comes from an explicit seconds<int>(1) or a std::chrono
	// integer duration (1s), both exercised above.
	constexpr seconds b_s(1_s);
	static_assert(std::is_floating_point_v<decltype(b_s.value())>);

	constexpr seconds c_s(1.0_s);
	static_assert(std::is_floating_point_v<decltype(c_s.value())>);

	constexpr seconds d_s(1_min);
	static_assert(std::is_floating_point_v<decltype(d_s.value())>);

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
	static constexpr radians ANGULAR_VALUE{-30.0_deg};
	EXPECT_EQ(-30.0_deg, ANGULAR_VALUE);
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
	static_assert(std::is_same_v<percent<double>, decltype(pcnt + 0)>);
	static_assert(std::is_same_v<percent<double>, decltype(0 + pcnt)>);
	static_assert(std::is_same_v<percent<int>, decltype(pcnt + pcnt)>);
	static_assert(std::is_same_v<meters<int>, decltype(length + length)>);

	static_assert(std::is_same_v<dimensionless<int>, decltype(dim - 0)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(0 - dim)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(dim - dim)>);
	static_assert(std::is_same_v<percent<double>, decltype(pcnt - 0)>);
	static_assert(std::is_same_v<percent<double>, decltype(0 - pcnt)>);
	static_assert(std::is_same_v<percent<int>, decltype(pcnt - pcnt)>);
	static_assert(std::is_same_v<meters<int>, decltype(length - length)>);

	static_assert(std::is_same_v<dimensionless<int>, decltype(dim * 1)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(1 * dim)>);
	static_assert(std::is_same_v<dimensionless<int>, decltype(dim * dim)>);
	// static_assert(std::is_same_v<percent<int>, decltype(pcnt * 1)>);
	// static_assert(std::is_same_v<percent<int>, decltype(1 * pcnt)>);
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
	static_assert(std::is_same_v<units::dimensionless<double>, decltype(1 / pcnt)>);
	static_assert(std::is_same_v<units::dimensionless<double>, decltype(pcnt / pcnt)>);

	static_assert(std::is_same_v<meters<int>, decltype(length / 1)>);
	static_assert(std::is_same_v<unit<inverse<meters<>>, int>, decltype(1 / length)>);
	static_assert(std::is_same_v<meters<int>, decltype(length / dim)>);
	static_assert(std::is_same_v<unit<inverse<meters<>>, int>, decltype(dim / length)>);
	static_assert(std::is_same_v<meters<int>, decltype(length / pcnt)>);
	static_assert(std::is_convertible_v<decltype(pcnt / length), unit<inverse<meters<>>, double>>);
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

// Regression guards for the class-based named-unit refactor: diagnostics/traits must report the FRIENDLY named
// type, and the trait/std specializations must see through the derived named class (not decay to the plain unit<...>
// base or hard-error). These lock in the behavior the coverage audit flagged as correct-but-untested.
TEST_F(UnitType, namedUnitReportedTypeIsPreserved)
{
	// (1) arithmetic RESULTS report the named type, not the equivalent-but-unnamed unit<strong_t<...>>.
	static_assert(std::is_same_v<decltype(meters<double>(2) * meters<double>(2)), square_meters<double>>);
	static_assert(std::is_same_v<decltype(meters<double>(10) / seconds<double>(2)), meters_per_second<double>>);
	static_assert(std::is_same_v<decltype(1.0 / seconds<double>(1)), hertz<double>>);       // inverse -> named
	static_assert(std::is_same_v<decltype(pow<3>(meters<double>(1))), cubic_meters<double>>);

	// (2) unit-math functions PRESERVE the named type on a dimensioned named input (audit: value-tested only before).
	static_assert(std::is_same_v<decltype(floor(meters<double>(1.5))), meters<double>>);
	static_assert(std::is_same_v<decltype(round(meters<double>(1.5))), meters<double>>);
	static_assert(std::is_same_v<decltype(trunc(meters<double>(1.5))), meters<double>>);
	static_assert(std::is_same_v<decltype(hypot(meters<double>(3), meters<double>(4))), meters<double>>);

	// (3) traits see through the derived named class.
	static_assert(std::is_same_v<traits::replace_underlying_t<meters<int>, double>, meters<double>>);
	static_assert(std::is_same_v<std::common_type_t<meters<int>, meters<double>>, meters<double>>);

	// (4) C1: std::numeric_limits<Named> returns the NAMED type (value AND type), not the plain base.
	static_assert(std::is_same_v<decltype(std::numeric_limits<meters<double>>::max()), meters<double>>);
	static_assert(std::is_same_v<decltype(std::numeric_limits<meters<double>>::lowest()), meters<double>>);
	EXPECT_EQ(std::numeric_limits<meters<double>>::max().to_linearized(), std::numeric_limits<double>::max());

	// (5) C2: common_type<dimensioned-named, scalar> is SFINAE-EMPTY (no `type`), exactly like the plain unit<...>
	//     form — never a hard error. A dimensionless-named + scalar still HAS a common type (interchangeable).
	static_assert(!detail::is_detected_v<common_type_of, meters<double>, double>, "dimensioned named + scalar: no common type (SFINAE-safe)");
	static_assert(!detail::is_detected_v<common_type_of, unit<conversion_factor<std::ratio<1>, dimension::length>, double>, double>, "plain dimensioned + scalar: also none (parity)");
	static_assert(detail::is_detected_v<common_type_of, percent<double>, double>, "dimensionless named + scalar: has a common type");

	// (6) abbreviation()/name() members resolve on the named form (incl. a COMPOUND named unit, per audit).
	EXPECT_STREQ("m", meters<double>(1).abbreviation());
	EXPECT_STREQ("meters", meters<double>(1).name());
	EXPECT_STREQ("mps", meters_per_second<double>(1).abbreviation());

	// (6a) minutes is defined via UNIT_ADD_WITH_PLURAL_TAG; that macro must still register the named class
	// so name()/abbreviation() resolve (a regression guard: they previously returned null, and a minutes
	// value streamed as its base unit "90 s" and could null-deref on name()).
	EXPECT_STREQ("min", minutes<double>(1).abbreviation());
	EXPECT_STREQ("minutes", minutes<double>(1).name());
	EXPECT_EQ(std::string("90 min"), to_string(minutes<double>(90)));

	// (7) the remaining unit-math functions PRESERVE the named type on a named input (audit: value-only before).
	static_assert(std::is_same_v<decltype(min(meters<double>(1), meters<double>(2))), meters<double>>);
	static_assert(std::is_same_v<decltype(max(meters<double>(1), meters<double>(2))), meters<double>>);
	static_assert(std::is_same_v<decltype(fmod(meters<double>(5), meters<double>(2))), meters<double>>);
	static_assert(std::is_same_v<decltype(copysign(meters<double>(3), -1.0)), meters<double>>);
	static_assert(std::is_same_v<decltype(fabs(meters<double>(-3))), meters<double>>);
	static_assert(std::is_same_v<decltype(abs(meters<double>(-3))), meters<double>>);
	static_assert(std::is_same_v<decltype(sqrt(square_meters<double>(4))), meters<double>>);

	// (8) a NON-registered derived CF stays the plain unit<...> (identity rewrap): dividing two unlike named units
	//     whose quotient has no named class must NOT invent a name. meters/kilograms has no named unit.
	static_assert(!detail::is_named_unit_v<decltype(meters<double>(1) / kilograms<double>(1))>,
		"a derived CF with no registered named class stays the plain unit<...>");
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

// The result of a same-dimension +/- is expressed in the LEFT operand's unit (the caller controls it by
// operand order), so the value reads in the unit they wrote and no anonymous sub-unit is conjured. The
// left-operand's underlying is widened to the common lossless unit only when it is integral and cannot hold
// the right operand without truncation — the case where integer exactness requires the finest common unit.
TEST_F(UnitType, arithmeticResultIsLeftOperandUnit)
{
	using namespace units::length;
	using namespace units::mass;

	// Floating point: the result is the LEFT operand's unit, both orders — caller controls it by ordering.
	static_assert(std::is_same_v<decltype(meters<double>(1) - feet<double>(1)), meters<double>>);
	static_assert(std::is_same_v<decltype(feet<double>(1) - meters<double>(1)), feet<double>>);
	static_assert(std::is_same_v<decltype(meters<double>(1) + feet<double>(1)), meters<double>>);
	static_assert(std::is_same_v<decltype(kilometers<double>(1) - meters<double>(1)), kilometers<double>>);

	// The value reads correctly in that left-operand unit.
	EXPECT_NEAR(7.73203815, (kilograms<double>(10) - units::mass::pounds<double>(5)).value(), 1.0e-6);   // 10 kg - 5 lb, in kg (LHS)
	EXPECT_NEAR(9.0, (meters<double>(10) - feet<double>(3.280839895013123)).value(), 1.0e-9);  // 10 m - 1 m in m

	// A named result carries its friendly name (not an anonymous unit): abbreviation is non-empty.
	EXPECT_STRNE("", (meters<double>(1) - feet<double>(1)).abbreviation());
	EXPECT_STREQ("m", (meters<double>(1) - feet<double>(1)).abbreviation());

	// Integer, commensurable: the left operand cannot hold the finer right operand losslessly, so the result
	// reconciles to the finer real named unit (meters), exact.
	static_assert(std::is_same_v<decltype(kilometers<int>(1) - meters<int>(500)), meters<int>>);
	EXPECT_EQ(500, (kilometers<int>(1) - meters<int>(500)).value());

	// Integer, incommensurable (meters vs feet): no real named unit holds the result without truncation, so it
	// falls to the finest common (anonymous) unit — the exact reconciliation. The VALUE stays correct.
	{
		const auto d = meters<int>(3) - feet<int>(1);   // 3 m - 1 ft, exact in the common sub-unit
		EXPECT_NEAR(2.6952, meters<double>(d).value(), 1.0e-4);
	}
}

// The common type of two same-dimension units recovers a friendly name even when the finer operand's unit is
// registered as a composed conversion factor (centimeters is centi<meters>, minutes is 60 seconds): the
// reconciliation result names centimeters / minutes rather than an anonymous sub-unit, while a cross-kind pair
// that shares a dimension and ratio (torque's newton_meters and energy's joules) stays symmetric and is not
// renamed from one kind to the other.
TEST_F(UnitType, commonTypeRecoversComposedName)
{
	using namespace units::length;
	using namespace units::time;

	using m_cm  = std::common_type_t<meters<double>, centimeters<double>>;
	using cm_m  = std::common_type_t<centimeters<double>, meters<double>>;
	using hr_min = std::common_type_t<hours<double>, minutes<double>>;
	EXPECT_STREQ("cm", m_cm().abbreviation());
	EXPECT_STREQ("cm", cm_m().abbreviation());
	EXPECT_STREQ("min", hr_min().abbreviation());   // hours (3600 s) and minutes (60 s) reconcile to minutes, the finer

	// common_type stays symmetric (a type function must be); recovery never breaks that.
	static_assert(std::is_same_v<std::common_type_t<meters<double>, centimeters<double>>,
								 std::common_type_t<centimeters<double>, meters<double>>>);
	// Cross-kind equivalent pair (same dimension AND ratio) is NOT renamed and stays order-independent.
	static_assert(std::is_same_v<std::common_type_t<units::torque::newton_meters<double>, units::energy::joules<double>>,
								 std::common_type_t<units::energy::joules<double>, units::torque::newton_meters<double>>>);

	// The name recovery preserves the RATIO (integer exactness): comparison across scales is unaffected.
	static_assert(units::length::kilometers<int>(1) == units::length::meters<int>(1000));
	static_assert(!(units::length::meters<int>(1) == units::length::feet<int>(3)));
}

// Affine (offset-carrying) units: the difference of two absolute temperatures is a DELTA — the datum
// offsets cancel and the result must not re-apply an offset. Previously celsius(0) - kelvin(0) read 546.30 K
// (the +273.15 offset was re-applied); the delta is 273.15 K. Absolute affine ADDITION is disabled (no
// physical meaning), so only subtraction is exercised here.
TEST_F(UnitType, affineTemperatureSubtractionIsADelta)
{
	using namespace units::temperature;
	EXPECT_NEAR(273.15, kelvin<double>(celsius<double>(0.0) - kelvin<double>(0.0)).value(), 5.0e-11);
	EXPECT_NEAR(100.0, kelvin<double>(celsius<double>(100.0) - fahrenheit<double>(32.0)).value(), 5.0e-11);
	EXPECT_NEAR(15.0, kelvin<double>(celsius<double>(20.0) - celsius<double>(5.0)).value(), 5.0e-11);
	EXPECT_NEAR(56.0, kelvin<double>(fahrenheit<double>(212.0) - fahrenheit<double>(111.2)).value(), 5.0e-9);
	// The result of an affine subtraction is a non-affine (delta) unit: converting it applies no offset.
	static_assert(!traits::is_affine_unit_v<decltype(celsius<double>(0.0) - kelvin<double>(0.0))>,
		"an affine temperature difference must be a non-affine delta type");
	// A non-affine same-dimension subtraction/addition is unaffected.
	EXPECT_NEAR(2.0, meters<double>(meters<double>(5.0) - meters<double>(3.0)).value(), 5.0e-12);
}

// Compound assignment on an affine unit treats the rhs as a RELATIVE delta and moves the absolute point in
// place ("warm/cool by N degrees"), staying in the lhs unit. This is the point-centric convenience: the
// point/delta distinction stays a quiet detail rather than a pervasive type calculus.
TEST_F(UnitType, affineTemperatureCompoundAssignmentMovesPoint)
{
	using namespace units::temperature;
	celsius<double> a(20.0);
	a += celsius<double>(5.0);
	EXPECT_NEAR(25.0, a.value(), 5.0e-12);    // warmed by 5 degrees, still absolute celsius
	a -= celsius<double>(10.0);
	EXPECT_NEAR(15.0, a.value(), 5.0e-12);    // cooled by 10 degrees

	// The result stays an absolute affine point (not converted to a delta type).
	static_assert(traits::is_affine_unit_v<decltype(a)>, "compound assignment keeps the affine point type");

	// Non-affine compound assignment is unchanged.
	meters<double> m(5.0);
	m += meters<double>(3.0);
	EXPECT_NEAR(8.0, m.value(), 5.0e-12);
	m -= meters<double>(2.0);
	EXPECT_NEAR(6.0, m.value(), 5.0e-12);
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
	auto n = 5_pct / 4_pct;
	static_assert(std::is_same_v<units::dimensionless<double>, decltype(n)>);
	EXPECT_DOUBLE_EQ(n, 1.25);

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

	// Coarser lhs: the result is the common (finer) unit, so the operator is not order-dependent. This
	// direction previously failed to compile (the result was declared as the lhs unit, and converting the
	// finer common result back to the coarser integer kilometers is lossy → the constructor was disabled).
	constexpr kilometers f_km(3);
	constexpr auto       g_m = f_km % b_m;    // 3000 m % 1800 m = 1200 m, in meters (the finer common unit)
	EXPECT_EQ(1200, g_m.value());
	static_assert(has_equivalent_conversion_factor(g_m, b_m));

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

	// Integer-percent iteration with modulo. `%` requires integral operands, so the modulus and remainder are
	// explicit percent<int> (a literal is floating-point); the iterator is likewise percent<int>. This exercises
	// that integer-backed ratio-dimensionless units support modulo arithmetic.
	std::vector<percent<int>> vec;
	for (percent<int> i(1); i <= 100_pct; ++i)
	{
		if (i % percent<int>(10) == percent<int>(0))
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

// True iff `a % b` / `a %= b` are valid expressions. `%` is an integer count operation on point values, so it is
// well-formed only when the operands share a tick scale and are integer-backed. These concepts let the contract be
// asserted directly: a case that must NOT compile is a `static_assert(!Can...<...>)`.
template<class A, class B>
concept can_modulo = requires(A a, B b) { a % b; };
template<class A, class B>
concept can_modulo_assign = requires(A a, B b) { a %= b; };

// The full modulo contract, proven exhaustively rather than by inference: which operand pairs `%`/`%=` accept and
// which it rejects at compile time. Integer-backed operands of the SAME tick scale (or a ratio unit modulo a plain
// dimensionless / bare integer) are accepted; two DIFFERENT ratio-scaled dimensionless units share no tick, and any
// floating-point operand is not a count, so both are rejected -- a floating remainder is `fmod`'s job.
TEST_F(UnitType, moduloContract)
{
	// Accepted: same tick scale, integer-backed.
	static_assert(can_modulo<percent<int>, percent<int>>);
	static_assert(can_modulo<parts_per_million<int>, parts_per_million<int>>);
	static_assert(can_modulo<meters<int>, meters<int>>);
	static_assert(can_modulo<meters<int>, kilometers<int>>);
	static_assert(can_modulo<dimensionless<int>, dimensionless<int>>);

	// Accepted: a ratio-scaled unit modulo a plain dimensionless or a bare integer (a pure count).
	static_assert(can_modulo<percent<int>, dimensionless<int>>);
	static_assert(can_modulo<dimensionless<int>, percent<int>>);
	static_assert(can_modulo<percent<int>, int>);
	static_assert(can_modulo<dimensionless<int>, int>);
	static_assert(can_modulo<meters<int>, int>);

	// Rejected: two DIFFERENT ratio-scaled dimensionless units share no tick scale, so the remainder of their point
	// counts has no meaning.
	static_assert(!can_modulo<percent<int>, parts_per_million<int>>);
	static_assert(!can_modulo<parts_per_million<int>, percent<int>>);
	static_assert(!can_modulo<percent<int>, parts_per_billion<int>>);
	static_assert(!can_modulo<parts_per_million<int>, parts_per_billion<int>>);

	// Rejected: `%` is integer-only. A floating-point operand is not a count; use `fmod` for a floating remainder.
	static_assert(!can_modulo<percent<double>, percent<double>>);
	static_assert(!can_modulo<meters<double>, meters<double>>);
	static_assert(!can_modulo<dimensionless<double>, dimensionless<double>>);
	static_assert(!can_modulo<percent<double>, dimensionless<double>>);
	static_assert(!can_modulo<meters<double>, int>);
	static_assert(!can_modulo<percent<int>, double>);

	// `%=` follows the same contract.
	static_assert(can_modulo_assign<percent<int>, percent<int>>);
	static_assert(can_modulo_assign<meters<int>, meters<int>>);
	static_assert(can_modulo_assign<percent<int>, dimensionless<int>>);
	static_assert(!can_modulo_assign<percent<int>, parts_per_million<int>>);
	static_assert(!can_modulo_assign<percent<double>, percent<double>>);
	static_assert(!can_modulo_assign<meters<double>, meters<double>>);

	// The accepted integer cases compute the expected count remainder.
	EXPECT_EQ(2_pct, percent<int>(12) % percent<int>(5));
	EXPECT_EQ(2_pct, percent<int>(12) % dimensionless<int>(5));
	EXPECT_EQ(meters<int>(1), meters<int>(10) % meters<int>(3));
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

	c *= dimensionless<int>(2);

	EXPECT_EQ((meters<int>(8)), c);

	c *= 2;

	EXPECT_EQ((meters<int>(16)), c);

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

	d_dim *= dimensionless<int>(2);

	EXPECT_EQ((dimensionless<int>(8)), d_dim);

	d_dim *= 2;

	EXPECT_EQ((dimensionless<int>(16)), d_dim);

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

	c /= dimensionless<int>(2);

	EXPECT_EQ((meters<int>(8)), c);

	c /= 2;

	EXPECT_EQ((meters<int>(4)), c);

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

	d_dim /= dimensionless<int>(2);

	EXPECT_EQ((dimensionless<int>(8)), d_dim);

	d_dim /= 2;

	EXPECT_EQ((dimensionless<int>(4)), d_dim);

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
	EXPECT_EQ(2_pct, b_s);

	b_s %= dimensionless<int>(5);
	EXPECT_EQ(2_pct, b_s);

	b_s %= 2;
	EXPECT_EQ(0_pct, b_s);
}

TEST_F(UnitType, PpbPerYearCompoundUnitType)
{
	using ppb_per_year = decltype(ppb/yr);

	parts_per_million concentration = 9.71_ppb;

	EXPECT_EQ(concentration, 0.00971_ppm);
	EXPECT_DOUBLE_EQ(concentration, 0.00000000971);

	ppb_per_year rate(0.109);

	auto elapsed = 2013.9_yr-1994_yr;

	parts_per_million val = concentration + rate*elapsed;
	EXPECT_NEAR(val, 1.18791e-08, 1e-12);
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

	// named-unit to<>(): returns a unit of the requested type, mirroring convert<>()
	constexpr auto asFeet = meters<double>(3.0).to<feet>();
	static_assert(std::is_same_v<std::remove_const_t<decltype(asFeet)>, feet<double>>);
	EXPECT_NEAR(9.84252, asFeet.to<double>(), 5.0e-6);

	constexpr auto asMeters = centimeters<double>(100.0).to<meters>();
	static_assert(std::is_same_v<std::remove_const_t<decltype(asMeters)>, meters<double>>);
	EXPECT_DOUBLE_EQ(1.0, asMeters.to<double>());

	// arithmetic-type to<>() is unchanged: extracts the underlying value
	EXPECT_DOUBLE_EQ(3.0, meters<double>(3.0).to<double>());

	// convert<>() is callable on a const unit (const-qualified overloads)
	const meters<double> constMeters(3.0);
	EXPECT_NEAR(9.84252, constMeters.convert<feet>().to<double>(), 5.0e-6);
	EXPECT_NEAR(9.84252, constMeters.to<feet>().to<double>(), 5.0e-6);
}

TEST_F(UnitType, integerConversionWidensIntermediate)
{
	// A mul-then-divide conversion (feet -> meters is 381/1250) computes the intermediate product in a
	// double-width integer, so `value * 381` does not overflow before `/ 1250` recovers a value that fits the
	// target. Without the widening this silently overflowed for large magnitudes.
	const auto big = units::convert<feet<std::int64_t>>(feet<std::int64_t>(0)); // (touch the header)
	(void)big;

	// 5e16 ft * 381 = 1.905e19 overflows int64 (max ~9.2e18), but 5e16 * 381 / 1250 = 1.524e16 fits.
	const std::int64_t v      = 50'000'000'000'000'000LL;
	const auto         meters = units::convert<units::length::meters<std::int64_t>>(feet<std::int64_t>(v));
	EXPECT_EQ(15'240'000'000'000'000LL, meters.value());

	// Ordinary and negative magnitudes are exact and unchanged (widening never alters a result that already fit).
	EXPECT_EQ(381, units::convert<units::length::meters<std::int64_t>>(feet<std::int64_t>(1250)).value());
	EXPECT_EQ(-381, units::convert<units::length::meters<std::int64_t>>(feet<std::int64_t>(-1250)).value());
	EXPECT_EQ(0, units::convert<units::length::meters<std::int64_t>>(feet<std::int64_t>(0)).value());

	// Floating-point conversions are unaffected (the widening is integer-only).
	EXPECT_NEAR(0.3048, units::convert<units::length::meters<double>>(feet<double>(1.0)).value(), 1e-9);
}

TEST_F(UnitType, floatingPointConversionIsCorrectlyRounded)
{
	// The floating-point conversion path is not widened (that would be platform-dependent for no gain); it is
	// already correctly rounded. Pin that so a future refactor cannot silently degrade it: the result must be
	// within half a ULP of the high-precision reference, and a round-trip must be bit-stable.
	for (double v : {1.0, 3.0, 1234.56789, 1.0e6, 987654321.123456, 1.0e15})
	{
		const double      lib = units::convert<units::length::meters<double>>(feet<double>(v)).value();
		const long double ref = static_cast<long double>(v) * 381.0L / 1250.0L;
		const double      ulp = static_cast<double>(std::nextafter(static_cast<double>(ref), static_cast<double>(ref) + 1.0) - static_cast<double>(ref));
		EXPECT_LE(std::abs(static_cast<long double>(lib) - ref), 0.5L * ulp);

		const double back = units::convert<feet<double>>(units::convert<units::length::meters<double>>(feet<double>(v))).value();
		EXPECT_DOUBLE_EQ(v, back); // round-trip bit-stable
	}
	// Affine + pi conversions stay exact to the last bit.
	EXPECT_DOUBLE_EQ(98.6, units::convert<fahrenheit<double>>(celsius<double>(37.0)).value());

	// A big value through a fractional ratio must not lose a representable answer to intermediate overflow:
	// 1e306 ft -> m is ~3.048e305 (fits double), even though value * 381 = 3.81e308 would overflow. The
	// conversion divides first in that regime and returns the finite result rather than infinity.
	const double extreme = units::convert<units::length::meters<double>>(feet<double>(1.0e306)).value();
	EXPECT_TRUE(std::isfinite(extreme));
	EXPECT_NEAR(3.048e305, extreme, 3.048e305 * 1e-12);
}

TEST_F(UnitType, mixedSignednessComparesByValue)
{
	// A signed-rep and an unsigned-rep unit of the same dimension compare by mathematical value, not by C++'s
	// int/unsigned wraparound: -1 m is less than 1 m even when one side is unsigned.
	EXPECT_TRUE(meters<int>(-1) < meters<unsigned>(1u));
	EXPECT_FALSE(meters<int>(-1) > meters<unsigned>(1u));
	EXPECT_FALSE(meters<int>(-1) == meters<unsigned>(1u));
	EXPECT_TRUE(meters<int>(-5) != meters<unsigned>(5u));  // NOT equal via wraparound
	EXPECT_TRUE(meters<int>(2) == meters<unsigned>(2u));   // genuinely equal
	EXPECT_TRUE(meters<unsigned>(1u) > meters<int>(-1));   // reversed operands

	// Same-signedness and cross-unit comparisons are unchanged.
	EXPECT_TRUE(meters<int>(3) < meters<int>(5));
	EXPECT_TRUE(kilometers<int>(1) == meters<int>(1000));
	EXPECT_TRUE(meters<int>(1500) > kilometers<int>(1));

	// Floating-point comparison is unaffected.
	EXPECT_TRUE(meters<double>(1.5) < meters<double>(2.5));
	EXPECT_TRUE(meters<double>(-1.0) < meters<double>(1.0));
}

TEST_F(UnitType, hashOfLargeValueDoesNotOverflow)
{
	// std::hash forwards through a unit conversion; a large value converted to a fine unit used to overflow the
	// intermediate. With the widened conversion the hash of a big value is computed without undefined behavior
	// (run under -fsanitize=undefined this must not trip). Equal values under one key type hash equally.
	const auto h = std::hash<millimeters<std::int64_t>>()(kilometers<std::int64_t>(3000)); // 3e9 mm
	EXPECT_EQ(h, std::hash<millimeters<std::int64_t>>()(millimeters<std::int64_t>(3'000'000'000LL)));
	EXPECT_EQ(std::hash<meters<int>>()(meters<int>(7)), std::hash<meters<int>>()(meters<int>(7)));
}

TEST_F(UnitType, exactIntegralNarrowingConstructor)
{
	using units::data::bits;
	using units::data::bytes;
	using units::data::nibbles;

	// A compile-time-known finer integral value that is an exact whole number of the coarser unit converts, at
	// compile time, to the exact count. `bits` is ratio<1,8> of `bytes`, so 16 bits is exactly 2 bytes.
	constexpr bytes<int> two = bits<int>(16);
	static_assert(two.value() == 2, "16 bits is 2 bytes");
	EXPECT_EQ(2, two.value());

	constexpr bytes<int> one = bits<int>(8);
	static_assert(one.value() == 1, "8 bits is 1 byte");

	// Negative carriers convert exactly (divisibility ignores sign).
	constexpr bytes<int> negTwo = bits<int>(-16);
	static_assert(negTwo.value() == -2, "-16 bits is -2 bytes");
	EXPECT_EQ(-2, negTwo.value());

	// A multi-step finer ratio: a nibble is 4 bits, so 2 nibbles is exactly 1 byte.
	constexpr bytes<int> fromNibbles = nibbles<int>(2);
	static_assert(fromNibbles.value() == 1, "2 nibbles is 1 byte");

	// The reverse direction is already lossless and unchanged (coarser -> finer never truncates).
	constexpr bits<int> sixteen = bytes<int>(2);
	static_assert(sixteen.value() == 16, "2 bytes is 16 bits");

	// The exactness test rides in a double-width intermediate, so a large exact value still converts (no overflow
	// of value*num before the divide). 2^60 bits is exactly 2^57 bytes.
	constexpr bytes<std::int64_t> big = bits<std::int64_t>(1LL << 60);
	static_assert(big.value() == (1LL << 57), "2^60 bits is 2^57 bytes");
}

TEST_F(UnitType, runtimeLossyRoundingConversion)
{
	using units::data::bits;
	using units::data::bytes;

	// A genuinely run-time finer value need not be a whole number of the coarser unit; the caller states the
	// rounding intent with the target-taking round/floor/ceil/trunc, mirroring std::chrono::floor<To>.
	const bits<int> seventeen(17); // 2.125 bytes
	EXPECT_EQ(2, units::floor<bytes<int>>(seventeen).value());
	EXPECT_EQ(3, units::ceil<bytes<int>>(seventeen).value());
	EXPECT_EQ(2, units::round<bytes<int>>(seventeen).value());
	EXPECT_EQ(2, units::trunc<bytes<int>>(seventeen).value());

	const bits<int> twenty(20); // 2.5 bytes -> round halfway away from zero
	EXPECT_EQ(2, units::floor<bytes<int>>(twenty).value());
	EXPECT_EQ(3, units::ceil<bytes<int>>(twenty).value());
	EXPECT_EQ(3, units::round<bytes<int>>(twenty).value());
	EXPECT_EQ(2, units::trunc<bytes<int>>(twenty).value());

	// Negative values distinguish floor (toward -inf) from trunc (toward zero).
	const bits<int> negSeventeen(-17); // -2.125 bytes
	EXPECT_EQ(-3, units::floor<bytes<int>>(negSeventeen).value());
	EXPECT_EQ(-2, units::ceil<bytes<int>>(negSeventeen).value());
	EXPECT_EQ(-2, units::round<bytes<int>>(negSeventeen).value());
	EXPECT_EQ(-2, units::trunc<bytes<int>>(negSeventeen).value());

	// An exactly-divisible run-time value rounds to itself under every mode.
	const bits<int> sixteen(16); // exactly 2 bytes
	EXPECT_EQ(2, units::floor<bytes<int>>(sixteen).value());
	EXPECT_EQ(2, units::ceil<bytes<int>>(sixteen).value());
	EXPECT_EQ(2, units::round<bytes<int>>(sixteen).value());
	EXPECT_EQ(2, units::trunc<bytes<int>>(sixteen).value());

	// The rounding is exact integer arithmetic, so a magnitude beyond 2^53 (where a double intermediate would lose
	// the fractional byte and round the wrong way) is still correct. 2^53+1 bits is 1125899906842624.125 bytes.
	const bits<std::int64_t> above2p53(9007199254740993LL);
	EXPECT_EQ(1125899906842624LL, units::floor<bytes<std::int64_t>>(above2p53).value());
	EXPECT_EQ(1125899906842625LL, units::ceil<bytes<std::int64_t>>(above2p53).value());
	EXPECT_EQ(1125899906842624LL, units::round<bytes<std::int64_t>>(above2p53).value());
	EXPECT_EQ(1125899906842624LL, units::trunc<bytes<std::int64_t>>(above2p53).value());
	// A byte count above 2^53 itself: 2^56+12 bits is 2^53+1.5 bytes, floor must land on the exact 2^53+1.
	EXPECT_EQ((std::int64_t(1) << 53) + 1, units::floor<bytes<std::int64_t>>(bits<std::int64_t>((std::int64_t(1) << 56) + 12)).value());
	// A large negative value: floor goes toward negative infinity even past 2^53.
	EXPECT_EQ(-((std::int64_t(1) << 57) + 1), units::floor<bytes<std::int64_t>>(bits<std::int64_t>(-((std::int64_t(1) << 60) + 5))).value());

	// A result that does not fit the target integer wraps like any integer narrowing (the semantics of
	// std::chrono::floor<To>), never an out-of-range floating-to-integer conversion: 3e9 bytes exceeds int.
	EXPECT_EQ(static_cast<int>(3000000000LL), units::floor<bytes<int>>(bits<std::int64_t>(8 * 3000000000LL)).value());

	// The target-taking overloads do not shadow the deduced-argument rounding math functions.
	EXPECT_DOUBLE_EQ(3.0, units::floor(meters<double>(3.7)).value());
	EXPECT_DOUBLE_EQ(4.0, units::ceil(meters<double>(3.7)).value());
	EXPECT_DOUBLE_EQ(4.0, units::round(meters<double>(3.7)).value());
	EXPECT_DOUBLE_EQ(3.0, units::trunc(meters<double>(3.7)).value());
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
	EXPECT_STREQ("8 ft3", output.c_str());

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

	// a unit with no abbreviation renders its DIMENSION form, and must match operator<< exactly (single-spaced)
	using canonical_acceleration = unit<conversion_factor<std::ratio<1>, dimension::acceleration>, double>;
	const canonical_acceleration d_accel(9.81);
	std::ostringstream streamed;
	streamed << d_accel;
	EXPECT_EQ(streamed.str(), to_string(d_accel));
	EXPECT_STREQ("9.81 m s^-2", to_string(d_accel).c_str());
}

// The platform spellings of the two locales this test needs, and the command that installs them.
#if defined(_MSC_VER)
static constexpr const char* GERMAN_LOCALE = "de-DE";
static constexpr const char* US_LOCALE     = "en-US";
static constexpr const char* LOCALE_HINT   = "install the German and US locales";
#elif defined(__APPLE__)
// BSD libc (macOS) only recognizes the canonical `.UTF-8` spelling, not glibc's `de_DE.utf8` alias.
static constexpr const char* GERMAN_LOCALE = "de_DE.UTF-8";
static constexpr const char* US_LOCALE     = "en_US.UTF-8";
static constexpr const char* LOCALE_HINT   = "install the German and US locales";
#else
static constexpr const char* GERMAN_LOCALE = "de_DE.utf8";
static constexpr const char* US_LOCALE     = "en_US.utf8";
static constexpr const char* LOCALE_HINT   = "install the German and US locales, e.g. `sudo locale-gen de_DE.UTF-8 en_US.UTF-8`";
#endif

TEST_F(UnitType, to_string_locale)
{
	struct lconv*     lc;
	std::string       output;
	std::stringstream os1;
	std::stringstream os2;

	// A locale this test needs may not be present on the host; that is an environmental precondition,
	// not a library defect, so skip (with the install hint) rather than throw from std::locale.
	if (setlocale(LC_ALL, GERMAN_LOCALE) == nullptr || setlocale(LC_ALL, US_LOCALE) == nullptr)
	{
		GTEST_SKIP() << "requires the German and US locales; " << LOCALE_HINT;
	}

	// German locale
	setlocale(LC_ALL, GERMAN_LOCALE);
	os1.imbue(std::locale(GERMAN_LOCALE));

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
	setlocale(LC_ALL, US_LOCALE);
	os2.imbue(std::locale(US_LOCALE));

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

TEST_F(UnitType, dimensionlessDecibelLiteral)
{
	// the `_dB` literal yields the dimensionless decibel; its stored value is the dB figure
	auto gainDb = -20.0_dB;
	static_assert(std::is_same_v<decltype(gainDb), decibels<double>>);
	EXPECT_DOUBLE_EQ(-20.0, gainDb.raw());

	// only a floating-point _dB literal exists; an integer decibel is rejected at compile time
	// (see the errorMessages harness), because a decibel scale cannot use an integral underlying type
	auto gainDb2 = 6.0_dB;
	static_assert(std::is_same_v<decltype(gainDb2), decibels<double>>);

	// name/abbreviation resolve for the dimensionless decibel
	decibels<double> ratioDb(6.0);
	EXPECT_STREQ("decibels", ratioDb.name());
	EXPECT_STREQ("dB", ratioDb.abbreviation());

	// coexists with the power decibel literals (distinct types, distinct suffixes)
	static_assert(std::is_same_v<decltype(0.0_dBW), dBW<double>>);
	static_assert(std::is_same_v<decltype(0.0_dBm), dBm<double>>);
	static_assert(!std::is_same_v<decibels<double>, dBW<double>>);

	// a floating-point underlying type other than double is allowed (only integral types are rejected)
	decibels<float> asFloat(20.0f);
	EXPECT_NEAR(100.0, static_cast<double>(asFloat.to_linearized()), 5.0e-4);

#if !defined(UNIT_LIB_DISABLE_IOSTREAM)
	// the streamed form of a decibel-arithmetic result uses the dB abbreviation (adding dB multiplies linear)
	testing::internal::CaptureStdout();
	std::cout << (decibels<double>(3.0) + decibels<double>(3.0));
	std::string output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("6 dB", output.c_str());
#endif
}

TEST_F(UnitType, dBAddition)
{
	bool isSame;

	auto result_dbw = dBW<double>(10.0) + decibels<double>(30.0);
	EXPECT_NEAR(40.0, result_dbw.value(), 5.0e-5);
	result_dbw = dBW<double>(10.0) + decibels<double>(30.0);
	EXPECT_NEAR(40.0, result_dbw.value(), 5.0e-5);
	result_dbw = decibels<double>(12.0) + dBW<double>(30.0);
	EXPECT_NEAR(42.0, result_dbw.value(), 5.0e-5);
	result_dbw = decibels<double>(12.0) + dBW<double>(30.0);
	EXPECT_NEAR(42.0, result_dbw.value(), 5.0e-5);
	isSame = std::is_same_v<decltype(result_dbw), dBW<double>>;
	EXPECT_TRUE(isSame);

	auto result_dbm = decibels<double>(30.0) + dBm<double>(20.0);
	EXPECT_NEAR(50.0, result_dbm.value(), 5.0e-5);
	result_dbm = decibels<double>(30.0) + dBm<double>(20.0);
	EXPECT_NEAR(50.0, result_dbm.value(), 5.0e-5);

	// Adding two absolute decibel LEVELS (dBW + dBm, both dimensioned) is a point + point and is ill-formed:
	// two power levels do not sum by adding their dB numbers (that would be a product of powers). The addition
	// operator for two dimensioned same-dimension decibel operands is deleted; the compile-time rejection is
	// proven by the errorMessages case decibel_level_plus_level.cpp (a deleted overload is still selected by
	// overload resolution, so it cannot be probed with a `requires` expression — the negative test lives there).
}

TEST_F(UnitType, dBAffineSemantics)
{
	// A dimensioned decibel value (dBW, dBm) is an absolute LEVEL — a point on a logarithmic reference scale.
	// A dimensionless decibel (decibels) is a relative GAIN — a delta. The defined operations mirror an affine
	// space: level + gain -> level, gain + gain -> gain, level - level -> gain. (level + level is ill-formed;
	// see dBAddition and the decibel_level_plus_level errorMessages case.)

	// level + gain -> level (the point stays a dimensioned power level, moved by the gain)
	const auto boosted = dBW<double>(10.0) + decibels<double>(3.0);
	EXPECT_NEAR(13.0, boosted.value(), 5.0e-5);
	static_assert(std::is_same_v<std::remove_const_t<decltype(boosted)>, dBW<double>>, "level + gain stays a level");
	static_assert(traits::is_power_unit_v<decltype(boosted)>, "a dBW level is a power");

	// gain + gain -> gain (two relative ratios compound; their dB numbers add)
	const auto chained = decibels<double>(3.0) + decibels<double>(3.0);
	EXPECT_NEAR(6.0, chained.value(), 5.0e-5);
	static_assert(traits::is_dimensionless_unit_v<decltype(chained)>, "gain + gain is a dimensionless gain");

	// level - level -> gain (the ratio of two levels is a relative dB, i.e. a delta)
	const auto ratio = dBW<double>(30.0) - dBW<double>(10.0);
	EXPECT_NEAR(20.0, ratio.value(), 5.0e-5);
	static_assert(traits::is_dimensionless_unit_v<decltype(ratio)>, "level - level is a dimensionless gain");
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
	// A literal is always floating-point — an integer literal (16_m) yields the same type as 16.0_m, matching
	// the unit-constant form (16 * m is also floating-point), so a value written inline never silently becomes
	// integer-backed (16_m / 5_m is 3.2, not 3). An integer-backed quantity remains available explicitly
	// (meters<int>(16)) or by CTAD from an integer argument (meters(16)); a whole-number literal converts into
	// one at compile time (meters<int> m = 16_m), while a fractional literal is a compile error.
	static_assert(std::is_same_v<decltype(16.2_m), meters<double>>);
	static_assert(std::is_same_v<decltype(16_m), meters<double>>);
	static_assert(std::is_same_v<decltype(16_m), decltype(16.0_m)>);
	EXPECT_TRUE(meters<double>(16.2) == 16.2_m);
	EXPECT_TRUE(meters<double>(16) == 16.0_m);
	EXPECT_TRUE(meters<double>(16) == 16_m);
	EXPECT_DOUBLE_EQ(3.2, (16_m / 5_m).value());   // floating-point division, not integer truncation
	static_assert(std::is_same_v<decltype(meters<int>(16)), meters<int>>);   // integer reachable explicitly
	static_assert(std::is_same_v<decltype(meters(16)), meters<int>>);        // and by CTAD from an int argument
	EXPECT_EQ(16, (meters<int>{16_m}).value());     // whole-number literal narrows into meters<int> at compile time
	EXPECT_TRUE(meters<int>(16) == 16_m);           // meters<int> compares equal to the floating-point literal

	static_assert(std::is_same_v<decltype(11.2_ft), feet<double>>);
	static_assert(std::is_same_v<decltype(11_ft), feet<double>>);
	EXPECT_TRUE(feet<double>(11.2) == 11.2_ft);
	EXPECT_TRUE(feet<double>(11) == 11.0_ft);
	EXPECT_EQ(11, (feet<int>{11_ft}).value());      // whole-number literal narrows into feet<int>
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

// A whole-number floating-point quantity converts into an integer-backed unit of the same dimension at
// compile time; the ordinary run-time converting constructor still rejects a floating-to-integral conversion
// (a fractional or run-time value is ill-formed — proven by the errorMessages case
// narrow_fractional_literal_to_int.cpp). This is what lets `feet<int> f = 16_ft;` compile now that a literal
// is floating-point, without allowing a lossy run-time narrowing.
TEST_F(UnitType, compileTimeNarrowingToIntegral)
{
	// Whole-number literal narrows into the same unit's integral form, at compile time.
	static_assert(meters<int>{16_m}.value() == 16);
	static_assert(feet<int>{11_ft}.value() == 11);

	// Cross-unit whole conversions narrow when exact: 1000 m is exactly 1 km.
	static_assert(kilometers<int>{1000_m}.value() == 1);
	static_assert(meters<int>{1_km}.value() == 1000);

	// Ratio-dimensionless units narrow on their stored point count, not the fraction: 50_pct is percent<int> 50.
	static_assert(percent<int>{50_pct}.raw() == 50);

	// Widening int -> double is a normal implicit conversion.
	static_assert(std::is_constructible_v<meters<double>, meters<int>>);
	// The floating-to-integral narrowing constructor is consteval, so it is well-formed only in a constant
	// expression — a run-time value cannot invoke it, and a fractional value is ill-formed even in one. Both
	// rejections are proven by the errorMessages case narrow_fractional_literal_to_int.cpp; a type trait cannot
	// express "constructible only in a constant expression," so the guard lives there rather than as a
	// static_assert here.

	// Run-time confirmation the compile-time narrowing stored the right value.
	constexpr feet<int> f{16_ft};
	EXPECT_EQ(16, f.value());
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

	static_assert(2 * m * (3 * m) == 6 * m2);

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
	EXPECT_NEAR(14.593902937206364, test, 5.0e-13);

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

TEST_F(ConversionFactor, volume_flow_rate)
{
	double test;
	bool   same;

	same = std::is_same_v<cubic_meters_per_second<double>::conversion_factor, traits::strong_t<conversion_factor<std::ratio<1>, dimension::volume_flow_rate>>>;
	EXPECT_TRUE(same);

	same = traits::is_same_dimension_unit_v<liters_per_second<double>, cubic_meters_per_second<double>>;
	EXPECT_TRUE(same);

	// a volume divided by a time is a volume flow rate (dimension derived from volume/time)
	same = traits::is_same_dimension_unit_v<decltype(liters<double>(1.0) / seconds<double>(1.0)), liters_per_second<double>>;
	EXPECT_TRUE(same);

	test = liters_per_second<double>(cubic_meters_per_second<double>(1.0)).value();
	EXPECT_DOUBLE_EQ(1000.0, test);
	test = liters_per_minute<double>(liters_per_second<double>(1.0)).value();
	EXPECT_DOUBLE_EQ(60.0, test);
	test = cubic_feet_per_minute<double>(cubic_feet_per_second<double>(1.0)).value();
	EXPECT_DOUBLE_EQ(60.0, test);
	test = liters_per_second<double>(gallons_per_minute<double>(1.0)).value();
	EXPECT_NEAR(0.0630901964, test, 5.0e-10);
	test = liters_per_second<double>(cubic_feet_per_second<double>(1.0)).value();
	EXPECT_NEAR(28.316846592, test, 5.0e-9);
	test = cubic_meters_per_hour<double>(gallons_per_minute<double>(100.0)).value();
	EXPECT_NEAR(22.712470704, test, 5.0e-9);
	test = cubic_meters_per_hour<double>(cubic_feet_per_minute<double>(1.0)).value();
	EXPECT_NEAR(1.69901079552, test, 5.0e-11);

	// each remaining named unit checked against its most natural base
	test = cubic_meters_per_hour<double>(cubic_meters_per_second<double>(1.0)).value();
	EXPECT_DOUBLE_EQ(3600.0, test);
	test = gallons_per_minute<double>(gallons_per_hour<double>(60.0)).value();
	EXPECT_DOUBLE_EQ(1.0, test);
	test = liters_per_second<double>(gallons_per_hour<double>(1.0)).value();
	EXPECT_NEAR(0.00105150327, test, 5.0e-11);

	// deriving the dimension from volume / time means the composed quantity IS the named unit: it
	// resolves to liters_per_second and streams with that unit's abbreviation, and converts as expected
	same = std::is_same_v<decltype(5.0_L / 1.0_s), liters_per_second<double>>;
	EXPECT_TRUE(same);
	test = gallons_per_minute<double>(5.0_L / 1.0_s).value();
	EXPECT_NEAR(79.2516157, test, 5.0e-7);
#if !defined(UNIT_LIB_DISABLE_IOSTREAM)
	{
		testing::internal::CaptureStdout();
		std::cout << (5.0_L / 1.0_s);
		std::string output = testing::internal::GetCapturedStdout();
		EXPECT_STREQ("5 L_per_s", output.c_str());
	}
#endif
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

TEST_F(ConversionFactor, angular_acceleration_and_jerk)
{
	double test;
	bool   same;

	same = std::is_same_v<radians_per_second_squared<double>::conversion_factor, traits::strong_t<conversion_factor<std::ratio<1>, dimension::angular_acceleration>>>;
	EXPECT_TRUE(same);
	same = std::is_same_v<radians_per_second_cubed<double>::conversion_factor, traits::strong_t<conversion_factor<std::ratio<1>, dimension::angular_jerk>>>;
	EXPECT_TRUE(same);

	test = radians_per_second_squared<double>(2.25).value();
	EXPECT_EQ(2.25, test);
	test = radians_per_second_squared<double>(degrees_per_second_squared<double>(37.5)).value();
	EXPECT_NEAR(0.65449847, test, 5.0e-8); // 37.5 deg = 37.5 * pi/180 rad

	// The remaining angular-acceleration measures convert into radians per second squared: a gradian is 1/400 turn,
	// a turn (one revolution) is 2 pi radians, and a metric prefix scales the SI unit. Fractional inputs are used so a
	// wrong conversion factor produces a visibly wrong result rather than coinciding on a whole number.
	test = radians_per_second_squared<double>(gradians_per_second_squared<double>(37.5)).value();
	EXPECT_NEAR(0.58904862, test, 5.0e-8); // 37.5 gon = 37.5/400 turn = 0.09375 turn = 0.58904862 rad
	test = radians_per_second_squared<double>(revolutions_per_second_squared<double>(2.5)).value();
	EXPECT_NEAR(15.70796327, test, 5.0e-8); // 2.5 rev = 5 pi rad
	test = revolutions_per_minute_squared<double>(radians_per_second_squared<double>(1.0)).value();
	EXPECT_NEAR(572.95779513, test, 5.0e-8); // 1 rad/s^2 = 3600/(2pi) rev/min^2
	test = milliradians_per_second_squared<double>(radians_per_second_squared<double>(0.125)).value();
	EXPECT_NEAR(125.0, test, 5.0e-8);

	test = radians_per_second_cubed<double>(2.75).value();
	EXPECT_EQ(2.75, test);
	test = radians_per_second_cubed<double>(degrees_per_second_cubed<double>(37.5)).value();
	EXPECT_NEAR(0.65449847, test, 5.0e-8); // 37.5 deg = 37.5 * pi/180 rad
	test = radians_per_second_cubed<double>(gradians_per_second_cubed<double>(37.5)).value();
	EXPECT_NEAR(0.58904862, test, 5.0e-8);
	test = radians_per_second_cubed<double>(revolutions_per_second_cubed<double>(2.5)).value();
	EXPECT_NEAR(15.70796327, test, 5.0e-8);
	test = kiloradians_per_second_cubed<double>(radians_per_second_cubed<double>(1234.5)).value();
	EXPECT_NEAR(1.2345, test, 5.0e-12);

	same = traits::is_same_dimension_unit_v<decltype(radians_per_second<double>(1.0) / seconds<double>(1.0)), radians_per_second_squared<double>>;
	EXPECT_TRUE(same);
	same = traits::is_same_dimension_unit_v<decltype(radians_per_second_squared<double>(1.0) / seconds<double>(1.0)), radians_per_second_cubed<double>>;
	EXPECT_TRUE(same);
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
	// 1 statvolt == c/1e6 volts == 299.792458 V (was defined inverted before 3.4.2).
	test = volts<double>(statvolts<double>(1.0)).value();
	EXPECT_NEAR(299.792458, test, 5.0e-5);
	test = statvolts<double>(volts<double>(299.792458)).value();
	EXPECT_NEAR(1.0, test, 5.0e-5);
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

	test = newton_meters<double>(torque::pound_feet<double>(1.0)).value();
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
	EXPECT_NEAR(515.3788183931962, test, 5.0e-11);
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
	static_assert(traits::is_energy_density_unit_v<decltype(J / m3)>);
	static_assert(traits::is_pressure_unit_v<decltype(J / m3)>); // energy density IS pressure

	constexpr joules_per_meter_cubed test = 64.0 * J / 2.0_m3;
	EXPECT_EQ(test, 32.0_J_per_m3);
}

TEST_F(ConversionFactor, irradiance)
{
	static_assert(std::is_same_v<watts_per_meter_squared<double>, decltype(W / m2)>);
	static_assert(traits::is_irradiance_unit_v<decltype(W / m2)>);

	constexpr watts_per_meter_squared test = 42.0 * W / 2.0_m2;
	EXPECT_EQ(test, 21.0_W_per_m2);
}

TEST_F(ConversionFactor, radiance)
{
	static_assert(std::is_same_v<watts_per_steradian_per_meter_squared<double>, decltype(W / sr / m2)>);
	static_assert(traits::is_radiance_unit_v<decltype(W / sr / m2)>);

	constexpr watts_per_steradian_per_meter_squared test = 44.0 * W / 2.0_sr / 2.0_m2;
	EXPECT_EQ(test, 11.0_W_per_srm2);
}

TEST_F(ConversionFactor, radiant_intensity)
{
	static_assert(std::is_same_v<watts_per_steradian<double>, decltype(W / sr)>);
	static_assert(traits::is_radiant_intensity_unit_v<decltype(W / sr)>);

	constexpr watts_per_steradian test = 44.0 * W / 2.0_sr;
	EXPECT_EQ(test, 22.0_W_per_sr);
}

TEST_F(ConversionFactor, spectral_flux)
{
	static_assert(std::is_same_v<watts_per_meter<double>, decltype(W / m)>);
	static_assert(traits::is_spectral_flux_unit_v<decltype(W / m)>);

	constexpr watts_per_meter test = 44.0 * W / 4.0_m;
	EXPECT_EQ(test, 11.0_W_per_m);
}

TEST_F(ConversionFactor, spectral_intensity)
{
	static_assert(std::is_same_v<watts_per_steradian_per_meter<double>, decltype(W / sr / m)>);
	static_assert(traits::is_spectral_intensity_unit_v<decltype(W / sr / m)>);

	constexpr watts_per_steradian_per_meter test = 44.0 * W / 2.0_sr / 4.0_m;
	EXPECT_EQ(test, 5.5_W_per_srm);
}

TEST_F(ConversionFactor, spectral_irradiance)
{
	static_assert(std::is_same_v<watts_per_meter_cubed<double>, decltype(W / m3)>);
	static_assert(traits::is_spectral_irradiance_unit_v<decltype(W / m3)>);

	constexpr watts_per_meter_cubed test = 44.0 * W / 2.0_m3;
	EXPECT_EQ(test, 22_W_per_m3);
}

TEST_F(ConversionFactor, spectral_radiance)
{
	static_assert(std::is_same_v<watts_per_steradian_per_meter_cubed<double>, decltype(W / sr / m3)>);
	static_assert(traits::is_spectral_radiance_unit_v<decltype(W / sr / m3)>);

	constexpr watts_per_steradian_per_meter_cubed test = 44.0 * W / 2.0_sr / 2.0_m3;
	EXPECT_EQ(test, 11_W_per_srm3);
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

TEST_F(ConversionFactor, unitsAddedIn3_4_2)
{
	// Each new unit is checked against the exact ratio to its canonical parent.
	// length
	EXPECT_DOUBLE_EQ(4.0, rods<double>(chains<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(100.0, links<double>(chains<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(1.0, inches<double>(barleycorns<double>(3.0)).value());
	EXPECT_DOUBLE_EQ(1.0, yards<double>(nails<double>(16.0)).value());
	EXPECT_DOUBLE_EQ(9.0, inches<double>(spans<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(1.0, inches<double>(picas<double>(6.0)).value());
	EXPECT_DOUBLE_EQ(1.0, inches<double>(points<double>(72.0)).value());
	// velocity
	EXPECT_DOUBLE_EQ(1.0, feet_per_second<double>(feet_per_minute<double>(60.0)).value());
	EXPECT_DOUBLE_EQ(1000.0, meters_per_second<double>(kilometers_per_second<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(0.0254, meters_per_second<double>(inches_per_second<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(1.0, feet_per_second<double>(meters_per_minute<double>(18.288)).value());
	// area
	EXPECT_DOUBLE_EQ(1.0, acres<double>(roods<double>(4.0)).value());
	EXPECT_DOUBLE_EQ(1.0, acres<double>(square_rods<double>(160.0)).value());
	// angle
	EXPECT_DOUBLE_EQ(1.0, turns<double>(angular_mils<double>(6400.0)).value());
	EXPECT_DOUBLE_EQ(1.0, turns<double>(compass_points<double>(32.0)).value());
	// time
	EXPECT_DOUBLE_EQ(14.0, days<double>(fortnights<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(10.0, julian_years<double>(decades<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(100.0, julian_years<double>(centuries<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(1000.0, julian_years<double>(millennia<double>(1.0)).value());
	// data
	EXPECT_DOUBLE_EQ(1.0, bytes<double>(nibbles<double>(2.0)).value());
	// radiation
	EXPECT_DOUBLE_EQ(0.01, sieverts<double>(roentgens_equivalent_man<double>(1.0)).value());
	// substance
	EXPECT_DOUBLE_EQ(453.59237, mols<double>(pound_moles<double>(1.0)).value());
	// mass
	EXPECT_DOUBLE_EQ(1.0, mass::pounds<double>(grains<double>(7000.0)).value());
	EXPECT_DOUBLE_EQ(1.0, mass::ounces<double>(avoirdupois_drams<double>(16.0)).value());
	EXPECT_DOUBLE_EQ(480.0, grains<double>(troy_ounces<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(12.0, troy_ounces<double>(troy_pounds<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(24.0, grains<double>(pennyweights<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(112.0, mass::pounds<double>(hundredweights<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(100.0, mass::pounds<double>(short_hundredweights<double>(1.0)).value());
	// force
	EXPECT_DOUBLE_EQ(1000.0, force::pounds<double>(kips<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(1.0, force::pounds<double>(ounces_force<double>(16.0)).value());
	EXPECT_DOUBLE_EQ(0.00980665, newtons<double>(grams_force<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(2000.0, force::pounds<double>(short_tons_force<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(2240.0, force::pounds<double>(long_tons_force<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(1000.0, newtons<double>(sthenes<double>(1.0)).value());
	// pressure
	EXPECT_DOUBLE_EQ(98066.5, pascals<double>(technical_atmospheres<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(1000.0, pounds_per_square_inch<double>(kips_per_square_inch<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(0.1, pascals<double>(baryes<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(1000.0, pascals<double>(piezes<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(98.0665, pascals<double>(centimeters_of_water<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(9.80665, pascals<double>(millimeters_of_water<double>(1.0)).value());
	EXPECT_NEAR(1.0, pounds_per_square_inch<double>(pounds_per_square_foot<double>(144.0)).value(), 1e-9);
	// energy
	EXPECT_DOUBLE_EQ(1.0e-7, joules<double>(ergs<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(4.1868, joules<double>(calories_it<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(4.184e9, joules<double>(tons_of_tnt<double>(1.0)).value());
	// power
	EXPECT_DOUBLE_EQ(735.49875, watts<double>(metric_horsepower<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(746.0, watts<double>(electrical_horsepower<double>(1.0)).value());
	// charge / current
	EXPECT_DOUBLE_EQ(10.0, coulombs<double>(abcoulombs<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(10.0, amperes<double>(abamperes<double>(1.0)).value());
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

// min/max return the LEFT operand's named unit when that is lossless (both operands floating point, or the right
// converts into the left without integer truncation), matching operator+/-. They select an operand, so they do NOT
// floating-point promote: an integral same-unit min stays integral, and the integer-lossy fallback is the exact
// std::common_type_t of the two units (unlike hypot/fmax, which promote). The floating-point cases return a NAMED
// unit -- never an anonymous common unit for otherwise-representable operands: min(5m, 3ft) is meters<double>(0.9144),
// not an anonymous 1/381-foot unit holding 1143. clamp mirrors this in the value's own unit.
TEST_F(UnitMath, minMaxClampReturnLeftOperandUnit)
{
	// Return type: floating-point operands -> the LEFT operand's named unit, in either order (the anonymous common
	// unit never leaks for representable floating-point operands).
	static_assert(std::is_same_v<decltype(min(meters<double>(5), feet<double>(3))), meters<double>>);
	static_assert(std::is_same_v<decltype(min(feet<double>(3), meters<double>(5))), feet<double>>);
	static_assert(std::is_same_v<decltype(max(meters<double>(5), feet<double>(3))), meters<double>>);
	static_assert(std::is_same_v<decltype(max(feet<double>(3), meters<double>(5))), feet<double>>);

	// General across dimensions: the same left-operand-unit rule holds for time.
	static_assert(std::is_same_v<decltype(min(seconds<double>(90), minutes<double>(1))), seconds<double>>);
	static_assert(std::is_same_v<decltype(min(minutes<double>(1), seconds<double>(90))), minutes<double>>);
	static_assert(std::is_same_v<decltype(max(seconds<double>(90), minutes<double>(1))), seconds<double>>);
	static_assert(std::is_same_v<decltype(max(minutes<double>(1), seconds<double>(90))), minutes<double>>);

	// Integer-lossy fallback: feet<int> does not convert into meters<int> without truncation, so the result is NOT the
	// left int unit but the exact common unit -- min/max select an operand and do not promote, so no float wrapping.
	static_assert(!std::is_same_v<decltype(min(meters<int>(5), feet<int>(3))), meters<int>>);
	static_assert(std::is_same_v<decltype(min(meters<int>(5), feet<int>(3))), std::common_type_t<meters<int>, feet<int>>>);
	static_assert(!std::is_same_v<decltype(max(meters<int>(5), feet<int>(3))), meters<int>>);
	static_assert(std::is_same_v<decltype(max(meters<int>(5), feet<int>(3))), std::common_type_t<meters<int>, feet<int>>>);

	// Same-unit integral stays integral -- no float promotion when both operands share the unit.
	static_assert(std::is_same_v<decltype(min(meters<int>(5), meters<int>(3))), meters<int>>);
	static_assert(std::is_same_v<decltype(max(meters<int>(5), meters<int>(3))), meters<int>>);

	// clamp returns the value's own unit when lossless, including the cross-unit case (lo/hi in another named unit).
	static_assert(std::is_same_v<decltype(clamp(meters<double>(2), feet<double>(1), meters<double>(3))), meters<double>>);
	static_assert(std::is_same_v<decltype(clamp(feet<double>(0.5), feet<double>(1), feet<double>(3))), feet<double>>);

	// Values: min/max carry the correct physical quantity in the left operand's unit.
	EXPECT_NEAR(0.9144, min(meters<double>(5), feet<double>(3)).value(), 5.0e-9);   // 3 ft expressed in meters
	EXPECT_DOUBLE_EQ(5.0, max(meters<double>(5), feet<double>(3)).value());
	EXPECT_EQ(3, min(meters<int>(5), meters<int>(3)).value());

	// clamp: above hi -> hi, below lo -> lo, in range -> value, each in the value's unit.
	EXPECT_DOUBLE_EQ(3.0, clamp(meters<double>(5), meters<double>(1), meters<double>(3)).value());
	EXPECT_DOUBLE_EQ(1.0, clamp(meters<double>(0.0), meters<double>(1), meters<double>(3)).value());
	EXPECT_DOUBLE_EQ(2.0, clamp(meters<double>(2), meters<double>(1), meters<double>(3)).value());
	EXPECT_DOUBLE_EQ(1.0, clamp(feet<double>(0.5), feet<double>(1), feet<double>(3)).value());

	// Cross-unit clamp: value 2 m is within [1 ft, 3 m], returned in the value's unit (meters).
	EXPECT_DOUBLE_EQ(2.0, clamp(meters<double>(2), feet<double>(1), meters<double>(3)).value());
}

// Regression for issue #394: fmax/fmin select the larger/smaller quantity and keep the result in the common
// unit's raw scale, including for ratio-scaled dimensionless units. fmax(50%, 25%) is 50% (raw 50), not 0.5%.
// The functions have a single evaluation path built on .raw() (not the normalized .value()), so the C++23
// constexpr result and the runtime result are necessarily identical -- there is no separate constexpr branch
// that could diverge, contrary to the issue's premise.
TEST_F(UnitMath, fmaxFminKeepRawScale)
{
	EXPECT_EQ(percent<double>(50), fmax(percent<double>(50), percent<double>(25)));
	EXPECT_EQ(percent<double>(25), fmin(percent<double>(50), percent<double>(25)));
	EXPECT_DOUBLE_EQ(50.0, fmax(percent<double>(50), percent<double>(25)).raw());
	EXPECT_DOUBLE_EQ(25.0, fmin(percent<double>(50), percent<double>(25)).raw());

	// Mixed dimensioned units resolve in their common unit and pick the physically larger/smaller value.
	EXPECT_EQ(meters<double>(1), fmax(meters<double>(1), centimeters<double>(50)));
	EXPECT_EQ(centimeters<double>(50), fmin(meters<double>(1), centimeters<double>(50)));

	// The constexpr result equals the runtime result -- the single .raw() path cannot diverge at compile time.
	// Guarded to standard libraries whose <cmath> makes fmax/fmin constexpr (C++23 P0533).
#if defined(__cpp_lib_constexpr_cmath) || (defined(__GLIBCXX__) && !defined(__clang__))
	static_assert(fmax(percent<double>(50), percent<double>(25)).raw() == 50.0, "constexpr fmax must be 50%, not 0.5%");
	static_assert(fmin(percent<double>(50), percent<double>(25)).raw() == 25.0, "constexpr fmin must be 25%");
#endif
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
	auto input1  = -0.41614683654;
	auto input2  = 0;
	auto input3  = -0.70710678118654752440084436210485;
	auto input4  = 0;
	auto out1 = 2;
	auto out2 = 1.570796326795;
	auto out3 = 135;
	auto out4 = 90;
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), acos(dimensionless<double>(input1)).to<double>(), 5.0e-11);
	EXPECT_NEAR(angle::radians<double>(out2).to<double>(), acos(dimensionless<int>(input2)).to<double>(), 5.0e-11);
	EXPECT_NEAR(angle::degrees<double>(out3).to<double>(), angle::degrees<double>(acos(dimensionless<double>(input3))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degrees<double>(out4).to<double>(), angle::degrees<double>(acos(dimensionless<int>(input4))).to<double>(), 5.0e-12);
	auto uin1 = input1 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin2 = input2 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin3 = input3 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin4 = input4 * 1.0_m * (1.0 / (1000.0_mm));
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), acos(uin1).to<double>(), 5.0e-11);
	EXPECT_NEAR(angle::radians<double>(out2).to<double>(), acos(uin2).to<double>(), 5.0e-11);
	EXPECT_NEAR(angle::degrees<double>(out3).to<double>(), angle::degrees<double>(acos(uin3)).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degrees<double>(out4).to<double>(), angle::degrees<double>(acos(uin4)).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, asin)
{
	static_assert(std::is_same_v<angle::radians<double>, decltype(asin(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<angle::radians<double>, decltype(asin(dimensionless<int>(0)))>);
	auto input1  = 0.90929742682;
	auto input2  = 1;
	auto input3  = 0.70710678118654752440084436210485;
	auto input4  = 1;
	auto out1 = 1.14159265;
	auto out2 = 1.570796326795;
	auto out3 = 45;
	auto out4 = 90;
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), asin(dimensionless<double>(input1)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::radians<double>(out2).to<double>(), asin(dimensionless<int>(input2)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::degrees<double>(out3).to<double>(), angle::degrees<double>(asin(dimensionless<double>(input3))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degrees<double>(out4).to<double>(), angle::degrees<double>(asin(dimensionless<int>(input4))).to<double>(), 5.0e-12);
	auto uin1 = input1 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin2 = input2 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin3 = input3 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin4 = input4 * 1.0_m * (1.0 / (1000.0_mm));
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), asin(uin1).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::radians<double>(out2).to<double>(), asin(uin2).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::degrees<double>(out3).to<double>(), angle::degrees<double>(asin(uin3)).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degrees<double>(out4).to<double>(), angle::degrees<double>(asin(uin4)).to<double>(), 5.0e-12);
}

// Regression: the inverse trig / hyperbolic family must promote a FRACTIONAL integer-underlying
// dimensionless argument to floating point before the cmath call. A scaled dimensionless such as
// percent<int>(50) has value 0.5 but an integer underlying of 50; converting to the raw underlying
// truncated 0.5 -> 0, so asin/acos/atan (and the inverse hyperbolics) returned the wrong result for any
// non-whole ratio. They must match std::* on the promoted value, exactly as the forward trig already does.
TEST_F(UnitMath, inverseTrigPromotesFractionalIntegerUnderlying)
{
	const percent<int> half(50);    // value() == 0.5, underlying int == 50
	EXPECT_NEAR(std::asin(0.5), asin(half).to<double>(), 5.0e-12);
	EXPECT_NEAR(std::acos(0.5), acos(half).to<double>(), 5.0e-12);
	EXPECT_NEAR(std::atan(0.5), atan(half).to<double>(), 5.0e-12);
	EXPECT_NEAR(std::asinh(0.5), asinh(half).to<double>(), 5.0e-12);
	EXPECT_NEAR(std::atanh(0.5), atanh(half).to<double>(), 5.0e-12);
	// acosh needs an argument >= 1; use 150% = 1.5.
	const percent<int> onePointFive(150);
	EXPECT_NEAR(std::acosh(1.5), acosh(onePointFive).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, atan)
{
	static_assert(std::is_same_v<angle::radians<double>, decltype(atan(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<angle::radians<double>, decltype(atan(dimensionless<int>(0)))>);
	auto input1  = -2.18503986326;
	auto input2  = 1;
	auto input3  = -1;
	auto input4  = 1;
	auto out1 = -1.14159265;
	auto out2 = 0.785398163397;
	auto out3 = -45;
	auto out4 = 45;
	EXPECT_NEAR(angle::radians<double>(out1).to<double>(), atan(dimensionless<double>(input1)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::radians<double>(out2).to<double>(), atan(dimensionless<int>(input2)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::degrees<double>(out3).to<double>(), angle::degrees<double>(atan(dimensionless<double>(input3))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degrees<double>(out4).to<double>(), angle::degrees<double>(atan(dimensionless<int>(input4))).to<double>(), 5.0e-12);
	auto uin1 = input1 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin2 = input2 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin3 = input3 * 1.0_m * (1.0 / (1000.0_mm));
	auto uin4 = input4 * 1.0_m * (1.0 / (1000.0_mm));
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

// Hyperbolic functions operate on a dimensionless real (a hyperbolic angle), not a geometric angle: they
// take a dimensionless argument with no radian conversion, and the inverse functions return dimensionless.
TEST_F(UnitMath, cosh)
{
	static_assert(std::is_same_v<dimensionless<double>, decltype(cosh(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<dimensionless<double>, decltype(cosh(dimensionless<int>(0)))>);
	EXPECT_NEAR(std::cosh(2.0), cosh(dimensionless<double>(2.0)).to<double>(), 5.0e-11);
	EXPECT_NEAR(std::cosh(2.0), cosh(dimensionless<int>(2)).to<double>(), 5.0e-11);
	// a ratio-dimensionless argument uses its normalized value (50% -> 0.5)
	EXPECT_NEAR(std::cosh(0.5), cosh(percent<double>(50)).to<double>(), 5.0e-11);
}

TEST_F(UnitMath, sinh)
{
	static_assert(std::is_same_v<dimensionless<double>, decltype(sinh(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<dimensionless<double>, decltype(sinh(dimensionless<int>(0)))>);
	EXPECT_NEAR(std::sinh(2.0), sinh(dimensionless<double>(2.0)).to<double>(), 5.0e-11);
	EXPECT_NEAR(std::sinh(2.0), sinh(dimensionless<int>(2)).to<double>(), 5.0e-11);
	EXPECT_NEAR(std::sinh(0.5), sinh(percent<double>(50)).to<double>(), 5.0e-11);
}

TEST_F(UnitMath, tanh)
{
	static_assert(std::is_same_v<dimensionless<double>, decltype(tanh(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<dimensionless<double>, decltype(tanh(dimensionless<int>(0)))>);
	EXPECT_NEAR(std::tanh(2.0), tanh(dimensionless<double>(2.0)).to<double>(), 5.0e-11);
	EXPECT_NEAR(std::tanh(2.0), tanh(dimensionless<int>(2)).to<double>(), 5.0e-11);
	EXPECT_NEAR(std::tanh(0.5), tanh(percent<double>(50)).to<double>(), 5.0e-11);
}

TEST_F(UnitMath, acosh)
{
	static_assert(std::is_same_v<dimensionless<double>, decltype(acosh(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<dimensionless<double>, decltype(acosh(dimensionless<int>(0)))>);
	EXPECT_NEAR(std::acosh(2.0), acosh(dimensionless<double>(2.0)).to<double>(), 5.0e-11);
	EXPECT_NEAR(std::acosh(2.0), acosh(dimensionless<int>(2)).to<double>(), 5.0e-11);
	auto uins = 2.0 * 1.0_m * (1.0 / (1000.0_mm));   // a dimensionless expression
	EXPECT_NEAR(std::acosh(2.0), acosh(uins).to<double>(), 5.0e-11);
}

TEST_F(UnitMath, asinh)
{
	static_assert(std::is_same_v<dimensionless<double>, decltype(asinh(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<dimensionless<double>, decltype(asinh(dimensionless<int>(0)))>);
	EXPECT_NEAR(std::asinh(2.0), asinh(dimensionless<double>(2.0)).to<double>(), 5.0e-9);
	EXPECT_NEAR(std::asinh(2.0), asinh(dimensionless<int>(2)).to<double>(), 5.0e-9);
	auto uins = 2.0 * 1.0_m * (1.0 / (1000.0_mm));
	EXPECT_NEAR(std::asinh(2.0), asinh(uins).to<double>(), 5.0e-9);
}

TEST_F(UnitMath, atanh)
{
	static_assert(std::is_same_v<dimensionless<double>, decltype(atanh(dimensionless<double>(0)))>);
	static_assert(std::is_same_v<dimensionless<double>, decltype(atanh(dimensionless<int>(0)))>);
	EXPECT_NEAR(std::atanh(0.5), atanh(dimensionless<double>(0.5)).to<double>(), 5.0e-9);
	EXPECT_NEAR(std::atanh(0.0), atanh(dimensionless<int>(0)).to<double>(), 5.0e-9);
	auto uins = 0.5 * 1.0_m * (1.0 / (1000.0_mm));
	EXPECT_NEAR(std::atanh(0.5), atanh(uins).to<double>(), 5.0e-9);
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

	// A scaled dimensionless unit (percent) must not have its scale applied twice: modf(202.5%) is an
	// integral 200% and a fractional 2.5%, i.e. value() 2.0 and 0.025 (regression for issue #312). The
	// fractional part carries only the rounding of std::modf itself, so it is compared with a tolerance.
	percent<double> pintpart;
	auto            pfracpart = modf(percent<double>(202.5), &pintpart);
	EXPECT_DOUBLE_EQ(2.0, pintpart.value());
	EXPECT_NEAR(0.025, pfracpart.value(), 1e-12);
	// sign is carried on both parts
	auto npfracpart = modf(percent<double>(-202.5), &pintpart);
	EXPECT_DOUBLE_EQ(-2.0, pintpart.value());
	EXPECT_NEAR(-0.025, npfracpart.value(), 1e-12);
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
	// Named-result parity with pow<2> -> square_meters above: pow<3> of a length reports the named volume unit
	// (cubic_meters<double>), the friendly type, rather than the equivalent-but-unnamed unit<strong_t<cubed<...>>>.
	static_assert(std::is_same_v<decltype(cube), cubic_meters<double>>);

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
	EXPECT_EQ(resultPct, 40.0_pct);
	EXPECT_EQ(0.4, resultPct);
}

TEST_F(UnitMath, hypot)
{
	static_assert(std::is_same_v<meters<double>, decltype(hypot(meters<double>(3.0), meters<double>(4.0)))>);
	EXPECT_NEAR(meters<double>(5.0).to<double>(), (hypot(meters<double>(3.0), meters<double>(4.0))).to<double>(), 5.0e-9);

	// Mixed floating-point units resolve to the left operand's unit (lossless), so the result is readable in x's
	// unit and the documented "in x's unit" contract holds. See hypotMixedUnitsReturnLeftOperandUnit for the full
	// two-argument-math contract (issue #393).
	static_assert(std::is_same_v<feet<double>, decltype(hypot(feet<double>(3.0), meters<double>(1.2192)))>);
	EXPECT_NEAR(feet<double>(5.0).to<double>(), feet<double>(hypot(feet<double>(3.0), meters<double>(1.2192))).to<double>(), 5.0e-9);
}

// Regression for issue #393: the two-argument math functions (hypot, fmax, fmin, fmod, fdim) return the result in
// the LEFT operand's unit when that is lossless -- both operands floating point, or the right operand converts into
// the left's unit without truncation -- so the value is readable in a named unit the caller wrote instead of an
// anonymous common unit. This mirrors operator+/operator- (lhs_result_unit_t). When returning the left unit WOULD
// truncate an integer, the functions fall back to the finest common unit so no value is lost.
TEST_F(UnitMath, mixedUnitMathReturnsLeftOperandUnit)
{
	// Floating-point operands -> left operand's unit, in either order.
	static_assert(std::is_same_v<feet<double>, decltype(hypot(feet<double>(3.0), meters<double>(1.0)))>);
	static_assert(std::is_same_v<meters<double>, decltype(hypot(meters<double>(3.0), feet<double>(1.0)))>);
	static_assert(std::is_same_v<feet<double>, decltype(fmax(feet<double>(3.0), meters<double>(1.0)))>);
	static_assert(std::is_same_v<meters<double>, decltype(fmin(meters<double>(3.0), feet<double>(1.0)))>);
	static_assert(std::is_same_v<feet<double>, decltype(fdim(feet<double>(3.0), meters<double>(1.0)))>);
	static_assert(std::is_same_v<feet<double>, decltype(fmod(feet<double>(3.0), meters<double>(1.0)))>);

	// The result carries the correct physical quantity in that unit: 3 ft and 4 ft legs -> 5 ft hypotenuse.
	EXPECT_NEAR(5.0, hypot(feet<double>(3.0), feet<double>(4.0)).raw(), 5.0e-9);

	// Integer-lossy fallback: meters<int> cannot hold feet<int> without truncation, so the result stays in the
	// finest common unit (floating-point promoted) and no value is lost -- the anonymous unit earns its keep here.
	using LossyResult = decltype(fmax(meters<int>(3), feet<int>(4)));
	static_assert(!std::is_same_v<LossyResult, meters<int>>, "integer-lossy result must not collapse to the left int unit");
	static_assert(std::is_same_v<LossyResult, detail::floating_point_promotion_t<std::common_type_t<meters<int>, feet<int>>>>,
		"integer-lossy result falls back to the common unit");
}

TEST_F(UnitMath, ceil)
{
	double val = 101.1;
	EXPECT_EQ(ceil(val), ceil(meters<double>(val)).to<double>());
	static_assert(std::is_same_v<meters<double>, decltype(ceil(meters<double>(val)))>);

	// ceil must match std::ceil across the WHOLE domain, including non-finite and magnitudes beyond the
	// 2^63 range of a signed integer. A prior hand-rolled `static_cast<long long>` path returned garbage
	// (~-9.22e18) for NaN, +/-Inf, and |x| >= 2^63; delegating to std::ceil fixes it.
	const double nan = std::numeric_limits<double>::quiet_NaN();
	const double inf = std::numeric_limits<double>::infinity();
	EXPECT_TRUE(std::isnan(ceil(meters<double>(nan)).to<double>()));
	EXPECT_EQ(inf, ceil(meters<double>(inf)).to<double>());
	EXPECT_EQ(-inf, ceil(meters<double>(-inf)).to<double>());
	EXPECT_EQ(std::ceil(1e19), ceil(meters<double>(1e19)).to<double>());
	EXPECT_EQ(std::ceil(-1e19), ceil(meters<double>(-1e19)).to<double>());
	// ordinary positive/negative rounding stays correct.
	EXPECT_EQ(3.0, ceil(meters<double>(2.3)).to<double>());
	EXPECT_EQ(-2.0, ceil(meters<double>(-2.3)).to<double>());
}

TEST_F(UnitMath, floor)
{
	double val = 101.1;
	EXPECT_EQ(floor(val), floor(dimensionless<double>(val)));

	// floor must match std::floor across the whole domain (see ceil above for the fixed regression).
	const double nan = std::numeric_limits<double>::quiet_NaN();
	const double inf = std::numeric_limits<double>::infinity();
	EXPECT_TRUE(std::isnan(floor(meters<double>(nan)).to<double>()));
	EXPECT_EQ(inf, floor(meters<double>(inf)).to<double>());
	EXPECT_EQ(-inf, floor(meters<double>(-inf)).to<double>());
	EXPECT_EQ(std::floor(1e19), floor(meters<double>(1e19)).to<double>());
	EXPECT_EQ(std::floor(-1e19), floor(meters<double>(-1e19)).to<double>());
	EXPECT_EQ(2.0, floor(meters<double>(2.3)).to<double>());
	EXPECT_EQ(-3.0, floor(meters<double>(-2.3)).to<double>());
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

	// Regression for #373: the three operands may be in DIFFERENT units of their dimensions, and each must
	// be reconciled to the result unit before the fused multiply-add. Feeding each operand's own-unit raw
	// value combined inconsistent bases (6 ft * 3 ft + 1 m^2 wrongly gave ~1.0 m^2 instead of 2.672255).
	using units::literals::operator""_ft;
	using units::literals::operator""_m2;
	const auto crossUnit = units::fma(6.0_ft, 3.0_ft, 1.0_m2);
	EXPECT_NEAR(2.67225472, square_meters<double>(crossUnit).to<double>(), 1.0e-6);

	// Cross-DIMENSION product: (speed * time) + length, all reconciled to meters.
	const auto crossDim = units::fma(10.0_mps, 2.0_s, 5.0_m);
	EXPECT_NEAR(25.0, meters<double>(crossDim).to<double>(), 1.0e-9);

	// Same-unit exact case stays exact.
	EXPECT_EQ(square_meters<double>(10.0), (units::fma(meters<double>(2.0), meters<double>(3.0), square_meters<double>(4.0))));

	// Integer-underlying operands promote (matching C's usual arithmetic conversions) and stay correct.
	const auto intFma = units::fma(meters<int>(2), meters<int>(3), square_meters<int>(4));
	EXPECT_NEAR(10.0, square_meters<double>(intFma).to<double>(), 1.0e-9);
	static_assert(!std::is_integral_v<typename decltype(intFma)::underlying_type>, "fma result must be floating-point-promoted like C");
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

TEST_F(UnitMath, signbit)
{
	meters<> zero(0.0);
	meters<> pos(1.0);
	meters<> neg(-1.0);
	meters<> negZero(-0.0);

	EXPECT_FALSE(std::signbit(zero));
	EXPECT_FALSE(std::signbit(pos));
	EXPECT_TRUE(std::signbit(neg));
	EXPECT_TRUE(std::signbit(negZero));
}

TEST_F(UnitMath, stdExtensions)
{
	meters<> zero(0.0);
	meters<> nan(NAN);
	meters<> inf(INFINITY);

	EXPECT_TRUE(std::isnan(nan));
	EXPECT_FALSE(std::isnan(inf));
	EXPECT_FALSE(std::isnan(zero));

	EXPECT_TRUE(std::isinf(inf));
	EXPECT_FALSE(std::isinf(nan));
	EXPECT_FALSE(std::isinf(zero));

	EXPECT_TRUE(std::isfinite(zero));
	EXPECT_FALSE(std::isfinite(nan));
	EXPECT_FALSE(std::isfinite(inf));
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

	EXPECT_EQ(8.0_m3, result9);
	EXPECT_EQ(4.0_m2, result10);
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

TEST(ConcentrationSemantics, scalar_multiply_returns_dimensionless)
{
	auto x = 2 * 50_pct;
	EXPECT_DOUBLE_EQ(x.value(), 1.0);  // dimensionless
}

TEST(ConcentrationSemantics, abs_preserves_percent)
{
	percent<double> pct1 = 100.0_pct;
	percent<double> pct2 = 70.0_pct;
	auto delta = units::fabs(pct1 - pct2);
	EXPECT_EQ(delta, 30.0_pct);
}

TEST(ConcentrationSemantics, PercentPpmPpbConvertToDimensionlessFraction)
{
	// Percent: 50% == 0.5 dimensionless
	EXPECT_NEAR(dimensionless(50.0_pct).to<double>(), 0.5, 0.0);

	// ppm: 1 ppm == 1e-6 dimensionless
	EXPECT_NEAR(dimensionless(1.0_ppm).to<double>(), 1.0e-6, 0.0);

	// ppb: 1 ppb == 1e-9 dimensionless
	EXPECT_NEAR(dimensionless(1.0_ppb).to<double>(), 1.0e-9, 0.0);

	// chained conversion sanity: 1000 ppb == 1 ppm
	EXPECT_NEAR(parts_per_million(1000.0_ppb).raw(), 1.0, 0.0);
	EXPECT_NEAR(parts_per_million(1.0_ppm).raw(), 1.0, 0.0);
}

TEST(ConcentrationSemantics, ScalarTimesPercentYieldsDimensionless)
{
	auto x = 2 * 50.0_pct;

	// We want this to be a pure dimensionless "1", not 100_pct.
	EXPECT_NEAR(dimensionless(x).to<double>(), 1.0, 0.0);

	// Also check commutativity
	auto y = 50.0_pct * 2;
	EXPECT_NEAR(dimensionless(y).to<double>(), 1.0, 0.0);
}

TEST(ConcentrationSemantics, PercentMathPreservesPercentRepresentation)
{
	auto whole = 100.0_pct;
	auto most  = 70.0_pct;

	// subtraction should preserve the unit: 100% - 70% = 30%
	auto diff = whole - most;
	EXPECT_NEAR(diff.raw(), 30.0, 0.0);

	// fabs should preserve percent representation: fabs(30%) == 30%
	auto f = units::fabs(diff);
	EXPECT_NEAR(f.raw(), 30.0, 0.0);

	// abs should preserve percent representation
	auto magnitude = units::abs(-30_pct);
	EXPECT_NEAR(magnitude.raw(), 30.0, 0.0);

	// fmin/fmax should preserve percent representation
	auto mn = units::fmin(whole, most);
	EXPECT_NEAR(mn.raw(), 70.0, 0.0);

	auto mx = units::fmax(whole, most);
	EXPECT_NEAR(mx.raw(), 100.0, 0.0);

	// fdim should preserve percent representation: fdim(70%, 100%) == 0%
	auto pd = units::fdim(most, whole);
	EXPECT_NEAR(pd.raw(), 0.0, 0.0);
}

TEST(ConcentrationSemantics, TranscendentalsUseNormalizedValue)
{
	// log(50%) = log(0.5)
	auto x = units::log(50_pct);
	EXPECT_NEAR(dimensionless(x).to<double>(), std::log(0.5), 1e-15);

	// exp(0%) = exp(0) = 1
	auto y = units::exp(0_pct);
	EXPECT_NEAR(dimensionless(y).to<double>(), 1.0, 1e-15);
}

TEST(ConcentrationSemantics, RatioDimlessPreservedInCompoundDivision)
{
	using pct_per_m = decltype(1_pct / 1_m);
	using inv_m     = unit<inverse<meters<>>, double>;

	static_assert(!std::is_same_v<pct_per_m, inv_m>);
	static_assert(units::traits::is_same_dimension_unit_v<pct_per_m, inv_m>);
	static_assert(std::is_convertible_v<pct_per_m, inv_m>);
}

TEST(ConcentrationSemantics, PctPerMeterUsesPointsNumerator)
{
	auto x = 50_pct / 2_m;           // 25 pct/m in points space
	EXPECT_DOUBLE_EQ(x.raw(), 25.0);

	// When converted to 1/m it should be fraction per meter: 0.25 / m
	unit<inverse<meters<>>, double> y = x;
	EXPECT_DOUBLE_EQ(y.value(), 0.25 / 1.0); // == 0.25 (per m)
}

TEST(ConcentrationSemantics, CommonTypePpmPpb)
{
	using CT = std::common_type_t<parts_per_million<double>, parts_per_billion<double>>;
	static_assert(units::traits::is_same_dimension_unit_v<CT, parts_per_million<double>>);

	CT inPpm  = 1.0_ppm;
	CT inPpb  = 1000.0_ppb;
	EXPECT_DOUBLE_EQ(inPpm.raw(), inPpb.raw());
	EXPECT_DOUBLE_EQ(inPpm.value(), inPpb.value());
}

TEST(ConcentrationSemantics, UnitCastUsesNormalizedForRatioDimless)
{
	EXPECT_DOUBLE_EQ(units::unit_cast<double>(50_pct), 0.5);
	EXPECT_EQ(units::unit_cast<int>(50_pct), 0); // yes, surprising, but locks policy
}

TEST(ConcentrationSemantics, DimensionlessDivPercentIsNotSameAsScalarDivPercent)
{
	auto scalarQuotient = 1.0 / 50_pct;                 // scalar/percent -> dimensionless, uses rhs.value()
	EXPECT_DOUBLE_EQ(scalarQuotient, 2.0);

	double dimensionlessQuotient = dimensionless(1.0) / 50_pct;  // currently -> inverse(percent) style
	// Nail down expected behavior (whatever you decide it should be).
	// If keeping current behavior:
	EXPECT_DOUBLE_EQ(dimensionlessQuotient, 2);     // because 1 / rhs.raw() = 1/50
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
	// A composed unit the library does not name prints as the raw dimension form (value + dimension
	// exponents), not a friendly abbreviation. Volume per time-squared has no named unit.
	using liters_per_second_squared = decltype(1.0_L / (1.0_s * 1.0_s));

	liters_per_second_squared original(5);
	liters_per_second_squared copy = original;

	EXPECT_DOUBLE_EQ(original.to<double>(), copy.to<double>());

	testing::internal::CaptureStdout();
	std::cout << original;
	std::string output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("0.005 m^3 s^-2", output.c_str());
}

TEST_F(CaseStudies, idealGasLaw)
{
	// PV = nRT, solved for pressure. Temperature is a factor in the product just like any other quantity —
	// a physicist writes the equation directly, with no unwrapping of the affine scale. One mole at 273.15 K
	// in 22.414 L is one standard atmosphere.
	const substance::mols<>       n = substance::mols<>(1.0);
	const temperature::kelvin<>   T = temperature::kelvin<>(273.15);
	const volume::liters<>        V = volume::liters<>(22.414);
	const auto                    R = energy::joules<>(8.314462618) / (substance::mols<>(1.0) * temperature::kelvin<>(1.0));

	const pressure::pascals<double> P = (n * R * T) / V;

	EXPECT_NEAR(101325.0, P.value(), 1.0);
}

//======================================================================================================================
//	BACKLOG-CLEANUP additions — viscosity (#205), compound-assign (#257), torque naming (#311)
//======================================================================================================================

namespace
{
	class Viscosity : public ::testing::Test
	{
	};
	class CompoundAssign : public ::testing::Test
	{
	};
	class TorqueNaming : public ::testing::Test
	{
	};
	class CgsBiot : public ::testing::Test
	{
	};
} // namespace

// ---- #205: biot ----------------------------------------------------------------------------------------------------
TEST_F(CgsBiot, biotIsAbampere)
{
	using units::current::biots;
	// biot is the CGS-EMU name for the abampere: exactly 10 amperes
	EXPECT_DOUBLE_EQ(10.0, units::amperes<double>(biots<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(30.0, units::amperes<double>(biots<double>(3.0)).value());
	// round-trips through amperes
	EXPECT_DOUBLE_EQ(5.0, biots<double>(units::amperes<double>(50.0)).value());
	// same type as abamperes (it is an alias)
	static_assert(std::is_same_v<biots<double>, units::current::abamperes<double>>);
	// same dimension as current
	static_assert(traits::is_current_unit_v<biots<double>>);
}

// ---- #205: dynamic viscosity (poise) -------------------------------------------------------------------------------
TEST_F(Viscosity, dynamicViscosityUnitsAndDimension)
{
	using namespace units::dynamic_viscosity;
	// poise = 0.1 Pa*s ; centipoise = 0.01 poise = 0.001 Pa*s
	EXPECT_DOUBLE_EQ(0.1, pascal_seconds<double>(poise<double>(1.0)).value());
	EXPECT_DOUBLE_EQ(1.0, poise<double>(centipoise<double>(100.0)).value());
	EXPECT_NEAR(0.001, pascal_seconds<double>(centipoise<double>(1.0)).value(), 5.0e-15);
	// water is ~1 cP ~ 0.001 Pa*s
	EXPECT_NEAR(0.001, pascal_seconds<double>(centipoise<double>(1.0)).value(), 5.0e-15);
	// dimension: pressure * time
	static_assert(traits::is_dynamic_viscosity_unit_v<poise<double>>);
	static_assert(traits::is_dynamic_viscosity_unit_v<pascal_seconds<double>>);
	static_assert(!traits::is_dynamic_viscosity_unit_v<units::pascals<double>>);
	static_assert(traits::is_same_dimension_unit_v<poise<double>, pascal_seconds<double>>);
	// a pressure times a time IS a dynamic viscosity
	static_assert(traits::is_same_dimension_unit_v<decltype(units::pascals<double>(1) * units::seconds<double>(1)), pascal_seconds<double>>);
}

// ---- #205: kinematic viscosity (stokes) ----------------------------------------------------------------------------
TEST_F(Viscosity, kinematicViscosityUnitsAndDimension)
{
	using namespace units::kinematic_viscosity;
	// stokes = 1e-4 m^2/s ; centistokes = 0.01 stokes = 1e-6 m^2/s
	EXPECT_NEAR(1.0e-4, square_meters_per_second<double>(stokes<double>(1.0)).value(), 5.0e-16);
	EXPECT_DOUBLE_EQ(1.0, stokes<double>(centistokes<double>(100.0)).value());
	EXPECT_NEAR(1.0e-6, square_meters_per_second<double>(centistokes<double>(1.0)).value(), 5.0e-18);
	// dimension: area / time
	static_assert(traits::is_kinematic_viscosity_unit_v<stokes<double>>);
	static_assert(traits::is_kinematic_viscosity_unit_v<square_meters_per_second<double>>);
	static_assert(!traits::is_kinematic_viscosity_unit_v<units::square_meters<double>>);
	// dynamic and kinematic viscosity are DISTINCT dimensions
	static_assert(!traits::is_same_dimension_unit_v<stokes<double>, units::dynamic_viscosity::poise<double>>);
}

// ---- #257: compound assignment keeps the lhs type and value; warns on lossy integer scale -------------------------
TEST_F(CompoundAssign, multiplyKeepsTypeAndValue)
{
	// double lhs: exact
	units::meters<double> len(10.0);
	len *= 2.0;
	static_assert(std::is_same_v<decltype(len), units::meters<double>>);
	EXPECT_DOUBLE_EQ(20.0, len.value());
	len *= 0.5;
	EXPECT_DOUBLE_EQ(10.0, len.value());
	// integer lhs, integer factor: exact, no narrowing
	units::meters<int> ilen(10);
	ilen *= 3;
	static_assert(std::is_same_v<decltype(ilen), units::meters<int>>);
	EXPECT_EQ(30, ilen.value());
	// integer lhs scaled by a floating-point factor narrows and surfaces -Wfloat-conversion; that lossy path's
	// value/type behavior is proven in the dedicated lossyCompoundAssign translation unit (compiled with the
	// float-conversion diagnostic disabled) and the diagnostic itself by test/errorMessages/cases.
}

TEST_F(CompoundAssign, divideKeepsTypeAndValue)
{
	units::meters<double> len(10.0);
	len /= 4.0;
	EXPECT_DOUBLE_EQ(2.5, len.value());
	units::meters<int> ilen(10);
	ilen /= 2;
	EXPECT_EQ(5, ilen.value());
	static_assert(std::is_same_v<decltype(ilen), units::meters<int>>);
}

TEST_F(CompoundAssign, worksAcrossDimensions)
{
	// the operators are generic over the (non-ratio-dimensionless) unit; spot-check several dimensions
	units::seconds<double> dur(60.0);
	dur *= 2.0;
	EXPECT_DOUBLE_EQ(120.0, dur.value());
	units::kilograms<double> mass(5.0);
	mass /= 2.0;
	EXPECT_DOUBLE_EQ(2.5, mass.value());
	units::newtons<double> f(10.0);
	f *= 3.0;
	EXPECT_DOUBLE_EQ(30.0, f.value());
	units::meters_per_second<double> v(26.8224);
	v *= 2.0;
	EXPECT_NEAR(53.6448, v.value(), 5.0e-9);
}

// ---- #311: torque pound_feet is the named unit; foot_pounds is a deprecated alias ---------------------------------
TEST_F(TorqueNaming, poundFeetIsTheTorqueUnit)
{
	using units::torque::pound_feet;
	// pound_feet == foot * pound-force
	EXPECT_NEAR(1.3558179483314004, units::newton_meters<double>(pound_feet<double>(1.0)).value(), 5.0e-9);
	static_assert(traits::is_torque_unit_v<pound_feet<double>>);
	// torque and energy share the force*length dimension in this library, so the distinction is by NAME, not
	// dimension: pound_feet (torque, lbf*ft) and energy::foot_pounds (energy, ft*lbf) are the same magnitude but
	// carry different abbreviations so a reader can tell which is meant
	static_assert(traits::is_energy_unit_v<units::energy::foot_pounds<double>>);
	EXPECT_NEAR(units::newton_meters<double>(pound_feet<double>(1.0)).value(), units::joules<double>(units::energy::foot_pounds<double>(1.0)).value(), 5.0e-9);
#if !defined(UNIT_LIB_DISABLE_IOSTREAM)
	// prints with the correct engineering abbreviation
	testing::internal::CaptureStdout();
	std::cout << pound_feet<double>(10.0);
	std::string out = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("10 lbf_ft", out.c_str());
#endif
}

//======================================================================================================================
//	SERIALIZATION — exhaustive coverage
//======================================================================================================================

namespace
{
	// Round-trips a quantity through a REAL external boundary: serialize -> write the raw bytes to a temp file ->
	// read them back into a fresh buffer (no link to the original any_unit) -> deserialize that buffer. This proves the
	// on-disk byte stream is self-sufficient and decodes correctly; it is deliberately NOT `deserialize(serialize(q))`,
	// which could short-circuit an in-memory any_unit and never exercise the wire encode/decode.
	template<class Q>
	void expectRoundTrip(Q quantity)
	{
		const units::any_unit encoded = units::serialize(quantity);

		// write the bytes out through the C-interface face (data()/size()) exactly as a caller would to a file
		static std::atomic<unsigned> counter{0};
		const std::filesystem::path  path =
			std::filesystem::temp_directory_path() / ("units_roundtrip_" + std::to_string(counter.fetch_add(1)) + ".bin");
		{
			std::ofstream out(path, std::ios::binary);
			ASSERT_TRUE(out.is_open());
			out.write(encoded.data(), static_cast<std::streamsize>(encoded.size()));
		}

		// read the raw bytes back into a fresh buffer that has NO connection to `encoded`
		std::vector<std::byte> fromDisk;
		{
			std::ifstream in(path, std::ios::binary);
			ASSERT_TRUE(in.is_open());
			in.seekg(0, std::ios::end);
			const std::streamoff length = in.tellg();
			in.seekg(0, std::ios::beg);
			fromDisk.resize(static_cast<std::size_t>(length));
			in.read(reinterpret_cast<char*>(fromDisk.data()), length);
		}
		std::filesystem::remove(path);

		// the bytes on disk match what the any_unit reported it wrote
		ASSERT_EQ(encoded.size(), fromDisk.size());

		// decode the disk buffer with no prior knowledge of the type
		const auto erased = units::deserialize(fromDisk);
		ASSERT_TRUE(erased.has_value());
		const auto back = erased->template to<Q>();
		ASSERT_TRUE(back.has_value());
		const double a = quantity.template to<double>();
		const double b = back->template to<double>();
		if (std::isnan(a))
			EXPECT_TRUE(std::isnan(b));
		else
			EXPECT_DOUBLE_EQ(a, b);
		// the typed fast path decodes the same disk buffer to the same value
		const auto direct = units::deserialize<Q>(fromDisk);
		ASSERT_TRUE(direct.has_value());
		if (std::isnan(a))
			EXPECT_TRUE(std::isnan(direct->template to<double>()));
		else
			EXPECT_DOUBLE_EQ(a, direct->template to<double>());
	}
} // namespace

TEST_F(Serialization, roundTripLength)
{
	expectRoundTrip(units::meters<double>(100.0));
	expectRoundTrip(units::feet<double>(3.5));
	expectRoundTrip(units::miles<double>(2.0));
	expectRoundTrip(units::kilometers<double>(2.5));
	expectRoundTrip(units::nanometers<double>(500.0));
	expectRoundTrip(units::furlongs<double>(10.0));
	expectRoundTrip(units::nautical_miles<double>(1.0));
	expectRoundTrip(units::rods<double>(4.0));
	expectRoundTrip(units::picas<double>(6.0));
}

TEST_F(Serialization, roundTripMass)
{
	expectRoundTrip(units::kilograms<double>(5.0));
	expectRoundTrip(units::grams<double>(250.0));
	expectRoundTrip(units::mass::pounds<double>(10.0));
	expectRoundTrip(units::slugs<double>(1.0));
	expectRoundTrip(units::stone<double>(11.0));
	expectRoundTrip(units::troy_ounces<double>(2.0));
	expectRoundTrip(units::carats<double>(0.5));
}

TEST_F(Serialization, roundTripTime)
{
	expectRoundTrip(units::seconds<double>(60.0));
	expectRoundTrip(units::minutes<double>(1.5));
	expectRoundTrip(units::hours<double>(24.0));
	expectRoundTrip(units::milliseconds<double>(500.0));
	expectRoundTrip(units::fortnights<double>(1.0));
	expectRoundTrip(units::julian_years<double>(1.0));
}

TEST_F(Serialization, roundTripAngle)
{
	expectRoundTrip(units::radians<double>(3.14159));
	expectRoundTrip(units::degrees<double>(90.0));
	expectRoundTrip(units::turns<double>(0.25));
	expectRoundTrip(units::gradians<double>(100.0));
	expectRoundTrip(units::angular_mils<double>(1600.0));
}

TEST_F(Serialization, roundTripAngularAccelerationAndJerk)
{
	expectRoundTrip(units::radians_per_second_squared<double>(2.375));
	expectRoundTrip(units::degrees_per_second_squared<double>(91.25));
	expectRoundTrip(units::radians_per_second_cubed<double>(3.125));
	expectRoundTrip(units::degrees_per_second_cubed<double>(47.5));
	expectRoundTrip(units::gradians_per_second_squared<double>(63.75));
	expectRoundTrip(units::revolutions_per_minute_squared<double>(1234.5));
	expectRoundTrip(units::milliradians_per_second_squared<double>(500.0));
	expectRoundTrip(units::gradians_per_second_cubed<double>(63.75));
	expectRoundTrip(units::revolutions_per_second_cubed<double>(2.5));
	expectRoundTrip(units::kiloradians_per_second_cubed<double>(1.25));
}

TEST_F(Serialization, roundTripTemperature)
{
	expectRoundTrip(units::kelvin<double>(300.0));
	expectRoundTrip(units::celsius<double>(100.0));
	expectRoundTrip(units::fahrenheit<double>(212.0));
	expectRoundTrip(units::rankine<double>(491.67));
}

TEST_F(Serialization, roundTripCurrentAndCharge)
{
	expectRoundTrip(units::amperes<double>(2.0));
	expectRoundTrip(units::milliamperes<double>(500.0));
	expectRoundTrip(units::coulombs<double>(1.0));
	expectRoundTrip(units::ampere_hours<double>(3.0));
}

// Ratio-scaled dimensionless units (percent, parts-per-million, ...) carry a scale ratio between their point value
// and their physical value: 50 percent is the physical value 0.5. Reconstructing from the physical value rather than
// the point value would rescale by that ratio, so serialize(50 percent) must read back as the physical value 0.5.
TEST_F(Serialization, roundTripConcentration)
{
	expectRoundTrip(units::concentration::percent<double>(37.25));
	expectRoundTrip(units::concentration::percent<double>(0.0));
	expectRoundTrip(units::concentration::percent<double>(103.75));
	expectRoundTrip(units::concentration::parts_per_million<double>(312500.5));
	expectRoundTrip(units::concentration::parts_per_billion<double>(1.875));
	expectRoundTrip(units::concentration::parts_per_trillion<double>(42.125));
	expectRoundTrip(units::dimensionless<double>(0.53125));

	// Integer-backed ratio-scaled dimensionless units travel the integer narrowing path on decode; a percent whose
	// point value is a whole number reconstructs exactly, agreeing with the floating-point representation.
	expectRoundTrip(units::concentration::percent<int>(50));
	expectRoundTrip(units::concentration::parts_per_million<int>(300000));
	expectRoundTrip(units::dimensionless<int>(7));
}

// Physical constants exercise compound dimensions and extreme magnitudes (Boltzmann's constant near 1.4e-23,
// Avogadro's number near 6e23) through the canonical-base conversion; each must recover its physical value.
TEST_F(Serialization, roundTripPhysicalConstants)
{
	expectRoundTrip(units::constants::k_B);   // Boltzmann constant, joules per kelvin, ~1.38e-23
	expectRoundTrip(units::constants::N_A);    // Avogadro's number, per mole, ~6.02e23
	expectRoundTrip(units::constants::R);      // gas constant, joules per kelvin per mole
	expectRoundTrip(units::constants::h);      // Planck constant, joule-seconds, ~6.63e-34
	expectRoundTrip(units::constants::c);      // speed of light, meters per second
	expectRoundTrip(units::constants::sigma);  // Stefan-Boltzmann constant, watts per square meter per kelvin^4
}

TEST_F(Serialization, roundTripSubstanceAndLuminous)
{
	expectRoundTrip(units::mols<double>(2.0));
	expectRoundTrip(units::candelas<double>(60.0));
	expectRoundTrip(units::lumens<double>(800.0));
	expectRoundTrip(units::lux<double>(500.0));
}

TEST_F(Serialization, roundTripData)
{
	expectRoundTrip(units::bytes<double>(1024.0));
	expectRoundTrip(units::kilobytes<double>(4.0));
	expectRoundTrip(units::gigabytes<double>(2.0));
	expectRoundTrip(units::kibibytes<double>(1.0));
	expectRoundTrip(units::exbibytes<double>(1.0));
	expectRoundTrip(units::bits<double>(8.0));
	expectRoundTrip(units::gigabits<double>(10.0));
}

TEST_F(Serialization, roundTripDerivedDimensions)
{
	expectRoundTrip(units::meters_per_second<double>(26.8224));
	expectRoundTrip(units::miles_per_hour<double>(60.0));
	expectRoundTrip(units::knots<double>(100.0));
	expectRoundTrip(units::meters_per_second_squared<double>(9.81));
	expectRoundTrip(units::newtons<double>(10.0));
	expectRoundTrip(units::force::pounds<double>(25.0));
}

TEST_F(Serialization, roundTripEnergyPowerPressure)
{
	expectRoundTrip(units::joules<double>(500.0));
	expectRoundTrip(units::kilowatt_hours<double>(3.0));
	expectRoundTrip(units::watts<double>(60.0));
	expectRoundTrip(units::horsepower<double>(1.0));
	expectRoundTrip(units::pascals<double>(101325.0));
	expectRoundTrip(units::atmospheres<double>(1.0));
	expectRoundTrip(units::pounds_per_square_inch<double>(14.7));
}

TEST_F(Serialization, roundTripElectromagnetic)
{
	expectRoundTrip(units::volts<double>(120.0));
	expectRoundTrip(units::farads<double>(0.001));
	expectRoundTrip(units::ohms<double>(50.0));
	expectRoundTrip(units::henries<double>(2.0));
	expectRoundTrip(units::webers<double>(1.0));
	expectRoundTrip(units::teslas<double>(1.5));
}

TEST_F(Serialization, roundTripAreaVolumeFlow)
{
	expectRoundTrip(units::square_meters<double>(25.0));
	expectRoundTrip(units::acres<double>(2.0));
	expectRoundTrip(units::cubic_meters<double>(3.0));
	expectRoundTrip(units::liters<double>(2.0));
	expectRoundTrip(units::gallons<double>(5.0));
	expectRoundTrip(units::liters_per_second<double>(1.5));
	expectRoundTrip(units::gallons_per_minute<double>(10.0));
	expectRoundTrip(units::cubic_feet_per_second<double>(1.0));
}

TEST_F(Serialization, roundTripFrequencyDensityTorque)
{
	expectRoundTrip(units::hertz<double>(60.0));
	expectRoundTrip(units::kilohertz<double>(44.1));
	expectRoundTrip(units::kilograms_per_cubic_meter<double>(1000.0));
	expectRoundTrip(units::newton_meters<double>(50.0));
}

TEST_F(Serialization, roundTripDecibelScale)
{
	// the decibel scale is non-linear (logarithmic); the stored SI-base value must still round-trip the dB reading
	expectRoundTrip(units::dBW<double>(10.0));
	expectRoundTrip(units::dBW<double>(-20.0));
	expectRoundTrip(units::dBm<double>(-3.0));
	expectRoundTrip(units::decibels<double>(6.0));
	expectRoundTrip(units::decibels<double>(0.0));

	// a decibel stream collapses back to the same reading
	const auto v = units::deserialize(units::serialize(units::dBW<double>(23.0)));
	ASSERT_TRUE(v);
	const auto back = v->to<units::dBW<double>>();
	ASSERT_TRUE(back);
	EXPECT_NEAR(23.0, back->to<double>(), 5.0e-9);
}

TEST_F(Serialization, crossUnitConversionNotTautological)
{
	// These serialize as unit A and deserialize into a DIFFERENT unit B of the same dimension, then assert against
	// values computed here by hand (NOT read back from the library). A tautological round-trip (serialize/deserialize
	// merely shuffling the same number) would fail these, because the value must actually cross a unit conversion.

	// 60 mph -> m/s : 1 mile = 1609.344 m, 1 h = 3600 s -> 60*1609.344/3600 = 26.8224 exactly
	{
		const auto v = units::deserialize(units::serialize(units::miles_per_hour<double>(60.0)));
		ASSERT_TRUE(v);
		const auto mpsA = v->to<units::meters_per_second<double>>();
		ASSERT_TRUE(mpsA);
		EXPECT_NEAR(26.8224, mpsA->value(), 5.0e-10);
	}
	// 100 ft -> m : 1 ft = 0.3048 m -> 30.48 m
	{
		const auto v = units::deserialize(units::serialize(units::feet<double>(100.0)));
		ASSERT_TRUE(v);
		EXPECT_NEAR(30.48, v->to<units::meters<double>>()->value(), 5.0e-11);
	}
	// 1 kg -> pounds (mass): 1 lb = 0.45359237 kg -> 1/0.45359237 = 2.2046226218... lb
	{
		const auto v = units::deserialize(units::serialize(units::kilograms<double>(1.0)));
		ASSERT_TRUE(v);
		EXPECT_NEAR(2.2046226218487757, v->to<units::mass::pounds<double>>()->value(), 5.0e-12);
	}
	// 1 hour -> seconds : 3600
	{
		const auto v = units::deserialize(units::serialize(units::hours<double>(1.0)));
		ASSERT_TRUE(v);
		EXPECT_DOUBLE_EQ(3600.0, v->to<units::seconds<double>>()->value());
	}
	// 100 celsius -> fahrenheit : 100*9/5 + 32 = 212 (affine translation must survive the stream)
	{
		const auto v = units::deserialize(units::serialize(units::celsius<double>(100.0)));
		ASSERT_TRUE(v);
		EXPECT_NEAR(212.0, v->to<units::fahrenheit<double>>()->value(), 5.0e-11);
	}
	// 0 celsius -> kelvin : 273.15
	{
		const auto v = units::deserialize(units::serialize(units::celsius<double>(0.0)));
		ASSERT_TRUE(v);
		EXPECT_NEAR(273.15, v->to<units::kelvin<double>>()->value(), 5.0e-12);
	}
	// 180 degrees -> radians : pi
	{
		const auto v = units::deserialize(units::serialize(units::degrees<double>(180.0)));
		ASSERT_TRUE(v);
		EXPECT_NEAR(3.141592653589793, v->to<units::radians<double>>()->value(), 5.0e-15);
	}
	// 1 kibibyte -> bytes : 1024 ; 1 kilobyte -> bytes : 1000 (binary vs decimal prefixes, distinct)
	{
		const auto kib = units::deserialize(units::serialize(units::kibibytes<double>(1.0)));
		const auto kbV  = units::deserialize(units::serialize(units::kilobytes<double>(1.0)));
		ASSERT_TRUE(kib && kbV);
		EXPECT_DOUBLE_EQ(1024.0, kib->to<units::bytes<double>>()->value());
		EXPECT_DOUBLE_EQ(1000.0, kbV->to<units::bytes<double>>()->value());
	}
	// 1 atmosphere -> pascals : 101325 exactly
	{
		const auto v = units::deserialize(units::serialize(units::atmospheres<double>(1.0)));
		ASSERT_TRUE(v);
		EXPECT_NEAR(101325.0, v->to<units::pascals<double>>()->value(), 5.0e-7);
	}
	// 1 kWh -> joules : 3.6e6
	{
		const auto v = units::deserialize(units::serialize(units::kilowatt_hours<double>(1.0)));
		ASSERT_TRUE(v);
		EXPECT_NEAR(3.6e6, v->to<units::joules<double>>()->value(), 5.0e-3);
	}
	// serialize m/s, read back as mph : 26.8224 m/s -> 60 mph (the reverse of the first case)
	{
		const auto v = units::deserialize(units::serialize(units::meters_per_second<double>(26.8224)));
		ASSERT_TRUE(v);
		EXPECT_NEAR(60.0, v->to<units::miles_per_hour<double>>()->value(), 5.0e-10);
	}
}

TEST_F(Serialization, edgeValues)
{
	expectRoundTrip(units::meters<double>(0.0));
	expectRoundTrip(units::meters<double>(-42.5));
	expectRoundTrip(units::meters<double>(1e300));
	expectRoundTrip(units::meters<double>(1e-300));
	expectRoundTrip(units::meters<double>(3.141592653589793));
	expectRoundTrip(units::meters<double>(9000000000000.0));
	expectRoundTrip(units::meters<double>(std::numeric_limits<double>::quiet_NaN()));
	expectRoundTrip(units::meters<double>(std::numeric_limits<double>::infinity()));
	expectRoundTrip(units::meters<double>(-std::numeric_limits<double>::infinity()));
}

TEST_F(Serialization, underlyingTypes)
{
	expectRoundTrip(units::meters<double>(1.5));
	expectRoundTrip(units::meters<float>(1.5f));
	expectRoundTrip(units::meters<int>(7));
	expectRoundTrip(units::seconds<float>(0.25f));
	expectRoundTrip(units::kilograms<int>(5));
}

TEST_F(Serialization, valueKinds)
{
	// whole value -> integer varint (tersest)
	EXPECT_LT(units::serialize(units::meters<double>(5.0)).size(), units::serialize(units::meters<double>(5.5)).size());
	// exact-float value -> f32; irrational -> f64
	const auto f32bytes = units::serialize(units::meters<double>(1.5));   // exact as float
	const auto f64bytes = units::serialize(units::meters<double>(0.1));   // not exact as float
	EXPECT_LT(f32bytes.size(), f64bytes.size());
}

TEST_F(Serialization, collapseMethods)
{
	const auto bytes = units::serialize(60.0_mph);
	const auto v     = units::deserialize(bytes);
	ASSERT_TRUE(v);

	// to<> : safe, expected
	const auto mpsV = v->to<units::meters_per_second<double>>();
	ASSERT_TRUE(mpsV);
	EXPECT_NEAR(26.8224, mpsV->value(), 5.0e-9);
	const auto mphV = v->to<units::miles_per_hour<double>>();
	ASSERT_TRUE(mphV);
	EXPECT_NEAR(60.0, mphV->value(), 5.0e-9);

	// to<> wrong dimension -> error, no throw
	const auto wrong = v->to<units::kilograms<double>>();
	EXPECT_FALSE(wrong);
	EXPECT_EQ(units::deserialize_error::dimension_mismatch, wrong.error());

	// try_to : throwing
	EXPECT_NEAR(26.8224, v->try_to<units::meters_per_second<double>>().value(), 5.0e-9);
	EXPECT_THROW((void)v->try_to<units::kilograms<double>>(), std::runtime_error);

	// unit_cast : the free-function throwing idiom, same result as try_to
	EXPECT_NEAR(26.8224, units::unit_cast<units::meters_per_second<double>>(*v).value(), 5.0e-9);
	EXPECT_THROW((void)units::unit_cast<units::kilograms<double>>(*v), std::runtime_error);

	// visit : canonical unit, no target named
	bool visited = false;
	v->visit(
		[&](auto q)
		{
			visited = true;
			EXPECT_NEAR(26.8224, q.template to<double>(), 5.0e-9);
		});
	EXPECT_TRUE(visited);

	// is<>
	EXPECT_TRUE(v->is<units::dimension::velocity>());
	EXPECT_FALSE(v->is<units::dimension::mass>());
	EXPECT_NEAR(26.8224, v->value_in_base(), 5.0e-9);
}

TEST_F(Serialization, visitResolvesManyDimensions)
{
	auto check = [](auto quantity, double expectedBase)
	{
		const auto v = units::deserialize(units::serialize(quantity));
		ASSERT_TRUE(v);
		bool visited = false;
		v->visit(
			[&](auto q)
			{
				visited = true;
				EXPECT_NEAR(expectedBase, q.template to<double>(), std::abs(expectedBase) * 1e-9 + 1e-12);
			});
		EXPECT_TRUE(visited);
	};
	check(units::meters<double>(100.0), 100.0);
	check(units::newtons<double>(10.0), 10.0);
	check(units::joules<double>(5.0), 5.0);
	check(units::watts<double>(60.0), 60.0);
	check(units::pascals<double>(101325.0), 101325.0);
	check(units::hertz<double>(60.0), 60.0);
	check(units::amperes<double>(2.0), 2.0);
	check(units::liters_per_second<double>(1.0), 0.001);
}

TEST_F(Serialization, lossyIntegerTargetRejected)
{
	// a fractional value cannot be represented in an integer-underlying target
	const auto bytes = units::serialize(1.5_m);
	const auto v     = units::deserialize(bytes);
	ASSERT_TRUE(v);
	const auto asInt = v->to<units::meters<int>>();
	EXPECT_FALSE(asInt);
	EXPECT_EQ(units::deserialize_error::lossy_target, asInt.error());
	// a whole value converts to an integer target fine
	const auto whole = units::deserialize(units::serialize(units::meters<double>(3.0)));
	ASSERT_TRUE(whole);
	const auto ok = whole->to<units::meters<int>>();
	ASSERT_TRUE(ok);
	EXPECT_EQ(3, ok->value());
}

TEST_F(Serialization, errorPaths)
{
	// an owning copy of the bytes, so the tamper cases below can mutate it
	const units::any_unit    encoded = units::serialize(60.0_mph);
	const std::vector<std::byte> good(encoded.bytes().begin(), encoded.bytes().end());

	// empty buffer -> truncated
	{
		std::vector<std::byte> empty;
		const auto             r = units::deserialize(empty);
		EXPECT_FALSE(r);
		EXPECT_EQ(units::deserialize_error::truncated, r.error());
	}
	// bad version byte
	{
		auto bad = good;
		bad[0]   = std::byte{0xFF};
		const auto r = units::deserialize(bad);
		EXPECT_FALSE(r);
		EXPECT_EQ(units::deserialize_error::bad_version, r.error());
	}
	// truncated mid-stream (drop the value/tail)
	{
		std::vector<std::byte> partial(good.begin(), good.begin() + 3);
		const auto             r = units::deserialize(partial);
		EXPECT_FALSE(r);
		EXPECT_EQ(units::deserialize_error::truncated, r.error());
	}
	// typed deserialize of a wrong dimension -> dimension_mismatch
	{
		const auto r = units::deserialize<units::kilograms<double>>(good);
		EXPECT_FALSE(r);
		EXPECT_EQ(units::deserialize_error::dimension_mismatch, r.error());
	}
}

TEST_F(Serialization, userDefinedDimensionIsExtensible)
{
	// the whole point: a dimension the library never defined round-trips with no central table.
	// serialize via the generated unit constant (units::px, in the inline screen namespace) to exercise it too.
	const auto bytes = units::serialize(1920.0 * units::px);
	const auto v     = units::deserialize(bytes);
	ASSERT_TRUE(v);
	EXPECT_TRUE(v->is<units::dimension::pixels>());
	EXPECT_FALSE(v->is<units::dimension::length>());
	const auto pxV = v->to<units::screen::dots<double>>();
	ASSERT_TRUE(pxV);
	EXPECT_DOUBLE_EQ(1920.0, pxV->value());
	// zero-candidate visit cannot know a user dimension (throws); explicit candidate resolves it
	EXPECT_THROW(v->visit([](auto) {}), std::runtime_error);
	bool visited = false;
	v->visit<units::dimension::pixels>([&](auto q) { visited = true; EXPECT_DOUBLE_EQ(1920.0, q.value()); });
	EXPECT_TRUE(visited);
}

TEST_F(Serialization, compoundOfUserDimensions)
{
	// a compound spanning multiple base dimensions (pixels / second): arbitrary arity, no fixed ceiling
	auto rate  = units::screen::dots<double>(60.0) / units::seconds<double>(1.0);
	auto bytes = units::serialize(rate);
	auto v     = units::deserialize(bytes);
	ASSERT_TRUE(v);
	EXPECT_EQ(2u, v->identity().terms.size());
	auto back = v->to<decltype(rate)>();
	ASSERT_TRUE(back);
	EXPECT_DOUBLE_EQ(rate.template to<double>(), back->template to<double>());
}

TEST_F(Serialization, dimensionMismatchAcrossManyPairs)
{
	// every serialized dimension rejects collapse into an unrelated dimension
	auto reject = [](auto quantity, auto wrongTargetPrototype)
	{
		using Wrong  = decltype(wrongTargetPrototype);
		const auto v = units::deserialize(units::serialize(quantity));
		ASSERT_TRUE(v);
		const auto r = v->template to<Wrong>();
		EXPECT_FALSE(r);
		if (!r)
		{
			EXPECT_EQ(units::deserialize_error::dimension_mismatch, r.error());
		}
	};
	reject(units::meters<double>(1.0), units::seconds<double>(0.0));
	reject(units::kilograms<double>(1.0), units::newtons<double>(0.0));
	reject(units::joules<double>(1.0), units::watts<double>(0.0));
	reject(units::hertz<double>(1.0), units::seconds<double>(0.0));
	reject(units::amperes<double>(1.0), units::coulombs<double>(0.0));
	reject(units::square_meters<double>(1.0), units::meters<double>(0.0));
	reject(units::cubic_meters<double>(1.0), units::square_meters<double>(0.0));
	reject(units::meters_per_second<double>(1.0), units::meters<double>(0.0));
}

TEST_F(Serialization, wireStabilityGolden)
{
	// a frozen fixture guards the wire format against silent drift. 100 m: version(1) + header(kind=ivarint,0)
	// + count(1) + hash("length" 8 bytes) + zigzag-exponent(1 -> 2) + value-varint(100 -> 200 = 0xC8 0x01)
	const units::any_unit bytesU = units::serialize(units::meters<double>(100.0));
	const auto bytes = bytesU.bytes();
	ASSERT_EQ(14u, bytes.size());
	EXPECT_EQ(std::byte{1}, bytes[0]);       // version
	EXPECT_EQ(std::byte{0}, bytes[1]);       // header: value_kind::ivarint, no fracExp
	EXPECT_EQ(std::byte{1}, bytes[2]);       // one dimension term
	// bytes[3..10] the 8-byte length-name hash; bytes[11] exponent(=2 zigzag); bytes[12..13] value 100 (varint 200)
	const std::uint64_t lengthHash = units::detail::name_hash("length");
	for (unsigned int i = 0; i < 8; ++i)
		EXPECT_EQ(std::byte{static_cast<std::uint8_t>(lengthHash >> (8 * i))}, bytes[3 + i]);
}

TEST_F(Serialization, nameHashIsStableAndDistinct)
{
	// the wire key is a hash of the dimension name; distinct built-in names must not collide
	std::array<std::string_view, 9> names{"length", "mass", "time", "current", "temperature", "amount of substance", "luminous intensity", "angle", "data"};
	for (std::size_t i = 0; i < names.size(); ++i)
		for (std::size_t j = i + 1; j < names.size(); ++j)
			EXPECT_NE(units::detail::name_hash(names[i]), units::detail::name_hash(names[j]));
	// stable: same input, same hash
	EXPECT_EQ(units::detail::name_hash("length"), units::detail::name_hash("length"));
}

// ---- second wave: broader, more redundant coverage --------------------------------------------------------------

TEST_F(Serialization, roundTripManyLengthUnits)
{
	expectRoundTrip(units::millimeters<double>(12.0));
	expectRoundTrip(units::centimeters<double>(2.5));
	expectRoundTrip(units::decimeters<double>(3.0));
	expectRoundTrip(units::micrometers<double>(50.0));
	expectRoundTrip(units::inches<double>(6.0));
	expectRoundTrip(units::yards<double>(100.0));
	expectRoundTrip(units::chains<double>(2.0));
	expectRoundTrip(units::fathoms<double>(3.0));
	expectRoundTrip(units::mils<double>(500.0));
	expectRoundTrip(units::astronomical_units<double>(1.0));
	expectRoundTrip(units::lightyears<double>(4.0));
	expectRoundTrip(units::parsecs<double>(1.0));
	expectRoundTrip(units::hands<double>(15.0));
	expectRoundTrip(units::barleycorns<double>(9.0));
}

TEST_F(Serialization, roundTripManyTimeUnits)
{
	expectRoundTrip(units::nanoseconds<double>(250.0));
	expectRoundTrip(units::microseconds<double>(100.0));
	expectRoundTrip(units::days<double>(3.0));
	expectRoundTrip(units::weeks<double>(2.0));
	expectRoundTrip(units::years<double>(1.0));
	expectRoundTrip(units::decades<double>(1.0));
	expectRoundTrip(units::centuries<double>(1.0));
}

TEST_F(Serialization, roundTripManyVelocityUnits)
{
	expectRoundTrip(units::feet_per_second<double>(100.0));
	expectRoundTrip(units::kilometers_per_hour<double>(120.0));
	expectRoundTrip(units::feet_per_minute<double>(500.0));
	expectRoundTrip(units::inches_per_second<double>(12.0));
	expectRoundTrip(units::kilometers_per_second<double>(7.8));
}

TEST_F(Serialization, roundTripManyPressureUnits)
{
	expectRoundTrip(units::bars<double>(2.0));
	expectRoundTrip(units::millibars<double>(1013.0));
	expectRoundTrip(units::torrs<double>(760.0));
	expectRoundTrip(units::millimeters_of_mercury<double>(760.0));
	expectRoundTrip(units::kilopascals<double>(101.325));
	expectRoundTrip(units::baryes<double>(10.0));
}

TEST_F(Serialization, roundTripManyEnergyPowerUnits)
{
	expectRoundTrip(units::kilojoules<double>(4.0));
	expectRoundTrip(units::calories<double>(500.0));
	expectRoundTrip(units::british_thermal_units<double>(1.0));
	expectRoundTrip(units::ergs<double>(1000.0));
	expectRoundTrip(units::watt_hours<double>(50.0));
	expectRoundTrip(units::kilowatts<double>(3.0));
	expectRoundTrip(units::megawatts<double>(1.0));
	expectRoundTrip(units::metric_horsepower<double>(2.0));
}

TEST_F(Serialization, everyBuiltinDimensionRoundTripsViaVisit)
{
	// serialize one quantity per built-in dimension and confirm the default zero-candidate visit resolves it,
	// with the visited canonical value equal to the SI-base value. Exhaustive over the dimension zoo.
	auto viaVisit = [](auto quantity)
	{
		const double base = units::detail::canonical_unit_t<traits::dimension_of_t<typename decltype(quantity)::conversion_factor>>(quantity).value();
		const auto   v    = units::deserialize(units::serialize(quantity));
		EXPECT_TRUE(v.has_value());
		bool visited = false;
		v->visit(
			[&](auto q)
			{
				visited = true;
				EXPECT_NEAR(base, q.template to<double>(), std::abs(base) * 1e-9 + 1e-12);
			});
		EXPECT_TRUE(visited);
	};
	viaVisit(units::meters<double>(2.0));
	viaVisit(units::kilograms<double>(3.0));
	viaVisit(units::seconds<double>(4.0));
	viaVisit(units::amperes<double>(1.0));
	viaVisit(units::kelvin<double>(300.0));
	viaVisit(units::mols<double>(2.0));
	viaVisit(units::candelas<double>(5.0));
	viaVisit(units::radians<double>(1.0));
	viaVisit(units::steradians<double>(1.0));
	viaVisit(units::bytes<double>(64.0));
	viaVisit(units::hertz<double>(50.0));
	viaVisit(units::meters_per_second<double>(10.0));
	viaVisit(units::radians_per_second<double>(2.0));
	viaVisit(units::meters_per_second_squared<double>(9.8));
	viaVisit(units::newtons<double>(5.0));
	viaVisit(units::square_meters<double>(4.0));
	viaVisit(units::cubic_meters<double>(2.0));
	viaVisit(units::liters_per_second<double>(1.0));
	viaVisit(units::pascals<double>(1000.0));
	viaVisit(units::coulombs<double>(1.0));
	viaVisit(units::joules<double>(7.0));
	viaVisit(units::watts<double>(9.0));
	viaVisit(units::volts<double>(12.0));
	viaVisit(units::farads<double>(0.01));
	viaVisit(units::ohms<double>(100.0));
	viaVisit(units::siemens<double>(0.1));
	viaVisit(units::webers<double>(1.0));
	viaVisit(units::henries<double>(2.0));
	viaVisit(units::lumens<double>(500.0));
	viaVisit(units::lux<double>(300.0));
	viaVisit(units::newton_meters<double>(20.0));
	viaVisit(units::kilograms_per_cubic_meter<double>(998.0));
}

// #395: dimensionless and the angular acceleration/jerk dimensions were added to builtin_dimensions, so the default
// zero-candidate visit resolves them and to_string() names them instead of rendering the raw '#<hash>' fallback.
// The angular acceleration and jerk dimensions resolve through the default visit() candidate set and render with a
// name, not the raw '#<hash>' fallback -- the substantive #395 additions.
TEST_F(Serialization, angularAccelerationAndJerkResolveByDefaultVisit)
{
	bool visited = false;

	visited = false;
	units::serialize(units::radians_per_second_squared<double>(1.375)).visit([&](const auto&) { visited = true; });
	EXPECT_TRUE(visited);
	EXPECT_EQ(std::string::npos, units::serialize(units::radians_per_second_squared<double>(1.375)).to_string().find('#'));

	visited = false;
	units::serialize(units::radians_per_second_cubed<double>(2.125)).visit([&](const auto&) { visited = true; });
	EXPECT_TRUE(visited);
	EXPECT_EQ(std::string::npos, units::serialize(units::radians_per_second_cubed<double>(2.125)).to_string().find('#'));
}

// A generic units-visitor -- the visit() contract's `[](auto q)` shape -- also accepts a plain arithmetic value,
// because a dimensionless unit converts implicitly to its underlying type and `.to<double>()` reads any unit. This
// establishes that dispatching the empty (dimensionless) signature as a raw scalar rather than a unit wrapper would
// satisfy the same visitor. It also shows the limit: a serialized percent and a serialized plain scalar of equal
// physical value are indistinguishable once erased, so a scalar dispatch recovers the value but never the "percent"
// scale -- the same identity collapse every unit undergoes (a serialized kilometer visits as canonical meters).
TEST_F(Serialization, genericVisitorAcceptsScalarAndPercentCollapsesToValue)
{
	using units::concentration::percent;

	// A generic visitor written for units reads a raw double through the same `.to<double>()` / implicit-scalar path.
	const auto readValue = [](const auto& q) -> double {
		if constexpr (units::traits::is_unit_v<std::decay_t<decltype(q)>>)
			return q.template to<double>();
		else
			return static_cast<double>(q); // a raw arithmetic value is read directly
	};
	EXPECT_DOUBLE_EQ(0.3725, readValue(units::dimensionless<double>(0.3725)));
	EXPECT_DOUBLE_EQ(0.3725, readValue(0.3725)); // the same visitor handles a bare double

	// percent(37.25) is the physical 0.3725; erased, it is identical to dimensionless(0.3725), so neither the value
	// nor a scalar/canonical dispatch can recover the percent scale -- it must be named explicitly to read as percent.
	EXPECT_DOUBLE_EQ(percent<double>(37.25).to<double>(), units::dimensionless<double>(0.3725).to<double>());
}

// Every ratio-scaled dimensionless unit (percent, parts-per-million/billion/trillion) and a plain dimensionless
// quantity of the SAME physical value share ONE erased dimension identity: the empty base signature. So the wire
// form carries only the physical (SI-base) value, never the "percent"/"ppm" scale label. This documents the limit
// and the safe recovery path -- the crux of why dimensionless is not a default visit() candidate: dispatching the
// empty signature (as a unit OR as a scalar) can only ever yield the physical value, and would silently flatten
// every ratio-scaled dimensionless quantity to that value. Naming the exact unit (to<percent>() / visit<percent>)
// is the only way to read it back on its own scale.
TEST_F(Serialization, ratioScaledDimensionlessEraseToPhysicalValueOnly)
{
	using units::concentration::percent;
	using units::concentration::parts_per_million;
	using units::concentration::parts_per_billion;

	// 37.25 percent, 372500 ppm, and 372500000 ppb are all the physical value 0.3725; each erases identically to a
	// plain dimensionless(0.3725). Equal renderings, no raw '#' hash.
	const auto pct  = units::serialize(percent<double>(37.25));
	const auto ppm  = units::serialize(parts_per_million<double>(372500.0));
	const auto ppb  = units::serialize(parts_per_billion<double>(372500000.0));
	const auto bare = units::serialize(units::dimensionless<double>(0.3725));
	EXPECT_EQ(bare.to_string(), pct.to_string());
	EXPECT_EQ(bare.to_string(), ppm.to_string());
	EXPECT_EQ(bare.to_string(), ppb.to_string());
	EXPECT_EQ(std::string::npos, pct.to_string().find('#'));

	// Reading each back by VALUE recovers the physical magnitude (0.3725) -- the scale is gone.
	const auto asValue = units::serialize(percent<double>(37.25)).to<units::dimensionless<double>>();
	ASSERT_TRUE(asValue.has_value());
	EXPECT_DOUBLE_EQ(0.3725, asValue->to<double>());

	// Reading it back NAMED as percent recovers the percent scale (raw 37.25) -- the caller must supply the unit.
	const auto asPercent = units::serialize(percent<double>(37.25)).to<percent<double>>();
	ASSERT_TRUE(asPercent.has_value());
	EXPECT_DOUBLE_EQ(37.25, asPercent->raw());
	EXPECT_DOUBLE_EQ(0.3725, asPercent->to<double>());

	// A default visit() resolves the empty signature to dimensionless and hands the visitor the physical value, the
	// same for percent, ppm, ppb, and a bare scalar -- proving a canonical dispatch cannot preserve the scale.
	double vPct = 0.0, vPpm = 0.0;
	units::serialize(percent<double>(37.25)).visit([&](const auto& q) { vPct = q.template to<double>(); });
	units::serialize(parts_per_million<double>(372500.0)).visit([&](const auto& q) { vPpm = q.template to<double>(); });
	EXPECT_DOUBLE_EQ(0.3725, vPct);
	EXPECT_DOUBLE_EQ(vPct, vPpm);
}

// A dimensionless stream resolves under the default candidate set instead of hard-throwing: the empty signature is
// dispatched to dimensionless directly. This covers the common case of a same-dimension ratio (meters / meters),
// which is dimensionless, being serialized and visited -- it must not throw. A dimensionless renders as a bare
// number with no dimension tag. An explicit candidate set that does not include the stream's dimension still throws.
TEST_F(Serialization, dimensionlessResolvesUnderDefaultVisitWithoutThrowing)
{
	const auto ratio = units::meters<double>(3.0) / units::meters<double>(4.0); // 0.75, dimensionless
	const auto blob  = units::serialize(ratio);

	bool   visited = false;
	double value   = 0.0;
	EXPECT_NO_THROW(blob.visit([&](const auto& q) { visited = true; value = q.template to<double>(); }));
	EXPECT_TRUE(visited);
	EXPECT_DOUBLE_EQ(0.75, value);

	// Bare number, no "[dimensionless]" tag, no raw '#' hash.
	EXPECT_EQ("0.75", blob.to_string());
	EXPECT_EQ(std::string::npos, blob.to_string().find('['));

	// A plain scalar likewise resolves rather than throwing.
	EXPECT_NO_THROW(units::serialize(units::dimensionless<double>(0.53125)).visit([](const auto&) {}));

	// But an explicit, mismatched candidate set is still strict: naming the wrong dimension throws.
	EXPECT_THROW(units::serialize(units::meters<double>(5.0)).visit<units::dimension::mass>([](const auto&) {}), std::runtime_error);
}

// #395 recurrence guard: every dimension in serialization.h's builtin_dimensions tuple must resolve to a named
// rendering. A dimension missing from the tuple degrades to the raw '#<hash>' fallback, which this fails on. When a
// new dimension is added to the library, add its flagship unit line here; if this test then fails, the dimension was
// omitted from builtin_dimensions. One flagship unit per dimension, in tuple order.
TEST_F(Serialization, everyShippedDimensionResolvesByDefault)
{
	EXPECT_EQ(std::string::npos, units::serialize(units::dimensionless<double>(0.5)).to_string().find('#')) << "dimensionless missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::meters<double>(1.0)).to_string().find('#')) << "length missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::kilograms<double>(1.0)).to_string().find('#')) << "mass missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::seconds<double>(1.0)).to_string().find('#')) << "time missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::amperes<double>(1.0)).to_string().find('#')) << "current missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::kelvin<double>(1.0)).to_string().find('#')) << "temperature missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::mols<double>(1.0)).to_string().find('#')) << "substance missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::candelas<double>(1.0)).to_string().find('#')) << "luminous_intensity missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::radians<double>(1.0)).to_string().find('#')) << "angle missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::bytes<double>(1.0)).to_string().find('#')) << "data missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::steradians<double>(1.0)).to_string().find('#')) << "solid_angle missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::hertz<double>(1.0)).to_string().find('#')) << "frequency missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::meters_per_second<double>(1.0)).to_string().find('#')) << "velocity missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::radians_per_second<double>(1.0)).to_string().find('#')) << "angular_velocity missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::radians_per_second_squared<double>(1.0)).to_string().find('#')) << "angular_acceleration missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::radians_per_second_cubed<double>(1.0)).to_string().find('#')) << "angular_jerk missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::meters_per_second_squared<double>(1.0)).to_string().find('#')) << "acceleration missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::newtons<double>(1.0)).to_string().find('#')) << "force missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::square_meters<double>(1.0)).to_string().find('#')) << "area missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::cubic_meters<double>(1.0)).to_string().find('#')) << "volume missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::cubic_meters_per_second<double>(1.0)).to_string().find('#')) << "volume_flow_rate missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::pascals<double>(1.0)).to_string().find('#')) << "pressure missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::coulombs<double>(1.0)).to_string().find('#')) << "charge missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::joules<double>(1.0)).to_string().find('#')) << "energy missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::watts<double>(1.0)).to_string().find('#')) << "power missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::volts<double>(1.0)).to_string().find('#')) << "voltage missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::farads<double>(1.0)).to_string().find('#')) << "capacitance missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::ohms<double>(1.0)).to_string().find('#')) << "impedance missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::siemens<double>(1.0)).to_string().find('#')) << "conductance missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::webers<double>(1.0)).to_string().find('#')) << "magnetic_flux missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::henries<double>(1.0)).to_string().find('#')) << "inductance missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::lumens<double>(1.0)).to_string().find('#')) << "luminous_flux missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::lux<double>(1.0)).to_string().find('#')) << "illuminance missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::candelas_per_square_meter<double>(1.0)).to_string().find('#')) << "luminance missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::becquerels<double>(1.0)).to_string().find('#')) << "radioactivity missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::grams_per_mole<double>(1.0)).to_string().find('#')) << "substance_mass missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::molars<double>(1.0)).to_string().find('#')) << "substance_concentration missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::teslas<double>(1.0)).to_string().find('#')) << "magnetic_field_strength missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::watts_per_steradian<double>(1.0)).to_string().find('#')) << "radiant_intensity missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::watts_per_steradian_per_meter_squared<double>(1.0)).to_string().find('#')) << "radiance missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::watts_per_meter_squared<double>(1.0)).to_string().find('#')) << "irradiance missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::watts_per_steradian_per_meter<double>(1.0)).to_string().find('#')) << "spectral_intensity missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::watts_per_meter<double>(1.0)).to_string().find('#')) << "spectral_flux missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::watts_per_steradian_per_meter_cubed<double>(1.0)).to_string().find('#')) << "spectral_radiance missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::watts_per_meter_cubed<double>(1.0)).to_string().find('#')) << "spectral_irradiance missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::meters_per_second_cubed<double>(1.0)).to_string().find('#')) << "jerk missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::newton_meters<double>(1.0)).to_string().find('#')) << "torque missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::kilograms_per_cubic_meter<double>(1.0)).to_string().find('#')) << "density missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::joules_per_meter_cubed<double>(1.0)).to_string().find('#')) << "energy_density missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::concentration::percent<double>(1.0)).to_string().find('#')) << "concentration missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::bytes_per_second<double>(1.0)).to_string().find('#')) << "data_transfer_rate missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::pascal_seconds<double>(1.0)).to_string().find('#')) << "dynamic_viscosity missing from builtin_dimensions";
	EXPECT_EQ(std::string::npos, units::serialize(units::square_meters_per_second<double>(1.0)).to_string().find('#')) << "kinematic_viscosity missing from builtin_dimensions";
}

TEST_F(Serialization, determinismSameInputSameBytes)
{
	// serialization is a pure function of the value: same quantity -> identical bytes, every time
	EXPECT_TRUE(std::ranges::equal(units::serialize(60.0_mph).bytes(), units::serialize(60.0_mph).bytes()));
	EXPECT_TRUE(std::ranges::equal(units::serialize(units::meters<double>(3.14)).bytes(),
								   units::serialize(units::meters<double>(3.14)).bytes()));
}

TEST_F(Serialization, distinctQuantitiesDistinctBytes)
{
	// different value -> different quantity; different dimension -> different quantity
	EXPECT_NE(units::serialize(units::meters<double>(1.0)), units::serialize(units::meters<double>(2.0)));
	EXPECT_NE(units::serialize(units::meters<double>(1.0)), units::serialize(units::seconds<double>(1.0)));
	// same dimension, different unit but SAME base value -> EQUAL (self-describing by dimension+base)
	EXPECT_EQ(units::serialize(units::meters<double>(1000.0)), units::serialize(units::kilometers<double>(1.0)));
	// and byte-identical on the wire, since the encoding is a pure function of dimension + base
	EXPECT_TRUE(std::ranges::equal(units::serialize(units::meters<double>(1000.0)).bytes(),
								   units::serialize(units::kilometers<double>(1.0)).bytes()));
}

TEST_F(Serialization, valueKindIvarintForWholeNumbers)
{
	// a whole SI-base value uses the integer-varint kind (header low bits == 0)
	const units::any_unit wholeU = units::serialize(units::meters<double>(42.0));
	const auto whole = wholeU.bytes();
	EXPECT_EQ(std::byte{0}, std::byte{static_cast<std::uint8_t>(std::to_integer<std::uint8_t>(whole[1]) & 0x03)});
	// an exact-float value uses f32 (kind == 1)
	const units::any_unit f32U = units::serialize(units::meters<double>(1.5));
	const auto f32 = f32U.bytes();
	EXPECT_EQ(1, std::to_integer<std::uint8_t>(f32[1]) & 0x03);
	// an irrational value uses f64 (kind == 2)
	const units::any_unit f64U = units::serialize(units::meters<double>(0.1));
	const auto f64 = f64U.bytes();
	EXPECT_EQ(2, std::to_integer<std::uint8_t>(f64[1]) & 0x03);
}

TEST_F(Serialization, negativeAndFractionalExponents)
{
	// frequency is time^-1 (a negative exponent); the fracExp path and negative exponents must survive
	expectRoundTrip(units::hertz<double>(60.0));
	expectRoundTrip(units::becquerels<double>(1000.0));
	// area (length^2) and volume (length^3): positive multi-exponents
	expectRoundTrip(units::square_feet<double>(100.0));
	expectRoundTrip(units::cubic_inches<double>(50.0));
}

TEST_F(Serialization, typedFastPathMatchesErased)
{
	// deserialize<Unit> equals deserialize(...).to<Unit>() for a spread of units
	auto agree = [](auto quantity)
	{
		using Q          = decltype(quantity);
		const auto bytes = units::serialize(quantity);
		const auto typed = units::deserialize<Q>(bytes);
		const auto erased = units::deserialize(bytes);
		ASSERT_TRUE(typed.has_value());
		ASSERT_TRUE(erased.has_value());
		const auto viaErased = erased->template to<Q>();
		ASSERT_TRUE(viaErased.has_value());
		EXPECT_DOUBLE_EQ(typed->template to<double>(), viaErased->template to<double>());
	};
	agree(units::meters<double>(1.5));
	agree(units::newtons<double>(9.81));
	agree(units::celsius<double>(37.0));
	agree(units::dBW<double>(3.0));
	agree(units::gallons_per_minute<double>(12.0));
}

TEST_F(Serialization, peekIdentityMatchesDeserialized)
{
	// deserialize surfaces the identity; it must equal the compile-time identity of the source unit
	const auto v = units::deserialize(units::serialize(60.0_mph));
	ASSERT_TRUE(v);
	EXPECT_TRUE(v->identity() == units::detail::identity_of<units::meters_per_second<double>>());
	EXPECT_FALSE(v->identity() == units::detail::identity_of<units::kilograms<double>>());
}

TEST_F(Serialization, explicitCandidateDisambiguatesSharedSignature)
{
	// torque and energy share a dimension (force*length); an explicit candidate selects which the visitor sees
	const auto v = units::deserialize(units::serialize(units::newton_meters<double>(10.0)));
	ASSERT_TRUE(v);
	bool asTorque = false;
	v->visit<units::dimension::torque>([&](auto q) { asTorque = true; EXPECT_NEAR(10.0, q.template to<double>(), 5.0e-9); });
	EXPECT_TRUE(asTorque);
}

TEST_F(Serialization, truncatedAtEveryBoundary)
{
	// truncating the stream at EVERY length short of complete must yield truncated (never a crash or bad decode)
	const units::any_unit fullU = units::serialize(60.0_mph);
	const auto full = fullU.bytes();
	for (std::size_t n = 0; n < full.size(); ++n)
	{
		std::vector<std::byte> partial(full.begin(), full.begin() + static_cast<std::ptrdiff_t>(n));
		const auto             r = units::deserialize(partial);
		EXPECT_FALSE(r.has_value()) << "n=" << n;
		if (!r)
		{
			EXPECT_TRUE(r.error() == units::deserialize_error::truncated || r.error() == units::deserialize_error::bad_version) << "n=" << n;
		}
	}
	// the full stream decodes
	EXPECT_TRUE(units::deserialize(full).has_value());
}

TEST_F(Serialization, streamOfManyQuantitiesConcatenated)
{
	// serialize several quantities into one buffer and confirm each is independently well-formed on its own
	auto a = units::serialize(units::meters<double>(1.0));
	auto bytesB = units::serialize(units::seconds<double>(2.0));
	auto c = units::serialize(units::kilograms<double>(3.0));
	EXPECT_TRUE(units::deserialize(a)->is<units::dimension::length>());
	EXPECT_TRUE(units::deserialize(bytesB)->is<units::dimension::time>());
	EXPECT_TRUE(units::deserialize(c)->is<units::dimension::mass>());
}

TEST_F(Serialization, floatUnderlyingUsesF32OrSmaller)
{
	// a float-underlying quantity never needs f64 in the stream (its value is representable in <= 4 value bytes)
	const units::any_unit bytesFU = units::serialize(units::meters<float>(3.14159f));
	const auto bytesF = bytesFU.bytes();
	const std::uint8_t kind = std::to_integer<std::uint8_t>(bytesF[1]) & 0x03;
	EXPECT_NE(2, kind); // not f64
}

TEST_F(Serialization, unitCastAndTryToAgree)
{
	const auto v = units::deserialize(units::serialize(units::joules<double>(500.0)));
	ASSERT_TRUE(v);
	EXPECT_DOUBLE_EQ(v->try_to<units::joules<double>>().value(), units::unit_cast<units::joules<double>>(*v).value());
	EXPECT_DOUBLE_EQ(v->try_to<units::kilojoules<double>>().value(), units::unit_cast<units::kilojoules<double>>(*v).value());
}

// serialize returns an any_unit that OWNS its bytes; bytes()/data()/size() are three views of that one buffer.
TEST_F(Serialization, anyUnitOwnsItsBytes)
{
	const units::any_unit q = units::serialize(60.0_mph);
	EXPECT_GT(q.size(), 0u);
	EXPECT_EQ(q.size(), q.bytes().size());
	// data() is a const char* view of the same buffer bytes() spans
	EXPECT_EQ(static_cast<const void*>(q.data()), static_cast<const void*>(q.bytes().data()));
	// the buffer is valid for the object's lifetime: repeated access is stable
	EXPECT_EQ(q.data(), q.data());
	EXPECT_TRUE(std::ranges::equal(q.bytes(), q.bytes()));
}

// The C-interface face (data()/size()) drops straight into a std::ostream::write with NO cast at the call site,
// and the bytes read back from that stream decode without any prior knowledge of the type.
TEST_F(Serialization, dataAndSizeFeedAStreamNoCast)
{
	const units::any_unit q = units::serialize(units::meters<double>(100.0));

	std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
	stream.write(q.data(), static_cast<std::streamsize>(q.size())); // const char* + size, no reinterpret_cast here
	ASSERT_TRUE(stream.good());

	const std::string blob = stream.str();
	ASSERT_EQ(q.size(), blob.size());

	// read the raw bytes back into a fresh byte buffer and decode with no prior type knowledge
	std::vector<std::byte> raw(blob.size());
	std::memcpy(raw.data(), blob.data(), blob.size());
	const auto decoded = units::deserialize(raw);
	ASSERT_TRUE(decoded.has_value());
	const auto back = decoded->to<units::meters<double>>();
	ASSERT_TRUE(back.has_value());
	EXPECT_DOUBLE_EQ(100.0, back->value());
}

// The bytes on disk are self-sufficient: written from one process-state, read into a disconnected buffer, decoded.
TEST_F(Serialization, bytesAreSelfSufficientAcrossAFreshBuffer)
{
	const units::any_unit q = units::serialize(units::kilograms<double>(2.5));

	// copy the bytes into a buffer that has no relationship to q, then drop q entirely
	std::vector<std::byte> detached(q.bytes().begin(), q.bytes().end());

	const auto decoded = units::deserialize(detached);
	ASSERT_TRUE(decoded.has_value());
	const auto mass = decoded->to<units::kilograms<double>>();
	ASSERT_TRUE(mass.has_value());
	EXPECT_DOUBLE_EQ(2.5, mass->value());
}

// any_unit equality is "same dimension AND same base magnitude" — not an encoding or unit-name comparison.
TEST_F(Serialization, equalityIsDimensionAndMagnitude)
{
	// same quantity, different source unit -> equal (both 1000 m in SI base)
	EXPECT_EQ(units::serialize(units::meters<double>(1000.0)), units::serialize(units::kilometers<double>(1.0)));
	// same unit, same value -> equal
	EXPECT_EQ(units::serialize(60.0_mph), units::serialize(60.0_mph));
	// same dimension, different magnitude -> not equal
	EXPECT_NE(units::serialize(units::meters<double>(1.0)), units::serialize(units::meters<double>(2.0)));
	// different dimension, same numeric magnitude -> not equal
	EXPECT_NE(units::serialize(units::meters<double>(1.0)), units::serialize(units::seconds<double>(1.0)));
	// a round-tripped any_unit equals a freshly serialized one of the same quantity
	const auto decoded = units::deserialize(units::serialize(units::meters<double>(3.0)));
	ASSERT_TRUE(decoded.has_value());
	EXPECT_EQ(*decoded, units::serialize(units::meters<double>(3.0)));
	EXPECT_NE(*decoded, units::serialize(units::meters<double>(4.0)));
}

// any_unit is ordered WITHIN a dimension (by base magnitude) and UNORDERED across dimensions (partial_ordering).
TEST_F(Serialization, orderingWithinDimensionOnly)
{
	const units::any_unit shorter = units::serialize(units::meters<double>(3.0));
	const units::any_unit longer  = units::serialize(units::meters<double>(5.0));
	const units::any_unit sameLen = units::serialize(units::kilometers<double>(0.003)); // == 3 m
	const units::any_unit time    = units::serialize(units::seconds<double>(3.0));

	// same dimension -> ordered by base magnitude
	EXPECT_LT(shorter, longer);
	EXPECT_GT(longer, shorter);
	EXPECT_LE(shorter, sameLen);
	EXPECT_GE(sameLen, shorter);
	EXPECT_TRUE((shorter <=> sameLen) == std::partial_ordering::equivalent);
	EXPECT_TRUE((shorter <=> longer) == std::partial_ordering::less);

	// different dimension -> unordered: every relational is false
	EXPECT_FALSE(shorter < time);
	EXPECT_FALSE(shorter > time);
	EXPECT_FALSE(shorter <= time);
	EXPECT_FALSE(shorter >= time);
	EXPECT_TRUE((shorter <=> time) == std::partial_ordering::unordered);
}

// to_string() renders a NAMED-unit text form for a known dimension — the same text a concrete unit streams.
TEST_F(Serialization, toStringNamesKnownDimensions)
{
	// a known dimension renders in its canonical named unit, exactly as operator<<(ostream, unit) would
	const std::string length = units::serialize(units::meters<double>(100.0)).to_string();
	EXPECT_EQ(units::to_string(units::meters<double>(100.0)), length);
	EXPECT_NE(std::string::npos, length.find("100"));
	EXPECT_NE(std::string::npos, length.find('m'));
	EXPECT_EQ(std::string::npos, length.find('#')); // NOT the raw hash form

	// a value expressed in a non-canonical unit still names the canonical unit of its dimension (1 km -> "1000 m")
	const std::string serialized_km = units::serialize(units::kilometers<double>(1.0)).to_string();
	EXPECT_EQ(units::to_string(units::meters<double>(1000.0)), serialized_km);

	// a compound dimension renders its canonical dimension form (m s^-2), still no hash
	const std::string accel = units::serialize(units::meters_per_second_squared<double>(9.81)).to_string();
	EXPECT_EQ("9.81 m s^-2", accel);
	EXPECT_EQ(std::string::npos, accel.find('#'));
}

// to_string() degrades to the raw hash form for a dimension the library cannot name (the runtime->type wall).
TEST_F(Serialization, toStringFallsBackForUnknownDimension)
{
	// decode a hand-built record whose base dimension is a hash no built-in dimension owns
	units::any_unit value;
	{
		const units::any_unit meters = units::serialize(units::meters<double>(3.0));
		std::vector<std::byte> raw(meters.bytes().begin(), meters.bytes().end());
		// the record is [version][header][term-count][8-byte name-hash]...; the single term's hash begins at byte 3
		// (version=1, header=1, count=1 for a base dimension). Flip every hash byte so no known dimension matches.
		for (std::size_t i = 3; i < 3 + 8; ++i)
			raw[i] = static_cast<std::byte>(std::to_integer<std::uint8_t>(raw[i]) ^ 0xFF);
		const auto decoded = units::deserialize(std::span<const std::byte>(raw));
		ASSERT_TRUE(decoded.has_value());
		value = *decoded;
	}
	const std::string named = value.to_string();
	EXPECT_EQ(value.to_string_raw(), named);      // no known dimension matched -> raw fallback
	EXPECT_NE(std::string::npos, named.find('#')); // the raw hash form
}

// to_string_raw() is the honest, name-free rendering: always the hashed signature, identical for any dimension.
TEST_F(Serialization, toStringRawIsAlwaysHashKeyed)
{
	const std::string dimensionless = units::serialize(units::dimensionless<double>(0.25)).to_string_raw();
	EXPECT_NE(std::string::npos, dimensionless.find("0.25"));
	EXPECT_NE(std::string::npos, dimensionless.find("dimensionless"));

	// #395: dimensionless is a builtin dimension, so to_string() resolves it to the named form and diverges from the
	// raw hash-keyed rendering, which keys every dimension — including dimensionless — the same way regardless
	EXPECT_NE(dimensionless, units::serialize(units::dimensionless<double>(0.25)).to_string());
	EXPECT_EQ(std::string::npos, units::serialize(units::dimensionless<double>(0.25)).to_string().find('#'));

	const std::string length = units::serialize(units::meters<double>(100.0)).to_string_raw();
	EXPECT_NE(std::string::npos, length.find("100"));
	EXPECT_NE(std::string::npos, length.find('['));
	EXPECT_NE(std::string::npos, length.find('#')); // a hashed base-dimension term, even for a known dimension
}

// assign_to() collapses into an existing variable, returning whether the dimension matched and leaving it untouched if not.
TEST_F(Serialization, assignToMismatchTolerant)
{
	const units::any_unit erased = units::serialize(units::kilometers<double>(1.5));

	// a matching dimension is assigned (into the target's own unit), and reported assigned
	units::meters<double> length{0.0};
	EXPECT_TRUE(erased.assign_to(length));
	EXPECT_DOUBLE_EQ(1500.0, length.value());

	// a mismatched dimension leaves the target untouched and returns false — an expected outcome, not a throw
	units::seconds<double> duration{42.0};
	EXPECT_FALSE(erased.assign_to(duration));
	EXPECT_DOUBLE_EQ(42.0, duration.value()); // unchanged

	// the realiq idiom: fan one erased quantity across several typed fields, assigning only where it fits
	units::meters<double>  intoLength{0.0};
	units::seconds<double> intoTime{0.0};
	const bool tookLength = erased.assign_to(intoLength);
	const bool tookTime   = erased.assign_to(intoTime);
	EXPECT_TRUE(tookLength);
	EXPECT_FALSE(tookTime);

	// a value that cannot be represented exactly in an integral target is reported not-assigned (to's lossy_target)
	units::meters<int> integralLength{7};
	EXPECT_TRUE(units::serialize(units::meters<double>(5.0)).assign_to(integralLength)); // 5 fits
	EXPECT_EQ(5, integralLength.value());
	EXPECT_FALSE(units::serialize(units::meters<double>(2.5)).assign_to(integralLength)); // 2.5 does not
	EXPECT_EQ(5, integralLength.value());                                                 // unchanged
}

// operator<< writes the raw binary bytes; operator>> and deserialize(istream) read them back.
TEST_F(Serialization, streamOperatorsRoundTripBinary)
{
	std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);

	// << writes the exact serialized bytes (not text)
	const units::any_unit written = units::serialize(units::meters<double>(100.0));
	stream << written;
	EXPECT_EQ(written.size(), stream.str().size());

	// deserialize(istream) reads one record in a single expression
	const auto decoded = units::deserialize(stream);
	ASSERT_TRUE(decoded.has_value());
	EXPECT_DOUBLE_EQ(100.0, decoded->to<units::meters<double>>()->value());

	// operator>> reads the classic way; back-to-back records advance correctly
	std::stringstream seq(std::ios::in | std::ios::out | std::ios::binary);
	seq << units::serialize(units::meters<double>(1.0)) << units::serialize(units::seconds<double>(2.0));
	units::any_unit first;
	units::any_unit second;
	seq >> first >> second;
	ASSERT_TRUE(seq.good() || seq.eof());
	EXPECT_DOUBLE_EQ(1.0, first.to<units::meters<double>>()->value());
	EXPECT_DOUBLE_EQ(2.0, second.to<units::seconds<double>>()->value());

	// a malformed stream sets failbit and leaves the target unchanged
	std::stringstream bad(std::ios::in | std::ios::out | std::ios::binary);
	bad << "not a unit record";
	units::any_unit target = units::serialize(units::meters<double>(7.0));
	bad >> target;
	EXPECT_TRUE(bad.fail());
	EXPECT_DOUBLE_EQ(7.0, target.to<units::meters<double>>()->value()); // unchanged
}

// deserialize<Unit>(istream) reads and collapses in one checked step (the front-page idiom).
TEST_F(Serialization, typedDeserializeFromStream)
{
	std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
	stream << units::serialize(60.0_mph);

	// one call, one std::expected: reads the record AND collapses to the requested unit
	const auto speed = units::deserialize<units::kilometers_per_hour<double>>(stream);
	ASSERT_TRUE(speed.has_value());
	EXPECT_NEAR(96.56064, speed->value(), 1e-4);

	// a dimension mismatch is reported, not dereferenced blindly
	std::stringstream length(std::ios::in | std::ios::out | std::ios::binary);
	length << units::serialize(units::meters<double>(5.0));
	const auto wrong = units::deserialize<units::seconds<double>>(length);
	EXPECT_FALSE(wrong.has_value());
	EXPECT_EQ(units::deserialize_error::dimension_mismatch, wrong.error());
}

// visit() resolves EVERY library dimension by default — including ones added after the erased-visit feature.
// Guards against builtin_dimensions drifting behind the dimension set (a CI check also enforces this).
TEST_F(Serialization, visitResolvesRecentlyAddedDimensions)
{
	const auto expectResolves = [](const units::any_unit& v) {
		bool resolved = false;
		v.visit([&](auto) { resolved = true; });
		return resolved;
	};
	// dimensions that were once missing from builtin_dimensions must now resolve without being named
	EXPECT_TRUE(expectResolves(units::serialize(units::pascal_seconds<double>(5.0))));            // dynamic_viscosity
	EXPECT_TRUE(expectResolves(units::serialize(units::square_meters_per_second<double>(2.0))));  // kinematic_viscosity
	EXPECT_TRUE(expectResolves(units::serialize(units::gigabytes_per_second<double>(1.0))));      // data_transfer_rate
}

// std::hash makes any_unit a usable unordered-container key, consistent with operator==.
TEST_F(Serialization, hashableAsAKey)
{
	std::unordered_map<units::any_unit, std::string> byQuantity;
	byQuantity[units::serialize(units::meters<double>(1000.0))] = "one kilometer";

	// a different unit of the SAME quantity finds the same entry (equal => equal hash)
	const auto found = byQuantity.find(units::serialize(units::kilometers<double>(1.0)));
	ASSERT_NE(byQuantity.end(), found);
	EXPECT_EQ("one kilometer", found->second);

	// a different magnitude is a different key
	EXPECT_EQ(byQuantity.end(), byQuantity.find(units::serialize(units::meters<double>(2000.0))));

	// equal quantities hash equally (the contract std::hash must honor)
	const std::hash<units::any_unit> hasher;
	EXPECT_EQ(hasher(units::serialize(units::meters<double>(1000.0))), hasher(units::serialize(units::kilometers<double>(1.0))));
}

// A dimension with a FRACTIONAL exponent exercises the fracExp encode/decode path and the to_string denominator.
TEST_F(Serialization, fractionalExponentRoundTripsAndRenders)
{
	const units::any_unit q = units::serialize(3.0 * units::rt_m); // via the generated unit constant

	// to_string renders the fractional exponent as num/den
	EXPECT_NE(std::string::npos, q.to_string().find("1/2"));

	// the fracExp-flagged stream round-trips exactly through a real buffer
	std::vector<std::byte> raw(q.bytes().begin(), q.bytes().end());
	const auto             back = units::deserialize<units::root_meters<double>>(raw);
	ASSERT_TRUE(back.has_value());
	EXPECT_DOUBLE_EQ(3.0, back->value());

	// and the erased path preserves the fractional exponent in the decoded identity
	const auto erased = units::deserialize(raw);
	ASSERT_TRUE(erased.has_value());
	ASSERT_EQ(1u, erased->identity().terms.size());
	EXPECT_EQ(1, erased->identity().terms[0].num);
	EXPECT_EQ(2, erased->identity().terms[0].den);
}

// deserialize reports truncation at each point a record can be cut short (not just an empty buffer).
TEST_F(Serialization, truncationAtEveryStage)
{
	const units::any_unit          full = units::serialize(units::root_meters<double>(2.5)); // has terms + fracExp + value
	const std::vector<std::byte>   bytes(full.bytes().begin(), full.bytes().end());

	// cutting the stream at every length from 1 .. size-1 must fail cleanly (truncated/bad_version), never crash or
	// silently succeed — this walks the version/header/count/hash/exponent/den/value decode points.
	for (std::size_t n = 1; n < bytes.size(); ++n)
	{
		std::span<const std::byte> partial(bytes.data(), n);
		const auto                 r = units::deserialize(partial);
		EXPECT_FALSE(r.has_value()) << "a " << n << "-byte prefix should not decode";
		if (!r)
		{
			EXPECT_TRUE(r.error() == units::deserialize_error::truncated || r.error() == units::deserialize_error::bad_version) << "n=" << n;
		}
	}
}

// deserialize(std::istream&) on a non-seekable stream reports truncated rather than misreading.
TEST_F(Serialization, nonSeekableStreamReportsTruncated)
{
	// an ostringstream has no get area; reading from it via the istream overload cannot self-delimit a record
	std::ostringstream sink;
	std::istream       notReadable(sink.rdbuf()); // a stream whose tellg() is unusable for framing
	notReadable.setstate(std::ios::eofbit);        // force the unseekable/at-end condition
	const auto r = units::deserialize(notReadable);
	EXPECT_FALSE(r.has_value());
}

// the typed deserialize<Unit>(bytes) fast path propagates a decode error (not just a dimension mismatch).
TEST_F(Serialization, typedFastPathPropagatesDecodeError)
{
	std::vector<std::byte> garbage{std::byte{0xFF}, std::byte{0x00}}; // bad version byte
	const auto             r = units::deserialize<units::meters<double>>(garbage);
	EXPECT_FALSE(r.has_value());
	EXPECT_EQ(units::deserialize_error::bad_version, r.error());
}

//======================================================================================================================
//  std::format SUPPORT
//======================================================================================================================
//
// These exercise the units-aware std::formatter specialization: value-spec passthrough to the underlying arithmetic
// type's formatter, the unit-opts mini-language after '%', the label forms, the show flags, separators, the byte-
// identical cross-check against to_string/operator<<, the full public API surface, and every runtime throw path.

namespace
{
	// Render a unit through operator<< into a string, for the byte-identical cross-check against std::format("{}").
	template<class T>
	std::string ostreamString(const T& value)
	{
		std::ostringstream os;
		os << value;
		return os.str();
	}
} // namespace

//-----------------------------
//  DEFAULT: {} == to_string == operator<<
//-----------------------------

// The default spec "{}" must be byte-identical to units::to_string AND to an operator<< ostringstream for a named
// unit — this three-way equality is the anchor invariant of the whole feature.
TEST(Format, defaultMatchesToStringAndOstreamNamed)
{
	const auto m = 3.5_m;
	EXPECT_EQ(std::format("{}", m), "3.5 m");
	EXPECT_EQ(std::format("{}", m), units::to_string(m));
	EXPECT_EQ(std::format("{}", m), ostreamString(m));

	const auto ft = 6.0_ft;
	EXPECT_EQ(std::format("{}", ft), "6 ft");
	EXPECT_EQ(std::format("{}", ft), units::to_string(ft));
	EXPECT_EQ(std::format("{}", ft), ostreamString(ft));
}

// The same three-way equality for a percent (named dimensionless) unit: it prints its abbreviation "pct", NOT "%".
TEST(Format, defaultMatchesToStringAndOstreamPercent)
{
	const auto p = units::percent<double>(50);
	EXPECT_EQ(std::format("{}", p), "50 pct");
	EXPECT_EQ(std::format("{}", p), units::to_string(p));
	EXPECT_EQ(std::format("{}", p), ostreamString(p));
}

// A genuinely unnamed compound unit (ampere*meter has no named form) prints in dimension-list form; the three-way
// equality still holds, and the value is the base-unit value.
TEST(Format, defaultMatchesToStringAndOstreamUnnamedCompound)
{
	const auto am = units::amperes<double>(3) * units::meters<double>(2);
	EXPECT_EQ(std::format("{}", am), "6 A m");
	EXPECT_EQ(std::format("{}", am), units::to_string(am));
	EXPECT_EQ(std::format("{}", am), ostreamString(am));
}

// A named acceleration compound (meters/second^2 == "mps2") reduces to its NAMED abbreviation, not a dimension list.
TEST(Format, defaultMatchesToStringAndOstreamNamedCompound)
{
	const auto accel = units::meters<double>(6) / (units::seconds<double>(2) * units::seconds<double>(1));
	EXPECT_EQ(std::format("{}", accel), "3 mps2");
	EXPECT_EQ(std::format("{}", accel), units::to_string(accel));
	EXPECT_EQ(std::format("{}", accel), ostreamString(accel));
}

// An integer-underlying named unit: the value formatter is the underlying int type, so the default renders the int
// as-is; still byte-identical to to_string and operator<<.
TEST(Format, defaultMatchesToStringAndOstreamIntUnderlying)
{
	const units::meters<int> mi(42);
	EXPECT_EQ(std::format("{}", mi), "42 m");
	EXPECT_EQ(std::format("{}", mi), units::to_string(mi));
	EXPECT_EQ(std::format("{}", mi), ostreamString(mi));
}

// A unit needing floating-point promotion (float underlying) matches across all three sinks.
TEST(Format, defaultMatchesToStringAndOstreamFloatUnderlying)
{
	const units::meters<float> mf(3.5f);
	EXPECT_EQ(std::format("{}", mf), "3.5 m");
	EXPECT_EQ(std::format("{}", mf), units::to_string(mf));
	EXPECT_EQ(std::format("{}", mf), ostreamString(mf));
}

//-----------------------------
//  VALUE-SPEC PASSTHROUGH (float/double delegate)
//-----------------------------

// Precision variants reach the NUMBER and the unit label still appends.
TEST(Format, precisionReachesValueLabelAppends)
{
	EXPECT_EQ(std::format("{:.0f}", 3.5_m), "4 m"); // banker's-agnostic: 3.5 -> "4" at .0f (round-half-to-even)
	EXPECT_EQ(std::format("{:.2f}", 3.5_m), "3.50 m");
	EXPECT_EQ(std::format("{:.5f}", 3.5_m), "3.50000 m");
}

// Width pads the numeric field; the unit label is appended after the padded value.
TEST(Format, widthPadsValueThenLabel)
{
	EXPECT_EQ(std::format("{:8.2f}", 3.5_m), "    3.50 m");
	EXPECT_EQ(std::format("{:>10.2f}", 3.5_m), "      3.50 m");
}

// Fill + alignment (left/right/center) apply to the numeric field only.
TEST(Format, fillAndAlignApplyToValue)
{
	EXPECT_EQ(std::format("{:*>10.1f}", 3.5_m), "*******3.5 m");
	EXPECT_EQ(std::format("{:_^12}", 3.5_m), "____3.5_____ m");
	EXPECT_EQ(std::format("{:<8.1f}", 3.5_m), "3.5      m");
}

// Sign controls '+' and space-for-positive on the value.
TEST(Format, signControls)
{
	EXPECT_EQ(std::format("{:+.1f}", 3.5_m), "+3.5 m");
	EXPECT_EQ(std::format("{: }", 3.5_m), " 3.5 m");
	EXPECT_EQ(std::format("{:+.1f}", units::meters<double>(-3.5)), "-3.5 m");
}

// Zero-fill pads the numeric field with leading zeros, still appending the label.
TEST(Format, zeroFillValue)
{
	EXPECT_EQ(std::format("{:08.2f}", 3.5_m), "00003.50 m");
}

// Integer presentation types (x/#06x/b/d) work when the underlying type is an integer named unit — the value
// formatter delegate is the underlying int in that case, so the standard int grammar passes through.
TEST(Format, integerPresentationTypesOnIntUnit)
{
	const units::meters<int> mi(255);
	EXPECT_EQ(std::format("{:x}", mi), "ff m");
	EXPECT_EQ(std::format("{:#06x}", mi), "0x00ff m");
	EXPECT_EQ(std::format("{:b}", mi), "11111111 m");
	EXPECT_EQ(std::format("{:d}", mi), "255 m");
	// combined with a show flag
	EXPECT_EQ(std::format("{:x%v}", mi), "ff");
	EXPECT_EQ(std::format("{:x%u}", mi), "m");
}

//-----------------------------
//  LABEL FORMS: %a %n %b
//-----------------------------

// %a is explicitly the abbreviation form and equals the default.
TEST(Format, abbreviationFlagEqualsDefault)
{
	EXPECT_EQ(std::format("{:%a}", 3.5_m), std::format("{}", 3.5_m));
	EXPECT_EQ(std::format("{:%a}", 3.5_m), "3.5 m");
	EXPECT_EQ(std::format("{:.2f%a}", 3.5_m), std::format("{:.2f}", 3.5_m));
}

// %n emits the full unit name for a named unit.
TEST(Format, nameFlagFullName)
{
	EXPECT_EQ(std::format("{:%n}", 3.5_m), "3.5 meters");
	EXPECT_EQ(std::format("{:.3f%n}", 6.0_ft), "6.000 feet");
	EXPECT_EQ(std::format("{:%n}", units::kilometers<double>(2)), "2 kilometers");
	EXPECT_EQ(std::format("{:%n}", units::degrees<double>(90)), "90 degrees");
}

// %b converts BOTH the value and the label to SI base units.
TEST(Format, baseFlagConvertsToBaseSI)
{
	// A non-base named unit is converted: 6 ft = 1.8288 m.
	EXPECT_EQ(std::format("{:%b}", 6.0_ft), "1.8288 m");
	EXPECT_EQ(std::format("{:%b}", units::kilometers<double>(2)), "2000 m");
	// A named compound already expressed in base units is unchanged in value; its label decomposes.
	EXPECT_EQ(std::format("{:%b}", 9.81_mps), "9.81 m s^-1");
	// An already-base unit is unchanged.
	EXPECT_EQ(std::format("{:%b}", 3.5_m), "3.5 m");
	// The value-spec still applies to the (converted) number.
	EXPECT_EQ(std::format("{:.4f%b}", 10.0_fps), "3.0480 m s^-1");
}

// %a and %n never convert the value — they render the unit's OWN symbol/name.
TEST(Format, abbreviationAndNameNeverConvert)
{
	EXPECT_EQ(std::format("{:%a}", 6.0_ft), "6 ft");
	EXPECT_EQ(std::format("{:%n}", 6.0_ft), "6 feet");
	EXPECT_EQ(std::format("{:%a}", 10.0_fps), "10 fps");
	EXPECT_EQ(std::format("{:%a}", units::kilometers<double>(2)), "2 km");
}

// %b on an already-unnamed unit yields the same base-symbol form as the default label (the value is
// already in base units, so nothing changes).
TEST(Format, baseFlagOnUnnamedUnit)
{
	const auto am = units::amperes<double>(3) * units::meters<double>(2);
	EXPECT_EQ(std::format("{:%b}", am), "6 A m");
	EXPECT_EQ(std::format("{:%b}", am), std::format("{}", am));
}

// %n on an unnamed compound falls back to the base-symbol form (there is no full name to print).
TEST(Format, nameFlagOnUnnamedFallsBackToDimension)
{
	const auto am = units::amperes<double>(3) * units::meters<double>(2);
	EXPECT_EQ(std::format("{:%n}", am), "6 A m");
	EXPECT_EQ(std::format("{:%n}", am), std::format("{:%a}", am));
}

//-----------------------------
//  SHOW FLAGS: %v (value only) %u (unit only)
//-----------------------------

// %v suppresses the unit label and its separator — value only, no trailing space.
TEST(Format, showValueOnly)
{
	EXPECT_EQ(std::format("{:%v}", 3.5_m), "3.5");
	EXPECT_EQ(std::format("{:.2f%v}", 3.5_m), "3.50");
	EXPECT_EQ(std::format("{:*>10.1f%v}", 3.5_m), "*******3.5");
}

// %u suppresses the value AND the separator — unit label only, no leading space.
TEST(Format, showUnitOnly)
{
	EXPECT_EQ(std::format("{:%u}", 3.5_m), "m");
	EXPECT_EQ(std::format("{:%u}", 6.0_ft), "ft");
	// a value-spec is harmlessly parsed but the value is not emitted under %u.
	EXPECT_EQ(std::format("{:.2f%u}", 3.5_m), "m");
	// the full name under unit-only.
	EXPECT_EQ(std::format("{:%nu}", 3.5_m), "meters");
	// the base-SI form under unit-only (no leading space).
	EXPECT_EQ(std::format("{:%bu}", 9.81_mps), "m s^-1");
}

// %v / %u on an integer-underlying named unit.
TEST(Format, showFlagsIntUnderlying)
{
	const units::meters<int> mi(42);
	EXPECT_EQ(std::format("{:%v}", mi), "42");
	EXPECT_EQ(std::format("{:%u}", mi), "m");
}

// %u never emits a separator regardless of a supplied separator literal.
TEST(Format, unitOnlyIgnoresSeparator)
{
	EXPECT_EQ(std::format("{:%u'_'}", 3.5_m), "m");
	EXPECT_EQ(std::format("{:%u''}", 3.5_m), "m");
	EXPECT_EQ(std::format("{:%'_'u}", 3.5_m), "m");
}

//-----------------------------
//  SEPARATORS
//-----------------------------

// The default separator (no quotes) is a single space.
TEST(Format, defaultSeparatorIsSingleSpace)
{
	EXPECT_EQ(std::format("{:%a}", 3.5_m), "3.5 m");
	EXPECT_EQ(std::format("{}", 3.5_m), "3.5 m");
}

// An empty separator '' glues value and label together.
TEST(Format, emptySeparator)
{
	EXPECT_EQ(std::format("{:%a''}", 3.5_m), "3.5m");
	EXPECT_EQ(std::format("{:.2f%a''}", 3.5_m), "3.50m");
}

// A single-character separator literal.
TEST(Format, underscoreSeparator)
{
	EXPECT_EQ(std::format("{:%a'_'}", 3.5_m), "3.5_m");
}

// Escape sequences inside the separator: tab, newline, backslash, quote.
TEST(Format, escapeSeparators)
{
	EXPECT_EQ(std::format("{:%a'\t'}", 3.5_m), "3.5\tm");
	EXPECT_EQ(std::format("{:%a'\n'}", 3.5_m), "3.5\nm");
	EXPECT_EQ(std::format("{:%a'\\\\'}", 3.5_m), "3.5\\m");
	EXPECT_EQ(std::format("{:%a'\\''}", 3.5_m), "3.5'm");
}

// A multi-character separator literal.
TEST(Format, multiCharacterSeparator)
{
	EXPECT_EQ(std::format("{:%a' - '}", 3.5_m), "3.5 - m");
}

// A separator combined with the name form.
TEST(Format, separatorWithNameForm)
{
	EXPECT_EQ(std::format("{:%n'_'}", 3.5_m), "3.5_meters");
}

//-----------------------------
//  FLAG-ORDER INDEPENDENCE
//-----------------------------

// Form-then-separator and separator-then-form parse identically.
TEST(Format, formSeparatorOrderIndependent)
{
	EXPECT_EQ(std::format("{:%n'_'}", 3.5_m), std::format("{:%'_'n}", 3.5_m));
	EXPECT_EQ(std::format("{:%'_'n}", 3.5_m), "3.5_meters");
}

// Show-then-form and form-then-show parse identically (each category may appear once).
TEST(Format, showFormOrderIndependent)
{
	EXPECT_EQ(std::format("{:%va}", 3.5_m), std::format("{:%av}", 3.5_m));
	EXPECT_EQ(std::format("{:%va}", 3.5_m), "3.5");
	EXPECT_EQ(std::format("{:%ua}", 3.5_m), std::format("{:%au}", 3.5_m));
	EXPECT_EQ(std::format("{:%ua}", 3.5_m), "m");
}

//-----------------------------
//  API SURFACE
//-----------------------------

// std::format_to into a back_inserter produces the same text as std::format.
TEST(Format, formatToBackInserter)
{
	std::string out;
	std::format_to(std::back_inserter(out), "{:.2f%n}", 3.5_m);
	EXPECT_EQ(out, "3.50 meters");
	EXPECT_EQ(out, std::format("{:.2f%n}", 3.5_m));
}

// std::vformat with make_format_args honors a runtime spec.
TEST(Format, vformatRuntimeSpec)
{
	const auto  m    = 3.5_m;
	std::string spec = "{:%u}";
	EXPECT_EQ(std::vformat(spec, std::make_format_args(m)), "m");
	spec = "{:.2f%n}";
	EXPECT_EQ(std::vformat(spec, std::make_format_args(m)), "3.50 meters");
}

//-----------------------------
//  MANY UNIT TYPES / UNDERLYING TYPES
//-----------------------------

// A spread of unit types and underlying arithmetic types all format sensibly.
TEST(Format, manyUnitTypes)
{
	EXPECT_EQ(std::format("{}", units::meters<double>(1.5)), "1.5 m");
	EXPECT_EQ(std::format("{}", units::feet<double>(2.0)), "2 ft");
	EXPECT_EQ(std::format("{}", units::kilometers<double>(3.0)), "3 km");
	EXPECT_EQ(std::format("{}", units::degrees<double>(45.0)), "45 deg");
	EXPECT_EQ(std::format("{}", units::seconds<double>(10.0)), "10 s");
	EXPECT_EQ(std::format("{}", 9.81_mps), "9.81 mps");
	EXPECT_EQ(std::format("{}", units::percent<double>(25.0)), "25 pct");

	// vary the underlying type on the same dimension.
	EXPECT_EQ(std::format("{}", units::meters<int>(7)), "7 m");
	EXPECT_EQ(std::format("{}", units::meters<float>(7.25f)), "7.25 m");
	EXPECT_EQ(std::format("{}", units::meters<long>(7L)), "7 m");
}

//======================================================================================================================
//  std::format ERROR PATHS
//======================================================================================================================
//
// Every throw the parser can raise, one message per case, driven through std::vformat so the (runtime) format string
// reaches parse() and the std::format_error escapes to the caller. A LITERAL bad spec is a compile error instead —
// those live under test/errorMessages/cases/format_*.cpp.

// An unknown unit-format flag throws.
TEST(Format, throwsOnUnknownFlag)
{
	const auto m = 3.5_m;
	EXPECT_THROW((void)std::vformat("{:%z}", std::make_format_args(m)), std::format_error);
}

// A duplicated label-form flag throws.
TEST(Format, throwsOnDuplicateLabelForm)
{
	const auto m = 3.5_m;
	EXPECT_THROW((void)std::vformat("{:%aa}", std::make_format_args(m)), std::format_error);
	EXPECT_THROW((void)std::vformat("{:%an}", std::make_format_args(m)), std::format_error);
	EXPECT_THROW((void)std::vformat("{:%ba}", std::make_format_args(m)), std::format_error);
}

// A duplicated show flag throws.
TEST(Format, throwsOnDuplicateShowFlag)
{
	const auto m = 3.5_m;
	EXPECT_THROW((void)std::vformat("{:%vv}", std::make_format_args(m)), std::format_error);
	EXPECT_THROW((void)std::vformat("{:%vu}", std::make_format_args(m)), std::format_error);
	EXPECT_THROW((void)std::vformat("{:%uv}", std::make_format_args(m)), std::format_error);
}

// An unterminated separator literal throws.
TEST(Format, throwsOnUnterminatedSeparator)
{
	const auto m = 3.5_m;
	EXPECT_THROW((void)std::vformat("{:%a'foo}", std::make_format_args(m)), std::format_error);
}

// A dangling escape at the end of a separator throws.
TEST(Format, throwsOnDanglingEscape)
{
	const auto m = 3.5_m;
	EXPECT_THROW((void)std::vformat("{:%a'\\}", std::make_format_args(m)), std::format_error);
}

// A value-spec the underlying value formatter rejects throws.
TEST(Format, throwsOnInvalidValueSpec)
{
	const auto m = 3.5_m;
	EXPECT_THROW((void)std::vformat("{:Zf}", std::make_format_args(m)), std::format_error);
}

// A float presentation type on an integer-underlying unit's value formatter throws (the delegate is the int
// formatter, which rejects '.2f'); an int presentation type on a floating-point delegate likewise throws.
TEST(Format, throwsOnMismatchedValueTypeSpec)
{
	const units::meters<int> mi(3);
	EXPECT_THROW((void)std::vformat("{:.2f}", std::make_format_args(mi)), std::format_error);

	const units::meters<double> md(3.5);
	EXPECT_THROW((void)std::vformat("{:x}", std::make_format_args(md)), std::format_error);
}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
