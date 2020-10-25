#include <gtest/gtest.h>
#include <charconv>
#include <string_view>
#include "lib.h"



TEST(SimpleTest, test_valid_version)
{
    ASSERT_NE(0, version());
}

TEST(Cpp17Test, charconv)
{
	std::string_view str_num {"123123"};
	int num = 0;
	auto [_, ec] = std::from_chars(str_num.begin(), str_num.end(), num);
	ASSERT_EQ(std::errc(), ec) << std::make_error_code(ec);
	ASSERT_EQ(123123, num);
}



int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

