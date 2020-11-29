#include <iostream>
#include <map>
#include <vector>

#include "custom_allocator.hpp"
#include "custom_list.hpp"



std::vector<int> GenFibonachiSeq(size_t const qty)
{
    std::vector<int> memo;
    memo.reserve(qty);
    for (size_t i = 0; i < qty; ++i)
    {
        if (i > 1) { memo.push_back(memo[i-1] + memo[i-2]); }
        else       { memo.push_back(1); }
    }
    return memo;
}


int main()
{
    constexpr size_t MAX_NUM = 10;
    std::vector<int> memo = GenFibonachiSeq(MAX_NUM);

    {
        std::map<size_t, int> map_std_alloc;
        for (size_t i = 0; i < memo.size(); ++i) { map_std_alloc[i] = memo[i]; }

        using alloc_t = custom_allocator<std::pair<const size_t,int>>;
        alloc_t alloc;
        alloc.capacity(memo.size());

        std::map<size_t, int, std::less<size_t>, decltype(alloc)> map_alloc {alloc};
        for (size_t i = 0; i < memo.size(); ++i) { map_alloc[i] = memo[i]; }

        std::cout << "#\tstd\tcustom\n";
        for (size_t i = 0; i < memo.size(); ++i)
        {
            std::cout << i << '.'
                      << "\t" << map_std_alloc[i]
                      << '\t' << map_alloc[i] << '\n';
        }
        std::cout << "-------------------\n\n";
    }
    {
        custom_list<int> list_std_alloc;
        for (size_t i = 0; i < MAX_NUM; ++i) { list_std_alloc.push_back(memo[i]); }

        using alloc_t = custom_allocator<int>;
        alloc_t alloc;
        alloc.capacity(MAX_NUM);

        custom_list<int, alloc_t> list_alloc {alloc};
        for (size_t i = 0; i < MAX_NUM; ++i) { list_alloc.push_back(memo[i]); }

        std::cout << "---[ std ]---\n";
        for (int num : list_std_alloc) { std::cout << num << ' '; }
        std::cout << "\n-------------\n\n";

        std::cout << "---[ custom ]---\n";
        for (int num : list_alloc) { std::cout << num << ' '; }
        std::cout << "\n----------------\n\n";
    }
    return 0;
}

