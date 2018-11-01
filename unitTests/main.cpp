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
	EXPECT_TRUE(traits::is_conversion_factor_v<meters>);
	EXPECT_TRUE(traits::is_conversion_factor_v<feet>);
	EXPECT_TRUE(traits::is_conversion_factor_v<degrees_squared>);
	EXPECT_TRUE(traits::is_conversion_factor_v<meter_t<double>>);
}

TEST_F(TypeTraits, is_unit)
{
	EXPECT_FALSE(traits::is_unit_v<std::ratio<1>>);
	EXPECT_FALSE(traits::is_unit_v<double>);
	EXPECT_FALSE(traits::is_unit_v<meters>);
	EXPECT_FALSE(traits::is_unit_v<feet>);
	EXPECT_FALSE(traits::is_unit_v<degrees_squared>);
	EXPECT_TRUE(traits::is_unit_v<meter_t<double>>);
}

TEST_F(TypeTraits, unit_base)
{
	EXPECT_TRUE((std::is_same_v<traits::unit_base_t<unit<dimensionless_unit, int>>, unit<dimensionless_unit, int>>));
	EXPECT_TRUE((std::is_same_v<traits::unit_base_t<dimensionless<int>>, unit<dimensionless_unit, int>>));
	EXPECT_TRUE((std::is_same_v<traits::unit_base_t<const volatile unit<dimensionless_unit, int>>,
		unit<dimensionless_unit, int>>));
	EXPECT_TRUE(
		(std::is_same_v<traits::unit_base_t<const volatile dimensionless<int>>, unit<dimensionless_unit, int>>));
	EXPECT_TRUE((std::is_same_v<traits::unit_base_t<meter_t<double>>, unit<meter, double>>));
	EXPECT_TRUE((std::is_same_v<traits::unit_base_t<const volatile meter_t<double>>, unit<meter, double>>));
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
	EXPECT_FALSE((std::is_same_v<void, traits::conversion_factor_traits<meters>::conversion_ratio>));
}

TEST_F(TypeTraits, unit_traits)
{
	EXPECT_TRUE((std::is_same_v<void, traits::unit_traits<double>::underlying_type>));
	EXPECT_TRUE((std::is_same_v<double, traits::unit_traits<meter_t<double>>::underlying_type>));
	EXPECT_TRUE((std::is_same_v<void, traits::unit_traits<double>::value_type>));
	EXPECT_TRUE((std::is_same_v<double, traits::unit_traits<meter_t<double>>::value_type>));
}

TEST_F(TypeTraits, is_same_dimension)
{
	EXPECT_TRUE((traits::is_same_dimension_v<meters, meters>));
	EXPECT_TRUE((traits::is_same_dimension_v<meters, astronomical_units>));
	EXPECT_TRUE((traits::is_same_dimension_v<meters, parsecs>));

	EXPECT_TRUE((traits::is_same_dimension_v<meters, meters>));
	EXPECT_TRUE((traits::is_same_dimension_v<astronomical_units, meters>));
	EXPECT_TRUE((traits::is_same_dimension_v<parsecs, meters>));
	EXPECT_TRUE((traits::is_same_dimension_v<years, weeks>));

	EXPECT_FALSE((traits::is_same_dimension_v<meters, seconds>));
	EXPECT_FALSE((traits::is_same_dimension_v<seconds, meters>));
	EXPECT_FALSE((traits::is_same_dimension_v<years, meters>));
}

TEST_F(TypeTraits, inverse)
{
	double test;

	using htz         = traits::strong_t<inverse<seconds>>;
	bool shouldBeTrue = std::is_same_v<htz, hertz>;
	EXPECT_TRUE(shouldBeTrue);

	test = unit<inverse<fahrenheit>>(unit<inverse<celsius>>(1.0))();
	EXPECT_NEAR(5.0 / 9.0, test, 5.0e-5);

	test = unit<inverse<fahrenheit>>(unit<inverse<kelvin>>(6.0))();
	EXPECT_NEAR(10.0 / 3.0, test, 5.0e-5);
}

TEST_F(TypeTraits, strong)
{
	EXPECT_TRUE(
		(std::is_same_v<dimensionless_unit, traits::strong_t<detail::conversion_factor_base_t<dimensionless_unit>>>));
	EXPECT_TRUE((std::is_same_v<meter, traits::strong_t<conversion_factor<std::ratio<1>, dimension::length>>>));
	EXPECT_TRUE((std::is_same_v<kilometer, traits::strong_t<kilo<meter>>>));
	EXPECT_TRUE((std::is_same_v<square_meter, traits::strong_t<squared<meter>>>));
}

TEST_F(TypeTraits, dimension_of)
{
	using dim = traits::dimension_of_t<years>;

	EXPECT_TRUE((std::is_same_v<dim, dimension::time>));
	EXPECT_FALSE((std::is_same_v<dim, dimension::length>));
	EXPECT_FALSE((std::is_same_v<dim, units::time::days>));

	using dim2 = typename traits::conversion_factor_traits<
		typename traits::unit_traits<decltype(meters_per_second_t<double>(5))>::conversion_factor>::dimension_type;

	EXPECT_TRUE((std::is_same_v<dim2, dimension::velocity>));
	EXPECT_FALSE((std::is_same_v<dim2, dimension::time>));
	EXPECT_FALSE((std::is_same_v<dim2, units::velocity::mph>));
}

TEST_F(TypeTraits, has_linear_scale)
{
	EXPECT_TRUE((traits::has_linear_scale_v<dimensionless<double>>));
	EXPECT_TRUE((traits::has_linear_scale_v<meter_t<double>>));
	EXPECT_TRUE((traits::has_linear_scale_v<foot_t<double>>));
	EXPECT_TRUE((traits::has_linear_scale_v<watt_t<double>, dimensionless<double>>));
	EXPECT_TRUE((traits::has_linear_scale_v<dimensionless<double>, meter_t<double>>));
	EXPECT_TRUE((traits::has_linear_scale_v<meters_per_second_t<double>>));
	EXPECT_FALSE((traits::has_linear_scale_v<dB_t<double>>));
	EXPECT_FALSE((traits::has_linear_scale_v<dB_t<double>, meters_per_second_t<double>>));
}

TEST_F(TypeTraits, has_decibel_scale)
{
	EXPECT_FALSE((traits::has_decibel_scale_v<dimensionless<double>>));
	EXPECT_FALSE((traits::has_decibel_scale_v<meter_t<double>>));
	EXPECT_FALSE((traits::has_decibel_scale_v<foot_t<double>>));
	EXPECT_TRUE((traits::has_decibel_scale_v<dB_t<double>>));
	EXPECT_TRUE((traits::has_decibel_scale_v<dBW_t<double>>));

	EXPECT_TRUE((traits::has_decibel_scale_v<dBW_t<double>, dB_t<double>>));
	EXPECT_TRUE((traits::has_decibel_scale_v<dBW_t<double>, dBm_t<double>>));
	EXPECT_TRUE((traits::has_decibel_scale_v<dB_t<double>, dB_t<double>>));
	EXPECT_TRUE((traits::has_decibel_scale_v<dB_t<double>, dB_t<double>, dB_t<double>>));
	EXPECT_FALSE((traits::has_decibel_scale_v<dB_t<double>, dB_t<double>, meter_t<double>>));
	EXPECT_FALSE((traits::has_decibel_scale_v<meter_t<double>, dB_t<double>>));
}

TEST_F(TypeTraits, is_dimensionless_unit)
{
	EXPECT_TRUE((traits::is_dimensionless_unit_v<dimensionless<double>>));
	EXPECT_TRUE((traits::is_dimensionless_unit_v<const dimensionless<double>>));
	EXPECT_TRUE((traits::is_dimensionless_unit_v<const dimensionless<double>&>));
	EXPECT_TRUE((traits::is_dimensionless_unit_v<dimensionless<double>>));
	EXPECT_TRUE((traits::is_dimensionless_unit_v<dB_t<double>>));
	EXPECT_TRUE((traits::is_dimensionless_unit_v<dB_t<double>, dimensionless<double>>));
	EXPECT_TRUE((traits::is_dimensionless_unit_v<ppm_t<double>>));
	EXPECT_FALSE((traits::is_dimensionless_unit_v<meter_t<double>>));
	EXPECT_FALSE((traits::is_dimensionless_unit_v<dBW_t<double>>));
	EXPECT_FALSE((traits::is_dimensionless_unit_v<dBW_t<double>, dimensionless<double>>));
}

TEST_F(TypeTraits, is_length_unit)
{
	EXPECT_TRUE((traits::is_length_unit_v<meter>));
	EXPECT_TRUE((traits::is_length_unit_v<cubit>));
	EXPECT_FALSE((traits::is_length_unit_v<year>));
	EXPECT_FALSE((traits::is_length_unit_v<double>));

	EXPECT_TRUE((traits::is_length_unit_v<meter_t<double>>));
	EXPECT_TRUE((traits::is_length_unit_v<const meter_t<double>>));
	EXPECT_TRUE((traits::is_length_unit_v<const meter_t<double>&>));
	EXPECT_TRUE((traits::is_length_unit_v<cubit_t<double>>));
	EXPECT_FALSE((traits::is_length_unit_v<year_t<double>>));
	EXPECT_TRUE((traits::is_length_unit_v<meter_t<double>, cubit_t<double>>));
	EXPECT_FALSE((traits::is_length_unit_v<meter_t<double>, year_t<double>>));
}

TEST_F(TypeTraits, is_mass_unit)
{
	EXPECT_TRUE((traits::is_mass_unit_v<kilogram>));
	EXPECT_TRUE((traits::is_mass_unit_v<stone>));
	EXPECT_FALSE((traits::is_mass_unit_v<meter>));
	EXPECT_FALSE((traits::is_mass_unit_v<double>));

	EXPECT_TRUE((traits::is_mass_unit_v<kilogram_t<double>>));
	EXPECT_TRUE((traits::is_mass_unit_v<const kilogram_t<double>>));
	EXPECT_TRUE((traits::is_mass_unit_v<const kilogram_t<double>&>));
	EXPECT_TRUE((traits::is_mass_unit_v<stone_t<double>>));
	EXPECT_FALSE((traits::is_mass_unit_v<meter_t<double>>));
	EXPECT_TRUE((traits::is_mass_unit_v<kilogram_t<double>, stone_t<double>>));
	EXPECT_FALSE((traits::is_mass_unit_v<kilogram_t<double>, meter_t<double>>));
}

TEST_F(TypeTraits, is_time_unit)
{
	EXPECT_TRUE((traits::is_time_unit_v<second>));
	EXPECT_TRUE((traits::is_time_unit_v<year>));
	EXPECT_FALSE((traits::is_time_unit_v<meter>));
	EXPECT_FALSE((traits::is_time_unit_v<double>));

	EXPECT_TRUE((traits::is_time_unit_v<second_t<double>>));
	EXPECT_TRUE((traits::is_time_unit_v<const second_t<double>>));
	EXPECT_TRUE((traits::is_time_unit_v<const second_t<double>&>));
	EXPECT_TRUE((traits::is_time_unit_v<year_t<double>>));
	EXPECT_FALSE((traits::is_time_unit_v<meter_t<double>>));
	EXPECT_TRUE((traits::is_time_unit_v<second_t<double>, year_t<double>>));
	EXPECT_FALSE((traits::is_time_unit_v<second_t<double>, meter_t<double>>));
}

TEST_F(TypeTraits, is_angle_unit)
{
	EXPECT_TRUE((traits::is_angle_unit_v<angle::radian>));
	EXPECT_TRUE((traits::is_angle_unit_v<angle::degree>));
	EXPECT_FALSE((traits::is_angle_unit_v<watt>));
	EXPECT_FALSE((traits::is_angle_unit_v<double>));

	EXPECT_TRUE((traits::is_angle_unit_v<angle::radian_t<double>>));
	EXPECT_TRUE((traits::is_angle_unit_v<const angle::radian_t<double>>));
	EXPECT_TRUE((traits::is_angle_unit_v<const angle::radian_t<double>&>));
	EXPECT_TRUE((traits::is_angle_unit_v<angle::degree_t<double>>));
	EXPECT_FALSE((traits::is_angle_unit_v<watt_t<double>>));
	EXPECT_TRUE((traits::is_angle_unit_v<angle::radian_t<double>, angle::degree_t<double>>));
	EXPECT_FALSE((traits::is_angle_unit_v<angle::radian_t<double>, watt_t<double>>));
}

TEST_F(TypeTraits, is_current_unit)
{
	EXPECT_TRUE((traits::is_current_unit_v<current::ampere>));
	EXPECT_FALSE((traits::is_current_unit_v<volt>));
	EXPECT_FALSE((traits::is_current_unit_v<double>));

	EXPECT_TRUE((traits::is_current_unit_v<current::ampere_t<double>>));
	EXPECT_TRUE((traits::is_current_unit_v<const current::ampere_t<double>>));
	EXPECT_TRUE((traits::is_current_unit_v<const current::ampere_t<double>&>));
	EXPECT_FALSE((traits::is_current_unit_v<volt_t<double>>));
	EXPECT_TRUE((traits::is_current_unit_v<current::ampere_t<double>, current::milliampere_t<double>>));
	EXPECT_FALSE((traits::is_current_unit_v<current::ampere_t<double>, volt_t<double>>));
}

TEST_F(TypeTraits, is_temperature_unit)
{
	EXPECT_TRUE((traits::is_temperature_unit_v<fahrenheit>));
	EXPECT_TRUE((traits::is_temperature_unit_v<kelvin>));
	EXPECT_FALSE((traits::is_temperature_unit_v<cubit>));
	EXPECT_FALSE((traits::is_temperature_unit_v<double>));

	EXPECT_TRUE((traits::is_temperature_unit_v<fahrenheit_t<double>>));
	EXPECT_TRUE((traits::is_temperature_unit_v<const fahrenheit_t<double>>));
	EXPECT_TRUE((traits::is_temperature_unit_v<const fahrenheit_t<double>&>));
	EXPECT_TRUE((traits::is_temperature_unit_v<kelvin_t<double>>));
	EXPECT_FALSE((traits::is_temperature_unit_v<cubit_t<double>>));
	EXPECT_TRUE((traits::is_temperature_unit_v<fahrenheit_t<double>, kelvin_t<double>>));
	EXPECT_FALSE((traits::is_temperature_unit_v<cubit_t<double>, fahrenheit_t<double>>));
}

TEST_F(TypeTraits, is_substance_unit)
{
	EXPECT_TRUE((traits::is_substance_unit_v<substance::mol>));
	EXPECT_FALSE((traits::is_substance_unit_v<year>));
	EXPECT_FALSE((traits::is_substance_unit_v<double>));

	EXPECT_TRUE((traits::is_substance_unit_v<substance::mole_t<double>>));
	EXPECT_TRUE((traits::is_substance_unit_v<const substance::mole_t<double>>));
	EXPECT_TRUE((traits::is_substance_unit_v<const substance::mole_t<double>&>));
	EXPECT_FALSE((traits::is_substance_unit_v<year_t<double>>));
	EXPECT_TRUE((traits::is_substance_unit_v<substance::mole_t<double>, substance::mole_t<double>>));
	EXPECT_FALSE((traits::is_substance_unit_v<year_t<double>, substance::mole_t<double>>));
}

TEST_F(TypeTraits, is_luminous_intensity_unit)
{
	EXPECT_TRUE((traits::is_luminous_intensity_unit_v<candela>));
	EXPECT_FALSE((traits::is_luminous_intensity_unit_v<units::radiation::rad>));
	EXPECT_FALSE((traits::is_luminous_intensity_unit_v<double>));

	EXPECT_TRUE((traits::is_luminous_intensity_unit_v<candela_t<double>>));
	EXPECT_TRUE((traits::is_luminous_intensity_unit_v<const candela_t<double>>));
	EXPECT_TRUE((traits::is_luminous_intensity_unit_v<const candela_t<double>&>));
	EXPECT_FALSE((traits::is_luminous_intensity_unit_v<rad_t<double>>));
	EXPECT_TRUE((traits::is_luminous_intensity_unit_v<candela_t<double>, candela_t<double>>));
	EXPECT_FALSE((traits::is_luminous_intensity_unit_v<rad_t<double>, candela_t<double>>));
}

TEST_F(TypeTraits, is_solid_angle_unit)
{
	EXPECT_TRUE((traits::is_solid_angle_unit_v<steradian>));
	EXPECT_TRUE((traits::is_solid_angle_unit_v<degree_squared>));
	EXPECT_FALSE((traits::is_solid_angle_unit_v<angle::degree>));
	EXPECT_FALSE((traits::is_solid_angle_unit_v<double>));

	EXPECT_TRUE((traits::is_solid_angle_unit_v<steradian_t<double>>));
	EXPECT_TRUE((traits::is_solid_angle_unit_v<const steradian_t<double>>));
	EXPECT_TRUE((traits::is_solid_angle_unit_v<const degree_squared_t<double>&>));
	EXPECT_FALSE((traits::is_solid_angle_unit_v<angle::degree_t<double>>));
	EXPECT_TRUE((traits::is_solid_angle_unit_v<degree_squared_t<double>, steradian_t<double>>));
	EXPECT_FALSE((traits::is_solid_angle_unit_v<angle::degree_t<double>, steradian_t<double>>));
}

TEST_F(TypeTraits, is_frequency_unit)
{
	EXPECT_TRUE((traits::is_frequency_unit_v<hertz>));
	EXPECT_FALSE((traits::is_frequency_unit_v<second>));
	EXPECT_FALSE((traits::is_frequency_unit_v<double>));

	EXPECT_TRUE((traits::is_frequency_unit_v<hertz_t<double>>));
	EXPECT_TRUE((traits::is_frequency_unit_v<const hertz_t<double>>));
	EXPECT_TRUE((traits::is_frequency_unit_v<const hertz_t<double>&>));
	EXPECT_FALSE((traits::is_frequency_unit_v<second_t<double>>));
	EXPECT_TRUE((traits::is_frequency_unit_v<const hertz_t<double>&, gigahertz_t<double>>));
	EXPECT_FALSE((traits::is_frequency_unit_v<second_t<double>, hertz_t<double>>));
}

TEST_F(TypeTraits, is_velocity_unit)
{
	EXPECT_TRUE((traits::is_velocity_unit_v<meters_per_second>));
	EXPECT_TRUE((traits::is_velocity_unit_v<miles_per_hour>));
	EXPECT_FALSE((traits::is_velocity_unit_v<meters_per_second_squared>));
	EXPECT_FALSE((traits::is_velocity_unit_v<double>));

	EXPECT_TRUE((traits::is_velocity_unit_v<meters_per_second_t<double>>));
	EXPECT_TRUE((traits::is_velocity_unit_v<const meters_per_second_t<double>>));
	EXPECT_TRUE((traits::is_velocity_unit_v<const meters_per_second_t<double>&>));
	EXPECT_TRUE((traits::is_velocity_unit_v<miles_per_hour_t<double>>));
	EXPECT_FALSE((traits::is_velocity_unit_v<meters_per_second_squared_t<double>>));
	EXPECT_TRUE((traits::is_velocity_unit_v<miles_per_hour_t<double>, meters_per_second_t<double>>));
	EXPECT_FALSE((traits::is_velocity_unit_v<meters_per_second_squared_t<double>, meters_per_second_t<double>>));
}

TEST_F(TypeTraits, is_acceleration_unit)
{
	EXPECT_TRUE((traits::is_acceleration_unit_v<meters_per_second_squared>));
	EXPECT_TRUE((traits::is_acceleration_unit_v<acceleration::standard_gravity>));
	EXPECT_FALSE((traits::is_acceleration_unit_v<inch>));
	EXPECT_FALSE((traits::is_acceleration_unit_v<double>));

	EXPECT_TRUE((traits::is_acceleration_unit_v<meters_per_second_squared_t<double>>));
	EXPECT_TRUE((traits::is_acceleration_unit_v<const meters_per_second_squared_t<double>>));
	EXPECT_TRUE((traits::is_acceleration_unit_v<const meters_per_second_squared_t<double>&>));
	EXPECT_TRUE((traits::is_acceleration_unit_v<standard_gravity_t<double>>));
	EXPECT_FALSE((traits::is_acceleration_unit_v<inch_t<double>>));
	EXPECT_TRUE((traits::is_acceleration_unit_v<standard_gravity_t<double>, meters_per_second_squared_t<double>>));
	EXPECT_FALSE((traits::is_acceleration_unit_v<inch_t<double>, meters_per_second_squared_t<double>>));
}

TEST_F(TypeTraits, is_force_unit)
{
	EXPECT_TRUE((traits::is_force_unit_v<units::force::newton>));
	EXPECT_TRUE((traits::is_force_unit_v<units::force::dynes>));
	EXPECT_FALSE((traits::is_force_unit_v<meter>));
	EXPECT_FALSE((traits::is_force_unit_v<double>));

	EXPECT_TRUE((traits::is_force_unit_v<units::force::newton_t<double>>));
	EXPECT_TRUE((traits::is_force_unit_v<const units::force::newton_t<double>>));
	EXPECT_TRUE((traits::is_force_unit_v<const units::force::newton_t<double>&>));
	EXPECT_TRUE((traits::is_force_unit_v<units::force::dyne_t<double>>));
	EXPECT_FALSE((traits::is_force_unit_v<watt_t<double>>));
	EXPECT_TRUE((traits::is_force_unit_v<units::force::dyne_t<double>, units::force::newton_t<double>>));
	EXPECT_FALSE((traits::is_force_unit_v<watt_t<double>, units::force::newton_t<double>>));
}

TEST_F(TypeTraits, is_pressure_unit)
{
	EXPECT_TRUE((traits::is_pressure_unit_v<pressure::pascals>));
	EXPECT_TRUE((traits::is_pressure_unit_v<atmosphere>));
	EXPECT_FALSE((traits::is_pressure_unit_v<year>));
	EXPECT_FALSE((traits::is_pressure_unit_v<double>));

	EXPECT_TRUE((traits::is_pressure_unit_v<pascal_t<double>>));
	EXPECT_TRUE((traits::is_pressure_unit_v<const pascal_t<double>>));
	EXPECT_TRUE((traits::is_pressure_unit_v<const pascal_t<double>&>));
	EXPECT_TRUE((traits::is_pressure_unit_v<atmosphere_t<double>>));
	EXPECT_FALSE((traits::is_pressure_unit_v<year_t<double>>));
	EXPECT_TRUE((traits::is_pressure_unit_v<atmosphere_t<double>, pressure::pascal_t<double>>));
	EXPECT_FALSE((traits::is_pressure_unit_v<year_t<double>, pressure::pascal_t<double>>));
}

TEST_F(TypeTraits, is_charge_unit)
{
	EXPECT_TRUE((traits::is_charge_unit_v<coulomb>));
	EXPECT_FALSE((traits::is_charge_unit_v<watt>));
	EXPECT_FALSE((traits::is_charge_unit_v<double>));

	EXPECT_TRUE((traits::is_charge_unit_v<coulomb_t<double>>));
	EXPECT_TRUE((traits::is_charge_unit_v<const coulomb_t<double>>));
	EXPECT_TRUE((traits::is_charge_unit_v<const coulomb_t<double>&>));
	EXPECT_FALSE((traits::is_charge_unit_v<watt_t<double>>));
	EXPECT_TRUE((traits::is_charge_unit_v<const coulomb_t<double>&, coulomb_t<double>>));
	EXPECT_FALSE((traits::is_charge_unit_v<watt_t<double>, coulomb_t<double>>));
}

