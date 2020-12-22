#pragma once



struct IBulkHandler
{
    virtual ~IBulkHandler() {}

    virtual void OnBulk() = 0;
};

