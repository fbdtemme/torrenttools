#pragma once
#include <string_view>
#include <bencode/bencode.hpp>

using namespace std::string_literals;
namespace bc = bencode;

inline constexpr std::string_view data_integer = "i63e";
inline constexpr std::string_view data_string  = "4:spam";
inline constexpr std::string_view data_list    = "li2ei3ee";
inline constexpr std::string_view data_nested_list = "lie1l3:foo3:baree";
inline constexpr std::string_view data_dict    = "d4:spami1ee";
inline constexpr std::string_view data_nested_dict    = "d4:spami1e3:barli1ei2eee";

inline constexpr std::array descriptors_integer = {
        bc::descriptor(bc::descriptor_type::integer, 0, 63L),
        bc::descriptor(bc::descriptor_type::stop, 4),
};

inline constexpr std::array descriptors_string  = {
        bc::descriptor(bc::descriptor_type::string, 0, 2U, 4U),
        bc::descriptor(bc::descriptor_type::stop, 6),
};

inline constexpr std::array descriptors_list = {
        bc::descriptor(bc::descriptor_type::list, 0, 3, 2),
        bc::descriptor(bc::descriptor_type::integer | bc::descriptor_type::list_value, 1, 2),
        bc::descriptor(bc::descriptor_type::integer | bc::descriptor_type::list_value, 4, 3),
        bc::descriptor(bc::descriptor_type::list | bc::descriptor_type::end, 7, 3, 2),
        bc::descriptor(bc::descriptor_type::stop, 8),
};

constexpr std::array descriptors_nested_list = {
        bc::descriptor(bc::descriptor_type::list, 0, 6, 2),
        bc::descriptor(bc::descriptor_type::integer  | bc::descriptor_type::list_value, 1, 1),
        bc::descriptor(bc::descriptor_type::list | bc::descriptor_type::list_value, 4, 3, 2),
        bc::descriptor(bc::descriptor_type::string | bc::descriptor_type::list_value, 5, 2, 3),
        bc::descriptor(bc::descriptor_type::string | bc::descriptor_type::list_value, 10, 2, 3),
        bc::descriptor(bc::descriptor_type::list | bc::descriptor_type::end, 15, 3, 2),
        bc::descriptor(bc::descriptor_type::list | bc::descriptor_type::end, 16, 6, 2),
        bc::descriptor(bc::descriptor_type::stop, 17),
};

inline constexpr std::array descriptors_dict = {
        bc::descriptor(bc::descriptor_type::dict, 0, 3, 1),
        bc::descriptor(bc::descriptor_type::string  | bc::descriptor_type::dict_key, 1, 2, 4),
        bc::descriptor(bc::descriptor_type::integer | bc::descriptor_type::dict_value, 7, 1),
        bc::descriptor(bc::descriptor_type::dict | bc::descriptor_type::end, 10, 3, 1),
        bc::descriptor(bc::descriptor_type::stop, 11),
};

constexpr std::array descriptors_nested_dict = {
        bc::descriptor(bc::descriptor_type::dict, 0, 8, 2),
        bc::descriptor(bc::descriptor_type::string  | bc::descriptor_type::dict_key, 1, 2, 4),
        bc::descriptor(bc::descriptor_type::integer | bc::descriptor_type::dict_value, 7, 1),
        bc::descriptor(bc::descriptor_type::string  | bc::descriptor_type::dict_key, 10, 2, 3),
        bc::descriptor(bc::descriptor_type::list    | bc::descriptor_type::dict_value, 15, 3, 2),
        bc::descriptor(bc::descriptor_type::integer | bc::descriptor_type::list_value, 16, 1),
        bc::descriptor(bc::descriptor_type::integer | bc::descriptor_type::list_value, 19, 2),
        bc::descriptor(bc::descriptor_type::list    | bc::descriptor_type::end, 22, 3, 2),
        bc::descriptor(bc::descriptor_type::dict | bc::descriptor_type::end, 23, 8, 2),
        bc::descriptor(bc::descriptor_type::stop, 24),
};

inline const auto i_view_const = bc::bview(begin(descriptors_integer), data_integer.data());
inline const auto s_view_const  = bc::bview(begin(descriptors_string), data_string.data());
inline const auto l_view_const = bc::bview(begin(descriptors_list), data_list.data());
inline const auto l_view_nested_const = bc::bview(begin(descriptors_nested_list), data_nested_list.data());
inline const auto d_view_const = bc::bview(begin(descriptors_dict), data_dict.data());
inline const auto d_view_nested_const = bc::bview(begin(descriptors_nested_dict), data_nested_dict.data());

static auto i_view = bc::bview(begin(descriptors_integer), data_integer.data());
static auto s_view  = bc::bview(begin(descriptors_string), data_string.data());
static auto l_view = bc::bview(begin(descriptors_list), data_list.data());
static auto l_view_nested = bc::bview(begin(descriptors_nested_list), data_nested_list.data());
static auto d_view = bc::bview(begin(descriptors_dict), data_dict.data());
static auto d_view_nested = bc::bview(begin(descriptors_nested_dict), data_nested_dict.data());







