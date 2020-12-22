#include "StdoutBulkHandler.hpp"

#include <iostream>

#include "StdinCommandHandler.hpp"



StdoutBulkHandler::StdoutBulkHandler(StdinCommandHandler& cmd_handler)
    : m_cmdHandler(cmd_handler)
{
    m_cmdHandler.AddBulkHandler(*this);
}


void StdoutBulkHandler::OnBulk()
{
    auto const& bulk = m_cmdHandler.GetBulk();
    if (bulk.empty()) { return; }

    auto it   = bulk.begin();
    auto it_e = bulk.end();
    std::cout << "bulk: " << *it;
    for (++it; it != it_e; ++it)
    {
        std::cout << ", " << *it;
    }
    std::cout << '\n';
}

