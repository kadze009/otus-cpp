#include <gtest/gtest.h>
#include <map>
#include <vector>

#include "custom_allocator.hpp"

#define UNUSED(a)    (void)a


TEST(CustomAllocator, sanity)
{
    constexpr size_t EXP_CAPACITY = 5;
    using alloc_t  = custom_allocator<int>;
    using alloc_tt = std::allocator_traits<alloc_t>;
    custom_allocator<int> alloc {EXP_CAPACITY};

    EXPECT_EQ(EXP_CAPACITY, alloc.capacity());
    EXPECT_EQ(0, alloc.size());

    ASSERT_NO_THROW({ alloc.capacity(EXP_CAPACITY + 1); });
    EXPECT_EQ(0, alloc.size());
    EXPECT_EQ(EXP_CAPACITY + 1, alloc.capacity());

    ASSERT_NO_THROW({ alloc.capacity(EXP_CAPACITY - 1); });
    EXPECT_EQ(0, alloc.size());
    EXPECT_EQ(EXP_CAPACITY - 1, alloc.capacity());

    ASSERT_NO_THROW({ alloc.capacity(EXP_CAPACITY); });
    EXPECT_EQ(0, alloc.size());
    EXPECT_EQ(EXP_CAPACITY, alloc.capacity());

    int* p2i = nullptr;
    ASSERT_NO_THROW({ p2i = alloc_tt::allocate(alloc, 2); });
    ASSERT_NE(nullptr, p2i);
    EXPECT_EQ(EXP_CAPACITY, alloc.capacity());
    EXPECT_EQ(2, alloc.size());
    EXPECT_EQ(EXP_CAPACITY - 2, alloc_tt::max_size(alloc));

    ASSERT_NO_THROW({ alloc_tt::construct(alloc, p2i,     1); });
    ASSERT_NO_THROW({ alloc_tt::construct(alloc, p2i + 1, 2); });
    EXPECT_EQ(*p2i,     1);
    EXPECT_EQ(*p2i + 1, 2);

    ASSERT_THROW(alloc.capacity(EXP_CAPACITY - 1), std::runtime_error);
    ASSERT_THROW(alloc.capacity(EXP_CAPACITY + 1), std::runtime_error);
    ASSERT_THROW(UNUSED(alloc_tt::allocate(alloc, alloc_tt::max_size(alloc) + 1)), std::bad_alloc);
    EXPECT_EQ(2, alloc.size());
    EXPECT_EQ(EXP_CAPACITY - 2, alloc_tt::max_size(alloc));

    int* pLEFTi = nullptr;
    size_t const LEFT_NUM = alloc_tt::max_size(alloc);
    ASSERT_NO_THROW({ pLEFTi = alloc_tt::allocate(alloc, LEFT_NUM); });
    ASSERT_NE(nullptr, pLEFTi);
    EXPECT_EQ(EXP_CAPACITY, alloc.capacity());
    EXPECT_EQ(EXP_CAPACITY, alloc.size());
    EXPECT_EQ(0, alloc_tt::max_size(alloc));
    ASSERT_THROW(UNUSED(alloc_tt::allocate(alloc, 1)), std::bad_alloc);

    alloc_tt::deallocate(alloc, pLEFTi, LEFT_NUM);
    EXPECT_EQ(0, alloc_tt::max_size(alloc));
    EXPECT_EQ(EXP_CAPACITY, alloc.size());

    ASSERT_NO_THROW({ alloc_tt::destroy(alloc, p2i + 1); });
    ASSERT_NO_THROW({ alloc_tt::destroy(alloc, p2i); });
    ASSERT_NO_THROW({ alloc_tt::deallocate(alloc, p2i, 2); });
    EXPECT_EQ(0, alloc_tt::max_size(alloc));
    EXPECT_EQ(EXP_CAPACITY, alloc.capacity());
    EXPECT_EQ(EXP_CAPACITY, alloc.size());
}


TEST(CustomAllocator, defCtor)
{
    using alloc_t  = custom_allocator<int>;
    using alloc_tt = std::allocator_traits<alloc_t>;
    alloc_t alloc;
    EXPECT_EQ(0, alloc.capacity());
    EXPECT_EQ(0, alloc.size());
    EXPECT_EQ(0, alloc_tt::max_size(alloc));
    ASSERT_THROW(UNUSED(alloc_tt::allocate(alloc, 1)), std::bad_alloc);
}


