#include "IpV4_c.hpp"

#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <charconv>      // std::from_chars
#include <stdexcept>     // std::runtime_error, std::errc, std::make_error_code
#include <system_error>  // make_error_code

#include "utils.hpp"



void IpV4_c::assign(const std::string& str_ipv4)
{
    //TODO: don't use exception
    std::vector<std::string> bytes = split(str_ipv4, '.');
    if (bytes.size() != m_bytes.size())
    {
        std::stringstream ss{};
        ss << "IpV4_c::" <<  __FUNCTION__ << ": unexpected number of input digits "
           << "in IpV4[" << str_ipv4 << "]:"
           << " exp=" << m_bytes.size() << " act=" << bytes.size();
        throw std::runtime_error(ss.str());
    }
    for (size_t i = 0; i < m_bytes.size(); ++i)
    {
        const std::string& byte = bytes[i];
        auto[ptr, ec] = std::from_chars(
            byte.c_str(), byte.c_str() + byte.size(),
            m_bytes[i]);
        if (ec != std::errc())
        {
            std::stringstream ss{};
            ss << "IpV4_c::" << __FUNCTION__ << ": invalid digit [" << byte
               << "] in IpV4[" << str_ipv4 << "]. Error: " << make_error_code(ec);
            throw std::runtime_error(ss.str());
        }
    }
}



std::string IpV4_c::toString() const
{
    //TODO: don't use std::stringstream because of std::locale
    //      and have to return std::string
    //      Alternatives:
    //      - use: snprintf, std::to_chars
    //      - return: const char*
    std::stringstream ss;
    for (auto dig = m_bytes.cbegin(); dig != m_bytes.cend(); ++dig)
    {
        if (dig != m_bytes.cbegin())
        {
            ss << '.';
        }
        ss << static_cast<int>(*dig);
    }
    return ss.str();
}



bool IpV4_c::operator<(const IpV4_c& o) const
{
    if (this == &o) { return false; }
    return std::lexicographical_compare(
        m_bytes.cbegin(),   m_bytes.cend(),
        o.m_bytes.cbegin(), o.m_bytes.cend());
}



bool IpV4_c::match(const mask_t& mask) const
{
    for (size_t i = 0; i < mask.size(); ++i)
    {
        if (mask[i] == MATCH_SKIP_BYTE) { continue; }
        if (mask[i] != m_bytes[i])      { return false; }
    }
    return true;
}



bool IpV4_c::has_byte(uint8_t byte) const
{
    return std::count(m_bytes.cbegin(), m_bytes.cend(), byte) != 0;
}



std::ostream& operator<<(std::ostream& os, const IpV4_c& ip)
{
    os << ip.toString();
    return os;
}




namespace utils {

filtered_ip_pool_t filter(ip_pool_t& ip_pool, const IpV4_c::mask_t& mask)
{
    filtered_ip_pool_t f_pool;
    if (std::all_of(mask.cbegin(), mask.cend(), [](int v){ return v == IpV4_c::MATCH_SKIP_BYTE; }))
    {
        f_pool.reserve(ip_pool.size());
        for (auto& ip : ip_pool) { f_pool.emplace_back(&ip); }
    }
    else
    {
        //NOTE: evristic reserve
        f_pool.reserve(ip_pool.size() / 2);
        for (auto& ip : ip_pool)
        {
            if (ip.match(mask)) { f_pool.emplace_back(&ip); }
        }
    }
    return f_pool;
}



filtered_ip_pool_t filter_any(ip_pool_t& ip_pool, uint8_t byte)
{
    filtered_ip_pool_t f_pool;
    for (auto& ip : ip_pool)
    {
        if (ip.has_byte(byte)) { f_pool.emplace_back(&ip); }
    }
    return f_pool;
}



void print(const ip_pool_t& ip_pool)
{
    for (auto const& ip : ip_pool) { std::cout << ip << '\n'; }
}



void print(const filtered_ip_pool_t& ip_pool)
{
    for (auto const& ip_ptr : ip_pool) { std::cout << *ip_ptr << '\n'; }
}

} // namespace utils

