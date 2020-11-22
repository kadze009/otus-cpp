#pragma once

//#define ENABLE_LM

#ifdef ENABLE_LM
#    include <cstdio>
#    include <string_view>
constexpr char const* short_filename(char const* fname)
{
    std::string_view fname_sv {fname};
    size_t p = fname_sv.rfind('/');
    return (p != std::string_view::npos)
        ? fname_sv.substr(p + 1).data()
        : fname_sv.data();
}
#    define LM(fmt, ...)    fprintf(stderr, "%s:%d " fmt "\n", short_filename(__FILE__), __LINE__ __VA_OPT__(,) __VA_ARGS__)
#else
#    define LM(...)
#endif

