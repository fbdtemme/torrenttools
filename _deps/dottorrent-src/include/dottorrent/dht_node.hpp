#pragma once
#include <compare>
#include <cstdint>
#include <string>
#include <tuple>
#include <type_traits>

namespace dottorrent {

struct dht_node
{
    std::string url;
    std::uint16_t port;

    dht_node() noexcept = default;
    dht_node(std::string_view url, std::uint16_t port)
        : url(url)
        , port(port)
    {}

    dht_node(const dht_node&) = default;
    dht_node(dht_node&&) = default;
    dht_node& operator=(const dht_node&) = default;
    dht_node& operator=(dht_node&&) = default;

    bool operator==(const dht_node& that) const = default;
    std::strong_ordering operator<=>(const dht_node& that) const  = default;

    // friends for structured binding support
    template <std::size_t N> friend const auto& get(const dht_node&);
    template <std::size_t N> friend auto& get(dht_node&);
};

} // namespace dottorrent

//-----------------------------------------------------------------------------//
// Structured bindings support                                                 //
//-----------------------------------------------------------------------------//

namespace dottorrent {
template <std::size_t N>
const auto& get(const dht_node& a) {
    if      constexpr (N == 0) return a.url;
    else if constexpr (N == 1) return a.port;
}

template <std::size_t N>
auto& get(dht_node& a) {
    if      constexpr (N == 0) return a.url;
    else if constexpr (N == 1) return a.port;
}
}

namespace std {
template<>
struct tuple_size<dottorrent::dht_node>
        : std::integral_constant<std::size_t, 2> {};

template<std::size_t N>
struct tuple_element<N, dottorrent::dht_node> {
    using type = decltype(get<N>(std::declval<dottorrent::dht_node>()));
};
}