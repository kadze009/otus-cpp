#pragma once

#include <array>
#include <string>
#include <vector>

#include <cstdint>



class IpV4_c
{
public:
    static constexpr int MATCH_SKIP_BYTE = -1;
    using mask_t = std::array<int, 4>;

    IpV4_c(const std::string& str) { assign(str); }
    IpV4_c(const IpV4_c&) = default;

    void assign(const std::string& str_ipv4);

    std::string toString() const;

    bool operator<(const IpV4_c&) const;
    bool match(const mask_t& mask) const;
    bool has_byte(uint8_t) const;

private:
    std::array<uint8_t, 4> m_bytes;
};


std::ostream& operator<<(std::ostream&, const IpV4_c&);



using ip_pool_t          = std::vector<IpV4_c>;
using filtered_ip_pool_t = std::vector<IpV4_c*>;



namespace utils {

filtered_ip_pool_t filter(ip_pool_t&, const IpV4_c::mask_t&);
filtered_ip_pool_t filter_any(ip_pool_t&, uint8_t);

void print(const ip_pool_t&);
void print(const filtered_ip_pool_t&);

} // namespace utils

