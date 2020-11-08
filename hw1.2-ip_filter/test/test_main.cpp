#include <gtest/gtest.h>

#include <string>
#include <stdexcept>
#include <map>

#include "IpV4_c.hpp"
#include "utils.hpp"



TEST(IpV4, assignment_succ)
{
    const std::string exp_ip {"1.2.3.4"};
    ASSERT_NO_THROW({ IpV4_c{exp_ip}; });
    IpV4_c ip {exp_ip};
    ASSERT_EQ(exp_ip, ip.toString()) << ip.toString();

    const std::string exp_assign_ip {"11.22.33.44"};
    ASSERT_NO_THROW({ ip.assign(exp_assign_ip); });
    ASSERT_EQ(exp_assign_ip, ip.toString()) << ip.toString();
}


TEST(IpV4, assignWrongDigsNum_fail)
{
    const std::vector<std::string> wrong_ips = {
        "1.2.3.4.5",
        "1.2.3",
    };
    for (const std::string& wrong_ip : wrong_ips)
    {
        ASSERT_THROW(IpV4_c{wrong_ip}, std::runtime_error) << wrong_ip;
    }
}


TEST(IpV4, assignTooBigDigit_fail)
{
    const std::vector<std::string> wrong_ips = {
        "666.2.3.4",
        "1.2.3.666",
    };
    for (const std::string& wrong_ip : wrong_ips)
    {
        ASSERT_THROW(IpV4_c{wrong_ip}, std::runtime_error) << wrong_ip;
    }
}


TEST(IpV4, operatorLess_succ)
{
    IpV4_c ip_1 {"1.2.3.4"};
    IpV4_c ip_2 {"1.2.3.5"};
    ASSERT_TRUE(ip_1 < ip_2) << "IpV4[" << ip_1 << "] < IpV4[" << ip_2 << "]";
}


TEST(IpV4, match_succ)
{
    constexpr int SKIP = IpV4_c::MATCH_SKIP_BYTE;

    const IpV4_c ip {"1.2.3.4"};
    const std::vector<IpV4_c::mask_t> masks =
    {
        { SKIP, SKIP, SKIP, SKIP },
        {    1, SKIP, SKIP, SKIP },
        {    1,    2, SKIP, SKIP },
        { SKIP,    2, SKIP, SKIP },
        { SKIP,    2,    3, SKIP },
        { SKIP,    2,    3,    4 },
        {    1,    2,    3,    4 },
    };

    for (const auto& mask : masks)
    {
        EXPECT_TRUE(ip.match(mask))
            << "IpV4[" << ip.toString() << "] and Mask["
            << mask[0] << ',' << mask[1] << ','
            << mask[2] << ',' << mask[3] << ']';
    }
}


TEST(IpV4, match_fail)
{
    constexpr int SKIP = IpV4_c::MATCH_SKIP_BYTE;

    const IpV4_c ip {"1.2.3.4"};
    const std::vector<IpV4_c::mask_t> masks =
    {
        {    2, SKIP, SKIP, SKIP },
        { SKIP,    3, SKIP, SKIP },
        {    1,    3, SKIP, SKIP },
        { SKIP,    2,    4, SKIP },
        { SKIP,    2,    3,    5 },
        {    1,    2,    3,    5 },
    };

    for (const auto& mask : masks)
    {
        EXPECT_FALSE(ip.match(mask))
            << "IpV4[" << ip.toString() << "] and Mask["
            << mask[0] << ',' << mask[1] << ','
            << mask[2] << ',' << mask[3] << ']';
    }
}


TEST(IpV4, hasByte)
{
    IpV4_c ip {"1.2.3.4"};

    for (uint8_t byte : {1, 2, 3, 4})
    {
        EXPECT_TRUE(ip.has_byte(byte)) << "Expected byte: " << (int)byte;
    }
    for (uint8_t byte : {5, 10})
    {
        EXPECT_FALSE(ip.has_byte(byte)) << "Unexpected byte: " << (int)byte;
    }
}


TEST(IpV4, sort)
{
	ip_pool_t ip_pool =
	{
		{"3.2.1.4"},
		{"1.2.3.4"},
		{"11.12.13.14"},
		{"55.66.77.88"},
		{"3.2.10.1"},
		{"10.11.12.13"},
		{"1.2.3.5"},
	};
	// Forward sort
	std::sort(ip_pool.begin(), ip_pool.end());
	{
		const std::vector<std::string> exp_ips =
		{
			"1.2.3.4",
			"1.2.3.5",
			"3.2.1.4",
			"3.2.10.1",
			"10.11.12.13",
			"11.12.13.14",
			"55.66.77.88",
		};
		for (size_t i = 0; i < exp_ips.size(); ++i)
		{
			EXPECT_EQ(exp_ips[i], ip_pool[i].toString()) << "Index " << i;
		}
	}
	// Reverse sort
	std::sort(ip_pool.rbegin(), ip_pool.rend());
	{
		const std::vector<std::string> exp_ips =
		{
			"55.66.77.88",
			"11.12.13.14",
			"10.11.12.13",
			"3.2.10.1",
			"3.2.1.4",
			"1.2.3.5",
			"1.2.3.4",
		};
		for (size_t i = 0; i < exp_ips.size(); ++i)
		{
			EXPECT_EQ(exp_ips[i], ip_pool[i].toString()) << "Index " << i;
		}
	}
}



