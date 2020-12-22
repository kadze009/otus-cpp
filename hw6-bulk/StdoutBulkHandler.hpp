#pragma once

#include "IBulkHandler.hpp"



class StdinCommandHandler;


struct StdoutBulkHandler : public IBulkHandler
{
public:
    explicit StdoutBulkHandler(StdinCommandHandler&);
    ~StdoutBulkHandler() override = default;
    StdoutBulkHandler(StdoutBulkHandler const&)             = delete;
    StdoutBulkHandler& operator= (StdoutBulkHandler const&) = delete;

    void OnBulk() override;

private:
    StdinCommandHandler&    m_cmdHandler;    //TODO: unsafe owning. Should be weak_ptr
};

