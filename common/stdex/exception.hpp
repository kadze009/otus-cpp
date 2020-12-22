#pragma once

#include <exception>

#include <cstdarg>



namespace stdex {

class exception : public std::exception
{
public:
    static constexpr size_t MAX_MSG_SIZE = 256;

    exception(char const* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        vsnprintf(m_msg, sizeof(m_msg), fmt, args);
        va_end(args);
    }
    exception(exception const&)            = default;
    exception& operator=(exception const&) = default;

    char const* what() const noexcept override { return m_msg; }

private:
    char    m_msg[MAX_MSG_SIZE];
};

} // namespace stdex

