#include <gtest/gtest.h>

#include "custom_list.hpp"


namespace {

struct CountedClass
{
	static size_t counter;

	CountedClass() noexcept                    { counter += 1; }
	CountedClass(CountedClass const&) noexcept { counter += 1; }
	~CountedClass()                            { counter -= 1; }
};

size_t CountedClass::counter = 0;

} // namespace



TEST(CustomList, sanity)
{
	custom_list<size_t> list;
	EXPECT_EQ(0, list.size());
	EXPECT_TRUE(list.empty());
	EXPECT_TRUE(not list);

	constexpr size_t EXP_LIST_SIZE = 10;
	for (size_t i = 0; i < EXP_LIST_SIZE; ++i) { list.push_back(i); }
	EXPECT_EQ(EXP_LIST_SIZE, list.size());
	EXPECT_FALSE(list.empty());
	EXPECT_FALSE(not list);

	auto it   = list.begin();
	auto it_e = list.end();
	EXPECT_EQ(EXP_LIST_SIZE, std::distance(it, it_e));

	for (size_t i = 0; i < list.size(); ++i)
	{
		EXPECT_EQ(i, *it);
		++it;
	}
	EXPECT_EQ(it_e, it);
	EXPECT_EQ(0, std::distance(it, it_e));

	ASSERT_NO_THROW({ list.clear(); });
	EXPECT_EQ(0, list.size());
	EXPECT_TRUE(list.empty());
	EXPECT_TRUE(not list);
}


TEST(CustomList, checkCtorsDtors)
{
	custom_list<CountedClass> list;
	EXPECT_TRUE(list.empty());
	EXPECT_EQ(0, CountedClass::counter);

	constexpr size_t EXP_LIST_SIZE = 10;
	for (size_t i = 0; i < EXP_LIST_SIZE; ++i) { list.push_back(CountedClass{}); }
	EXPECT_EQ(EXP_LIST_SIZE, list.size());
	ASSERT_EQ(EXP_LIST_SIZE, CountedClass::counter);

	ASSERT_NO_THROW({ list.clear(); });
	EXPECT_TRUE(list.empty());
	ASSERT_EQ(0, CountedClass::counter);
}