TEST_F(TypeTraits, is_energy_unit)
{
	EXPECT_TRUE((traits::is_energy_unit_v<joule>));
	EXPECT_TRUE((traits::is_energy_unit_v<calorie>));
	EXPECT_FALSE((traits::is_energy_unit_v<watt>));
	EXPECT_FALSE((traits::is_energy_unit_v<double>));

	EXPECT_TRUE((traits::is_energy_unit_v<joule_t<double>>));
	EXPECT_TRUE((traits::is_energy_unit_v<const joule_t<double>>));
	EXPECT_TRUE((traits::is_energy_unit_v<const joule_t<double>&>));
	EXPECT_TRUE((traits::is_energy_unit_v<calorie_t<double>>));
	EXPECT_FALSE((traits::is_energy_unit_v<watt_t<double>>));
	EXPECT_TRUE((traits::is_energy_unit_v<calorie_t<double>, joule_t<double>>));
	EXPECT_FALSE((traits::is_energy_unit_v<watt_t<double>, joule_t<double>>));
}

TEST_F(TypeTraits, is_power_unit)
{
	EXPECT_TRUE((traits::is_power_unit_v<watt>));
	EXPECT_FALSE((traits::is_power_unit_v<henry>));
	EXPECT_FALSE((traits::is_power_unit_v<double>));

	EXPECT_TRUE((traits::is_power_unit_v<watt_t<double>>));
	EXPECT_TRUE((traits::is_power_unit_v<const watt_t<double>>));
	EXPECT_TRUE((traits::is_power_unit_v<const watt_t<double>&>));
	EXPECT_FALSE((traits::is_power_unit_v<henry_t<double>>));
	EXPECT_TRUE((traits::is_power_unit_v<const watt_t<double>&, watt_t<double>>));
	EXPECT_FALSE((traits::is_power_unit_v<henry_t<double>, watt_t<double>>));
}

TEST_F(TypeTraits, is_voltage_unit)
{
	EXPECT_TRUE((traits::is_voltage_unit_v<volt>));
	EXPECT_FALSE((traits::is_voltage_unit_v<henry>));
	EXPECT_FALSE((traits::is_voltage_unit_v<double>));

	EXPECT_TRUE((traits::is_voltage_unit_v<volt_t<double>>));
	EXPECT_TRUE((traits::is_voltage_unit_v<const volt_t<double>>));
	EXPECT_TRUE((traits::is_voltage_unit_v<const volt_t<double>&>));
	EXPECT_FALSE((traits::is_voltage_unit_v<henry_t<double>>));
	EXPECT_TRUE((traits::is_voltage_unit_v<const volt_t<double>&, volt_t<double>>));
	EXPECT_FALSE((traits::is_voltage_unit_v<henry_t<double>, volt_t<double>>));
}

TEST_F(TypeTraits, is_capacitance_unit)
{
	EXPECT_TRUE((traits::is_capacitance_unit_v<farad>));
	EXPECT_FALSE((traits::is_capacitance_unit_v<ohm>));
	EXPECT_FALSE((traits::is_capacitance_unit_v<double>));

	EXPECT_TRUE((traits::is_capacitance_unit_v<farad_t<double>>));
	EXPECT_TRUE((traits::is_capacitance_unit_v<const farad_t<double>>));
	EXPECT_TRUE((traits::is_capacitance_unit_v<const farad_t<double>&>));
	EXPECT_FALSE((traits::is_capacitance_unit_v<ohm_t<double>>));
	EXPECT_TRUE((traits::is_capacitance_unit_v<const farad_t<double>&, millifarad_t<double>>));
	EXPECT_FALSE((traits::is_capacitance_unit_v<ohm_t<double>, farad_t<double>>));
}

TEST_F(TypeTraits, is_impedance_unit)
{
	EXPECT_TRUE((traits::is_impedance_unit_v<ohm>));
	EXPECT_FALSE((traits::is_impedance_unit_v<farad>));
	EXPECT_FALSE((traits::is_impedance_unit_v<double>));

	EXPECT_TRUE((traits::is_impedance_unit_v<ohm_t<double>>));
	EXPECT_TRUE((traits::is_impedance_unit_v<const ohm_t<double>>));
	EXPECT_TRUE((traits::is_impedance_unit_v<const ohm_t<double>&>));
	EXPECT_FALSE((traits::is_impedance_unit_v<farad_t<double>>));
	EXPECT_TRUE((traits::is_impedance_unit_v<const ohm_t<double>&, milliohm_t<double>>));
	EXPECT_FALSE((traits::is_impedance_unit_v<farad_t<double>, ohm_t<double>>));
}

TEST_F(TypeTraits, is_conductance_unit)
{
	EXPECT_TRUE((traits::is_conductance_unit_v<siemens>));
	EXPECT_FALSE((traits::is_conductance_unit_v<volt>));
	EXPECT_FALSE((traits::is_conductance_unit_v<double>));

	EXPECT_TRUE((traits::is_conductance_unit_v<siemens_t<double>>));
	EXPECT_TRUE((traits::is_conductance_unit_v<const siemens_t<double>>));
	EXPECT_TRUE((traits::is_conductance_unit_v<const siemens_t<double>&>));
	EXPECT_FALSE((traits::is_conductance_unit_v<volt_t<double>>));
	EXPECT_TRUE((traits::is_conductance_unit_v<const siemens_t<double>&, millisiemens_t<double>>));
	EXPECT_FALSE((traits::is_conductance_unit_v<volt_t<double>, siemens_t<double>>));
}

TEST_F(TypeTraits, is_magnetic_flux_unit)
{
	EXPECT_TRUE((traits::is_magnetic_flux_unit_v<weber>));
	EXPECT_TRUE((traits::is_magnetic_flux_unit_v<maxwell>));
	EXPECT_FALSE((traits::is_magnetic_flux_unit_v<inch>));
	EXPECT_FALSE((traits::is_magnetic_flux_unit_v<double>));

	EXPECT_TRUE((traits::is_magnetic_flux_unit_v<weber_t<double>>));
	EXPECT_TRUE((traits::is_magnetic_flux_unit_v<const weber_t<double>>));
	EXPECT_TRUE((traits::is_magnetic_flux_unit_v<const weber_t<double>&>));
	EXPECT_TRUE((traits::is_magnetic_flux_unit_v<maxwell_t<double>>));
	EXPECT_FALSE((traits::is_magnetic_flux_unit_v<inch_t<double>>));
	EXPECT_TRUE((traits::is_magnetic_flux_unit_v<maxwell_t<double>, weber_t<double>>));
	EXPECT_FALSE((traits::is_magnetic_flux_unit_v<inch_t<double>, weber_t<double>>));
}

TEST_F(TypeTraits, is_magnetic_field_strength_unit)
{
	EXPECT_TRUE((traits::is_magnetic_field_strength_unit_v<units::magnetic_field_strength::tesla>));
	EXPECT_TRUE((traits::is_magnetic_field_strength_unit_v<gauss>));
	EXPECT_FALSE((traits::is_magnetic_field_strength_unit_v<volt>));
	EXPECT_FALSE((traits::is_magnetic_field_strength_unit_v<double>));

	EXPECT_TRUE((traits::is_magnetic_field_strength_unit_v<tesla_t<double>>));
	EXPECT_TRUE((traits::is_magnetic_field_strength_unit_v<const tesla_t<double>>));
	EXPECT_TRUE((traits::is_magnetic_field_strength_unit_v<const tesla_t<double>&>));
	EXPECT_TRUE((traits::is_magnetic_field_strength_unit_v<gauss_t<double>>));
	EXPECT_FALSE((traits::is_magnetic_field_strength_unit_v<volt_t<double>>));
	EXPECT_TRUE((traits::is_magnetic_field_strength_unit_v<gauss_t<double>, tesla_t<double>>));
	EXPECT_FALSE((traits::is_magnetic_field_strength_unit_v<volt_t<double>, tesla_t<double>>));
}

TEST_F(TypeTraits, is_inductance_unit)
{
	EXPECT_TRUE((traits::is_inductance_unit_v<henry>));
	EXPECT_FALSE((traits::is_inductance_unit_v<farad>));
	EXPECT_FALSE((traits::is_inductance_unit_v<double>));

	EXPECT_TRUE((traits::is_inductance_unit_v<henry_t<double>>));
	EXPECT_TRUE((traits::is_inductance_unit_v<const henry_t<double>>));
	EXPECT_TRUE((traits::is_inductance_unit_v<const henry_t<double>&>));
	EXPECT_FALSE((traits::is_inductance_unit_v<farad_t<double>>));
	EXPECT_TRUE((traits::is_inductance_unit_v<const henry_t<double>&, millihenry_t<double>>));
	EXPECT_FALSE((traits::is_inductance_unit_v<farad_t<double>, henry_t<double>>));
}

TEST_F(TypeTraits, is_luminous_flux_unit)
{
	EXPECT_TRUE((traits::is_luminous_flux_unit_v<lumen>));
	EXPECT_FALSE((traits::is_luminous_flux_unit_v<mass::pound>));
	EXPECT_FALSE((traits::is_luminous_flux_unit_v<double>));

	EXPECT_TRUE((traits::is_luminous_flux_unit_v<lumen_t<double>>));
	EXPECT_TRUE((traits::is_luminous_flux_unit_v<const lumen_t<double>>));
	EXPECT_TRUE((traits::is_luminous_flux_unit_v<const lumen_t<double>&>));
	EXPECT_FALSE((traits::is_luminous_flux_unit_v<mass::pound_t<double>>));
	EXPECT_TRUE((traits::is_luminous_flux_unit_v<const lumen_t<double>&, millilumen_t<double>>));
	EXPECT_FALSE((traits::is_luminous_flux_unit_v<mass::pound_t<double>, lumen_t<double>>));
}

TEST_F(TypeTraits, is_illuminance_unit)
{
	EXPECT_TRUE((traits::is_illuminance_unit_v<illuminance::footcandle>));
	EXPECT_TRUE((traits::is_illuminance_unit_v<illuminance::lux>));
	EXPECT_FALSE((traits::is_illuminance_unit_v<meter>));
	EXPECT_FALSE((traits::is_illuminance_unit_v<double>));

	EXPECT_TRUE((traits::is_illuminance_unit_v<footcandle_t<double>>));
	EXPECT_TRUE((traits::is_illuminance_unit_v<const footcandle_t<double>>));
	EXPECT_TRUE((traits::is_illuminance_unit_v<const footcandle_t<double>&>));
	EXPECT_TRUE((traits::is_illuminance_unit_v<lux_t<double>>));
	EXPECT_FALSE((traits::is_illuminance_unit_v<meter_t<double>>));
	EXPECT_TRUE((traits::is_illuminance_unit_v<lux_t<double>, footcandle_t<double>>));
	EXPECT_FALSE((traits::is_illuminance_unit_v<meter_t<double>, footcandle_t<double>>));
}

TEST_F(TypeTraits, is_radioactivity_unit)
{
	EXPECT_TRUE((traits::is_radioactivity_unit_v<sievert>));
	EXPECT_FALSE((traits::is_radioactivity_unit_v<year>));
	EXPECT_FALSE((traits::is_radioactivity_unit_v<double>));

	EXPECT_TRUE((traits::is_radioactivity_unit_v<sievert_t<double>>));
	EXPECT_TRUE((traits::is_radioactivity_unit_v<const sievert_t<double>>));
	EXPECT_TRUE((traits::is_radioactivity_unit_v<const sievert_t<double>&>));
	EXPECT_FALSE((traits::is_radioactivity_unit_v<year_t<double>>));
	EXPECT_TRUE((traits::is_radioactivity_unit_v<const sievert_t<double>&, millisievert_t<double>>));
	EXPECT_FALSE((traits::is_radioactivity_unit_v<year_t<double>, sievert_t<double>>));
}

TEST_F(TypeTraits, is_torque_unit)
{
	EXPECT_TRUE((traits::is_torque_unit_v<torque::newton_meter>));
	EXPECT_TRUE((traits::is_torque_unit_v<torque::foot_pound>));
	EXPECT_FALSE((traits::is_torque_unit_v<volume::cubic_meter>));
	EXPECT_FALSE((traits::is_torque_unit_v<double>));

	EXPECT_TRUE((traits::is_torque_unit_v<torque::newton_meter_t<double>>));
	EXPECT_TRUE((traits::is_torque_unit_v<const torque::newton_meter_t<double>>));
	EXPECT_TRUE((traits::is_torque_unit_v<const torque::newton_meter_t<double>&>));
	EXPECT_TRUE((traits::is_torque_unit_v<torque::foot_pound_t<double>>));
	EXPECT_FALSE((traits::is_torque_unit_v<volume::cubic_meter_t<double>>));
	EXPECT_TRUE((traits::is_torque_unit_v<torque::foot_pound_t<double>, torque::newton_meter_t<double>>));
	EXPECT_FALSE((traits::is_torque_unit_v<volume::cubic_meter_t<double>, torque::newton_meter_t<double>>));
}

TEST_F(TypeTraits, is_area_unit)
{
	EXPECT_TRUE((traits::is_area_unit_v<square_meter>));
	EXPECT_TRUE((traits::is_area_unit_v<hectare>));
	EXPECT_FALSE((traits::is_area_unit_v<astronomical_unit>));
	EXPECT_FALSE((traits::is_area_unit_v<double>));

	EXPECT_TRUE((traits::is_area_unit_v<square_meter_t<double>>));
	EXPECT_TRUE((traits::is_area_unit_v<const square_meter_t<double>>));
	EXPECT_TRUE((traits::is_area_unit_v<const square_meter_t<double>&>));
	EXPECT_TRUE((traits::is_area_unit_v<hectare_t<double>>));
	EXPECT_FALSE((traits::is_area_unit_v<astronomical_unit_t<double>>));
	EXPECT_TRUE((traits::is_area_unit_v<hectare_t<double>, square_meter_t<double>>));
	EXPECT_FALSE((traits::is_area_unit_v<astronomical_unit_t<double>, square_meter_t<double>>));
}

TEST_F(TypeTraits, is_volume_unit)
{
	EXPECT_TRUE((traits::is_volume_unit_v<cubic_meter>));
	EXPECT_TRUE((traits::is_volume_unit_v<cubic_foot>));
	EXPECT_FALSE((traits::is_volume_unit_v<square_feet>));
	EXPECT_FALSE((traits::is_volume_unit_v<double>));

	EXPECT_TRUE((traits::is_volume_unit_v<cubic_meter_t<double>>));
	EXPECT_TRUE((traits::is_volume_unit_v<const cubic_meter_t<double>>));
	EXPECT_TRUE((traits::is_volume_unit_v<const cubic_meter_t<double>&>));
	EXPECT_TRUE((traits::is_volume_unit_v<cubic_inch_t<double>>));
	EXPECT_FALSE((traits::is_volume_unit_v<foot_t<double>>));
	EXPECT_TRUE((traits::is_volume_unit_v<cubic_inch_t<double>, cubic_meter_t<double>>));
	EXPECT_FALSE((traits::is_volume_unit_v<foot_t<double>, cubic_meter_t<double>>));
}

TEST_F(TypeTraits, is_density_unit)
{
	EXPECT_TRUE((traits::is_density_unit_v<kilograms_per_cubic_meter>));
	EXPECT_TRUE((traits::is_density_unit_v<ounces_per_cubic_foot>));
	EXPECT_FALSE((traits::is_density_unit_v<year>));
	EXPECT_FALSE((traits::is_density_unit_v<double>));

	EXPECT_TRUE((traits::is_density_unit_v<kilograms_per_cubic_meter_t<double>>));
	EXPECT_TRUE((traits::is_density_unit_v<const kilograms_per_cubic_meter_t<double>>));
	EXPECT_TRUE((traits::is_density_unit_v<const kilograms_per_cubic_meter_t<double>&>));
	EXPECT_TRUE((traits::is_density_unit_v<ounces_per_cubic_foot_t<double>>));
	EXPECT_FALSE((traits::is_density_unit_v<year_t<double>>));
	EXPECT_TRUE((traits::is_density_unit_v<ounces_per_cubic_foot_t<double>, kilograms_per_cubic_meter_t<double>>));
	EXPECT_FALSE((traits::is_density_unit_v<year_t<double>, kilograms_per_cubic_meter_t<double>>));
}

TEST_F(TypeTraits, is_data_unit)
{
	EXPECT_TRUE((traits::is_data_unit_v<bit>));
	EXPECT_TRUE((traits::is_data_unit_v<byte>));
	EXPECT_TRUE((traits::is_data_unit_v<exabit>));
	EXPECT_TRUE((traits::is_data_unit_v<exabyte>));
	EXPECT_FALSE((traits::is_data_unit_v<year>));
	EXPECT_FALSE((traits::is_data_unit_v<double>));

	EXPECT_TRUE((traits::is_data_unit_v<bit_t<double>>));
	EXPECT_TRUE((traits::is_data_unit_v<const bit_t<double>>));
	EXPECT_TRUE((traits::is_data_unit_v<const bit_t<double>&>));
	EXPECT_TRUE((traits::is_data_unit_v<byte_t<double>>));
	EXPECT_FALSE((traits::is_data_unit_v<year_t<double>>));
	EXPECT_TRUE((traits::is_data_unit_v<bit_t<double>, byte_t<double>>));
	EXPECT_FALSE((traits::is_data_unit_v<year_t<double>, byte_t<double>>));
}

TEST_F(TypeTraits, is_data_transfer_rate_unit)
{
	EXPECT_TRUE((traits::is_data_transfer_rate_unit_v<Gbps>));
	EXPECT_TRUE((traits::is_data_transfer_rate_unit_v<GBps>));
	EXPECT_FALSE((traits::is_data_transfer_rate_unit_v<year>));
	EXPECT_FALSE((traits::is_data_transfer_rate_unit_v<double>));

	EXPECT_TRUE((traits::is_data_transfer_rate_unit_v<gigabits_per_second_t<double>>));
	EXPECT_TRUE((traits::is_data_transfer_rate_unit_v<const gigabytes_per_second_t<double>>));
	EXPECT_TRUE((traits::is_data_transfer_rate_unit_v<const gigabytes_per_second_t<double>&>));
	EXPECT_TRUE((traits::is_data_transfer_rate_unit_v<gigabytes_per_second_t<double>>));
	EXPECT_FALSE((traits::is_data_transfer_rate_unit_v<year_t<double>>));
	EXPECT_TRUE((traits::is_data_transfer_rate_unit_v<gigabits_per_second_t<double>, gigabytes_per_second_t<double>>));
	EXPECT_FALSE((traits::is_data_transfer_rate_unit_v<year_t<double>, gigabytes_per_second_t<double>>));
}

TEST_F(STDTypeTraits, std_common_type)
{
	static_assert(
		has_equivalent_conversion_factor(std::common_type_t<meter_t<double>, meter_t<double>>(), meter_t<double>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<kilometer_t<double>, kilometer_t<double>>(), kilometer_t<double>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<millimeter_t<double>, millimeter_t<double>>(), millimeter_t<double>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<meter_t<double>, kilometer_t<double>>(), meter_t<double>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<kilometer_t<double>, meter_t<double>>(), meter_t<double>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<meter_t<double>, millimeter_t<double>>(), millimeter_t<double>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<millimeter_t<double>, meter_t<double>>(), millimeter_t<double>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<millimeter_t<double>, kilometer_t<double>>(), millimeter_t<double>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<kilometer_t<double>, millimeter_t<double>>(), millimeter_t<double>()));
	static_assert(std::is_same_v<std::common_type_t<meter_t<double>, kilometer_t<double>>,
		std::common_type_t<kilometer_t<double>, meter_t<double>>>);
	static_assert(std::is_same_v<std::common_type_t<meter_t<double>, millimeter_t<double>>,
		std::common_type_t<millimeter_t<double>, meter_t<double>>>);
	static_assert(std::is_same_v<std::common_type_t<millimeter_t<double>, kilometer_t<double>>,
		std::common_type_t<kilometer_t<double>, millimeter_t<double>>>);

	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<unit<meters, int>, unit<meters, int>>(), unit<meters, int>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<unit<kilometers, int>, unit<kilometers, int>>(), unit<kilometers, int>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<unit<millimeters, int>, unit<millimeters, int>>(), unit<millimeters, int>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<unit<meters, int>, unit<kilometers, int>>(), unit<meters, int>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<unit<kilometers, int>, unit<meters, int>>(), unit<meters, int>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<unit<meters, int>, unit<millimeters, int>>(), unit<millimeters, int>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<unit<millimeters, int>, unit<meters, int>>(), unit<millimeters, int>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<unit<millimeters, int>, unit<kilometers, int>>(), unit<millimeters, int>()));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<unit<kilometers, int>, unit<millimeters, int>>(), unit<millimeters, int>()));
	static_assert(std::is_same_v<std::common_type_t<unit<meters, int>, unit<kilometers, int>>,
		std::common_type_t<unit<kilometers, int>, unit<meters, int>>>);
	static_assert(std::is_same_v<std::common_type_t<unit<meters, int>, unit<millimeters, int>>,
		std::common_type_t<unit<millimeters, int>, unit<meters, int>>>);
	static_assert(std::is_same_v<std::common_type_t<unit<millimeters, int>, unit<kilometers, int>>,
		std::common_type_t<unit<kilometers, int>, unit<millimeters, int>>>);

	using half_a_second  = unit<conversion_factor<std::ratio<1, 2>, seconds>, int>;
	using third_a_second = unit<conversion_factor<std::ratio<1, 3>, seconds>, int>;
	using sixth_a_second = unit<conversion_factor<std::ratio<1, 6>, seconds>, int>;

	static_assert(
		has_equivalent_conversion_factor(std::common_type_t<half_a_second, third_a_second>{}, sixth_a_second{}));
	static_assert(std::is_same_v<std::common_type_t<half_a_second, third_a_second>,
		std::common_type_t<third_a_second, half_a_second>>);
	static_assert(std::is_same_v<std::common_type_t<half_a_second, third_a_second>::underlying_type, int>);

	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<kelvin_t<double>, celsius_t<double>>{}, celsius_t<double>{}));
	static_assert(has_equivalent_conversion_factor(
		std::common_type_t<celsius_t<double>, kelvin_t<double>>{}, celsius_t<double>{}));
	static_assert(std::is_same_v<std::common_type_t<kelvin_t<double>, celsius_t<double>>,
		std::common_type_t<celsius_t<double>, kelvin_t<double>>>);

	using half_a_kelvin  = unit<conversion_factor<std::ratio<1, 2>, kelvin>, double>;
	using third_a_kelvin = unit<conversion_factor<std::ratio<1, 3>, kelvin>, int>;
	using sixth_a_kelvin = unit<conversion_factor<std::ratio<1, 6>, kelvin>, int>;

	static_assert(
		has_equivalent_conversion_factor(std::common_type_t<half_a_kelvin, third_a_kelvin>{}, sixth_a_kelvin{}));
	static_assert(std::is_same_v<std::common_type_t<half_a_kelvin, third_a_kelvin>,
		std::common_type_t<third_a_kelvin, half_a_kelvin>>);
	static_assert(std::is_same_v<std::common_type_t<half_a_kelvin, third_a_kelvin>::underlying_type, double>);

	static_assert(
		has_equivalent_conversion_factor(std::common_type_t<radian_t<double>, degree_t<double>>{}, degree_t<double>{}));
	static_assert(
		has_equivalent_conversion_factor(std::common_type_t<degree_t<double>, radian_t<double>>{}, degree_t<double>{}));
	static_assert(std::is_same_v<std::common_type_t<radian_t<double>, degree_t<double>>,
		std::common_type_t<degree_t<double>, radian_t<double>>>);

	using half_a_radian  = unit<conversion_factor<std::ratio<1, 2>, radians>, int>;
	using third_a_radian = unit<conversion_factor<std::ratio<1, 3>, radians>, double>;
	using sixth_a_radian = unit<conversion_factor<std::ratio<1, 6>, radians>, int>;

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
}

