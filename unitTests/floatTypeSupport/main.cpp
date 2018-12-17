#include <iostream>
#include <gtest/gtest.h>

#define UNIT_LIB_DEFAULT_TYPE float
#include <units.h>

TEST(UnitsWithFloatType, is_truly_unit_4_bytes_long)
{
    using namespace units::literals;
    using namespace units::length;

    auto l = 10_m;
    ASSERT_EQ(4, sizeof(l));
}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
