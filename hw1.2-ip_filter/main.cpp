#include <iostream>
#include <algorithm>

#include "IpV4_c.hpp"
#include "utils.hpp"



ip_pool_t make_ip_pool()
{
    ip_pool_t ip_pool;
    for(std::string line; std::getline(std::cin, line);)
    {
        std::vector<std::string> columns = split(line, '\t');
        ip_pool.emplace_back(columns.at(0));
    }
    return ip_pool;
}


int main()
{
    try
    {
        ip_pool_t ip_pool = make_ip_pool();

        std::sort(ip_pool.rbegin(), ip_pool.rend());

        utils::print(ip_pool);

        using mask_t = IpV4_c::mask_t;
        mask_t mask;
        std::fill(mask.begin(), mask.end(), IpV4_c::MATCH_SKIP_BYTE);

        mask[0] = 1;
        utils::print(utils::filter(ip_pool, mask));

        mask[0] = 46; mask[1] = 70;
        utils::print(utils::filter(ip_pool, mask));

        utils::print(utils::filter_any(ip_pool, 46));
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