TEST_F(STDSpecializations, hash)
{
	EXPECT_EQ(std::hash<meter_t<double>>()(3.14_m), std::hash<double>()(3.14));
	EXPECT_EQ(std::hash<millimeter_t<double>>()(3.14_m), std::hash<double>()(3.14e3));
	EXPECT_EQ(std::hash<millimeter_t<double>>()(3.14_mm), std::hash<double>()(3.14));
	EXPECT_EQ(std::hash<kilometer_t<double>>()(3.14_m), std::hash<double>()(3.14e-3));
	EXPECT_EQ(std::hash<kilometer_t<double>>()(3.14_km), std::hash<double>()(3.14));

	EXPECT_EQ((std::hash<unit<meters, int>>()(unit<meters, int>(42))), 42);
	EXPECT_EQ((std::hash<unit<millimeters, int>>()(unit<meters, int>(42))), 42000);
	EXPECT_EQ((std::hash<unit<millimeters, int>>()(unit<millimeters, int>(42))), 42);
	EXPECT_EQ((std::hash<unit<kilometers, int>>()(unit<kilometers, int>(42))), 42);

	EXPECT_EQ((std::hash<dimensionless<double>>()(3.14)), std::hash<double>()(3.14));
	EXPECT_EQ((std::hash<unit<dimensionless_unit, double>>()(3.14)), std::hash<double>()(3.14));
	EXPECT_EQ((std::hash<dimensionless<int>>()(42)), (std::hash<unit<dimensionless_unit, int>>()(42)));
}

TEST_F(UnitManipulators, squared)
{
	double test;

	test = unit<square_feet>(unit<squared<meters>>(0.092903))();
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

	test = unit<cubic_feet>(unit<cubed<meters>>(0.0283168))();
	EXPECT_NEAR(0.999998354619, test, 5.0e-13);
}

TEST_F(UnitManipulators, square_root)
{
	double test;

	test = meter_t<double>(unit<square_root<square_kilometer>>(1.0))();
	EXPECT_TRUE((traits::is_convertible_unit_v<square_root<square_kilometer>, kilometer>));
	EXPECT_NEAR(1000.0, test, 5.0e-13);
}

TEST_F(UnitManipulators, compound_unit)
{
	using acceleration1 = conversion_factor<std::ratio<1>, dimension::acceleration>;
	using acceleration2 = compound_conversion_factor<meters, inverse<seconds>, inverse<seconds>>;
	using acceleration3 = conversion_factor<std::ratio<1>,
		make_dimension<dimension::length, std::ratio<1>, dimension::time, std::ratio<-2>>>;
	using acceleration4 = compound_conversion_factor<meters, inverse<squared<seconds>>>;
	using acceleration5 = compound_conversion_factor<meters, squared<inverse<seconds>>>;

	bool areSame12 = std::is_same_v<acceleration1, acceleration2>;
	bool areSame23 = std::is_same_v<acceleration2, acceleration3>;
	bool areSame34 = std::is_same_v<acceleration3, acceleration4>;
	bool areSame45 = std::is_same_v<acceleration4, acceleration5>;

	EXPECT_TRUE(areSame12);
	EXPECT_TRUE(areSame23);
	EXPECT_TRUE(areSame34);
	EXPECT_TRUE(areSame45);

	// test that thing with translations still compile
	using arbitrary1 = compound_conversion_factor<meters, inverse<celsius>>;
	using arbitrary2 = compound_conversion_factor<meters, celsius>;
	using arbitrary3 = compound_conversion_factor<arbitrary1, arbitrary2>;
	EXPECT_TRUE((std::is_same_v<square_meters, traits::strong_t<arbitrary3>>));
}

TEST_F(UnitManipulators, dimensionalAnalysis)
{
	// these look like 'compound units', but the dimensional analysis can be REALLY handy if the
	// unit types aren't know (i.e. they themselves are template parameters), as you can get the resulting unit of the
	// operation.

	using velocity    = traits::strong_t<units::detail::unit_divide<meters, second>>;
	bool shouldBeTrue = std::is_same_v<meters_per_second, velocity>;
	EXPECT_TRUE(shouldBeTrue);

	using acceleration1 = conversion_factor<std::ratio<1>, dimension::acceleration>;
	using acceleration2 = units::detail::unit_divide<meters, units::detail::unit_multiply<seconds, seconds>>;
	shouldBeTrue        = std::is_same_v<acceleration1, acceleration2>;
	EXPECT_TRUE(shouldBeTrue);
}

TEST_F(UnitType, trivial)
{
	EXPECT_TRUE((std::is_trivial_v<meter_t<double>>));
	EXPECT_TRUE((std::is_trivially_assignable_v<meter_t<double>, meter_t<double>>));
	EXPECT_TRUE((std::is_trivially_constructible_v<meter_t<double>>));
	EXPECT_TRUE((std::is_trivially_copy_assignable_v<meter_t<double>>));
	EXPECT_TRUE((std::is_trivially_copy_constructible_v<meter_t<double>>));
	EXPECT_TRUE((std::is_trivially_copyable_v<meter_t<double>>));
	EXPECT_TRUE((std::is_trivially_default_constructible_v<meter_t<double>>));
	EXPECT_TRUE((std::is_trivially_destructible_v<meter_t<double>>));
	EXPECT_TRUE((std::is_trivially_move_assignable_v<meter_t<double>>));
	EXPECT_TRUE((std::is_trivially_move_constructible_v<meter_t<double>>));

	EXPECT_TRUE((std::is_trivial_v<dB_t<double>>));
	EXPECT_TRUE((std::is_trivially_assignable_v<dB_t<double>, dB_t<double>>));
	EXPECT_TRUE((std::is_trivially_constructible_v<dB_t<double>>));
	EXPECT_TRUE((std::is_trivially_copy_assignable_v<dB_t<double>>));
	EXPECT_TRUE((std::is_trivially_copy_constructible_v<dB_t<double>>));
	EXPECT_TRUE((std::is_trivially_copyable_v<dB_t<double>>));
	EXPECT_TRUE((std::is_trivially_default_constructible_v<dB_t<double>>));
	EXPECT_TRUE((std::is_trivially_destructible_v<dB_t<double>>));
	EXPECT_TRUE((std::is_trivially_move_assignable_v<dB_t<double>>));
	EXPECT_TRUE((std::is_trivially_move_constructible_v<dB_t<double>>));
}

TEST_F(UnitType, constructionFromArithmeticType)
{
	const meter_t<double> a_m(1.0);
	EXPECT_EQ(1.0, a_m());

	const meter_t<double> b_m(1);
	EXPECT_EQ(1, b_m());

	const unit<meters, int> c_m(1);
	EXPECT_EQ(1, c_m());

	const unit<dimensionless_unit, double> d_dim(1.0);
	EXPECT_EQ(1.0, d_dim());

	const unit<dimensionless_unit, double> e_dim(1);
	EXPECT_EQ(1, e_dim());

	const dimensionless<double> a_dim(1.0);
	EXPECT_EQ(1.0, a_dim());

	const dimensionless<double> b_dim(1);
	EXPECT_EQ(1, b_dim());

	const unit<dimensionless_unit, int> c_dim(1);
	EXPECT_EQ(1, c_dim());

	const dimensionless<int> f_dim(1);
	EXPECT_EQ(1, f_dim());
}

TEST_F(UnitType, constructionFromUnitType)
{
	const unit<meters, int> a_m(1);

	const unit<meters, int> b_m(a_m);
	EXPECT_EQ(1, b_m());

	const unit<millimeters, int> a_mm(b_m);
	EXPECT_EQ(1000, a_mm());

	const millimeter_t<int> b_mm(a_mm);
	EXPECT_EQ(1000, b_mm());

	const millimeter_t<int> c_mm(b_mm);
	EXPECT_EQ(1000, c_mm());

	const millimeter_t<int> d_mm(b_m);
	EXPECT_EQ(1000, d_mm());

	const meter_t<double> c_m(b_m);
	EXPECT_EQ(1.0, c_m());

	const meter_t<double> d_m(a_mm);
	EXPECT_EQ(1.0, d_m());

	const meter_t<double> e_m(b_mm);
	EXPECT_EQ(1.0, e_m());

	const meter_t<double> f_m(c_m);
	EXPECT_EQ(1.0, f_m());

	const meter_t<double> g_m(kilometer_t<int>(1));
	EXPECT_EQ(1000.0, g_m());

	const unit<dimensionless_unit, int> a_dim(1);

	const unit<dimensionless_unit, int> b_dim(a_dim);
	EXPECT_EQ(1, b_dim());

	const dimensionless<int> c_dim(b_dim);
	EXPECT_EQ(1, c_dim());

	const unit<dimensionless_unit, int> d_dim(c_dim);
	EXPECT_EQ(1, d_dim());

	const dimensionless<double> e_dim(d_dim);
	EXPECT_EQ(1, e_dim());

	const dimensionless<double> f_dim(c_dim);
	EXPECT_EQ(1, f_dim());

	const dimensionless<double> g_dim(f_dim);
	EXPECT_EQ(1, g_dim());
}

TEST_F(UnitType, CTAD)
{
	// Default ctor
	const meter_t z_m = 1.0_m;
	static_assert(std::is_same_v<std::remove_const_t<decltype(z_m)>, meter_t<double>>);

	// Underlying type, copy ctor, and same dimensioned units for `int` and `double`.
	const meter_t a_m(1);
	static_assert(std::is_same_v<std::remove_const_t<decltype(a_m)>, meter_t<int>>);

	const meter_t b_m(a_m);
	static_assert(std::is_same_v<std::remove_const_t<decltype(b_m)>, meter_t<int>>);

	const millimeter_t a_mm(b_m);
	static_assert(std::is_same_v<std::remove_const_t<decltype(a_mm)>, millimeter_t<int>>);

	const meter_t c_m(1.0);
	static_assert(std::is_same_v<std::remove_const_t<decltype(c_m)>, meter_t<double>>);

	const meter_t d_m(c_m);
	static_assert(std::is_same_v<std::remove_const_t<decltype(d_m)>, meter_t<double>>);

	const millimeter_t b_mm(d_m);
	static_assert(std::is_same_v<std::remove_const_t<decltype(b_mm)>, millimeter_t<double>>);

	const kilometer_t a_km(b_mm);
	static_assert(std::is_same_v<std::remove_const_t<decltype(a_km)>, kilometer_t<double>>);

	// Other underlying types.
	const meter_t e_m(short(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(e_m)>, meter_t<short>>);

	const meter_t f_m(1.0f);
	static_assert(std::is_same_v<std::remove_const_t<decltype(f_m)>, meter_t<float>>);

	const meter_t g_m(1LL);
	static_assert(std::is_same_v<std::remove_const_t<decltype(g_m)>, meter_t<long long>>);

	const meter_t h_m(1.0L);
	static_assert(std::is_same_v<std::remove_const_t<decltype(h_m)>, meter_t<long double>>);

	// `unit`.
	const meter_t i_m(unit<meter, int>(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(i_m)>, meter_t<int>>);

	const meter_t j_m(unit<meter, double>(1.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(j_m)>, meter_t<double>>);

	const meter_t k_m(unit<kilometer, int>(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(k_m)>, meter_t<int>>);

	const meter_t l_m(unit<kilometer, double>(1.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(l_m)>, meter_t<double>>);

	const meter_t m_m(unit<millimeter, double>(1.0));
	static_assert(std::is_same_v<std::remove_const_t<decltype(m_m)>, meter_t<double>>);

	// `std::chrono::duration`.
	using namespace std::chrono_literals;

	const second_t a_s(1_s);
	static_assert(std::is_integral_v<decltype(a_s())>);

	const second_t b_s(1.0_s);
	static_assert(std::is_floating_point_v<decltype(b_s())>);

	[[maybe_unused]] const second_t c_s(1_min);
	[[maybe_unused]] const second_t d_s(1.0_min);
	[[maybe_unused]] const second_t e_s(1.0_ms);

	// Dimensionless units.
	const dimensionless z_dim = 1.0;
	static_assert(std::is_same_v<std::remove_const_t<decltype(z_dim)>, dimensionless<double>>);

	const dimensionless a_dim(1);
	static_assert(std::is_same_v<std::remove_const_t<decltype(a_dim)>, dimensionless<int>>);

	const dimensionless b_dim(a_dim);
	static_assert(std::is_same_v<std::remove_const_t<decltype(b_dim)>, dimensionless<int>>);

	const percent_t a_per(b_dim);
	static_assert(std::is_same_v<std::remove_const_t<decltype(a_per)>, percent_t<int>>);

	const dimensionless c_dim(1.0);
	static_assert(std::is_same_v<std::remove_const_t<decltype(c_dim)>, dimensionless<double>>);

	const dimensionless d_dim(c_dim);
	static_assert(std::is_same_v<std::remove_const_t<decltype(d_dim)>, dimensionless<double>>);

	const percent_t b_per(d_dim);
	static_assert(std::is_same_v<std::remove_const_t<decltype(b_per)>, percent_t<double>>);

	const dimensionless e_dim(short(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(e_dim)>, dimensionless<short>>);

	const dimensionless f_dim(1.0f);
	static_assert(std::is_same_v<std::remove_const_t<decltype(f_dim)>, dimensionless<float>>);

	const dimensionless g_dim(1LL);
	static_assert(std::is_same_v<std::remove_const_t<decltype(g_dim)>, dimensionless<long long>>);

	const dimensionless h_dim(1.0L);
	static_assert(std::is_same_v<std::remove_const_t<decltype(h_dim)>, dimensionless<long double>>);

	const dimensionless i_dim(unit<dimensionless_unit, int>(1));
	static_assert(std::is_same_v<std::remove_const_t<decltype(i_dim)>, dimensionless<int>>);

	const dimensionless j_dim(unit<dimensionless_unit, double>(1.0));
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
	unit<dimensionless_unit, int> a_dim;
	a_dim = 1;
	EXPECT_EQ(1, a_dim());
	a_dim = 1.0;
	EXPECT_EQ(1.0, a_dim());

	dimensionless<int> b_dim;
	b_dim = 1;
	EXPECT_EQ(1, b_dim());
	b_dim = 1.0;
	EXPECT_EQ(1, b_dim());

	unit<dimensionless_unit, double> c_dim;
	c_dim = 1.0;
	EXPECT_EQ(1.0, c_dim());
	c_dim = 1;
	EXPECT_EQ(1, c_dim());

	dimensionless<double> d_dim;
	d_dim = 1.0;
	EXPECT_EQ(1.0, d_dim());
	d_dim = 1;
	EXPECT_EQ(1, d_dim());
}

TEST_F(UnitType, assignmentFromUnitType)
{
	unit<meters, int> a_m(1);
	a_m = +a_m;
	EXPECT_EQ(1, a_m());

	unit<millimeters, int> a_mm;
	a_mm = a_m;
	EXPECT_EQ(1000, a_mm());
	a_mm = +a_mm;
	EXPECT_EQ(1000, a_mm());

	millimeter_t<int> b_mm;
	b_mm = a_m;
	EXPECT_EQ(1000, b_mm());
	b_mm = a_mm;
	EXPECT_EQ(1000, b_mm());
	b_mm = +b_mm;
	EXPECT_EQ(1000, b_mm());

	a_mm = b_mm;
	EXPECT_EQ(1000, a_mm());

	meter_t<double> b_m;
	b_m = a_m;
	EXPECT_EQ(1, b_m());
	b_m = a_mm;
	EXPECT_EQ(1, b_m());
	b_m = b_mm;
	EXPECT_EQ(1, b_m());
	b_m = +b_m;
	EXPECT_EQ(1, b_m());
	b_m = unit<kilometers, int>(1);
	EXPECT_EQ(1000, b_m());

	unit<dimensionless_unit, int> a_dim(1);
	a_dim = +a_dim;
	EXPECT_EQ(1, a_dim());

	dimensionless<int> b_dim;
	b_dim = a_dim;
	EXPECT_EQ(1, b_dim());
	b_dim = +b_dim;
	EXPECT_EQ(1, b_dim());

	a_dim = b_dim;
	EXPECT_EQ(1, a_dim());

	dimensionless<double> c_dim;
	c_dim = a_dim;
	EXPECT_EQ(1, c_dim());
	c_dim = b_dim;
	EXPECT_EQ(1, c_dim());
	c_dim = +c_dim;
	EXPECT_EQ(1, c_dim());
}

TEST_F(UnitType, make_unit)
{
	const auto a_m = make_unit<meter_t<double>>(5.0);
	EXPECT_EQ(meter_t<double>(5.0), a_m);

	const auto b_m = make_unit<meter_t<double>>(5);
	EXPECT_EQ(meter_t<double>(5), b_m);

	const auto c_m = make_unit<unit<meters, int>>(5);
	EXPECT_EQ((unit<meters, int>(5)), c_m);

	const auto a_dim = make_unit<dimensionless<double>>(5.0);
	EXPECT_EQ(dimensionless<double>(5.0), a_dim);

	const auto b_dim = make_unit<dimensionless<double>>(5);
	EXPECT_EQ(dimensionless<double>(5), b_dim);

	const auto c_dim = make_unit<unit<dimensionless_unit, int>>(5);
	EXPECT_EQ((unit<dimensionless_unit, int>(5)), c_dim);
}

TEST_F(UnitType, unitTypeEquality)
{
	const meter_t<double> a_m(0);
	const meter_t<double> b_m(1);

	EXPECT_TRUE(a_m == a_m);
	EXPECT_FALSE(a_m == b_m);
	EXPECT_TRUE(a_m != b_m);
	EXPECT_FALSE(b_m != b_m);

	const unit<meters, int> c_m(0);
	const unit<meters, int> d_m(1);

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
	const meter_t<double> a_m(0);
	const foot_t<double> a_f(meter_t<double>(1));

	EXPECT_FALSE(a_m == a_f);
	EXPECT_TRUE(a_m != a_f);

	const unit<feet, int> b_f(0);
	const unit<meters, int> b_m(1);

	EXPECT_FALSE(b_f == b_m);
	EXPECT_TRUE(b_f != b_m);

	EXPECT_TRUE(a_m == b_f);
	EXPECT_TRUE(b_m == a_f);
	EXPECT_FALSE(a_m != b_f);
	EXPECT_FALSE(b_m != a_f);
}

TEST_F(UnitType, unitTypeRelational)
{
	const meter_t<double> a_m(0);
	const meter_t<double> b_m(1);

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

	const unit<meters, int> c_m(0);
	const unit<meters, int> d_m(1);

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

	const unit<dimensionless_unit, double> a_s(0);
	const unit<dimensionless_unit, int> b_s(1);

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
	const meter_t<double> a_m(0);
	const foot_t<double> a_f(meter_t<double>(1));

	EXPECT_FALSE(a_f < a_m);
	EXPECT_TRUE(a_m < a_f);
	EXPECT_FALSE(a_f <= a_m);
	EXPECT_TRUE(a_m <= a_f);
	EXPECT_TRUE(a_f > a_m);
	EXPECT_FALSE(a_m > a_f);
	EXPECT_TRUE(a_f >= a_m);
	EXPECT_FALSE(a_m >= a_f);

	const unit<feet, int> b_f(0);
	const unit<meters, int> b_m(1);

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

	using meter      = meter_t<int>;
	using meter_base = unit<units::meter, int>;

	meter m;
	meter_base b_m;

	using squared_meter = square_meter_t<int>;
	using inverse_meter = unit<inverse<units::meter>, int>;

	static_assert(std::is_same_v<dimless, decltype(+dim)>);
	static_assert(std::is_same_v<dimless_base, decltype(+base)>);

	static_assert(std::is_same_v<meter, decltype(+m)>);
	static_assert(std::is_same_v<meter_base, decltype(+b_m)>);

	static_assert(std::is_same_v<dimless, decltype(-dim)>);
	static_assert(std::is_same_v<dimless_base, decltype(-base)>);

	static_assert(std::is_same_v<meter, decltype(-m)>);
	static_assert(std::is_same_v<meter_base, decltype(-b_m)>);

	static_assert(std::is_same_v<dimless, decltype(dim + 0)>);
	static_assert(std::is_same_v<dimless, decltype(0 + dim)>);
	static_assert(std::is_same_v<dimless, decltype(dim + dim)>);
	static_assert(std::is_same_v<dimless, decltype(dim + base)>);
	static_assert(std::is_same_v<dimless, decltype(base + dim)>);
	static_assert(std::is_same_v<dimless_base, decltype(base + base)>);
	static_assert(std::is_same_v<dimless_base, decltype(base + 0)>);
	static_assert(std::is_same_v<dimless_base, decltype(0 + base)>);

	static_assert(std::is_same_v<meter, decltype(m + m)>);
	static_assert(std::is_same_v<meter, decltype(m + b_m)>);
	static_assert(std::is_same_v<meter, decltype(b_m + m)>);
	static_assert(std::is_same_v<meter_base, decltype(b_m + b_m)>);

	static_assert(std::is_same_v<dimless, decltype(dim - 0)>);
	static_assert(std::is_same_v<dimless, decltype(0 - dim)>);
	static_assert(std::is_same_v<dimless, decltype(dim - dim)>);
	static_assert(std::is_same_v<dimless, decltype(dim - base)>);
	static_assert(std::is_same_v<dimless, decltype(base - dim)>);
	static_assert(std::is_same_v<dimless_base, decltype(base - base)>);
	static_assert(std::is_same_v<dimless_base, decltype(base - 0)>);
	static_assert(std::is_same_v<dimless_base, decltype(0 - base)>);

	static_assert(std::is_same_v<meter, decltype(m - m)>);
	static_assert(std::is_same_v<meter, decltype(m - b_m)>);
	static_assert(std::is_same_v<meter, decltype(b_m - m)>);
	static_assert(std::is_same_v<meter_base, decltype(b_m - b_m)>);

	static_assert(std::is_same_v<dimless, decltype(dim * 1)>);
	static_assert(std::is_same_v<dimless, decltype(1 * dim)>);
	static_assert(std::is_same_v<dimless, decltype(dim * dim)>);
	static_assert(std::is_same_v<dimless, decltype(dim * base)>);
	static_assert(std::is_same_v<dimless, decltype(base * dim)>);
	static_assert(std::is_same_v<dimless, decltype(base * base)>);
	static_assert(std::is_same_v<dimless_base, decltype(base * 1)>);
	static_assert(std::is_same_v<dimless_base, decltype(1 * base)>);

	static_assert(std::is_same_v<meter, decltype(m * 1)>);
	static_assert(std::is_same_v<meter, decltype(1 * m)>);
	static_assert(std::is_same_v<meter, decltype(m * dim)>);
	static_assert(std::is_same_v<meter, decltype(dim * m)>);
	static_assert(std::is_same_v<meter, decltype(m * base)>);
	static_assert(std::is_same_v<meter, decltype(base * m)>);

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

	static_assert(std::is_same_v<meter, decltype(m / 1)>);
	static_assert(std::is_same_v<inverse_meter, decltype(1 / m)>);
	static_assert(std::is_same_v<meter, decltype(m / dim)>);
	static_assert(std::is_same_v<inverse_meter, decltype(dim / m)>);
	static_assert(std::is_same_v<meter, decltype(m / base)>);
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

	static_assert(std::is_same_v<meter, decltype(m % 1)>);
	static_assert(std::is_same_v<meter, decltype(m % dim)>);
	static_assert(std::is_same_v<meter, decltype(m % base)>);
	static_assert(std::is_same_v<meter, decltype(m % m)>);
	static_assert(std::is_same_v<meter, decltype(m % b_m)>);
	static_assert(std::is_same_v<meter_base, decltype(b_m % 1)>);
	static_assert(std::is_same_v<meter_base, decltype(b_m % dim)>);
	static_assert(std::is_same_v<meter_base, decltype(b_m % base)>);
	static_assert(std::is_same_v<meter_base, decltype(b_m % m)>);
	static_assert(std::is_same_v<meter_base, decltype(b_m % b_m)>);
}

TEST_F(UnitType, unitTypeAddition)
{
	// units
	meter_t<double> a_m(1.0), c_m;
	foot_t<double> b_ft(3.28084);
	const unit<meters, int> f_m(1);
	const std::common_type_t<unit<meters, int>, unit<feet, int>> g(f_m);

	double d = meter_t<double>(b_ft)();
	EXPECT_NEAR(1.0, d, 5.0e-5);
	d = meter_t<double>(g)();
	EXPECT_NEAR(1.0, d, 5.0e-5);

	c_m = a_m + b_ft;
	EXPECT_NEAR(2.0, c_m(), 5.0e-5);
	c_m = f_m + g;
	EXPECT_NEAR(2.0, c_m(), 5.0e-5);
	c_m = a_m + g;
	EXPECT_NEAR(2.0, c_m(), 5.0e-5);
	c_m = f_m + b_ft;
	EXPECT_NEAR(2.0, c_m(), 5.0e-5);

	c_m = b_ft + meter_t<double>(3);
	EXPECT_NEAR(4.0, c_m(), 5.0e-5);
	c_m = g + unit<meters, int>(3);
	EXPECT_NEAR(4.0, c_m(), 5.0e-5);
	c_m = b_ft + unit<meters, int>(3);
	EXPECT_NEAR(4.0, c_m(), 5.0e-5);
	c_m = g + meter_t<double>(3);
	EXPECT_NEAR(4.0, c_m(), 5.0e-5);

	foot_t<double> e_ft = b_ft + meter_t<double>(3);
	EXPECT_NEAR(13.12336, e_ft(), 5.0e-6);
	e_ft = g + unit<meters, int>(3);
	EXPECT_NEAR(13.12336, e_ft(), 5.0e-6);
	e_ft = b_ft + unit<meters, int>(3);
	EXPECT_NEAR(13.12336, e_ft(), 5.0e-6);
	e_ft = g + meter_t<double>(3);
	EXPECT_NEAR(13.12336, e_ft(), 5.0e-6);

	// dimensionless
	dimensionless<double> sresult = dimensionless<double>(1.0) + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, sresult, 5.0e-6);
	sresult = unit<dimensionless_unit, int>(1) + unit<dimensionless_unit, int>(1);
	EXPECT_NEAR(2.0, sresult, 5.0e-6);
	sresult = dimensionless<double>(1.0) + unit<dimensionless_unit, int>(1);
	EXPECT_NEAR(2.0, sresult, 5.0e-6);
	sresult = unit<dimensionless_unit, int>(1) + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, sresult, 5.0e-6);

	sresult = dimensionless<double>(1.0) + 1.0;
	EXPECT_NEAR(2.0, sresult, 5.0e-6);
	sresult = unit<dimensionless_unit, int>(1) + 1;
	EXPECT_NEAR(2.0, sresult, 5.0e-6);
	sresult = dimensionless<double>(1.0) + 1;
	EXPECT_NEAR(2.0, sresult, 5.0e-6);
	sresult = unit<dimensionless_unit, int>(1) + 1.0;
	EXPECT_NEAR(2.0, sresult, 5.0e-6);

	sresult = 1.0 + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, sresult, 5.0e-6);
	sresult = 1 + unit<dimensionless_unit, int>(1);
	EXPECT_NEAR(2.0, sresult, 5.0e-6);
	sresult = 1.0 + unit<dimensionless_unit, int>(1);
	EXPECT_NEAR(2.0, sresult, 5.0e-6);
	sresult = 1 + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, sresult, 5.0e-6);

	d = dimensionless<double>(1.0) + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, d, 5.0e-6);
	d = unit<dimensionless_unit, int>(1) + unit<dimensionless_unit, int>(1);
	EXPECT_NEAR(2.0, d, 5.0e-6);
	d = dimensionless<double>(1.0) + unit<dimensionless_unit, int>(1);
	EXPECT_NEAR(2.0, d, 5.0e-6);
	d = unit<dimensionless_unit, int>(1) + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, d, 5.0e-6);

	d = dimensionless<double>(1.0) + 1.0;
	EXPECT_NEAR(2.0, d, 5.0e-6);
	d = unit<dimensionless_unit, int>(1) + 1;
	EXPECT_NEAR(2.0, d, 5.0e-6);
	d = dimensionless<double>(1.0) + 1;
	EXPECT_NEAR(2.0, d, 5.0e-6);
	d = unit<dimensionless_unit, int>(1) + 1.0;
	EXPECT_NEAR(2.0, d, 5.0e-6);

	d = 1.0 + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, d, 5.0e-6);
	d = 1. + unit<dimensionless_unit, int>(1);
	EXPECT_NEAR(2.0, d, 5.0e-6);
	d = 1.0 + unit<dimensionless_unit, int>(1);
	EXPECT_NEAR(2.0, d, 5.0e-6);
	d = 1 + dimensionless<double>(1.0);
	EXPECT_NEAR(2.0, d, 5.0e-6);
}

