#pragma once

namespace bencode::detail {

template <std::size_t Idx>
struct implementation_tag
{
    static constexpr std::size_t index = Idx;
};

namespace implementation {
inline constexpr auto serial = implementation_tag<0>{};
inline constexpr auto swar = implementation_tag<1>{};
inline constexpr auto sse41 = implementation_tag<2>{};
inline constexpr auto avx2 = implementation_tag<3>{};
inline constexpr auto neon = implementation_tag<4>{};
};


struct from_chars_result
{
    const char* ptr;
    parsing_errc ec;
};

}