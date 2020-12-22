#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#include "StdinCommandHandler.hpp"
#include "StdoutBulkHandler.hpp"



namespace {

struct CounterBulkHandler : public IBulkHandler
{
    CounterBulkHandler() = default;
    ~CounterBulkHandler() = default;
    CounterBulkHandler(CounterBulkHandler const&)            = delete;
    CounterBulkHandler& operator=(CounterBulkHandler const&) = delete;

    // IBulkHandler
    void OnBulk() override
    {
        ++num_OnBulk;
        last_bulk_size = m_handler->GetBulk().size();
    }

    void Reset(StdinCommandHandler& handler)
    {
        m_handler = &handler;
        handler.AddBulkHandler(*this);
        num_OnBulk = 0;
        last_bulk_size = 0;
    }

private:
    StdinCommandHandler const* m_handler = nullptr;

public:
    size_t    num_OnBulk     = 0;
    size_t    last_bulk_size = 0;
};

} // namespace



TEST(StdinCommandHandler, SanityWoBraces)
{
    auto gen_vector = []()
    {
        return std::vector<std::string>{ "c1", "c2", "c3", "c4", "c5", };
    };
    CounterBulkHandler counter_bulk_handler;
    {
        StdinCommandHandler cmd_handler{1};
        counter_bulk_handler.Reset(cmd_handler);
        auto lines = gen_vector();
        for (std::string& line : lines) { cmd_handler.OnNewCmd(std::move(line)); }
        EXPECT_EQ(lines.size(), counter_bulk_handler.num_OnBulk);
        EXPECT_EQ(1,            counter_bulk_handler.last_bulk_size);
        cmd_handler.OnEof();
        EXPECT_EQ(lines.size() + 1, counter_bulk_handler.num_OnBulk);
        ASSERT_EQ(0,                counter_bulk_handler.last_bulk_size);
    }
    {
        StdinCommandHandler cmd_handler{2};
        counter_bulk_handler.Reset(cmd_handler);
        auto lines = gen_vector();
        for (std::string& line : lines) { cmd_handler.OnNewCmd(std::move(line)); }
        size_t exp_size = lines.size() / 2;
        EXPECT_EQ(exp_size, counter_bulk_handler.num_OnBulk);
        ASSERT_EQ(2,        counter_bulk_handler.last_bulk_size);
        cmd_handler.OnEof();
        EXPECT_EQ(exp_size + 1, counter_bulk_handler.num_OnBulk);
        ASSERT_EQ(1,            counter_bulk_handler.last_bulk_size);
    }
}


TEST(StdinCommandHandler, SanityWBraces)
{
    auto gen_vector = []()
    {
        return std::vector<std::string>
        {
            "c1", "c2", "{", "c4", "{", "c5", "c6", "}", "}", "c7", "c8",
        };
    };
    CounterBulkHandler counter_bulk_handler;
    {
        StdinCommandHandler cmd_handler{1};
        counter_bulk_handler.Reset(cmd_handler);
        auto lines = gen_vector();
        for (std::string& line : lines) { cmd_handler.OnNewCmd(std::move(line)); }
        EXPECT_EQ(6, counter_bulk_handler.num_OnBulk); // 5 + the first '{'
        ASSERT_EQ(1, counter_bulk_handler.last_bulk_size);
        cmd_handler.OnEof();
        EXPECT_EQ(7, counter_bulk_handler.num_OnBulk);
        ASSERT_EQ(0, counter_bulk_handler.last_bulk_size);
    }
    {
        StdinCommandHandler cmd_handler{4};
        counter_bulk_handler.Reset(cmd_handler);
        auto lines = gen_vector();
        for (std::string& line : lines) { cmd_handler.OnNewCmd(std::move(line)); }
        EXPECT_EQ(2, counter_bulk_handler.num_OnBulk);
        ASSERT_EQ(3, counter_bulk_handler.last_bulk_size);
        cmd_handler.OnEof();
        EXPECT_EQ(3, counter_bulk_handler.num_OnBulk);
        ASSERT_EQ(2, counter_bulk_handler.last_bulk_size);
    }
    {
        StdinCommandHandler cmd_handler{4};
        counter_bulk_handler.Reset(cmd_handler);
        auto lines = gen_vector();
        // 7 -- (index of "c6") + 1
        for (size_t i = 0; i < 7; ++i) { cmd_handler.OnNewCmd(std::move(lines[i])); }
        EXPECT_EQ(1, counter_bulk_handler.num_OnBulk);
        ASSERT_EQ(2, counter_bulk_handler.last_bulk_size);
        cmd_handler.OnEof();
        EXPECT_EQ(1, counter_bulk_handler.num_OnBulk);
        ASSERT_EQ(2, counter_bulk_handler.last_bulk_size);
    }
}


TEST(StdoutBulkHandler, Sanity)
{
    std::streambuf* coutBuf = std::cout.rdbuf();
    std::ostringstream strCout;
    std::cout.rdbuf(strCout.rdbuf());

    std::vector<std::string> lines =
    {
        "c1", "c2", "{", "c4", "{", "c5", "c6", "}", "}", "c7", "c8",
    };
    StdinCommandHandler cmd_handler{4};
    StdoutBulkHandler   bulk_handler{cmd_handler};
    for (std::string& line : lines) { cmd_handler.OnNewCmd(std::move(line)); }
    cmd_handler.OnEof();
    std::cout.rdbuf(coutBuf);
    char const* const exp_output =
        "bulk: c1, c2\n"
        "bulk: c4, c5, c6\n"
        "bulk: c7, c8\n";
    ASSERT_STREQ(exp_output, strCout.str().c_str());
}



int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