TEST(CustomAllocator, capacityCtor)
{
    using alloc_t  = custom_allocator<int>;
    using alloc_tt = std::allocator_traits<alloc_t>;
    alloc_t alloc {5};
    EXPECT_EQ(5, alloc.capacity());
    EXPECT_EQ(0, alloc.size());
    EXPECT_EQ(5, alloc_tt::max_size(alloc));
}


TEST(CustomAllocator, copyCtor)
{
    custom_allocator<int> alloc_1 {3};
    EXPECT_EQ(3, alloc_1.capacity());
    EXPECT_EQ(0, alloc_1.size());
    int* pi = nullptr;
    ASSERT_NO_THROW({ pi = alloc_1.allocate(1); });
    ASSERT_NE(nullptr, pi);
    EXPECT_EQ(1, alloc_1.size());

    custom_allocator<double> alloc_2 {alloc_1};
    EXPECT_EQ(3, alloc_2.capacity());
    EXPECT_EQ(0, alloc_2.size());
    ASSERT_FALSE(alloc_1 == alloc_2);
    ASSERT_FALSE(alloc_1 != alloc_2);

    custom_allocator<int> alloc_3 = alloc_1;
    EXPECT_EQ(3, alloc_3.capacity());
    EXPECT_EQ(0, alloc_3.size());
    ASSERT_FALSE(alloc_1 == alloc_3);
    ASSERT_FALSE(alloc_1 != alloc_3);

    ASSERT_NO_THROW({ alloc_1.deallocate(pi, 1); });
    EXPECT_EQ(1, alloc_1.size());
}


TEST(CustomAllocator, DISABLED_inVector_unexpectedCunstruct)
{
    using alloc_t = custom_allocator<int>;
    alloc_t alloc {3};
    std::vector<int, decltype(alloc)> v {alloc};
    ASSERT_EQ(0, v.capacity());
    ASSERT_THROW(v.reserve(10), std::length_error);
    EXPECT_EQ(3, v.get_allocator().capacity());
    ASSERT_NO_THROW({ v.push_back(5); });
    ASSERT_EQ(1, v.capacity());
    ASSERT_NO_THROW({ v.push_back(6); });
    ASSERT_EQ(2, v.capacity());

    //NOTE: I expected that std::vector threw ANY exception because
    // custom_allocator had retured 0 from max_size(). But std::vector
    // called construct(p) with invalid p (I've seen p=0x08).
    // It seems to be the bug in the std::vector implementation.
    ASSERT_THROW(v.push_back(7), std::bad_alloc);
}


TEST(CustomAllocator, DISABLED_inVector_vectorUseUnallocatedMemory)
{
    using alloc_t = custom_allocator<int>;
    alloc_t alloc {4};
    std::vector<int, decltype(alloc)> v {alloc};
    ASSERT_EQ(0, v.capacity());
    ASSERT_THROW(v.reserve(10), std::length_error);
    EXPECT_EQ(4, v.get_allocator().capacity());
    ASSERT_NO_THROW({ v.push_back(5); });
    ASSERT_EQ(1, v.capacity());
    ASSERT_NO_THROW({ v.push_back(6); });
    ASSERT_EQ(2, v.capacity());

    //NOTE: I expected that std::vector threw ANY exception because
    // custom_allocator had retured 1 from max_size(). But std::vector
    // allocated this one object and then used it and next two pointers
    // (the last two pointers pointed on free memory.
    // It seems to be the bug in the std::vector implementation.
    ASSERT_THROW(v.push_back(7), std::bad_alloc);
}


TEST(CustomAllocator, inMap)
{
    using alloc_t = custom_allocator<std::pair<const int, int>>;
    alloc_t alloc {2};
    std::map<int, int, std::less<int>, alloc_t> map {alloc};
    ASSERT_NO_THROW({ map[1] = 1; });
    ASSERT_NO_THROW({ map[2] = 2; });
    ASSERT_THROW(map.insert({3,3}), std::bad_alloc);
}
