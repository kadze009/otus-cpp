#include <gtest/gtest.h>

#include "custom_list.hpp"
#include "custom_allocator.hpp"



TEST(CustomListAndAlloc, sanity)
{
    constexpr size_t MAX_CAPACITY = 10;
    using alloc_t = custom_allocator<size_t>;
    alloc_t alloc {MAX_CAPACITY};
    custom_list<size_t, alloc_t> list {alloc};
    EXPECT_TRUE(list.empty());

    for (size_t i = 0; i < MAX_CAPACITY; ++i)
    {
        ASSERT_NO_THROW({ list.push_back(i); }) << "i = " << i;
    }
    EXPECT_EQ(MAX_CAPACITY, list.size());
    ASSERT_THROW(list.push_back(1), std::bad_alloc);
    EXPECT_EQ(MAX_CAPACITY, list.size());

    ASSERT_NO_THROW({ list.clear(); });
    EXPECT_TRUE(list.empty());
}


TEST(CustomListAndAlloc, emptyCustomAllocator)
{
    custom_list<size_t, custom_allocator<size_t>> list;
    EXPECT_TRUE(list.empty());
    ASSERT_THROW(list.push_back(1), std::bad_alloc);
    EXPECT_TRUE(list.empty());
}



int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

