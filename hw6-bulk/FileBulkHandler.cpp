#include "FileBulkHandler.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include "StdinCommandHandler.hpp"



FileBulkHandler::FileBulkHandler(StdinCommandHandler& cmd_handler)
    : m_cmdHandler(cmd_handler)
{
    cmd_handler.AddBulkHandler(*this);
}


void FileBulkHandler::OnBulk()
{
    auto const& bulk = m_cmdHandler.GetBulk();
    if (bulk.empty()) { return; }

    auto tp = m_cmdHandler.FirstCmdTimePoint();
    std::stringstream ss;
    ss << "bulk"
       << std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count()
       << ".log";
    std::string log_name = ss.str();
    std::ofstream bulk_log {log_name};
    if (not bulk_log.is_open())
    {
        std::cerr << "Can't create log file [" << log_name << "]\n";
        return;
    }

    auto it   = bulk.begin();
    auto it_e = bulk.end();
    bulk_log << *it;
    for (++it; it != it_e; ++it)
    {
        bulk_log << ' ' << *it;
    }
    bulk_log << '\n';
}