TEST_F(UnitType, unitTypeUnaryAddition)
{
	meter_t<double> a_m(1.0);

	EXPECT_EQ(++a_m, meter_t<double>(2));
	EXPECT_EQ(a_m++, meter_t<double>(2));
	EXPECT_EQ(a_m, meter_t<double>(3));
	EXPECT_EQ(+a_m, meter_t<double>(3));
	EXPECT_EQ(a_m, meter_t<double>(3));

	dBW_t<double> b_dBW(1.0);

	EXPECT_EQ(++b_dBW, dBW_t<double>(2));
	EXPECT_EQ(b_dBW++, dBW_t<double>(2));
	EXPECT_EQ(b_dBW, dBW_t<double>(3));
	EXPECT_EQ(+b_dBW, dBW_t<double>(3));
	EXPECT_EQ(b_dBW, dBW_t<double>(3));
}

TEST_F(UnitType, unitTypeSubtraction)
{
	meter_t<double> a_m(1.0), c_m;
	foot_t<double> b_ft(3.28084);
	const unit<meters, int> f_m(1);
	const std::common_type_t<unit<meters, int>, unit<feet, int>> g(f_m);

	c_m = a_m - b_ft;
	EXPECT_NEAR(0.0, c_m(), 5.0e-5);
	c_m = f_m - g;
	EXPECT_NEAR(0.0, c_m(), 5.0e-5);
	c_m = a_m - g;
	EXPECT_NEAR(0.0, c_m(), 5.0e-5);
	c_m = f_m - b_ft;
	EXPECT_NEAR(0.0, c_m(), 5.0e-5);

	c_m = b_ft - meter_t<double>(1);
	EXPECT_NEAR(0.0, c_m(), 5.0e-5);
	c_m = g - unit<meters, int>(1);
	EXPECT_NEAR(0.0, c_m(), 5.0e-5);
	c_m = b_ft - unit<meters, int>(1);
	EXPECT_NEAR(0.0, c_m(), 5.0e-5);
	c_m = g - meter_t<double>(1);
	EXPECT_NEAR(0.0, c_m(), 5.0e-5);

	foot_t<double> e_ft = b_ft - meter_t<double>(1);
	EXPECT_NEAR(0.0, e_ft(), 5.0e-6);
	e_ft = g - unit<meters, int>(1);
	EXPECT_NEAR(0.0, e_ft(), 5.0e-6);
	e_ft = b_ft - unit<meters, int>(1);
	EXPECT_NEAR(0.0, e_ft(), 5.0e-6);
	e_ft = g - meter_t<double>(1);
	EXPECT_NEAR(0.0, e_ft(), 5.0e-6);

	dimensionless<double> sresult = dimensionless<double>(1.0) - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, sresult, 5.0e-6);
	sresult = unit<dimensionless_unit, int>(1) - unit<dimensionless_unit, int>(1);
	EXPECT_NEAR(0.0, sresult, 5.0e-6);
	sresult = dimensionless<double>(1.0) - unit<dimensionless_unit, int>(1);
	EXPECT_NEAR(0.0, sresult, 5.0e-6);
	sresult = unit<dimensionless_unit, int>(1) - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, sresult, 5.0e-6);

	sresult = dimensionless<double>(1.0) - 1.0;
	EXPECT_NEAR(0.0, sresult, 5.0e-6);
	sresult = unit<dimensionless_unit, int>(1) - 1;
	EXPECT_NEAR(0.0, sresult, 5.0e-6);
	sresult = dimensionless<double>(1.0) - 1;
	EXPECT_NEAR(0.0, sresult, 5.0e-6);
	sresult = unit<dimensionless_unit, int>(1) - 1.0;
	EXPECT_NEAR(0.0, sresult, 5.0e-6);

	sresult = 1.0 - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, sresult, 5.0e-6);
	sresult = 1 - unit<dimensionless_unit, int>(1);
	EXPECT_NEAR(0.0, sresult, 5.0e-6);
	sresult = 1.0 - unit<dimensionless_unit, int>(1);
	EXPECT_NEAR(0.0, sresult, 5.0e-6);
	sresult = 1 - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, sresult, 5.0e-6);

	double d = dimensionless<double>(1.0) - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, d, 5.0e-6);
	d = unit<dimensionless_unit, int>(1) - unit<dimensionless_unit, int>(1);
	EXPECT_NEAR(0.0, d, 5.0e-6);
	d = dimensionless<double>(1.0) - unit<dimensionless_unit, int>(1);
	EXPECT_NEAR(0.0, d, 5.0e-6);
	d = unit<dimensionless_unit, int>(1) - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, d, 5.0e-6);

	d = dimensionless<double>(1.0) - 1.0;
	EXPECT_NEAR(0.0, d, 5.0e-6);
	d = unit<dimensionless_unit, int>(1) - 1;
	EXPECT_NEAR(0.0, d, 5.0e-6);
	d = dimensionless<double>(1.0) - 1;
	EXPECT_NEAR(0.0, d, 5.0e-6);
	d = unit<dimensionless_unit, int>(1) - 1.0;
	EXPECT_NEAR(0.0, d, 5.0e-6);

	d = 1.0 - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, d, 5.0e-6);
	d = 1 - unit<dimensionless_unit, int>(1);
	EXPECT_NEAR(0.0, d, 5.0e-6);
	d = 1.0 - unit<dimensionless_unit, int>(1);
	EXPECT_NEAR(0.0, d, 5.0e-6);
	d = 1 - dimensionless<double>(1.0);
	EXPECT_NEAR(0.0, d, 5.0e-6);
}

TEST_F(UnitType, unitTypeUnarySubtraction)
{
	meter_t<double> a_m(4.0);

	EXPECT_EQ(--a_m, meter_t<double>(3));
	EXPECT_EQ(a_m--, meter_t<double>(3));
	EXPECT_EQ(a_m, meter_t<double>(2));
	EXPECT_EQ(-a_m, meter_t<double>(-2));
	EXPECT_EQ(a_m, meter_t<double>(2));

	dBW_t<double> b_dBW(4.0);

	EXPECT_EQ(--b_dBW, dBW_t<double>(3));
	EXPECT_EQ(b_dBW--, dBW_t<double>(3));
	EXPECT_EQ(b_dBW, dBW_t<double>(2));
	EXPECT_EQ(-b_dBW, dBW_t<double>(-2));
	EXPECT_EQ(b_dBW, dBW_t<double>(2));
}

TEST_F(UnitType, unitTypeMultiplication)
{
	meter_t<double> a_m(1.0), b_m(2.0);
	foot_t<double> a_ft(3.28084);
	const unit<meters, int> d_m(1), e_m(2);
	const std::common_type_t<unit<meters, int>, unit<feet, int>> f(d_m);

	auto c_m2 = a_m * b_m;
	EXPECT_NEAR(2.0, c_m2(), 5.0e-5);
	c_m2 = d_m * e_m;
	EXPECT_NEAR(2.0, c_m2(), 5.0e-5);
	c_m2 = a_m * e_m;
	EXPECT_NEAR(2.0, c_m2(), 5.0e-5);
	c_m2 = d_m * b_m;
	EXPECT_NEAR(2.0, c_m2(), 5.0e-5);

	c_m2 = b_m * meter_t<double>(2);
	EXPECT_NEAR(4.0, c_m2(), 5.0e-5);
	c_m2 = e_m * unit<meters, int>(2);
	EXPECT_NEAR(4.0, c_m2(), 5.0e-5);
	c_m2 = b_m * unit<meters, int>(2);
	EXPECT_NEAR(4.0, c_m2(), 5.0e-5);
	c_m2 = e_m * meter_t<double>(2);
	EXPECT_NEAR(4.0, c_m2(), 5.0e-5);

	c_m2 = b_m * a_ft;
	EXPECT_NEAR(2.0, c_m2(), 5.0e-5);
	c_m2 = e_m * f;
	EXPECT_NEAR(2.0, c_m2(), 5.0e-5);
	c_m2 = b_m * f;
	EXPECT_NEAR(2.0, c_m2(), 5.0e-5);
	c_m2 = e_m * a_ft;
	EXPECT_NEAR(2.0, c_m2(), 5.0e-5);

	auto c_m = b_m * 2.0;
	EXPECT_NEAR(4.0, c_m(), 5.0e-5);
	c_m = e_m * 2;
	EXPECT_NEAR(4.0, c_m(), 5.0e-5);
	c_m = b_m * 2;
	EXPECT_NEAR(4.0, c_m(), 5.0e-5);
	c_m = e_m * 2.0;
	EXPECT_NEAR(4.0, c_m(), 5.0e-5);

	c_m = 2.0 * b_m;
	EXPECT_NEAR(4.0, c_m(), 5.0e-5);
	c_m = 2 * e_m;
	EXPECT_NEAR(4.0, c_m(), 5.0e-5);
	c_m = 2.0 * e_m;
	EXPECT_NEAR(4.0, c_m(), 5.0e-5);
	c_m = 2 * b_m;
	EXPECT_NEAR(4.0, c_m(), 5.0e-5);

	double convert = dimensionless<double>(3.14);
	EXPECT_NEAR(3.14, convert, 5.0e-5);
	convert = unit<dimensionless_unit, int>(3);
	EXPECT_NEAR(3, convert, 5.0e-5);

	dimensionless<double> sresult = dimensionless<double>(5.0) * dimensionless<double>(4.0);
	EXPECT_NEAR(20.0, sresult(), 5.0e-5);
	sresult = unit<dimensionless_unit, int>(5) * unit<dimensionless_unit, int>(4);
	EXPECT_NEAR(20.0, sresult(), 5.0e-5);
	sresult = dimensionless<double>(5.0) * unit<dimensionless_unit, int>(4);
	EXPECT_NEAR(20.0, sresult(), 5.0e-5);
	sresult = unit<dimensionless_unit, int>(5) * dimensionless<double>(4.0);
	EXPECT_NEAR(20.0, sresult(), 5.0e-5);

	sresult = dimensionless<double>(5.0) * 4.0;
	EXPECT_NEAR(20.0, sresult(), 5.0e-5);
	sresult = unit<dimensionless_unit, int>(5) * 4;
	EXPECT_NEAR(20.0, sresult(), 5.0e-5);
	sresult = dimensionless<double>(5.0) * 4;
	EXPECT_NEAR(20.0, sresult(), 5.0e-5);
	sresult = unit<dimensionless_unit, int>(5) * 4.0;
	EXPECT_NEAR(20.0, sresult(), 5.0e-5);

	sresult = 4.0 * dimensionless<double>(5.0);
	EXPECT_NEAR(20.0, sresult(), 5.0e-5);
	sresult = 4 * unit<dimensionless_unit, int>(5);
	EXPECT_NEAR(20.0, sresult(), 5.0e-5);
	sresult = 4.0 * unit<dimensionless_unit, int>(5);
	EXPECT_NEAR(20.0, sresult(), 5.0e-5);
	sresult = 4 * dimensionless<double>(5.0);
	EXPECT_NEAR(20.0, sresult(), 5.0e-5);

	double result = dimensionless<double>(5.0) * dimensionless<double>(4.0);
	EXPECT_NEAR(20.0, result, 5.0e-5);
	result = unit<dimensionless_unit, int>(5) * unit<dimensionless_unit, int>(4);
	EXPECT_NEAR(20.0, result, 5.0e-5);
	result = dimensionless<double>(5.0) * unit<dimensionless_unit, int>(4);
	EXPECT_NEAR(20.0, result, 5.0e-5);
	result = unit<dimensionless_unit, int>(5) * dimensionless<double>(4.0);
	EXPECT_NEAR(20.0, result, 5.0e-5);

	result = dimensionless<double>(5.0) * 4.0;
	EXPECT_NEAR(20.0, result, 5.0e-5);
	result = unit<dimensionless_unit, int>(5) * 4;
	EXPECT_NEAR(20.0, result, 5.0e-5);
	result = dimensionless<double>(5.0) * 4;
	EXPECT_NEAR(20.0, result, 5.0e-5);
	result = unit<dimensionless_unit, int>(5) * 4.0;
	EXPECT_NEAR(20.0, result, 5.0e-5);

	result = 4.0 * dimensionless<double>(5.0);
	EXPECT_NEAR(20.0, result, 5.0e-5);
	result = 4 * unit<dimensionless_unit, int>(5);
	EXPECT_NEAR(20.0, result, 5.0e-5);
	result = 4.0 * unit<dimensionless_unit, int>(5);
	EXPECT_NEAR(20.0, result, 5.0e-5);
	result = 4 * dimensionless<double>(5.0);
	EXPECT_NEAR(20.0, result, 5.0e-5);
}

TEST_F(UnitType, unitTypeMixedUnitMultiplication)
{
	meter_t<double> a_m(1.0);
	foot_t<double> b_ft(3.28084);
	unit<inverse<meter>> i_m(2.0);
	const unit<meters, int> b_m(1);
	const std::common_type_t<unit<meters, int>, unit<feet, int>> f(b_m);
	const unit<inverse<meter>, int> i_i_m(2);

	// resultant unit is square of the common type unit
	// you can get whatever (compatible) type you want if you ask explicitly
	unit<squared<meter>> c_m2 = a_m * b_ft;
	EXPECT_NEAR(1.0, c_m2(), 5.0e-5);
	c_m2 = b_m * f;
	EXPECT_NEAR(1.0, c_m2(), 5.0e-5);
	c_m2 = a_m * f;
	EXPECT_NEAR(1.0, c_m2(), 5.0e-5);
	c_m2 = b_m * b_ft;
	EXPECT_NEAR(1.0, c_m2(), 5.0e-5);

	unit<squared<foot>> c_ft2 = b_ft * a_m;
	EXPECT_NEAR(10.7639111056, c_ft2(), 5.0e-7);
	c_ft2 = f * b_m;
	EXPECT_NEAR(10.7639111056, c_ft2(), 5.0e-6);
	c_ft2 = b_ft * b_m;
	EXPECT_NEAR(10.7639111056, c_ft2(), 5.0e-7);
	c_ft2 = f * a_m;
	EXPECT_NEAR(10.7639111056, c_ft2(), 5.0e-6);

	square_meter_t<double> d_m2 = b_ft * a_m;
	EXPECT_NEAR(1.0, d_m2(), 5.0e-5);
	d_m2 = f * b_m;
	EXPECT_NEAR(1.0, d_m2(), 5.0e-5);
	d_m2 = b_ft * b_m;
	EXPECT_NEAR(1.0, d_m2(), 5.0e-5);
	d_m2 = f * a_m;
	EXPECT_NEAR(1.0, d_m2(), 5.0e-5);

	// a unit times a sclar ends up with the same units.
	meter_t<double> e_m = a_m * dimensionless<double>(3.0);
	EXPECT_NEAR(3.0, e_m(), 5.0e-5);
	e_m = b_m * unit<dimensionless_unit, int>(3);
	EXPECT_NEAR(3.0, e_m(), 5.0e-5);
	e_m = a_m * unit<dimensionless_unit, int>(3);
	EXPECT_NEAR(3.0, e_m(), 5.0e-5);
	e_m = b_m * dimensionless<double>(3.0);
	EXPECT_NEAR(3.0, e_m(), 5.0e-5);

	e_m = dimensionless<double>(4.0) * a_m;
	EXPECT_NEAR(4.0, e_m(), 5.0e-5);
	e_m = unit<dimensionless_unit, int>(4) * b_m;
	EXPECT_NEAR(4.0, e_m(), 5.0e-5);
	e_m = dimensionless<double>(4) * b_m;
	EXPECT_NEAR(4.0, e_m(), 5.0e-5);
	e_m = unit<dimensionless_unit, int>(4) * a_m;
	EXPECT_NEAR(4.0, e_m(), 5.0e-5);

	// unit times its inverse results in a dimensionless
	dimensionless<double> s = a_m * i_m;
	EXPECT_NEAR(2.0, s, 5.0e-5);
	s = b_m * i_i_m;
	EXPECT_NEAR(2.0, s, 5.0e-5);
	s = a_m * i_i_m;
	EXPECT_NEAR(2.0, s, 5.0e-5);
	s = b_m * i_m;
	EXPECT_NEAR(2.0, s, 5.0e-5);

	c_m2 = b_ft * meter_t<double>(2);
	EXPECT_NEAR(2.0, c_m2(), 5.0e-5);
	c_m2 = f * unit<meters, int>(2);
	EXPECT_NEAR(2.0, c_m2(), 5.0e-5);
	c_m2 = b_ft * unit<meters, int>(2);
	EXPECT_NEAR(2.0, c_m2(), 5.0e-5);
	c_m2 = f * meter_t<double>(2);
	EXPECT_NEAR(2.0, c_m2(), 5.0e-5);

	unit<squared<foot>> e_ft2 = b_ft * meter_t<double>(3);
	EXPECT_NEAR(32.2917333168, e_ft2(), 5.0e-6);
	e_ft2 = f * unit<meters, int>(3);
	EXPECT_NEAR(32.2917333168, e_ft2(), 5.0e-6);
	e_ft2 = b_ft * unit<meters, int>(3);
	EXPECT_NEAR(32.2917333168, e_ft2(), 5.0e-6);
	e_ft2 = f * meter_t<double>(3);
	EXPECT_NEAR(32.2917333168, e_ft2(), 5.0e-6);

	auto mps = meter_t<double>(10.0) * unit<inverse<seconds>>(1.0);
	EXPECT_EQ(mps, meters_per_second_t<double>(10));
	mps = unit<meters, int>(10) * unit<inverse<seconds>, int>(1);
	EXPECT_EQ(mps, meters_per_second_t<double>(10));
	mps = meter_t<double>(10.0) * unit<inverse<seconds>, int>(1);
	EXPECT_EQ(mps, meters_per_second_t<double>(10));
	mps = unit<meters, int>(10) * unit<inverse<seconds>>(1.0);
	EXPECT_EQ(mps, meters_per_second_t<double>(10));
}

TEST_F(UnitType, unitTypedimensionlessMultiplication)
{
	meter_t<double> a_m(1.0);

	auto result_m = dimensionless<double>(3.0) * a_m;
	EXPECT_NEAR(3.0, result_m(), 5.0e-5);

	result_m = a_m * dimensionless<double>(4.0);
	EXPECT_NEAR(4.0, result_m(), 5.0e-5);

	result_m = 3.0 * a_m;
	EXPECT_NEAR(3.0, result_m(), 5.0e-5);

	result_m = a_m * 4.0;
	EXPECT_NEAR(4.0, result_m(), 5.0e-5);

	bool isSame = std::is_same_v<decltype(result_m), meter_t<double>>;
	EXPECT_TRUE(isSame);
}

