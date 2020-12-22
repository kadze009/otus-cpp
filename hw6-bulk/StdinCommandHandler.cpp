#include "StdinCommandHandler.hpp"

#include <algorithm>

#include "IBulkHandler.hpp"



void StdinCommandHandler::OnNewCmd(std::string&& cmd)
{
    if (cmd.empty()) { return; }
    bool const need_notify = [this](std::string&& cmd)
    {
        if (cmd == "{")
        {
            ++m_nest_count;
            return 1 == m_nest_count;
        }
        else if (cmd == "}")
        {
            --m_nest_count;
            return IsMainMode();
        }
        else
        {
            if (m_cmds.empty())
            {
                m_firstCmdTp = clock_t::now();
            }
            m_cmds.emplace_back(std::move(cmd));
            return IsMainMode()
                && m_cmds.size() == m_bulk_size;
        }
    }(std::move(cmd));
    if (need_notify)
    {
        NotifyAllHandlers();
        m_cmds.clear();
    }
}


void StdinCommandHandler::OnEof()
{
    if (IsMainMode()) { NotifyAllHandlers(); }
    m_cmds.clear();
}


void StdinCommandHandler::AddBulkHandler(IBulkHandler& bh)
{
    handlers_t& subs = m_handlers;
    auto it = std::find(subs.begin(), subs.end(), &bh);
    if (subs.end() == it) { subs.push_back(&bh); }
}


void StdinCommandHandler::NotifyAllHandlers()
{
    for (IBulkHandler* handler : m_handlers) { handler->OnBulk(); }
}