TEST(Utils, filter)
{
    constexpr int SKIP = IpV4_c::MATCH_SKIP_BYTE;
    IpV4_c::mask_t mask;
    ip_pool_t ip_pool =
    {
        {"1.2.3.4"},
        {"1.3.3.5"},
        {"1.3.3.5"},
        {"2.2.2.4"},
    };

    using exp_ips_t = std::vector<std::string>;
    auto check_filter = [&ip_pool, &mask](int lineno, const exp_ips_t& exp_ips) -> bool
    {
        filtered_ip_pool_t fp = utils::filter(ip_pool, mask);
        if (fp.size() != exp_ips.size())
        {
            std::cerr << "[line:" << lineno << "] Unexpected number of IPs: "
                      << "exp=" << exp_ips.size() << " "
                      << "act=" << fp.size() << '\n';
            return false;
        }
        for (size_t i = 0; i < fp.size(); ++i)
        {
            if (exp_ips[i] != fp[i]->toString())
            {
                std::cerr << "[line:" << lineno << "] "
                          << "Detected invalid IP #" << i << ": "
                          << "exp = [" << exp_ips[i] << "] "
                          << "act = [" << fp[i]->toString() << "]\n";
                return false;
            }
        }
        return true;
    };

#define CHECK(...) ASSERT_TRUE(check_filter(__LINE__, {__VA_ARGS__}))
    mask = { SKIP, SKIP, SKIP, SKIP };
    CHECK("1.2.3.4", "1.3.3.5", "1.3.3.5", "2.2.2.4");

    mask = { 1, SKIP, SKIP, SKIP };
    CHECK("1.2.3.4", "1.3.3.5", "1.3.3.5");

    mask = { SKIP, 2, SKIP, 4 };
    CHECK("1.2.3.4", "2.2.2.4");

    mask = { SKIP, SKIP, 3, SKIP };
    CHECK("1.2.3.4", "1.3.3.5", "1.3.3.5");

    mask = { SKIP, SKIP, SKIP, 4 };
    CHECK("1.2.3.4", "2.2.2.4");

    mask = { 1, 2, 3, 4 };
    CHECK("1.2.3.4");

    mask = { 4, SKIP, SKIP, SKIP };
    CHECK();

    mask = { 4, 2, 3, 4 };
    CHECK();
#undef CHECK
}


TEST(Utils, filter_any)
{
    ip_pool_t ip_pool =
    {
        {"1.2.3.4"},
        {"1.3.3.5"},
        {"1.3.3.5"},
        {"2.2.2.4"},
    };
    uint8_t check_byte = 0;

    using exp_ips_t = std::vector<std::string>;
    auto check_filter_any = [&ip_pool, &check_byte](int lineno, const exp_ips_t& exp_ips) -> bool
    {
        filtered_ip_pool_t fp = utils::filter_any(ip_pool, check_byte);
        if (fp.size() != exp_ips.size())
        {
            std::cerr << "[line:" << lineno << "] Unexpected number of IPs: "
                      << "exp=" << exp_ips.size() << " "
                      << "act=" << fp.size() << '\n';
            return false;
        }
        for (size_t i = 0; i < fp.size(); ++i)
        {
            if (exp_ips[i] != fp[i]->toString())
            {
                std::cerr << "[line:" << lineno << "] "
                          << "Detected invalid IP #" << i << ": "
                          << "exp = [" << exp_ips[i] << "] "
                          << "act = [" << fp[i]->toString() << "]\n";
                return false;
            }
        }
        return true;
    };

#define CHECK(...) ASSERT_TRUE(check_filter_any(__LINE__, {__VA_ARGS__}))
    check_byte = 1;
    CHECK("1.2.3.4", "1.3.3.5", "1.3.3.5");

    check_byte = 3;
    CHECK("1.2.3.4", "1.3.3.5", "1.3.3.5");

    check_byte = 5;
    CHECK("1.3.3.5", "1.3.3.5");

    check_byte = 10;
    CHECK();
#undef CHECK
}


TEST(Utils, split)
{
    using parts_t = std::vector<std::string>;
    auto check_split =
        [](int lineno, const parts_t& act_parts, const parts_t& exp_parts) -> bool
    {
        if (exp_parts.size() != act_parts.size())
        {
            std::cerr << "[line:" << lineno << "] Unexpexted number of parts: "
                      << "exp = " << exp_parts.size() << ' '
                      << "act = " << act_parts.size() << ".\nAct parts: ";
            for (const std::string& act_part : act_parts)
            {
                std::cerr << "[" << act_part << "], ";
            }
            return false;
        }
        for (size_t i = 0; i < exp_parts.size(); ++i)
        {
            if (exp_parts[i] != act_parts[i])
            {
                std::cerr << "[line:" << lineno << "] Unexpected part #" << i << ": "
                          << "exp = [" << exp_parts[i] << "] "
                          << "act = [" << act_parts[i] << "]\n";
                return false;
            }
        }
        return true;
    };

#define CHECK(str, delim, ...) EXPECT_TRUE(check_split(__LINE__, split(str, delim), {__VA_ARGS__}))
    CHECK("",                '.',     "");
    CHECK("11",              '.',     "11");
    CHECK("..",              '.',     "", "", "");
    CHECK("11.",             '.',     "11", "");
    CHECK(".11",             '.',     "", "11");
    CHECK("11.22",           '.',     "11", "22");
    CHECK("1.2.3\t1\t10",    '\t',    "1.2.3", "1", "10");
#undef CHECK
}




int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