TEST_F(UnitType, unitTypeDivision)
{
	meter_t<double> a_m(1.0), b_m(2.0);
	foot_t<double> a_ft(3.28084);
	second_t<double> a_sec(10.0);
	const unit<meters, int> d_m(1), e_m(2);
	const std::common_type_t<unit<meters, int>, unit<feet, int>> j(d_m);
	const unit<seconds, int> b_sec(10);
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
	c = unit<dimensionless_unit, int>(1) / 2;
	EXPECT_EQ(0, c);
	c = dimensionless<double>(1.0) / 2;
	EXPECT_NEAR(0.5, c, 5.0e-5);
	c = unit<dimensionless_unit, int>(1) / 2.0;
	EXPECT_NEAR(0.5, c, 5.0e-5);
	isSame = std::is_same_v<decltype(c), dimensionless<double>>;
	EXPECT_TRUE(isSame);

	c = 1.0 / dimensionless<double>(2.0);
	EXPECT_NEAR(0.5, c, 5.0e-5);
	c = 1 / unit<dimensionless_unit, int>(2);
	EXPECT_EQ(0, c);
	c = 1.0 / unit<dimensionless_unit, int>(2);
	EXPECT_NEAR(0.5, c, 5.0e-5);
	c = 1 / dimensionless<double>(2.0);
	EXPECT_NEAR(0.5, c, 5.0e-5);
	isSame = std::is_same_v<decltype(c), dimensionless<double>>;
	EXPECT_TRUE(isSame);

	double d = dimensionless<double>(1.0) / 2.0;
	EXPECT_NEAR(0.5, d, 5.0e-5);

	auto e = a_m / a_sec;
	EXPECT_NEAR(0.1, e(), 5.0e-5);
	e = d_m / b_sec;
	EXPECT_EQ(0, e());
	e = a_m / b_sec;
	EXPECT_NEAR(0.1, e(), 5.0e-5);
	e = d_m / a_sec;
	EXPECT_NEAR(0.1, e(), 5.0e-5);
	isSame = std::is_same_v<decltype(e), meters_per_second_t<double>>;
	EXPECT_TRUE(isSame);

	auto f = a_m / 8.0;
	EXPECT_NEAR(0.125, f(), 5.0e-5);
	f = d_m / 8;
	EXPECT_EQ(0, f());
	f = a_m / 8;
	EXPECT_NEAR(0.125, f(), 5.0e-5);
	f = d_m / 8.0;
	EXPECT_NEAR(0.125, f(), 5.0e-5);
	isSame = std::is_same_v<decltype(f), meter_t<double>>;
	EXPECT_TRUE(isSame);

	auto g = 4.0 / b_m;
	EXPECT_NEAR(2.0, g(), 5.0e-5);
	g = 4 / e_m;
	EXPECT_NEAR(2.0, g(), 5.0e-5);
	g = 4.0 / e_m;
	EXPECT_NEAR(2.0, g(), 5.0e-5);
	g = 4 / b_m;
	EXPECT_NEAR(2.0, g(), 5.0e-5);
	isSame = std::is_same_v<decltype(g), unit<inverse<meters>>>;
	EXPECT_TRUE(isSame);

	auto mph                        = mile_t<double>(60.0) / hour_t<double>(1.0);
	meters_per_second_t<double> mps = mph;
	EXPECT_NEAR(26.8224, mps(), 5.0e-5);
	mps = unit<miles, int>(60) / unit<hours, int>(1);
	EXPECT_NEAR(26.8224, mps(), 5.0e-5);
	mps = mile_t<double>(60.0) / unit<hours, int>(1);
	EXPECT_NEAR(26.8224, mps(), 5.0e-5);
	mps = unit<miles, int>(60) / hour_t<double>(1.0);
	EXPECT_NEAR(26.8224, mps(), 5.0e-5);

	auto h = 10.0_rad / 2.0_rad;
	EXPECT_NEAR(5, h, 5.0e-5);
	h = unit<radians, int>(10) / unit<radians, int>(2);
	EXPECT_NEAR(5, h, 5.0e-5);
	h = 10.0_rad / unit<radians, int>(2);
	EXPECT_NEAR(5, h, 5.0e-5);
	h = unit<radians, int>(10) / 2.0_rad;
	EXPECT_NEAR(5, h, 5.0e-5);
	isSame = std::is_same_v<decltype(h), dimensionless<double>>;
	EXPECT_TRUE(isSame);

	auto i = (3.0_N * 2.0_m) / 6.0_J;
	EXPECT_NEAR(1, i, 5.0e-5);
	i = (unit<force::newtons, int>(3) * unit<meters, int>(2)) / unit<joules, int>(6);
	EXPECT_NEAR(1, i, 5.0e-5);
	i = (3.0_N * unit<meters, int>(2)) / unit<joules, int>(6);
	EXPECT_NEAR(1, i, 5.0e-5);
	i = (unit<force::newtons, int>(3) * unit<meters, int>(2)) / 6.0_J;
	EXPECT_NEAR(1, i, 5.0e-5);
	isSame = std::is_same_v<decltype(i), dimensionless<double>>;
	EXPECT_TRUE(isSame);
}

TEST_F(UnitType, unitTypeModulo)
{
	const unit<meters, int> a_m(2200);
	const unit<meters, int> b_m(1800);
	const unit<kilometers, int> a_km(2);

	const auto c_m = a_m % b_m;
	EXPECT_EQ(400, c_m());
	static_assert(has_equivalent_conversion_factor(c_m, a_m));

	const auto d_m = a_m % a_km;
	EXPECT_EQ(200, d_m());
	static_assert(has_equivalent_conversion_factor(d_m, a_m));

	const auto b_km = a_km % unit<dimensionless_unit, int>(3);
	EXPECT_EQ(2, b_km());
	static_assert(has_equivalent_conversion_factor(b_km, a_km));

	const auto e_m = a_m % 2000;
	EXPECT_EQ(200, e_m());
	static_assert(has_equivalent_conversion_factor(e_m, a_m));

	const unit<dimensionless_unit, int> a_s(12);
	const unit<dimensionless_unit, int> b_s(5);

	const auto c_s = a_s % b_s;
	EXPECT_EQ(2, c_s());
	static_assert(has_equivalent_conversion_factor(c_s, a_s));

	const auto d_s = a_s % 20;
	EXPECT_EQ(12, d_s());
	static_assert(has_equivalent_conversion_factor(d_s, a_s));
}

TEST_F(UnitType, compoundAssignmentAddition)
{
	// units
	meter_t<double> a(0.0);
	a += meter_t<double>(1.0);

	EXPECT_EQ(meter_t<double>(1.0), a);

	a += foot_t<double>(meter_t<double>(1));

	EXPECT_EQ(meter_t<double>(2.0), a);

	a += unit<meters, int>(1);

	EXPECT_EQ(meter_t<double>(3.0), a);

	a += std::common_type_t<unit<meters, int>, unit<feet, int>>(unit<meters, int>(1));

	EXPECT_EQ(meter_t<double>(4.0), a);

	unit<meters, int> c(0);
	c += unit<meters, int>(1);

	EXPECT_EQ((unit<meters, int>(1)), c);

	c += unit<kilometers, int>(1);

	EXPECT_EQ((unit<meters, int>(1001)), c);

	// dimensionlesss
	dimensionless<double> b(0);
	b += dimensionless<double>(1.0);

	EXPECT_EQ(dimensionless<double>(1.0), b);

	b += 1.0;

	EXPECT_EQ(dimensionless<double>(2.0), b);

	b += unit<dimensionless_unit, int>(1);

	EXPECT_EQ(dimensionless<double>(3.0), b);

	b += 1;

	EXPECT_EQ(dimensionless<double>(4.0), b);

	unit<dimensionless_unit, int> d(0);
	d += unit<dimensionless_unit, int>(1);

	EXPECT_EQ((unit<dimensionless_unit, int>(1)), d);

	d += 1;

	EXPECT_EQ((unit<dimensionless_unit, int>(2)), d);
}

TEST_F(UnitType, compoundAssignmentSubtraction)
{
	// units
	meter_t<double> a(2.0);
	a -= meter_t<double>(1.0);

	EXPECT_EQ(meter_t<double>(1.0), a);

	a -= foot_t<double>(meter_t<double>(1));

	EXPECT_EQ(meter_t<double>(0.0), a);

	a -= unit<meters, int>(1);

	EXPECT_EQ(meter_t<double>(-1.0), a);

	a -= std::common_type_t<unit<meters, int>, unit<feet, int>>(unit<meters, int>(1));

	EXPECT_EQ(meter_t<double>(-2.0), a);

	unit<meters, int> c(1);
	c -= unit<meters, int>(1);

	EXPECT_EQ((unit<meters, int>(0)), c);

	c -= unit<kilometers, int>(1);

	EXPECT_EQ((unit<meters, int>(-1000)), c);

	// dimensionlesss
	dimensionless<double> b(2);
	b -= dimensionless<double>(1.0);

	EXPECT_EQ(dimensionless<double>(1.0), b);

	b -= 1.0;

	EXPECT_EQ(dimensionless<double>(0), b);

	b -= unit<dimensionless_unit, int>(1);

	EXPECT_EQ(dimensionless<double>(-1.0), b);

	b -= 1;

	EXPECT_EQ(dimensionless<double>(-2.0), b);

	unit<dimensionless_unit, int> d(2);
	d -= unit<dimensionless_unit, int>(1);

	EXPECT_EQ((unit<dimensionless_unit, int>(1)), d);

	d -= 1;

	EXPECT_EQ((unit<dimensionless_unit, int>(0)), d);
}

TEST_F(UnitType, compoundAssignmentMultiplication)
{
	// units
	meter_t<double> a(2.0);
	a *= dimensionless<double>(2.0);

	EXPECT_EQ(meter_t<double>(4.0), a);

	a *= 2.0;

	EXPECT_EQ(meter_t<double>(8.0), a);

	a *= unit<dimensionless_unit, int>(2);

	EXPECT_EQ(meter_t<double>(16), a);

	a *= 2;

	EXPECT_EQ(meter_t<double>(32), a);

	unit<meters, int> c(2);
	c *= unit<dimensionless_unit, int>(2);

	EXPECT_EQ((unit<meters, int>(4)), c);

	c *= dimensionless<double>(2.0);

	EXPECT_EQ((unit<meters, int>(8)), c);

	c *= 2;

	EXPECT_EQ((unit<meters, int>(16)), c);

	c *= 2.0;

	EXPECT_EQ((unit<meters, int>(32)), c);

	// dimensionlesss
	dimensionless<double> b(2);
	b *= dimensionless<double>(2.0);

	EXPECT_EQ(dimensionless<double>(4.0), b);

	b *= 2.0;

	EXPECT_EQ(dimensionless<double>(8.0), b);

	b *= unit<dimensionless_unit, int>(2);

	EXPECT_EQ(dimensionless<double>(16.0), b);

	b *= 2;

	EXPECT_EQ(dimensionless<double>(32.0), b);

	unit<dimensionless_unit, int> d(2);
	d *= unit<dimensionless_unit, int>(2);

	EXPECT_EQ((unit<dimensionless_unit, int>(4)), d);

	d *= dimensionless<double>(2.0);

	EXPECT_EQ((unit<dimensionless_unit, int>(8)), d);

	d *= 2;

	EXPECT_EQ((unit<dimensionless_unit, int>(16)), d);

	d *= 2.0;

	EXPECT_EQ((unit<dimensionless_unit, int>(32)), d);
}

TEST_F(UnitType, compoundAssignmentDivision)
{
	// units
	meter_t<double> a(8.0);
	a /= dimensionless<double>(2.0);

	EXPECT_EQ(meter_t<double>(4.0), a);

	a /= 2.0;

	EXPECT_EQ(meter_t<double>(2.0), a);

	a /= unit<dimensionless_unit, int>(2);

	EXPECT_EQ(meter_t<double>(1), a);

	a /= 2;

	EXPECT_EQ(meter_t<double>(0.5), a);

	unit<meters, int> c(32);
	c /= unit<dimensionless_unit, int>(2);

	EXPECT_EQ((unit<meters, int>(16)), c);

	c /= dimensionless<double>(2.0);

	EXPECT_EQ((unit<meters, int>(8)), c);

	c /= 2;

	EXPECT_EQ((unit<meters, int>(4)), c);

	c /= 2.0;

	EXPECT_EQ((unit<meters, int>(2)), c);

	// dimensionlesss
	dimensionless<double> b(8);
	b /= dimensionless<double>(2.0);

	EXPECT_EQ(dimensionless<double>(4.0), b);

	b /= 2.0;

	EXPECT_EQ(dimensionless<double>(2.0), b);

	b /= unit<dimensionless_unit, int>(2);

	EXPECT_EQ(dimensionless<double>(1.0), b);

	b /= 2;

	EXPECT_EQ(dimensionless<double>(0.5), b);

	unit<dimensionless_unit, int> d(32);
	d /= unit<dimensionless_unit, int>(2);

	EXPECT_EQ((unit<dimensionless_unit, int>(16)), d);

	d /= dimensionless<double>(2.0);

	EXPECT_EQ((unit<dimensionless_unit, int>(8)), d);

	d /= 2;

	EXPECT_EQ((unit<dimensionless_unit, int>(4)), d);

	d /= 2.0;

	EXPECT_EQ((unit<dimensionless_unit, int>(2)), d);
}

TEST_F(UnitType, compoundAssignmentModulo)
{
	// units
	unit<meters, int> a_m(2200);

	a_m %= unit<meters, int>(2000);
	EXPECT_EQ(200, a_m());

	a_m %= unit<kilometers, int>(1);
	EXPECT_EQ(200, a_m());

	a_m %= unit<dimensionless_unit, int>(180);
	EXPECT_EQ(20, a_m());

	a_m %= dimensionless<double>(15.0);
	EXPECT_EQ(5, a_m());

	a_m %= 6;
	EXPECT_EQ(5, a_m());

	a_m %= 3.0;
	EXPECT_EQ(2, a_m());

	// dimensionless
	unit<dimensionless_unit, int> a_s(12);

	a_s %= unit<dimensionless_unit, int>(20);
	EXPECT_EQ(12, a_s());

	a_s %= dimensionless<double>(7.0);
	EXPECT_EQ(5, a_s());

	a_s %= 3;
	EXPECT_EQ(2, a_s());

	a_s %= 3.0;
	EXPECT_EQ(2, a_s());
}

TEST_F(UnitType, dimensionlessTypeImplicitConversion)
{
	double test = dimensionless<double>(3.0);
	EXPECT_DOUBLE_EQ(3.0, test);

	dimensionless<double> testS = 3.0;
	EXPECT_DOUBLE_EQ(3.0, testS);

	dimensionless<double> test3(ppm_t<double>(10));
	EXPECT_DOUBLE_EQ(0.00001, test3);

	dimensionless<double> test4;
	test4 = ppm_t<double>(1);
	EXPECT_DOUBLE_EQ(0.000001, test4);
}

TEST_F(UnitType, valueMethod)
{
	double test = meter_t<double>(3.0).to<double>();
	EXPECT_DOUBLE_EQ(3.0, test);

	auto test2 = meter_t<double>(4.0).value();
	EXPECT_DOUBLE_EQ(4.0, test2);
	EXPECT_TRUE((std::is_same_v<decltype(test2), double>));
}

TEST_F(UnitType, convertMethod)
{
	double test = meter_t<double>(3.0).convert<feet>().to<double>();
	EXPECT_NEAR(9.84252, test, 5.0e-6);
}

