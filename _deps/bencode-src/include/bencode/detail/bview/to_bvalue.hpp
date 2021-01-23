#pragma once
#include "bencode/detail/bencode_type.hpp"
#include "bencode/detail/utils.hpp"

#include "bencode/detail/bvalue/basic_bvalue.hpp"

namespace bencode::detail {

template <typename Policy>
constexpr void assign_to_bvalue_integer_impl(
        customization_point_type<integer_bview>,
        basic_bvalue<Policy>& bvalue, integer_bview value)
{
    bvalue.emplace_integer(value);
}

template <typename Policy>
constexpr void assign_to_bvalue_string_impl(
        customization_point_type<string_bview>,
        basic_bvalue<Policy>& bvalue, string_bview value)
{
    bvalue.emplace_string(value);
}

template <typename Policy>
constexpr void assign_to_bvalue_list_impl(
        customization_point_type<list_bview>,
        basic_bvalue<Policy>& bvalue, list_bview value)
{
    bvalue.emplace_list(value.begin(), value.end());
}

template <typename Policy>
constexpr void assign_to_bvalue_dict_impl(
        customization_point_type<dict_bview>,
        basic_bvalue<Policy>& bvalue, dict_bview value)
{
    bvalue.emplace_dict(value.begin(), value.end());
}

template <typename Policy>
constexpr void assign_bview_to_bvalue(
        customization_point_type<bview>,
        basic_bvalue<Policy>& bvalue, const bview& view)
{
    switch (view.type()) {
    case bencode_type::integer: {
        assign_to_bvalue_integer_impl(
            customization_for<integer_bview>, bvalue, get_integer(view));
        break;
    }
    case bencode_type::string: {
        assign_to_bvalue_string_impl(
            customization_for<string_bview>, bvalue, get_string(view));
        break;
    }
    case bencode_type::list: {
        assign_to_bvalue_list_impl(
            customization_for<list_bview>, bvalue, get_list(view));
        break;
    }
    case bencode_type::dict: {
        assign_to_bvalue_dict_impl(
            customization_for<dict_bview>, bvalue, get_dict(view));
    }
    }
}

} // bencode::detail
