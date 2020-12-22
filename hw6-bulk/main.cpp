#include <iostream>
#include <charconv>

#include "StdinCommandHandler.hpp"
#include "StdoutBulkHandler.hpp"
#include "FileBulkHandler.hpp"

#include "debug.hpp"
#include "stdex/exception.hpp"


namespace {

struct ArgParser
{
    ArgParser(int argc, char** argv)
        : m_argc{argc}
        , m_argv{argv}
    {
        if (m_argc != 2)
        {
            throw stdex::exception(
                "unexpected number of arguments: exp=2, act=%d",
                m_argc);
        }
    }

    size_t BulkSize() const
    {
        size_t bulk_size = 0;
        std::string_view arg {m_argv[1]};
        auto[ptr, ec] = std::from_chars(arg.begin(), arg.end(), bulk_size);
        if (ec != std::errc())
        {
            throw stdex::exception(
                "Can't convert argument [%.*s] to a number: %s",
                (int)arg.size(), arg.data(),
                std::make_error_code(ec).message().c_str());
        }
        else if (ptr != arg.end())
        {
            throw stdex::exception(
                "Can't convert argument [%.*s] to a number: "
                "[%s] is not the tail of the number",
                (int)arg.size(), arg.data(), ptr);
        }
        return bulk_size;
    }

    static char const* Usage() noexcept
    {
        return "Usage: bulk <BULK-SIZE>";
    }

private:
    int    const    m_argc;
    char** const    m_argv;
};

} // namespace



int main(int argc, char* argv[])
{
    int ret_code = 0;
    try
    {
        ArgParser arg_parser {argc, argv};
        StdinCommandHandler stdin_ch {arg_parser.BulkSize()};

        //TODO: there is a problem that `IBulkHandler`s have to be created
        // after `ICommandHandler`s because `ICommandHandler`s can be used by
        // `IBulkHandler`s' dtors.
        StdoutBulkHandler stdout_bh   {stdin_ch};
        FileBulkHandler   file_bh     {stdin_ch};

        for (std::string line; std::getline(std::cin, line);)
        {
            stdin_ch.OnNewCmd(std::move(line));
        }
        stdin_ch.OnEof();
    }
    catch (std::exception const& ex)
    {
        std::cerr << "Fatal error: " << ex.what() << "\n\n"
                  << ArgParser::Usage() << '\n';
        ret_code = 1;
    }
    return ret_code;
}

