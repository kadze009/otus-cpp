#pragma once

#include <iostream>
#include <type_traits>
#include <utility>

#include <cstdint>
#include <string_view>
#include <string>
#include <tuple>


//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#   define    DEBUG(expr)    expr
#else
#   define    DEBUG(expr)
#endif



void print_ip(std::string_view sv)
{
    DEBUG(std::cout << "STR:   ");
    std::cout << sv;
    std::cout << '\n';
}


template <
    typename T,
    typename = decltype(std::declval<T>().begin()),
    typename = decltype(std::declval<T>().end()),
    typename = std::enable_if_t<not std::is_convertible_v<T, std::string>>
    >
void print_ip(T const& cont)
{
    auto it   = cont.begin();
    auto it_b = cont.begin();
    auto it_e = cont.end();
    size_t num  = 4;
    DEBUG(std::cout << "CONT:  ");
    while (it != it_e && num > 0)
    {
        if (it != it_b) { std::cout << '.'; }
        std::cout << *it;
        ++it;
        --num;
    }
    std::cout << '\n';
}


template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
void print_ip(T val)
{
    auto v = static_cast<int32_t>(val);
    auto get_byte = [&v](size_t const n) -> uint32_t
    {
        return (v >> (n*8)) & 0xFF;
    };
    DEBUG(std::cout << "INT:   ");
    std::cout << get_byte(3) << '.'
              << get_byte(2) << '.'
              << get_byte(1) << '.'
              << get_byte(0);
    std::cout << '\n';
}


template<typename T, size_t... I>
void print_ip_tuple_impl(T const& ip, std::integer_sequence<size_t, I...>)
{
    using ip_t      = std::remove_cv_t<std::remove_reference_t<T>>;
    using element_t = std::tuple_element_t<0, ip_t>;
    static_assert((std::is_same_v<element_t, std::tuple_element_t<I, ip_t>> && ...),
                   "all pieces of `tuple` must be the same type");
    DEBUG(std::cout << "TUPLE: ");
    ((std::cout << (I == 0 ? "" : ".") << std::get<I>(ip)), ...);
    std::cout << '\n';
}

template <typename... ELS >
void print_ip(std::tuple<ELS...> const& ip)
{
    constexpr size_t tuple_size = sizeof...(ELS);
    static_assert(tuple_size == 4, "Expected tuple's size is 4");
    using indexes_t = std::make_integer_sequence<size_t, tuple_size>;
    print_ip_tuple_impl(ip, indexes_t{});
}

