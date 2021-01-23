#pragma once

#include "bencode/detail/bview/bview.hpp"
#include "bencode/detail/bpointer.hpp"
#include "bencode/detail/out_of_range.hpp"
#include "bencode/detail/bview/accessors.hpp"

namespace bencode::detail {

inline bview evaluate(const bpointer& pointer, const bview& bv)
{
    auto v = bv;

    for (const auto& token : pointer) {
        switch (v.type()) {
        case bencode_type::dict: {
            v = get_dict(v).at(token);
            break;
        }
        case bencode_type::list: {
            if (token=="-") [[unlikely]] {
                throw out_of_range("unresolved token '-': list index '-' is not supported");
            }

            std::size_t value;
            auto res = detail::from_chars(
                    token.data(), token.data() + token.size(), value, implementation::serial);

            if (res.ec != parsing_errc{}) [[unlikely]] {
                throw out_of_range(
                        fmt::format("unresolved token '{}': expected list index", token));
            }

            v = get_list(v).at(value);
            break;
        }
        default:
            throw out_of_range(
                    fmt::format("unresolved token '{}': expected list or dict but got {}",
                            token, v.type()));
        }
    }
    return v;
}

inline bool contains(const bpointer& pointer, const bview& bv)
{
    auto v = bv;

    for (const auto& token : pointer) {
        switch (v.type()) {
        case bencode_type::dict: {
            if (!get_dict(v).contains(token))
                return false;
            v = get_dict(v).at(token);
            break;
        }
        case bencode_type::list: {
            if (token=="-") [[unlikely]]
                return false;

            std::uint64_t value;
            auto res = detail::from_chars(
                    token.data(), token.data()+token.size(), value, implementation::serial);

            if (res.ec != parsing_errc{}) [[unlikely]]
                return false;

            if (value >= get_list(v).size())
                return false;

            v = get_list(v).at(value);
            break;
        }
        default:
            return false;
        }
    }
    return true;
}

}