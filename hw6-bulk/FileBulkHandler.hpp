#pragma once

#include "IBulkHandler.hpp"



class StdinCommandHandler;


struct FileBulkHandler : public IBulkHandler
{
public:
    explicit FileBulkHandler(StdinCommandHandler&);
    FileBulkHandler(FileBulkHandler const&)            = delete;
    FileBulkHandler& operator=(FileBulkHandler const&) = delete;

    void OnBulk() override;

private:
    StdinCommandHandler const&    m_cmdHandler;
};

