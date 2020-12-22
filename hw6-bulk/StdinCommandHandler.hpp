#pragma once

#include <cstdint>
#include <string>
#include <deque>
#include <vector>
#include <chrono>



struct IBulkHandler;


class StdinCommandHandler
{
public:
    using cmds_t      = std::deque<std::string>;
    using handlers_t  = std::vector<IBulkHandler*>;
    using clock_t     = std::chrono::system_clock;
    using clock_now_t = decltype(clock_t::now());

    explicit StdinCommandHandler(size_t bulk_size)
        : m_bulk_size(bulk_size)
        , m_firstCmdTp(clock_t::now())
    {
        constexpr size_t INIT_SUBS_CAPACITY = 2;
        m_handlers.reserve(INIT_SUBS_CAPACITY);
    }
    ~StdinCommandHandler() = default;
    StdinCommandHandler(StdinCommandHandler const&)            = delete;
    StdinCommandHandler& operator=(StdinCommandHandler const&) = delete;

    void OnNewCmd(std::string&&);
    void OnEof();

    cmds_t const& GetBulk()         const noexcept { return m_cmds; }
    clock_now_t FirstCmdTimePoint() const noexcept { return m_firstCmdTp; }

    void AddBulkHandler(IBulkHandler&);

private:
    void NotifyAllHandlers();
    bool IsMainMode() const noexcept { return 0 == m_nest_count; }

private:
    size_t const    m_bulk_size  = 0;
    size_t          m_nest_count = 0;
    clock_now_t     m_firstCmdTp;
    cmds_t          m_cmds;
    handlers_t      m_handlers;
};