#ifndef UNIT_LIB_DISABLE_IOSTREAM
TEST_F(UnitType, cout)
{
	testing::internal::CaptureStdout();
	std::cout << meters_per_second_t<double>(5);
	std::string output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("5 mps", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << degree_t<double>(349.87);
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("349.87 deg", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << meter_t<double>(1.0);
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("1 m", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << dB_t<double>(31.0);
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("31 dB", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << volt_t<double>(21.79);
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("21.79 V", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << dBW_t<double>(12.0);
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("12 dBW", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << dBm_t<double>(120.0);
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("120 dBm", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << miles_per_hour_t<double>(72.1);
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("72.1 mph", output.c_str());

	// undefined unit
	testing::internal::CaptureStdout();
	std::cout << pow<4>(meter_t<double>(2));
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("16 m^4", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << pow<3>(foot_t<double>(2));
	output = testing::internal::GetCapturedStdout();
	EXPECT_STREQ("8 cu_ft", output.c_str());

	testing::internal::CaptureStdout();
	std::cout << std::setprecision(9) << pow<4>(foot_t<double>(2));
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
	foot_t<double> a(3.5);
	EXPECT_STREQ("3.5 ft", units::length::to_string(a).c_str());

	meter_t<double> b(8);
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

	kilometer_t<double> de = 2.0_km;
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

	mile_t<double> us = 2.0_mi;
	EXPECT_STREQ("2 mi", units::length::to_string(us).c_str());

	us = 2.5_mi;
	EXPECT_STREQ("2.5 mi", units::length::to_string(us).c_str());
}

TEST_F(UnitType, nameAndAbbreviation)
{
	foot_t<double> a(3.5);
	EXPECT_STREQ("ft", unit_abbreviation_v<decltype(a)>);
	EXPECT_STREQ("ft", a.abbreviation());
	EXPECT_STREQ("foot", a.name());

	meter_t<double> b(8);
	EXPECT_STREQ("m", unit_abbreviation_v<decltype(b)>);
	EXPECT_STREQ("m", b.abbreviation());
	EXPECT_STREQ("meter", b.name());
}
#endif

TEST_F(UnitType, negative)
{
	meter_t<double> a(5.3);
	meter_t<double> b(-5.3);
	EXPECT_NEAR(a.to<double>(), -b.to<double>(), 5.0e-320);
	EXPECT_NEAR(b.to<double>(), -a.to<double>(), 5.0e-320);

	dB_t<double> c(2.87);
	dB_t<double> d(-2.87);
	EXPECT_NEAR(c.to<double>(), -d.to<double>(), 5.0e-320);
	EXPECT_NEAR(d.to<double>(), -c.to<double>(), 5.0e-320);

	ppm_t<double> e = -1 * ppm_t<double>(10);
	EXPECT_EQ(e, -ppm_t<double>(10));
	EXPECT_NEAR(-0.00001, e, 5.0e-10);
}

TEST_F(UnitType, concentration)
{
	ppb_t<double> a(ppm_t<double>(1));
	EXPECT_EQ(ppb_t<double>(1000), a);
	EXPECT_EQ(0.000001, a);
	EXPECT_EQ(0.000001, a.to<double>());

	dimensionless<double> b(ppm_t<double>(1));
	EXPECT_EQ(0.000001, b);

	dimensionless<double> c = ppb_t<double>(1);
	EXPECT_EQ(0.000000001, c);
}

TEST_F(UnitType, dBConversion)
{
	dBW_t<double> a_dbw(23.1);
	watt_t<double> a_w  = a_dbw;
	dBm_t<double> a_dbm = a_dbw;

	EXPECT_NEAR(204.173794, a_w(), 5.0e-7);
	EXPECT_NEAR(53.1, a_dbm(), 5.0e-7);

	milliwatt_t<double> b_mw(100000.0);
	watt_t<double> b_w  = b_mw;
	dBm_t<double> b_dbm = b_mw;
	dBW_t<double> b_dbw = b_mw;

	EXPECT_NEAR(100.0, b_w(), 5.0e-7);
	EXPECT_NEAR(50.0, b_dbm(), 5.0e-7);
	EXPECT_NEAR(20.0, b_dbw(), 5.0e-7);
}

TEST_F(UnitType, dBAddition)
{
	bool isSame;

	auto result_dbw = dBW_t<double>(10.0) + dB_t<double>(30.0);
	EXPECT_NEAR(40.0, result_dbw(), 5.0e-5);
	result_dbw = unit<watt, int, decibel_scale>(10) + unit<dimensionless_unit, int, decibel_scale>(30);
	EXPECT_NEAR(40.0, result_dbw(), 5.0e-5);
	result_dbw = dB_t<double>(12.0) + dBW_t<double>(30.0);
	EXPECT_NEAR(42.0, result_dbw(), 5.0e-5);
	result_dbw = unit<dimensionless_unit, int, decibel_scale>(12) + unit<watt, int, decibel_scale>(30);
	EXPECT_NEAR(42.0, result_dbw(), 2);
	isSame = std::is_same_v<decltype(result_dbw), dBW_t<double>>;
	EXPECT_TRUE(isSame);

	auto result_dbm = dB_t<double>(30.0) + dBm_t<double>(20.0);
	EXPECT_NEAR(50.0, result_dbm(), 5.0e-5);
	result_dbm = unit<dimensionless_unit, int, decibel_scale>(30) + unit<milliwatt, int, decibel_scale>(20);
	EXPECT_NEAR(50.0, result_dbm(), 5.0e-5);

	// adding dBW to dBW is something you probably shouldn't do, but let's see if it works...
	auto result_dBW2 = dBW_t<double>(10.0) + dBm_t<double>(40.0);
	EXPECT_NEAR(80.0, result_dBW2(), 5.0e-5);
	result_dBW2 = unit<watt, int, decibel_scale>(10) + unit<milliwatt, int, decibel_scale>(40);
	EXPECT_NEAR(80.0, result_dBW2(), 5.0e-5);
	isSame = std::is_same_v<decltype(result_dBW2), unit<squared<milliwatts>, double, decibel_scale>>;
	EXPECT_TRUE(isSame);
}

TEST_F(UnitType, dBSubtraction)
{
	bool isSame;

	auto result_dbw = dBW_t<double>(10.0) - dB_t<double>(30.0);
	EXPECT_NEAR(-20.0, result_dbw(), 5.0e-5);
	result_dbw = unit<watt, int, decibel_scale>(10) - unit<dimensionless_unit, int, decibel_scale>(30);
	EXPECT_EQ(-INFINITY, result_dbw());
	isSame = std::is_same_v<decltype(result_dbw), dBW_t<double>>;
	EXPECT_TRUE(isSame);

	auto result_dbm = dBm_t<double>(100.0) - dB_t<double>(30.0);
	EXPECT_NEAR(70.0, result_dbm(), 5.0e-5);
	result_dbm = unit<milliwatt, int, decibel_scale>(100) - unit<dimensionless_unit, int, decibel_scale>(30); // NaN
	//	EXPECT_NEAR(70.0, result_dbm(), 5.0e-5);
	isSame = std::is_same_v<decltype(result_dbm), dBm_t<double>>;
	EXPECT_TRUE(isSame);

	auto result_db = dBW_t<double>(100.0) - dBW_t<double>(80.0);
	EXPECT_NEAR(20.0, result_db(), 5.0e-5);
	result_db = unit<watt, int, decibel_scale>(100) - unit<watt, int, decibel_scale>(80); // NaN
	//	EXPECT_NEAR(20.0, result_db(), 5.0e-5);
	isSame = std::is_same_v<decltype(result_db), dB_t<double>>;
	EXPECT_TRUE(isSame);

	result_db = dB_t<double>(100.0) - dB_t<double>(80.0);
	EXPECT_NEAR(20.0, result_db(), 5.0e-5);
	result_db =
		unit<dimensionless_unit, int, decibel_scale>(100) - unit<dimensionless_unit, int, decibel_scale>(80); // NaN
	//	EXPECT_NEAR(20.0, result_db(), 5.0e-5);
	isSame = std::is_same_v<decltype(result_db), dB_t<double>>;
	EXPECT_TRUE(isSame);
}

TEST_F(UnitType, unit_cast)
{
	meter_t<double> test1(5.7);
	hectare_t<double> test2(16);

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
	EXPECT_TRUE((std::is_same_v<decltype(16.2_m), meter_t<double>>));
	EXPECT_TRUE((std::is_same_v<decltype(16_m), meter_t<int>>));
	EXPECT_TRUE(meter_t<double>(16.2) == 16.2_m);
	EXPECT_TRUE(meter_t<double>(16) == 16.0_m);
	EXPECT_TRUE(meter_t<int>(16) == 16_m);

	EXPECT_TRUE((std::is_same_v<decltype(11.2_ft), foot_t<double>>));
	EXPECT_TRUE((std::is_same_v<decltype(11_ft), foot_t<int>>));
	EXPECT_TRUE(foot_t<double>(11.2) == 11.2_ft);
	EXPECT_TRUE(foot_t<double>(11) == 11.0_ft);
	EXPECT_TRUE(foot_t<int>(11) == 11_ft);

	// auto using literal syntax
	auto x = 10.0_m;
	EXPECT_TRUE((std::is_same_v<decltype(x), meter_t<double>>));
	EXPECT_TRUE(meter_t<double>(10) == x);

	// conversion using literal syntax
	foot_t<double> y = 0.3048_m;
	EXPECT_TRUE(1.0_ft == y);

	// Pythagorean theorem
	meter_t<double> a = 3.0_m;
	meter_t<double> b = 4.0_m;
	meter_t<double> c = sqrt(pow<2>(a) + pow<2>(b));
	EXPECT_TRUE(c == 5.0_m);
}

TEST_F(ConversionFactor, length)
{
	double test;
	test = nanometer_t<double>(0.000000001_m)();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = micrometer_t<double>(meter_t<double>(0.000001))();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = millimeter_t<double>(meter_t<double>(0.001))();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = centimeter_t<double>(meter_t<double>(0.01))();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = kilometer_t<double>(meter_t<double>(1000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = meter_t<double>(meter_t<double>(1.0))();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = foot_t<double>(meter_t<double>(0.3048))();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = mile_t<double>(meter_t<double>(1609.344))();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = inch_t<double>(meter_t<double>(0.0254))();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = nautical_mile_t<double>(meter_t<double>(1852.0))();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = astronomical_unit_t<double>(meter_t<double>(149597870700.0))();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = lightyear_t<double>(meter_t<double>(9460730472580800.0))();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = parsec_t<double>(meter_t<double>(3.08567758e16))();
	EXPECT_NEAR(1.0, test, 5.0e7);

	test = foot_t<double>(foot_t<double>(6.3))();
	EXPECT_NEAR(6.3, test, 5.0e-5);
	test = inch_t<double>(foot_t<double>(6.0))();
	EXPECT_NEAR(72.0, test, 5.0e-5);
	test = foot_t<double>(inch_t<double>(6.0))();
	EXPECT_NEAR(0.5, test, 5.0e-5);
	test = foot_t<double>(meter_t<double>(1.0))();
	EXPECT_NEAR(3.28084, test, 5.0e-5);
	test = nautical_mile_t<double>(mile_t<double>(6.3))();
	EXPECT_NEAR(5.47455, test, 5.0e-6);
	test = meter_t<double>(mile_t<double>(11.0))();
	EXPECT_NEAR(17702.8, test, 5.0e-2);
	test = chain_t<double>(meter_t<double>(1.0))();
	EXPECT_NEAR(0.0497097, test, 5.0e-7);

	EXPECT_EQ(metre_t<double>(1), meter_t<double>(1));
}

TEST_F(ConversionFactor, mass)
{
	double test;

	test = gram_t<double>(kilogram_t<double>(1.0e-3))();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = microgram_t<double>(kilogram_t<double>(1.0e-9))();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = milligram_t<double>(kilogram_t<double>(1.0e-6))();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = kilogram_t<double>(kilogram_t<double>(1.0))();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = metric_ton_t<double>(kilogram_t<double>(1000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = mass::pound_t<double>(kilogram_t<double>(0.453592))();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = long_ton_t<double>(kilogram_t<double>(1016.05))();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = short_ton_t<double>(kilogram_t<double>(907.185))();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = mass::ounce_t<double>(kilogram_t<double>(0.0283495))();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = carat_t<double>(kilogram_t<double>(0.0002))();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = kilogram_t<double>(slug_t<double>(1.0))();
	EXPECT_NEAR(14.593903, test, 5.0e-7);

	test = carat_t<double>(mass::pound_t<double>(6.3))();
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

	year_t<double> twoYears(2.0);
	week_t<double> twoYearsInWeeks = twoYears;
	EXPECT_NEAR(week_t<double>(104.286).to<double>(), twoYearsInWeeks.to<double>(), 5.0e-4);

	double test;

	test = second_t<double>(second_t<double>(1.0))();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = nanosecond_t<double>(second_t<double>(1.0e-9))();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = microsecond_t<double>(second_t<double>(1.0e-6))();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = millisecond_t<double>(second_t<double>(1.0e-3))();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = minute_t<double>(second_t<double>(60.0))();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = hour_t<double>(second_t<double>(3600.0))();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = day_t<double>(second_t<double>(86400.0))();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = week_t<double>(second_t<double>(604800.0))();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = year_t<double>(second_t<double>(3.154e7))();
	EXPECT_NEAR(1.0, test, 5.0e3);

	test = week_t<double>(year_t<double>(2.0))();
	EXPECT_NEAR(104.2857142857143, test, 5.0e-14);
	test = minute_t<double>(hour_t<double>(4.0))();
	EXPECT_NEAR(240.0, test, 5.0e-14);
	test = day_t<double>(julian_year_t<double>(1.0))();
	EXPECT_NEAR(365.25, test, 5.0e-14);
	test = day_t<double>(gregorian_year_t<double>(1.0))();
	EXPECT_NEAR(365.2425, test, 5.0e-14);
}

TEST_F(ConversionFactor, angle)
{
	angle::degree_t<double> quarterCircleDeg(90.0);
	angle::radian_t<double> quarterCircleRad = quarterCircleDeg;
	EXPECT_NEAR(angle::radian_t<double>(detail::PI_VAL / 2.0).to<double>(), quarterCircleRad.to<double>(), 5.0e-12);

	double test;

	test = angle::radian_t<double>(angle::radian_t<double>(1.0))();
	EXPECT_NEAR(1.0, test, 5.0e-20);
	test = angle::milliradian_t<double>(angle::radian_t<double>(0.001))();
	EXPECT_NEAR(1.0, test, 5.0e-4);
	test = angle::degree_t<double>(angle::radian_t<double>(0.0174533))();
	EXPECT_NEAR(1.0, test, 5.0e-7);
	test = angle::arcminute_t<double>(angle::radian_t<double>(0.000290888))();
	EXPECT_NEAR(0.99999928265913, test, 5.0e-8);
	test = angle::arcsecond_t<double>(angle::radian_t<double>(4.8481e-6))();
	EXPECT_NEAR(0.999992407, test, 5.0e-10);
	test = angle::turn_t<double>(angle::radian_t<double>(6.28319))();
	EXPECT_NEAR(1.0, test, 5.0e-6);
	test = angle::gradian_t<double>(angle::radian_t<double>(0.015708))();
	EXPECT_NEAR(1.0, test, 5.0e-6);

	test = angle::radian_t<double>(angle::radian_t<double>(2.1))();
	EXPECT_NEAR(2.1, test, 5.0e-6);
	test = angle::gradian_t<double>(angle::arcsecond_t<double>(2.1))();
	EXPECT_NEAR(0.000648148, test, 5.0e-6);
	test = angle::degree_t<double>(angle::radian_t<double>(detail::PI_VAL))();
	EXPECT_NEAR(180.0, test, 5.0e-6);
	test = angle::radian_t<double>(angle::degree_t<double>(90.0))();
	EXPECT_NEAR(detail::PI_VAL / 2, test, 5.0e-6);
}

TEST_F(ConversionFactor, current)
{
	double test;

	test = current::milliampere_t<double>(current::ampere_t<double>(2.1))();
	EXPECT_NEAR(2100.0, test, 5.0e-6);
}

TEST_F(ConversionFactor, temperature)
{
	// temp conversion are weird/hard since they involve translations AND scaling.
	double test;

	test = kelvin_t<double>(kelvin_t<double>(72.0))();
	EXPECT_NEAR(72.0, test, 5.0e-5);
	test = fahrenheit_t<double>(fahrenheit_t<double>(72.0))();
	EXPECT_NEAR(72.0, test, 5.0e-5);
	test = fahrenheit_t<double>(kelvin_t<double>(300.0))();
	EXPECT_NEAR(80.33, test, 5.0e-5);
	test = kelvin_t<double>(fahrenheit_t<double>(451.0))();
	EXPECT_NEAR(505.928, test, 5.0e-4);
	test = celsius_t<double>(kelvin_t<double>(300.0))();
	EXPECT_NEAR(26.85, test, 5.0e-3);
	test = kelvin_t<double>(celsius_t<double>(451.0))();
	EXPECT_NEAR(724.15, test, 5.0e-3);
	test = celsius_t<double>(fahrenheit_t<double>(72.0))();
	EXPECT_NEAR(22.2222, test, 5.0e-5);
	test = fahrenheit_t<double>(celsius_t<double>(100.0))();
	EXPECT_NEAR(212.0, test, 5.0e-5);
	test = celsius_t<double>(fahrenheit_t<double>(32.0))();
	EXPECT_NEAR(0.0, test, 5.0e-5);
	test = fahrenheit_t<double>(celsius_t<double>(0.0))();
	EXPECT_NEAR(32.0, test, 5.0e-5);
	test = kelvin_t<double>(rankine_t<double>(100.0))();
	EXPECT_NEAR(55.5556, test, 5.0e-5);
	test = rankine_t<double>(kelvin_t<double>(100.0))();
	EXPECT_NEAR(180.0, test, 5.0e-5);
	test = rankine_t<double>(fahrenheit_t<double>(100.0))();
	EXPECT_NEAR(559.67, test, 5.0e-5);
	test = fahrenheit_t<double>(rankine_t<double>(72.0))();
	EXPECT_NEAR(-387.67, test, 5.0e-5);
	test = kelvin_t<double>(reaumur_t<double>(100.0))();
	EXPECT_NEAR(398.0, test, 5.0e-1);
	test = celsius_t<double>(reaumur_t<double>(80.0))();
	EXPECT_NEAR(100.0, test, 5.0e-5);
	test = reaumur_t<double>(celsius_t<double>(212.0))();
	EXPECT_NEAR(169.6, test, 5.0e-2);
	test = fahrenheit_t<double>(reaumur_t<double>(80.0))();
	EXPECT_NEAR(212.0, test, 5.0e-5);
	test = reaumur_t<double>(fahrenheit_t<double>(37.0))();
	EXPECT_NEAR(2.222, test, 5.0e-3);
}

TEST_F(ConversionFactor, luminous_intensity)
{
	double test;

	test = millicandela_t<double>(candela_t<double>(72.0))();
	EXPECT_NEAR(72000.0, test, 5.0e-5);
	test = candela_t<double>(millicandela_t<double>(376.0))();
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

	same = std::is_same_v<traits::dimension_of_t<steradians>, traits::dimension_of_t<degrees_squared>>;
	EXPECT_TRUE(same);

	test = steradian_t<double>(steradian_t<double>(72.0))();
	EXPECT_NEAR(72.0, test, 5.0e-5);
	test = degree_squared_t<double>(steradian_t<double>(1.0))();
	EXPECT_NEAR(3282.8, test, 5.0e-2);
	test = spat_t<double>(steradian_t<double>(8.0))();
	EXPECT_NEAR(0.636619772367582, test, 5.0e-14);
	test = steradian_t<double>(degree_squared_t<double>(3282.8))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = degree_squared_t<double>(degree_squared_t<double>(72.0))();
	EXPECT_NEAR(72.0, test, 5.0e-5);
	test = spat_t<double>(degree_squared_t<double>(3282.8))();
	EXPECT_NEAR(1.0 / (4 * detail::PI_VAL), test, 5.0e-5);
	test = steradian_t<double>(spat_t<double>(1.0 / (4 * detail::PI_VAL)))();
	EXPECT_NEAR(1.0, test, 5.0e-14);
	test = degree_squared_t<double>(spat_t<double>(1.0 / (4 * detail::PI_VAL)))();
	EXPECT_NEAR(3282.8, test, 5.0e-2);
	test = spat_t<double>(spat_t<double>(72.0))();
	EXPECT_NEAR(72.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, frequency)
{
	double test;

	test = kilohertz_t<double>(hertz_t<double>(63000.0))();
	EXPECT_NEAR(63.0, test, 5.0e-5);
	test = hertz_t<double>(hertz_t<double>(6.3))();
	EXPECT_NEAR(6.3, test, 5.0e-5);
	test = hertz_t<double>(kilohertz_t<double>(5.0))();
	EXPECT_NEAR(5000.0, test, 5.0e-5);
	test = hertz_t<double>(megahertz_t<double>(1.0))();
	EXPECT_NEAR(1.0e6, test, 5.0e-5);
}

TEST_F(ConversionFactor, velocity)
{
	double test;
	bool same;

	same = std::is_same_v<meters_per_second, traits::strong_t<conversion_factor<std::ratio<1>, dimension::velocity>>>;
	EXPECT_TRUE(same);
	same = traits::is_convertible_unit_v<miles_per_hour, meters_per_second>;
	EXPECT_TRUE(same);

	test = miles_per_hour_t<double>(meters_per_second_t<double>(1250.0))();
	EXPECT_NEAR(2796.17, test, 5.0e-3);
	test = kilometers_per_hour_t<double>(feet_per_second_t<double>(2796.17))();
	EXPECT_NEAR(3068.181418, test, 5.0e-7);
	test = miles_per_hour_t<double>(knot_t<double>(600.0))();
	EXPECT_NEAR(690.468, test, 5.0e-4);
	test = feet_per_second_t<double>(miles_per_hour_t<double>(120.0))();
	EXPECT_NEAR(176.0, test, 5.0e-5);
	test = meters_per_second_t<double>(feet_per_second_t<double>(10.0))();
	EXPECT_NEAR(3.048, test, 5.0e-5);
}

TEST_F(ConversionFactor, angular_velocity)
{
	double test;
	bool same;

	same = std::is_same_v<radians_per_second,
		traits::strong_t<conversion_factor<std::ratio<1>, dimension::angular_velocity>>>;
	EXPECT_TRUE(same);
	same = traits::is_convertible_unit_v<rpm, radians_per_second>;
	EXPECT_TRUE(same);

	test = milliarcseconds_per_year_t<double>(radians_per_second_t<double>(1.0))();
	EXPECT_NEAR(6.504e15, test, 1.0e12);
	test = radians_per_second_t<double>(degrees_per_second_t<double>(1.0))();
	EXPECT_NEAR(0.0174533, test, 5.0e-8);
	test = radians_per_second_t<double>(revolutions_per_minute_t<double>(1.0))();
	EXPECT_NEAR(0.10471975512, test, 5.0e-13);
	test = radians_per_second_t<double>(milliarcseconds_per_year_t<double>(1.0))();
	EXPECT_NEAR(1.537e-16, test, 5.0e-20);
}

TEST_F(ConversionFactor, acceleration)
{
	double test;

	test = meters_per_second_squared_t<double>(standard_gravity_t<double>(1.0))();
	EXPECT_NEAR(9.80665, test, 5.0e-10);
}

TEST_F(ConversionFactor, force)
{
	double test;

	test = units::force::newton_t<double>(units::force::newton_t<double>(1.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = units::force::pound_t<double>(units::force::newton_t<double>(6.3))();
	EXPECT_NEAR(1.4163, test, 5.0e-5);
	test = units::force::dyne_t<double>(units::force::newton_t<double>(5.0))();
	EXPECT_NEAR(500000.0, test, 5.0e-5);
	test = units::force::poundal_t<double>(units::force::newton_t<double>(2.1))();
	EXPECT_NEAR(15.1893, test, 5.0e-5);
	test = units::force::kilopond_t<double>(units::force::newton_t<double>(173.0))();
	EXPECT_NEAR(17.6411, test, 5.0e-5);
	test = units::force::kilopond_t<double>(units::force::poundal_t<double>(21.879))();
	EXPECT_NEAR(0.308451933, test, 5.0e-10);
}

TEST_F(ConversionFactor, area)
{
	double test;

	test = acre_t<double>(hectare_t<double>(6.3))();
	EXPECT_NEAR(15.5676, test, 5.0e-5);
	test = square_kilometer_t<double>(square_mile_t<double>(10.0))();
	EXPECT_NEAR(25.8999, test, 5.0e-5);
	test = square_meter_t<double>(square_inch_t<double>(4.0))();
	EXPECT_NEAR(0.00258064, test, 5.0e-9);
	test = square_foot_t<double>(acre_t<double>(5.0))();
	EXPECT_NEAR(217800.0, test, 5.0e-5);
	test = square_foot_t<double>(square_meter_t<double>(1.0))();
	EXPECT_NEAR(10.7639, test, 5.0e-5);
}

TEST_F(ConversionFactor, pressure)
{
	double test;

	test = torr_t<double>(pascal_t<double>(1.0))();
	EXPECT_NEAR(0.00750062, test, 5.0e-5);
	test = pounds_per_square_inch_t<double>(bar_t<double>(2.2))();
	EXPECT_NEAR(31.9083, test, 5.0e-5);
	test = bar_t<double>(atmosphere_t<double>(4.0))();
	EXPECT_NEAR(4.053, test, 5.0e-5);
	test = pascal_t<double>(torr_t<double>(800.0))();
	EXPECT_NEAR(106657.89474, test, 5.0e-5);
	test = atmosphere_t<double>(pounds_per_square_inch_t<double>(38.0))();
	EXPECT_NEAR(2.58575, test, 5.0e-5);
	test = pascal_t<double>(pounds_per_square_inch_t<double>(1.0))();
	EXPECT_NEAR(6894.76, test, 5.0e-3);
	test = bar_t<double>(pascal_t<double>(0.25))();
	EXPECT_NEAR(2.5e-6, test, 5.0e-5);
	test = atmosphere_t<double>(torr_t<double>(9.0))();
	EXPECT_NEAR(0.0118421, test, 5.0e-8);
	test = torr_t<double>(bar_t<double>(12.0))();
	EXPECT_NEAR(9000.74, test, 5.0e-3);
	test = pounds_per_square_inch_t<double>(atmosphere_t<double>(1.0))();
	EXPECT_NEAR(14.6959, test, 5.0e-5);

	EXPECT_EQ(133.322387415_Pa, 1.0_mmHg);
}

TEST_F(ConversionFactor, charge)
{
	double test;

	test = ampere_hour_t<double>(coulomb_t<double>(4.0))();
	EXPECT_NEAR(0.00111111, test, 5.0e-9);
	test = coulomb_t<double>(ampere_hour_t<double>(1.0))();
	EXPECT_NEAR(3600.0, test, 5.0e-6);
}

TEST_F(ConversionFactor, energy)
{
	double test;

	test = calorie_t<double>(joule_t<double>(8000.000464))();
	EXPECT_NEAR(1912.046, test, 5.0e-4);
	test = joule_t<double>(therm_t<double>(12.0))();
	EXPECT_NEAR(1.266e+9, test, 5.0e5);
	test = watt_hour_t<double>(megajoule_t<double>(100.0))();
	EXPECT_NEAR(27777.778, test, 5.0e-4);
	test = megajoule_t<double>(kilocalorie_t<double>(56.0))();
	EXPECT_NEAR(0.234304, test, 5.0e-7);
	test = therm_t<double>(kilojoule_t<double>(56.0))();
	EXPECT_NEAR(0.000530904, test, 5.0e-5);
	test = kilojoule_t<double>(british_thermal_unit_t<double>(18.56399995447))();
	EXPECT_NEAR(19.5860568, test, 5.0e-5);
	test = energy::foot_pound_t<double>(calorie_t<double>(18.56399995447))();
	EXPECT_NEAR(57.28776190423856, test, 5.0e-5);
	test = calorie_t<double>(megajoule_t<double>(1.0))();
	EXPECT_NEAR(239006.0, test, 5.0e-1);
	test = kilowatt_hour_t<double>(kilocalorie_t<double>(2.0))();
	EXPECT_NEAR(0.00232444, test, 5.0e-9);
	test = kilocalorie_t<double>(therm_t<double>(0.1))();
	EXPECT_NEAR(2521.04, test, 5.0e-3);
	test = megajoule_t<double>(watt_hour_t<double>(67.0))();
	EXPECT_NEAR(0.2412, test, 5.0e-5);
	test = watt_hour_t<double>(british_thermal_unit_t<double>(100.0))();
	EXPECT_NEAR(29.3071, test, 5.0e-5);
	test = british_thermal_unit_t<double>(calorie_t<double>(100.0))();
	EXPECT_NEAR(0.396567, test, 5.0e-5);
}

TEST_F(ConversionFactor, power)
{
	double test;

	test = watt_t<double>(unit<compound_conversion_factor<energy::foot_pounds, inverse<seconds>>>(550.0))();
	EXPECT_NEAR(745.7, test, 5.0e-2);
	test = gigawatt_t<double>(watt_t<double>(1000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-4);
	test = watt_t<double>(microwatt_t<double>(200000.0))();
	EXPECT_NEAR(0.2, test, 5.0e-4);
	test = watt_t<double>(horsepower_t<double>(100.0))();
	EXPECT_NEAR(74570.0, test, 5.0e-1);
	test = megawatt_t<double>(horsepower_t<double>(5.0))();
	EXPECT_NEAR(0.0037284994, test, 5.0e-7);
	test = horsepower_t<double>(kilowatt_t<double>(232.0))();
	EXPECT_NEAR(311.117, test, 5.0e-4);
	test = horsepower_t<double>(milliwatt_t<double>(1001.0))();
	EXPECT_NEAR(0.001342363, test, 5.0e-9);
}

TEST_F(ConversionFactor, voltage)
{
	double test;

	test = millivolt_t<double>(volt_t<double>(10.0))();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = volt_t<double>(picovolt_t<double>(1000000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = volt_t<double>(nanovolt_t<double>(1000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = volt_t<double>(microvolt_t<double>(1000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = volt_t<double>(millivolt_t<double>(1000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = volt_t<double>(kilovolt_t<double>(0.001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = volt_t<double>(megavolt_t<double>(0.000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = volt_t<double>(gigavolt_t<double>(0.000000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = volt_t<double>(statvolt_t<double>(299.792458))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = statvolt_t<double>(millivolt_t<double>(1000.0))();
	EXPECT_NEAR(299.792458, test, 5.0e-5);
	test = nanovolt_t<double>(abvolt_t<double>(0.1))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = abvolt_t<double>(microvolt_t<double>(0.01))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, capacitance)
{
	double test;

	test = millifarad_t<double>(farad_t<double>(10.0))();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = farad_t<double>(picofarad_t<double>(1000000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = farad_t<double>(nanofarad_t<double>(1000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = farad_t<double>(microfarad_t<double>(1000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = farad_t<double>(millifarad_t<double>(1000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = farad_t<double>(kilofarad_t<double>(0.001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = farad_t<double>(megafarad_t<double>(0.000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = farad_t<double>(gigafarad_t<double>(0.000000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);

	auto f             = coulomb_t<double>(1) / volt_t<double>(1);
	farad_t<double> f2 = coulomb_t<double>(1) / volt_t<double>(1);
	EXPECT_TRUE((std::is_convertible_v<decltype(f), farad_t<double>>));

	auto one_farad = []() -> farad_t<double> { return coulomb_t<double>(1) / volt_t<double>(1); };

	EXPECT_EQ(1.0_F, one_farad());
}

TEST_F(ConversionFactor, impedance)
{
	double test;

	test = milliohm_t<double>(ohm_t<double>(10.0))();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = ohm_t<double>(picoohm_t<double>(1000000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = ohm_t<double>(nanoohm_t<double>(1000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = ohm_t<double>(microohm_t<double>(1000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = ohm_t<double>(milliohm_t<double>(1000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = ohm_t<double>(kiloohm_t<double>(0.001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = ohm_t<double>(megaohm_t<double>(0.000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = ohm_t<double>(gigaohm_t<double>(0.000000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, conductance)
{
	double test;

	test = millisiemens_t<double>(siemens_t<double>(10.0))();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = siemens_t<double>(picosiemens_t<double>(1000000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = siemens_t<double>(nanosiemens_t<double>(1000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = siemens_t<double>(microsiemens_t<double>(1000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = siemens_t<double>(millisiemens_t<double>(1000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = siemens_t<double>(kilosiemens_t<double>(0.001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = siemens_t<double>(megasiemens_t<double>(0.000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = siemens_t<double>(gigasiemens_t<double>(0.000000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, magnetic_flux)
{
	double test;

	test = milliweber_t<double>(weber_t<double>(10.0))();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = weber_t<double>(picoweber_t<double>(1000000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = weber_t<double>(nanoweber_t<double>(1000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = weber_t<double>(microweber_t<double>(1000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = weber_t<double>(milliweber_t<double>(1000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = weber_t<double>(kiloweber_t<double>(0.001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = weber_t<double>(megaweber_t<double>(0.000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = weber_t<double>(gigaweber_t<double>(0.000000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = weber_t<double>(maxwell_t<double>(100000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = maxwell_t<double>(nanoweber_t<double>(10.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, magnetic_field_strength)
{
	double test;

	test = millitesla_t<double>(tesla_t<double>(10.0))();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = tesla_t<double>(picotesla_t<double>(1000000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = tesla_t<double>(nanotesla_t<double>(1000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = tesla_t<double>(microtesla_t<double>(1000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = tesla_t<double>(millitesla_t<double>(1000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = tesla_t<double>(kilotesla_t<double>(0.001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = tesla_t<double>(megatesla_t<double>(0.000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = tesla_t<double>(gigatesla_t<double>(0.000000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = tesla_t<double>(gauss_t<double>(10000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = gauss_t<double>(nanotesla_t<double>(100000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, inductance)
{
	double test;

	test = millihenry_t<double>(henry_t<double>(10.0))();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = henry_t<double>(picohenry_t<double>(1000000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = henry_t<double>(nanohenry_t<double>(1000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = henry_t<double>(microhenry_t<double>(1000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = henry_t<double>(millihenry_t<double>(1000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = henry_t<double>(kilohenry_t<double>(0.001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = henry_t<double>(megahenry_t<double>(0.000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = henry_t<double>(gigahenry_t<double>(0.000000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, luminous_flux)
{
	double test;

	test = millilumen_t<double>(lumen_t<double>(10.0))();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = lumen_t<double>(picolumen_t<double>(1000000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lumen_t<double>(nanolumen_t<double>(1000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lumen_t<double>(microlumen_t<double>(1000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lumen_t<double>(millilumen_t<double>(1000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lumen_t<double>(kilolumen_t<double>(0.001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lumen_t<double>(megalumen_t<double>(0.000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lumen_t<double>(gigalumen_t<double>(0.000000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, illuminance)
{
	double test;

	test = millilux_t<double>(lux_t<double>(10.0))();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = lux_t<double>(picolux_t<double>(1000000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lux_t<double>(nanolux_t<double>(1000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lux_t<double>(microlux_t<double>(1000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lux_t<double>(millilux_t<double>(1000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lux_t<double>(kilolux_t<double>(0.001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lux_t<double>(megalux_t<double>(0.000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lux_t<double>(gigalux_t<double>(0.000000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);

	// 	test = lux_t<double>(footcandle_t<double>(0.092903))();
	// 	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = lumens_per_square_inch_t<double>(lux_t<double>(1550.0031000062))();
	EXPECT_NEAR(1.0, test, 5.0e-13);
	test = lux_t<double>(phot_t<double>(0.0001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, radiation)
{
	double test;

	test = millibecquerel_t<double>(becquerel_t<double>(10.0))();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = becquerel_t<double>(picobecquerel_t<double>(1000000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = becquerel_t<double>(nanobecquerel_t<double>(1000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = becquerel_t<double>(microbecquerel_t<double>(1000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = becquerel_t<double>(millibecquerel_t<double>(1000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = becquerel_t<double>(kilobecquerel_t<double>(0.001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = becquerel_t<double>(megabecquerel_t<double>(0.000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = becquerel_t<double>(gigabecquerel_t<double>(0.000000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);

	test = milligray_t<double>(gray_t<double>(10.0))();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = gray_t<double>(picogray_t<double>(1000000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = gray_t<double>(nanogray_t<double>(1000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = gray_t<double>(microgray_t<double>(1000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = gray_t<double>(milligray_t<double>(1000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = gray_t<double>(kilogray_t<double>(0.001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = gray_t<double>(megagray_t<double>(0.000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = gray_t<double>(gigagray_t<double>(0.000000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);

	test = millisievert_t<double>(sievert_t<double>(10.0))();
	EXPECT_NEAR(10000.0, test, 5.0e-5);
	test = sievert_t<double>(picosievert_t<double>(1000000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = sievert_t<double>(nanosievert_t<double>(1000000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = sievert_t<double>(microsievert_t<double>(1000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = sievert_t<double>(millisievert_t<double>(1000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = sievert_t<double>(kilosievert_t<double>(0.001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = sievert_t<double>(megasievert_t<double>(0.000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = sievert_t<double>(gigasievert_t<double>(0.000000001))();
	EXPECT_NEAR(1.0, test, 5.0e-5);

	test = curie_t<double>(becquerel_t<double>(37.0e9))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = rutherford_t<double>(becquerel_t<double>(1000000.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = gray_t<double>(rad_t<double>(100.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, torque)
{
	double test;

	test = newton_meter_t<double>(torque::foot_pound_t<double>(1.0))();
	EXPECT_NEAR(1.355817948, test, 5.0e-5);
	test = newton_meter_t<double>(inch_pound_t<double>(1.0))();
	EXPECT_NEAR(0.112984829, test, 5.0e-5);
	test = newton_meter_t<double>(foot_poundal_t<double>(1.0))();
	EXPECT_NEAR(4.214011009e-2, test, 5.0e-5);
	test = newton_meter_t<double>(meter_kilogram_t<double>(1.0))();
	EXPECT_NEAR(9.80665, test, 5.0e-5);
	test = meter_kilogram_t<double>(inch_pound_t<double>(86.79616930855788))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = inch_pound_t<double>(foot_poundal_t<double>(2.681170713))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
}

TEST_F(ConversionFactor, volume)
{
	double test;

	test = cubic_meter_t<double>(cubic_meter_t<double>(1.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = cubic_meter_t<double>(cubic_millimeter_t<double>(1.0))();
	EXPECT_NEAR(1.0e-9, test, 5.0e-5);
	test = cubic_meter_t<double>(cubic_kilometer_t<double>(1.0))();
	EXPECT_NEAR(1.0e9, test, 5.0e-5);
	test = cubic_meter_t<double>(liter_t<double>(1.0))();
	EXPECT_NEAR(0.001, test, 5.0e-5);
	test = cubic_meter_t<double>(milliliter_t<double>(1.0))();
	EXPECT_NEAR(1.0e-6, test, 5.0e-5);
	test = cubic_meter_t<double>(cubic_inch_t<double>(1.0))();
	EXPECT_NEAR(1.6387e-5, test, 5.0e-10);
	test = cubic_meter_t<double>(cubic_foot_t<double>(1.0))();
	EXPECT_NEAR(0.0283168, test, 5.0e-8);
	test = cubic_meter_t<double>(cubic_yard_t<double>(1.0))();
	EXPECT_NEAR(0.764555, test, 5.0e-7);
	test = cubic_meter_t<double>(cubic_mile_t<double>(1.0))();
	EXPECT_NEAR(4.168e+9, test, 5.0e5);
	test = cubic_meter_t<double>(gallon_t<double>(1.0))();
	EXPECT_NEAR(0.00378541, test, 5.0e-8);
	test = cubic_meter_t<double>(quart_t<double>(1.0))();
	EXPECT_NEAR(0.000946353, test, 5.0e-10);
	test = cubic_meter_t<double>(pint_t<double>(1.0))();
	EXPECT_NEAR(0.000473176, test, 5.0e-10);
	test = cubic_meter_t<double>(cup_t<double>(1.0))();
	EXPECT_NEAR(0.00024, test, 5.0e-6);
	test = cubic_meter_t<double>(volume::fluid_ounce_t<double>(1.0))();
	EXPECT_NEAR(2.9574e-5, test, 5.0e-5);
	test = cubic_meter_t<double>(barrel_t<double>(1.0))();
	EXPECT_NEAR(0.158987294928, test, 5.0e-13);
	test = cubic_meter_t<double>(bushel_t<double>(1.0))();
	EXPECT_NEAR(0.0352391, test, 5.0e-8);
	test = cubic_meter_t<double>(cord_t<double>(1.0))();
	EXPECT_NEAR(3.62456, test, 5.0e-6);
	test = cubic_meter_t<double>(cubic_fathom_t<double>(1.0))();
	EXPECT_NEAR(6.11644, test, 5.0e-6);
	test = cubic_meter_t<double>(tablespoon_t<double>(1.0))();
	EXPECT_NEAR(1.4787e-5, test, 5.0e-10);
	test = cubic_meter_t<double>(teaspoon_t<double>(1.0))();
	EXPECT_NEAR(4.9289e-6, test, 5.0e-11);
	test = cubic_meter_t<double>(pinch_t<double>(1.0))();
	EXPECT_NEAR(616.11519921875e-9, test, 5.0e-20);
	test = cubic_meter_t<double>(dash_t<double>(1.0))();
	EXPECT_NEAR(308.057599609375e-9, test, 5.0e-20);
	test = cubic_meter_t<double>(drop_t<double>(1.0))();
	EXPECT_NEAR(82.14869322916e-9, test, 5.0e-9);
	test = cubic_meter_t<double>(fifth_t<double>(1.0))();
	EXPECT_NEAR(0.00075708236, test, 5.0e-12);
	test = cubic_meter_t<double>(dram_t<double>(1.0))();
	EXPECT_NEAR(3.69669e-6, test, 5.0e-12);
	test = cubic_meter_t<double>(gill_t<double>(1.0))();
	EXPECT_NEAR(0.000118294, test, 5.0e-10);
	test = cubic_meter_t<double>(peck_t<double>(1.0))();
	EXPECT_NEAR(0.00880977, test, 5.0e-9);
	test = cubic_meter_t<double>(sack_t<double>(9.4591978))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = cubic_meter_t<double>(shot_t<double>(1.0))();
	EXPECT_NEAR(4.43603e-5, test, 5.0e-11);
	test = cubic_meter_t<double>(strike_t<double>(1.0))();
	EXPECT_NEAR(0.07047814033376, test, 5.0e-5);
	test = milliliter_t<double>(volume::fluid_ounce_t<double>(1.0))();
	EXPECT_NEAR(29.5735, test, 5.0e-5);
}

TEST_F(ConversionFactor, density)
{
	double test;

	test = kilograms_per_cubic_meter_t<double>(kilograms_per_cubic_meter_t<double>(1.0))();
	EXPECT_NEAR(1.0, test, 5.0e-5);
	test = kilograms_per_cubic_meter_t<double>(grams_per_milliliter_t<double>(1.0))();
	EXPECT_NEAR(1000.0, test, 5.0e-5);
	test = kilograms_per_cubic_meter_t<double>(kilograms_per_liter_t<double>(1.0))();
	EXPECT_NEAR(1000.0, test, 5.0e-5);
	test = kilograms_per_cubic_meter_t<double>(ounces_per_cubic_foot_t<double>(1.0))();
	EXPECT_NEAR(1.001153961, test, 5.0e-10);
	test = kilograms_per_cubic_meter_t<double>(ounces_per_cubic_inch_t<double>(1.0))();
	EXPECT_NEAR(1.729994044e3, test, 5.0e-7);
	test = kilograms_per_cubic_meter_t<double>(ounces_per_gallon_t<double>(1.0))();
	EXPECT_NEAR(7.489151707, test, 5.0e-10);
	test = kilograms_per_cubic_meter_t<double>(pounds_per_cubic_foot_t<double>(1.0))();
	EXPECT_NEAR(16.01846337, test, 5.0e-9);
	test = kilograms_per_cubic_meter_t<double>(pounds_per_cubic_inch_t<double>(1.0))();
	EXPECT_NEAR(2.767990471e4, test, 5.0e-6);
	test = kilograms_per_cubic_meter_t<double>(pounds_per_gallon_t<double>(1.0))();
	EXPECT_NEAR(119.8264273, test, 5.0e-8);
	test = kilograms_per_cubic_meter_t<double>(slugs_per_cubic_foot_t<double>(1.0))();
	EXPECT_NEAR(515.3788184, test, 5.0e-6);
}

TEST_F(ConversionFactor, concentration)
{
	double test;

	test = ppm_t<double>(1.0);
	EXPECT_NEAR(1.0e-6, test, 5.0e-12);
	test = ppb_t<double>(1.0);
	EXPECT_NEAR(1.0e-9, test, 5.0e-12);
	test = ppt_t<double>(1.0);
	EXPECT_NEAR(1.0e-12, test, 5.0e-12);
	test = percent_t<double>(18.0);
	EXPECT_NEAR(0.18, test, 5.0e-12);
}

TEST_F(ConversionFactor, data)
{
	EXPECT_EQ(8, (bit_t<double>(byte_t<double>(1))()));

	EXPECT_EQ(1000, (byte_t<double>(kilobyte_t<double>(1))()));
	EXPECT_EQ(1000, (kilobyte_t<double>(megabyte_t<double>(1))()));
	EXPECT_EQ(1000, (megabyte_t<double>(gigabyte_t<double>(1))()));
	EXPECT_EQ(1000, (gigabyte_t<double>(terabyte_t<double>(1))()));
	EXPECT_EQ(1000, (terabyte_t<double>(petabyte_t<double>(1))()));
	EXPECT_EQ(1000, (petabyte_t<double>(exabyte_t<double>(1))()));

	EXPECT_EQ(1024, (byte_t<double>(kibibyte_t<double>(1))()));
	EXPECT_EQ(1024, (kibibyte_t<double>(mebibyte_t<double>(1))()));
	EXPECT_EQ(1024, (mebibyte_t<double>(gibibyte_t<double>(1))()));
	EXPECT_EQ(1024, (gibibyte_t<double>(tebibyte_t<double>(1))()));
	EXPECT_EQ(1024, (tebibyte_t<double>(pebibyte_t<double>(1))()));
	EXPECT_EQ(1024, (pebibyte_t<double>(exbibyte_t<double>(1))()));

	EXPECT_EQ(93750000, (kibibit_t<double>(gigabyte_t<double>(12))()));

	EXPECT_EQ(1000, (bit_t<double>(kilobit_t<double>(1))()));
	EXPECT_EQ(1000, (kilobit_t<double>(megabit_t<double>(1))()));
	EXPECT_EQ(1000, (megabit_t<double>(gigabit_t<double>(1))()));
	EXPECT_EQ(1000, (gigabit_t<double>(terabit_t<double>(1))()));
	EXPECT_EQ(1000, (terabit_t<double>(petabit_t<double>(1))()));
	EXPECT_EQ(1000, (petabit_t<double>(exabit_t<double>(1))()));

	EXPECT_EQ(1024, (bit_t<double>(kibibit_t<double>(1))()));
	EXPECT_EQ(1024, (kibibit_t<double>(mebibit_t<double>(1))()));
	EXPECT_EQ(1024, (mebibit_t<double>(gibibit_t<double>(1))()));
	EXPECT_EQ(1024, (gibibit_t<double>(tebibit_t<double>(1))()));
	EXPECT_EQ(1024, (tebibit_t<double>(pebibit_t<double>(1))()));
	EXPECT_EQ(1024, (pebibit_t<double>(exbibit_t<double>(1))()));

	// Source: https://en.wikipedia.org/wiki/Binary_prefix
	EXPECT_NEAR(percent_t<double>(2.4), kibibyte_t<double>(1) / kilobyte_t<double>(1) - 1, 0.005);
	EXPECT_NEAR(percent_t<double>(4.9), mebibyte_t<double>(1) / megabyte_t<double>(1) - 1, 0.005);
	EXPECT_NEAR(percent_t<double>(7.4), gibibyte_t<double>(1) / gigabyte_t<double>(1) - 1, 0.005);
	EXPECT_NEAR(percent_t<double>(10.0), tebibyte_t<double>(1) / terabyte_t<double>(1) - 1, 0.005);
	EXPECT_NEAR(percent_t<double>(12.6), pebibyte_t<double>(1) / petabyte_t<double>(1) - 1, 0.005);
	EXPECT_NEAR(percent_t<double>(15.3), exbibyte_t<double>(1) / exabyte_t<double>(1) - 1, 0.005);
}

TEST_F(ConversionFactor, data_transfer_rate)
{
	EXPECT_EQ(8, (bits_per_second_t<double>(bytes_per_second_t<double>(1))()));

	EXPECT_EQ(1000, (bytes_per_second_t<double>(kilobytes_per_second_t<double>(1))()));
	EXPECT_EQ(1000, (kilobytes_per_second_t<double>(megabytes_per_second_t<double>(1))()));
	EXPECT_EQ(1000, (megabytes_per_second_t<double>(gigabytes_per_second_t<double>(1))()));
	EXPECT_EQ(1000, (gigabytes_per_second_t<double>(terabytes_per_second_t<double>(1))()));
	EXPECT_EQ(1000, (terabytes_per_second_t<double>(petabytes_per_second_t<double>(1))()));
	EXPECT_EQ(1000, (petabytes_per_second_t<double>(exabytes_per_second_t<double>(1))()));

	EXPECT_EQ(1024, (bytes_per_second_t<double>(kibibytes_per_second_t<double>(1))()));
	EXPECT_EQ(1024, (kibibytes_per_second_t<double>(mebibytes_per_second_t<double>(1))()));
	EXPECT_EQ(1024, (mebibytes_per_second_t<double>(gibibytes_per_second_t<double>(1))()));
	EXPECT_EQ(1024, (gibibytes_per_second_t<double>(tebibytes_per_second_t<double>(1))()));
	EXPECT_EQ(1024, (tebibytes_per_second_t<double>(pebibytes_per_second_t<double>(1))()));
	EXPECT_EQ(1024, (pebibytes_per_second_t<double>(exbibytes_per_second_t<double>(1))()));

	EXPECT_EQ(93750000, (kibibits_per_second_t<double>(gigabytes_per_second_t<double>(12))()));

	EXPECT_EQ(1000, (bits_per_second_t<double>(kilobits_per_second_t<double>(1))()));
	EXPECT_EQ(1000, (kilobits_per_second_t<double>(megabits_per_second_t<double>(1))()));
	EXPECT_EQ(1000, (megabits_per_second_t<double>(gigabits_per_second_t<double>(1))()));
	EXPECT_EQ(1000, (gigabits_per_second_t<double>(terabits_per_second_t<double>(1))()));
	EXPECT_EQ(1000, (terabits_per_second_t<double>(petabits_per_second_t<double>(1))()));
	EXPECT_EQ(1000, (petabits_per_second_t<double>(exabits_per_second_t<double>(1))()));

	EXPECT_EQ(1024, (bits_per_second_t<double>(kibibits_per_second_t<double>(1))()));
	EXPECT_EQ(1024, (kibibits_per_second_t<double>(mebibits_per_second_t<double>(1))()));
	EXPECT_EQ(1024, (mebibits_per_second_t<double>(gibibits_per_second_t<double>(1))()));
	EXPECT_EQ(1024, (gibibits_per_second_t<double>(tebibits_per_second_t<double>(1))()));
	EXPECT_EQ(1024, (tebibits_per_second_t<double>(pebibits_per_second_t<double>(1))()));
	EXPECT_EQ(1024, (pebibits_per_second_t<double>(exbibits_per_second_t<double>(1))()));

	// Source: https://en.wikipedia.org/wiki/Binary_prefix
	EXPECT_NEAR(
		percent_t<double>(2.4), kibibytes_per_second_t<double>(1) / kilobytes_per_second_t<double>(1) - 1, 0.005);
	EXPECT_NEAR(
		percent_t<double>(4.9), mebibytes_per_second_t<double>(1) / megabytes_per_second_t<double>(1) - 1, 0.005);
	EXPECT_NEAR(
		percent_t<double>(7.4), gibibytes_per_second_t<double>(1) / gigabytes_per_second_t<double>(1) - 1, 0.005);
	EXPECT_NEAR(
		percent_t<double>(10.0), tebibytes_per_second_t<double>(1) / terabytes_per_second_t<double>(1) - 1, 0.005);
	EXPECT_NEAR(
		percent_t<double>(12.6), pebibytes_per_second_t<double>(1) / petabytes_per_second_t<double>(1) - 1, 0.005);
	EXPECT_NEAR(
		percent_t<double>(15.3), exbibytes_per_second_t<double>(1) / exabytes_per_second_t<double>(1) - 1, 0.005);
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
	EXPECT_TRUE((std::is_same_v<meter_t<double>, decltype(constants::pi * meter_t<double>(1))>));
	EXPECT_TRUE((std::is_same_v<meter_t<double>, decltype(meter_t<double>(1) * constants::pi)>));

	EXPECT_NEAR(detail::PI_VAL, (constants::pi * meter_t<double>(1)).to<double>(), 5.0e-10);
	EXPECT_NEAR(detail::PI_VAL, (meter_t<double>(1) * constants::pi).to<double>(), 5.0e-10);

	// explicit multiplication
	meter_t<double> a = constants::pi * meter_t<double>(1);
	meter_t<double> b = meter_t<double>(1) * constants::pi;

	EXPECT_NEAR(detail::PI_VAL, a.to<double>(), 5.0e-10);
	EXPECT_NEAR(detail::PI_VAL, b.to<double>(), 5.0e-10);

	// auto division
	EXPECT_TRUE((std::is_same_v<hertz_t<double>, decltype(constants::pi / second_t<double>(1))>));
	EXPECT_TRUE((std::is_same_v<second_t<double>, decltype(second_t<double>(1) / constants::pi)>));

	EXPECT_NEAR(detail::PI_VAL, (constants::pi / second_t<double>(1)).to<double>(), 5.0e-10);
	EXPECT_NEAR(1.0 / detail::PI_VAL, (second_t<double>(1) / constants::pi).to<double>(), 5.0e-10);

	// explicit
	hertz_t<double> c  = constants::pi / second_t<double>(1);
	second_t<double> d = second_t<double>(1) / constants::pi;

	EXPECT_NEAR(detail::PI_VAL, c.to<double>(), 5.0e-10);
	EXPECT_NEAR(1.0 / detail::PI_VAL, d.to<double>(), 5.0e-10);
}

TEST_F(ConversionFactor, constants)
{
	// Source: NIST "2014 CODATA recommended values"
	EXPECT_NEAR(299792458, constants::c(), 5.0e-9);
	EXPECT_NEAR(6.67408e-11, constants::G(), 5.0e-17);
	EXPECT_NEAR(6.626070040e-34, constants::h(), 5.0e-44);
	EXPECT_NEAR(1.2566370614e-6, constants::mu0(), 5.0e-17);
	EXPECT_NEAR(8.854187817e-12, constants::epsilon0(), 5.0e-21);
	EXPECT_NEAR(376.73031346177, constants::Z0(), 5.0e-12);
	EXPECT_NEAR(8987551787.3681764, constants::k_e(), 5.0e-6);
	EXPECT_NEAR(1.6021766208e-19, constants::e(), 5.0e-29);
	EXPECT_NEAR(9.10938356e-31, constants::m_e(), 5.0e-40);
	EXPECT_NEAR(1.672621898e-27, constants::m_p(), 5.0e-37);
	EXPECT_NEAR(9.274009994e-24, constants::mu_B(), 5.0e-32);
	EXPECT_NEAR(6.022140857e23, constants::N_A(), 5.0e14);
	EXPECT_NEAR(8.3144598, constants::R(), 5.0e-8);
	EXPECT_NEAR(1.38064852e-23, constants::k_B(), 5.0e-31);
	EXPECT_NEAR(96485.33289, constants::F(), 5.0e-5);
	EXPECT_NEAR(5.670367e-8, constants::sigma(), 5.0e-14);
}

TEST_F(ConversionFactor, std_chrono)
{
	nanosecond_t<double> a = std::chrono::nanoseconds(10);
	EXPECT_EQ(nanosecond_t<double>(10), a);
	microsecond_t<double> b = std::chrono::microseconds(10);
	EXPECT_EQ(microsecond_t<double>(10), b);
	millisecond_t<double> c = std::chrono::milliseconds(10);
	EXPECT_EQ(millisecond_t<double>(10), c);
	second_t<double> d = std::chrono::seconds(1);
	EXPECT_EQ(second_t<double>(1), d);
	minute_t<double> e = std::chrono::minutes(120);
	EXPECT_EQ(minute_t<double>(120), e);
	hour_t<double> f = std::chrono::hours(2);
	EXPECT_EQ(hour_t<double>(2), f);

	std::chrono::nanoseconds g = unit<nanosecond, int>(100);
	EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(g).count(), 100);
	std::chrono::nanoseconds h = unit<microsecond, int>(2);
	EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(h).count(), 2000);
	std::chrono::nanoseconds i = unit<millisecond, int>(1);
	EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(i).count(), 1000000);
	std::chrono::nanoseconds j = unit<second, int>(1);
	EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(j).count(), 1000000000);
	std::chrono::nanoseconds k = unit<minute, int>(1);
	EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(k).count(), 60000000000);
	std::chrono::nanoseconds l = unit<hour, int>(1);
	EXPECT_EQ(std::chrono::duration_cast<std::chrono::nanoseconds>(l).count(), 3600000000000);
}

TEST_F(ConversionFactor, squaredTemperature)
{
	using squared_celsius   = units::compound_conversion_factor<squared<celsius>>;
	using squared_celsius_t = units::unit<squared_celsius>;
	const squared_celsius_t right(100);
	const celsius_t<double> rootRight = sqrt(right);
	EXPECT_EQ(celsius_t<double>(10), rootRight);
}

TEST_F(UnitMath, min)
{
	meter_t<double> a(1);
	foot_t<double> c(1);
	EXPECT_EQ(c, units::min(a, c));

	const unit<meters> d(1);
	const unit<centimeters> e(99);
	EXPECT_EQ(e, units::min(d, e));
}

TEST_F(UnitMath, max)
{
	meter_t<double> a(1);
	foot_t<double> c(1);
	EXPECT_EQ(a, max(a, c));

	const unit<meters> d(1);
	const unit<centimeters> e(101);
	EXPECT_EQ(e, max(d, e));
}

TEST_F(UnitMath, cos)
{
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(cos(angle::radian_t<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(cos(unit<degrees, int>(0)))>));
	EXPECT_NEAR(dimensionless<double>(-0.41614683654), cos(angle::radian_t<double>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(-0.41614683654), cos(unit<radians, int>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(-0.70710678118), cos(angle::degree_t<double>(135)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(-0.70710678118), cos(unit<degrees, int>(135)), 5.0e-11);
}

TEST_F(UnitMath, sin)
{
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(sin(angle::radian_t<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(sin(unit<degrees, int>(0)))>));
	EXPECT_NEAR(dimensionless<double>(0.90929742682), sin(angle::radian_t<double>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(0.90929742682), sin(unit<radians, int>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(0.70710678118), sin(angle::degree_t<double>(135)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(0.70710678118), sin(unit<degrees, int>(135)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(0), sin(1.0_rad * units::constants::pi), 5.0e-16);
}

TEST_F(UnitMath, tan)
{
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(tan(angle::radian_t<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(tan(unit<degrees, int>(0)))>));
	EXPECT_NEAR(dimensionless<double>(-2.18503986326), tan(angle::radian_t<double>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(-2.18503986326), tan(unit<radians, int>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(-1.0), tan(angle::degree_t<double>(135)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(-1.0), tan(unit<degrees, int>(135)), 5.0e-11);
}

TEST_F(UnitMath, acos)
{
	EXPECT_TRUE((std::is_same_v<angle::radian_t<double>, decltype(acos(dimensionless<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<angle::radian_t<double>, decltype(acos(unit<dimensionless_unit, int>(0)))>));
	EXPECT_NEAR(
		angle::radian_t<double>(2).to<double>(), acos(dimensionless<double>(-0.41614683654)).to<double>(), 5.0e-11);
	EXPECT_NEAR(angle::radian_t<double>(1.570796326795).to<double>(),
		acos(unit<dimensionless_unit, int>(0)).to<double>(), 5.0e-11);
	EXPECT_NEAR(angle::degree_t<double>(135).to<double>(),
		angle::degree_t<double>(acos(dimensionless<double>(-0.70710678118654752440084436210485))).to<double>(),
		5.0e-12);
	EXPECT_NEAR(angle::degree_t<double>(90).to<double>(),
		angle::degree_t<double>(acos(unit<dimensionless_unit, int>(0))).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, asin)
{
	EXPECT_TRUE((std::is_same_v<angle::radian_t<double>, decltype(asin(dimensionless<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<angle::radian_t<double>, decltype(asin(unit<dimensionless_unit, int>(0)))>));
	EXPECT_NEAR(angle::radian_t<double>(1.14159265).to<double>(),
		asin(dimensionless<double>(0.90929742682)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::radian_t<double>(1.570796326795).to<double>(),
		asin(unit<dimensionless_unit, int>(1)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::degree_t<double>(45).to<double>(),
		angle::degree_t<double>(asin(dimensionless<double>(0.70710678118654752440084436210485))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degree_t<double>(90).to<double>(),
		angle::degree_t<double>(asin(unit<dimensionless_unit, int>(1))).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, atan)
{
	EXPECT_TRUE((std::is_same_v<angle::radian_t<double>, decltype(atan(dimensionless<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<angle::radian_t<double>, decltype(atan(unit<dimensionless_unit, int>(0)))>));
	EXPECT_NEAR(angle::radian_t<double>(-1.14159265).to<double>(),
		atan(dimensionless<double>(-2.18503986326)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::radian_t<double>(0.785398163397).to<double>(),
		atan(unit<dimensionless_unit, int>(1)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::degree_t<double>(-45).to<double>(),
		angle::degree_t<double>(atan(dimensionless<double>(-1.0))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degree_t<double>(45).to<double>(),
		angle::degree_t<double>(atan(unit<dimensionless_unit, int>(1))).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, atan2)
{
	EXPECT_TRUE(
		(std::is_same_v<angle::radian_t<double>, decltype(atan2(dimensionless<double>(1), dimensionless<double>(1)))>));
	EXPECT_TRUE((std::is_same_v<angle::radian_t<double>,
		decltype(atan2(unit<dimensionless_unit, int>(1), unit<dimensionless_unit, int>(1)))>));
	EXPECT_NEAR(angle::radian_t<double>(detail::PI_VAL / 4).to<double>(),
		atan2(dimensionless<double>(2), dimensionless<double>(2)).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::radian_t<double>(detail::PI_VAL / 4).to<double>(),
		atan2(unit<dimensionless_unit, int>(2), unit<dimensionless_unit, int>(2)).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degree_t<double>(45).to<double>(),
		angle::degree_t<double>(atan2(dimensionless<double>(2), dimensionless<double>(2))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degree_t<double>(45).to<double>(),
		angle::degree_t<double>(atan2(unit<dimensionless_unit, int>(2), unit<dimensionless_unit, int>(2))).to<double>(),
		5.0e-12);

	EXPECT_TRUE(
		(std::is_same_v<angle::radian_t<double>, decltype(atan2(dimensionless<double>(1), dimensionless<double>(1)))>));
	EXPECT_NEAR(angle::radian_t<double>(detail::PI_VAL / 6).to<double>(),
		atan2(dimensionless<double>(1), sqrt(dimensionless<double>(3))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::radian_t<double>(detail::PI_VAL / 6).to<double>(),
		atan2(unit<dimensionless_unit, int>(1), sqrt(unit<dimensionless_unit, int>(3))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degree_t<double>(30).to<double>(),
		angle::degree_t<double>(atan2(dimensionless<double>(1), sqrt(dimensionless<double>(3)))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degree_t<double>(30).to<double>(),
		angle::degree_t<double>(atan2(unit<dimensionless_unit, int>(1), sqrt(unit<dimensionless_unit, int>(3))))
			.to<double>(),
		5.0e-12);
}

TEST_F(UnitMath, cosh)
{
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(cosh(angle::radian_t<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(cosh(unit<degrees, int>(0)))>));
	EXPECT_NEAR(dimensionless<double>(3.76219569108), cosh(angle::radian_t<double>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(3.76219569108), cosh(unit<radians, int>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(5.32275215), cosh(angle::degree_t<double>(135)), 5.0e-9);
	EXPECT_NEAR(dimensionless<double>(5.32275215), cosh(unit<degrees, int>(135)), 5.0e-9);
}

TEST_F(UnitMath, sinh)
{
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(sinh(angle::radian_t<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(sinh(unit<degrees, int>(0)))>));
	EXPECT_NEAR(dimensionless<double>(3.62686040785), sinh(angle::radian_t<double>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(3.62686040785), sinh(unit<radians, int>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(5.22797192), sinh(angle::degree_t<double>(135)), 5.0e-9);
	EXPECT_NEAR(dimensionless<double>(5.22797192), sinh(unit<degrees, int>(135)), 5.0e-9);
}

TEST_F(UnitMath, tanh)
{
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(tanh(angle::radian_t<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<dimensionless<double>, decltype(tanh(unit<degrees, int>(0)))>));
	EXPECT_NEAR(dimensionless<double>(0.96402758007), tanh(angle::radian_t<double>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(0.96402758007), tanh(unit<radians, int>(2)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(0.98219338), tanh(angle::degree_t<double>(135)), 5.0e-11);
	EXPECT_NEAR(dimensionless<double>(0.98219338), tanh(unit<degrees, int>(135)), 5.0e-11);
}

TEST_F(UnitMath, acosh)
{
	EXPECT_TRUE((std::is_same_v<angle::radian_t<double>, decltype(acosh(dimensionless<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<angle::radian_t<double>, decltype(acosh(unit<dimensionless_unit, int>(0)))>));
	EXPECT_NEAR(angle::radian_t<double>(1.316957896924817).to<double>(), acosh(dimensionless<double>(2.0)).to<double>(),
		5.0e-11);
	EXPECT_NEAR(angle::radian_t<double>(1.316957896924817).to<double>(),
		acosh(unit<dimensionless_unit, int>(2)).to<double>(), 5.0e-11);
	EXPECT_NEAR(angle::degree_t<double>(75.456129290216893).to<double>(),
		angle::degree_t<double>(acosh(dimensionless<double>(2.0))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degree_t<double>(75.456129290216893).to<double>(),
		angle::degree_t<double>(acosh(unit<dimensionless_unit, int>(2))).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, asinh)
{
	EXPECT_TRUE((std::is_same_v<angle::radian_t<double>, decltype(asinh(dimensionless<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<angle::radian_t<double>, decltype(asinh(unit<dimensionless_unit, int>(0)))>));
	EXPECT_NEAR(
		angle::radian_t<double>(1.443635475178810).to<double>(), asinh(dimensionless<double>(2)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::radian_t<double>(1.443635475178810).to<double>(),
		asinh(unit<dimensionless_unit, int>(2)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::degree_t<double>(82.714219883108939).to<double>(),
		angle::degree_t<double>(asinh(dimensionless<double>(2))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degree_t<double>(82.714219883108939).to<double>(),
		angle::degree_t<double>(asinh(unit<dimensionless_unit, int>(2))).to<double>(), 5.0e-12);
}

TEST_F(UnitMath, atanh)
{
	EXPECT_TRUE((std::is_same_v<angle::radian_t<double>, decltype(atanh(dimensionless<double>(0)))>));
	EXPECT_TRUE((std::is_same_v<angle::radian_t<double>, decltype(atanh(unit<dimensionless_unit, int>(0)))>));
	EXPECT_NEAR(angle::radian_t<double>(0.549306144334055).to<double>(), atanh(dimensionless<double>(0.5)).to<double>(),
		5.0e-9);
	EXPECT_NEAR(angle::radian_t<double>(0).to<double>(), atanh(unit<dimensionless_unit, int>(0)).to<double>(), 5.0e-9);
	EXPECT_NEAR(angle::degree_t<double>(31.472923730945389).to<double>(),
		angle::degree_t<double>(atanh(dimensionless<double>(0.5))).to<double>(), 5.0e-12);
	EXPECT_NEAR(angle::degree_t<double>(0).to<double>(),
		angle::degree_t<double>(atanh(unit<dimensionless_unit, int>(0))).to<double>(), 5.0e-12);
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
	meter_t<double> value(10.0);

	auto sq = pow<2>(value);
	EXPECT_NEAR(100.0, sq(), 5.0e-2);
	isSame = std::is_same_v<decltype(sq), square_meter_t<double>>;
	EXPECT_TRUE(isSame);

	auto cube = pow<3>(value);
	EXPECT_NEAR(1000.0, cube(), 5.0e-2);
	isSame = std::is_same_v<decltype(cube), traits::strong_t<unit<traits::strong_t<cubed<meter>>>>>;
	EXPECT_TRUE(isSame);

	auto fourth = pow<4>(value);
	EXPECT_NEAR(10000.0, fourth(), 5.0e-2);
	isSame = std::is_same_v<decltype(fourth), unit<compound_conversion_factor<squared<meter>, squared<meter>>>>;
	EXPECT_TRUE(isSame);
}

TEST_F(UnitMath, sqrt)
{
	EXPECT_TRUE((std::is_same_v<meter_t<double>, decltype(sqrt(square_meter_t<double>(4.0)))>));
	EXPECT_NEAR(meter_t<double>(2.0).to<double>(), sqrt(square_meter_t<double>(4.0)).to<double>(), 5.0e-9);

	EXPECT_TRUE((std::is_same_v<angle::radian_t<double>, decltype(sqrt(steradian_t<double>(16.0)))>));
	EXPECT_NEAR(angle::radian_t<double>(4.0).to<double>(), sqrt(steradian_t<double>(16.0)).to<double>(), 5.0e-9);

	EXPECT_TRUE((std::is_convertible_v<foot_t<double>, decltype(sqrt(square_foot_t<double>(10.0)))>));

	// for rational conversion (i.e. no integral root) let's check a bunch of different ways this could go wrong
	foot_t<double> resultFt = sqrt(square_foot_t<double>(10.0));
	EXPECT_NEAR(foot_t<double>(3.16227766017).to<double>(), sqrt(square_foot_t<double>(10.0)).to<double>(), 5.0e-9);
	EXPECT_NEAR(foot_t<double>(3.16227766017).to<double>(), resultFt.to<double>(), 5.0e-9);
	EXPECT_EQ(resultFt, sqrt(square_foot_t<double>(10.0)));
}

TEST_F(UnitMath, hypot)
{
	EXPECT_TRUE((std::is_same_v<meter_t<double>, decltype(hypot(meter_t<double>(3.0), meter_t<double>(4.0)))>));
	EXPECT_NEAR(
		meter_t<double>(5.0).to<double>(), (hypot(meter_t<double>(3.0), meter_t<double>(4.0))).to<double>(), 5.0e-9);

	static_assert(
		traits::is_convertible_unit_v<foot_t<double>, decltype(hypot(foot_t<double>(3.0), meter_t<double>(1.2192)))>);
	EXPECT_NEAR(foot_t<double>(5.0).to<double>(),
		foot_t<double>(hypot(foot_t<double>(3.0), meter_t<double>(1.2192))).to<double>(), 5.0e-9);
}

TEST_F(UnitMath, ceil)
{
	double val = 101.1;
	EXPECT_EQ(ceil(val), ceil(meter_t<double>(val)).to<double>());
	EXPECT_TRUE((std::is_same_v<meter_t<double>, decltype(ceil(meter_t<double>(val)))>));
}

TEST_F(UnitMath, floor)
{
	double val = 101.1;
	EXPECT_EQ(floor(val), floor(dimensionless<double>(val)));
}

TEST_F(UnitMath, fmod)
{
	EXPECT_EQ(fmod(100.0, 101.2), fmod(meter_t<double>(100.0), meter_t<double>(101.2)).to<double>());
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
	meter_t<double> val(5.0);
	EXPECT_EQ(meter_t<double>(-5.0), copysign(val, sign));
	EXPECT_EQ(meter_t<double>(-5.0), copysign(val, angle::radian_t<double>(sign)));
}

TEST_F(UnitMath, fdim)
{
	EXPECT_EQ(meter_t<double>(0.0), fdim(meter_t<double>(8.0), meter_t<double>(10.0)));
	EXPECT_EQ(meter_t<double>(2.0), fdim(meter_t<double>(10.0), meter_t<double>(8.0)));
	EXPECT_NEAR(meter_t<double>(9.3904).to<double>(),
		meter_t<double>(fdim(meter_t<double>(10.0), foot_t<double>(2.0))).to<double>(),
		5.0e-320); // not sure why they aren't comparing exactly equal, but clearly they are.
}

TEST_F(UnitMath, fmin)
{
	EXPECT_EQ(meter_t<double>(8.0), fmin(meter_t<double>(8.0), meter_t<double>(10.0)));
	EXPECT_EQ(meter_t<double>(8.0), fmin(meter_t<double>(10.0), meter_t<double>(8.0)));
	EXPECT_EQ(foot_t<double>(2.0), fmin(meter_t<double>(10.0), foot_t<double>(2.0)));
}

TEST_F(UnitMath, fmax)
{
	EXPECT_EQ(meter_t<double>(10.0), fmax(meter_t<double>(8.0), meter_t<double>(10.0)));
	EXPECT_EQ(meter_t<double>(10.0), fmax(meter_t<double>(10.0), meter_t<double>(8.0)));
	EXPECT_EQ(meter_t<double>(10.0), fmax(meter_t<double>(10.0), foot_t<double>(2.0)));
}

TEST_F(UnitMath, fabs)
{
	EXPECT_EQ(meter_t<double>(10.0), fabs(meter_t<double>(-10.0)));
	EXPECT_EQ(meter_t<double>(10.0), fabs(meter_t<double>(10.0)));
}

TEST_F(UnitMath, abs)
{
	EXPECT_EQ(meter_t<double>(10.0), abs(meter_t<double>(-10.0)));
	EXPECT_EQ(meter_t<double>(10.0), abs(meter_t<double>(10.0)));
}

TEST_F(UnitMath, fma)
{
	meter_t<double> x(2.0);
	meter_t<double> y(3.0);
	square_meter_t<double> z(1.0);
	EXPECT_EQ(square_meter_t<double>(7.0), (units::fma(x, y, z)));
}

// Constexpr
TEST_F(Constexpr, construction)
{
	constexpr meter_t<double> result0(0);
	constexpr auto result1 = make_unit<meter_t<double>>(1);
	constexpr auto result2 = meter_t<double>(2);

	EXPECT_EQ(meter_t<double>(0), result0);
	EXPECT_EQ(meter_t<double>(1), result1);
	EXPECT_EQ(meter_t<double>(2), result2);

	EXPECT_TRUE(noexcept(meter_t<double>(0)));
	EXPECT_TRUE(noexcept(make_unit<meter_t<double>>(1)));
}

TEST_F(Constexpr, constants)
{
	EXPECT_TRUE(noexcept(constants::c()));
	EXPECT_TRUE(noexcept(constants::G()));
	EXPECT_TRUE(noexcept(constants::h()));
	EXPECT_TRUE(noexcept(constants::mu0()));
	EXPECT_TRUE(noexcept(constants::epsilon0()));
	EXPECT_TRUE(noexcept(constants::Z0()));
	EXPECT_TRUE(noexcept(constants::k_e()));
	EXPECT_TRUE(noexcept(constants::e()));
	EXPECT_TRUE(noexcept(constants::m_e()));
	EXPECT_TRUE(noexcept(constants::m_p()));
	EXPECT_TRUE(noexcept(constants::mu_B()));
	EXPECT_TRUE(noexcept(constants::N_A()));
	EXPECT_TRUE(noexcept(constants::R()));
	EXPECT_TRUE(noexcept(constants::k_B()));
	EXPECT_TRUE(noexcept(constants::F()));
	EXPECT_TRUE(noexcept(constants::sigma()));
}

TEST_F(Constexpr, arithmetic)
{
	[[maybe_unused]] constexpr auto result0(1.0_m + 1.0_m);
	[[maybe_unused]] constexpr auto result1(1.0_m - 1.0_m);
	[[maybe_unused]] constexpr auto result2(1.0_m * 1.0_m);
	[[maybe_unused]] constexpr auto result3(1.0_m / 1.0_m);
	[[maybe_unused]] constexpr auto result4(meter_t<double>(1) + meter_t<double>(1));
	[[maybe_unused]] constexpr auto result5(meter_t<double>(1) - meter_t<double>(1));
	[[maybe_unused]] constexpr auto result6(meter_t<double>(1) * meter_t<double>(1));
	[[maybe_unused]] constexpr auto result7(meter_t<double>(1) / meter_t<double>(1));
	[[maybe_unused]] constexpr auto result8(pow<2>(meter_t<double>(2)));
	constexpr auto result9  = pow<3>(2.0_m);
	constexpr auto result10 = 2.0_m * 2.0_m;

	EXPECT_TRUE(noexcept(1.0_m + 1.0_m));
	EXPECT_TRUE(noexcept(1.0_m - 1.0_m));
	EXPECT_TRUE(noexcept(1.0_m * 1.0_m));
	EXPECT_TRUE(noexcept(1.0_m / 1.0_m));
	EXPECT_TRUE(noexcept(meter_t<double>(1) + meter_t<double>(1)));
	EXPECT_TRUE(noexcept(meter_t<double>(1) - meter_t<double>(1)));
	EXPECT_TRUE(noexcept(meter_t<double>(1) * meter_t<double>(1)));
	EXPECT_TRUE(noexcept(meter_t<double>(1) / meter_t<double>(1)));
	EXPECT_TRUE(noexcept(pow<2>(meter_t<double>(2))));
	EXPECT_TRUE(noexcept(pow<3>(2.0_m)));
	EXPECT_TRUE(noexcept(2.0_m * 2.0_m));

	meter_t<double> m{42};
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
		meter_t<double> m{42};
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
	constexpr std::array<meter_t<double>, 5> arr{{0.0_m, 1.0_m, 2.0_m, 3.0_m, 4.0_m}};
	constexpr bool equal = (arr[3] == 3.0_m);
	EXPECT_TRUE(equal);
}

TEST_F(CaseStudies, radarRangeEquation)
{
	watt_t<double> P_t;           // transmit power
	dimensionless<double> G;      // gain
	meter_t<double> lambda;       // wavelength
	square_meter_t<double> sigma; // radar cross section
	meter_t<double> R;            // range
	kelvin_t<double> T_s;         // system noise temp
	hertz_t<double> B_n;          // bandwidth
	dimensionless<double> L;      // loss

	P_t    = megawatt_t<double>(1.4);
	G      = dB_t<double>(33.0);
	lambda = constants::c / megahertz_t<double>(2800);
	sigma  = square_meter_t<double>(1.0);
	R      = meter_t<double>(111000.0);
	T_s    = kelvin_t<double>(950.0);
	B_n    = megahertz_t<double>(1.67);
	L      = dB_t<double>(8.0);

	dimensionless<double> SNR = (P_t * pow<2>(G) * pow<2>(lambda) * sigma) /
		(pow<3>(4 * constants::pi) * pow<4>(R) * constants::k_B * T_s * B_n * L);

	EXPECT_NEAR(1.535, SNR(), 5.0e-4);
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
	const megabyte_t<int> data_size             = 100_MB;
	const megabytes_per_second_t<int> read_rate = 2_MBps;
	byte_t<int> read_progress                   = 10_MB;

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
