#pragma once

#include <bencode/detail/bencode_type.hpp>

namespace bencode {


/// Trait class that indicates a type can be serialized to bencode.
/// A valid specialization for type T defines a single static member bvalue of enum type `bencode_type`
/// that specifies the corresponding bencode object type for T.
template<typename T> struct serialization_traits {};


/// Enum that indicates if the type that is being described in a serialization_traits
/// contains sorted or unsorted keys.
/// This enables the library to sort keys before serializing when needed.
enum class dict_key_order {
    sorted,
    unsorted
};

/// Helper classes to create specializations of `serialisation_traits`.
/// @example:
///     template<> struct serialisation_traits<MyType> : serializes_to_runtime_type {};
struct serializes_to_runtime_type
{
    static constexpr auto type = bencode_type::uninitialized;
    static constexpr auto is_pointer = false;
};

/// Helper class to create specializations of `serialisation_traits`.
struct serializes_to_integer
{
    static constexpr auto type = bencode_type::integer;
    static constexpr auto is_pointer = false;
};

/// Helper class to create specializations of `serialisation_traits`.
struct serializes_to_string
{
    static constexpr auto type = bencode_type::string;
    static constexpr auto is_pointer = false;
};

/// Helper class to create specializations of `serialisation_traits`.
struct serializes_to_list
{
    static constexpr auto type = bencode_type::list;
    static constexpr auto is_pointer = false;
};

/// Helper class to create specializations of `serialisation_traits`.
/// @tparam Order Specifies if iterating over the keys happens in alphanumerical order.
/// @warning Passing dict_key_order::sorted when iterating over the key, value pairs is
///          an unspecified order will silently generate invalid bencoded data.
template <dict_key_order Order = dict_key_order::sorted>
struct serializes_to_dict
{
    static constexpr auto type = bencode_type::dict;
    static constexpr auto key_order = Order;
    static constexpr auto is_pointer = false;
};


/// A type T satisfied the serialisable concept if it has a valid specialization of `serialisation_traits`.
template <typename T>
concept serializable =
    requires() {
//        typename serialization_traits<T>::type;
        { serialization_traits<T>::type } -> std::convertible_to<bencode_type>;
    };

/// A type T satisfied the serializable_to concept if it is serializable
/// and has a serialization_trait<T>::type == BencodeType.
template <typename T, bencode_type BencodeType>
concept serializable_to =
    serializable<T> &&
    (serialization_traits<T>::type == BencodeType);


} // namespace bencode

#define BENCODE_SERIALIZES_TO(BENCODE_TYPE, TYPE)                                     \
template <> struct bencode::serialization_traits<TYPE> : serializes_to_##BENCODE_TYPE {}; \

#define BENCODE_SERIALIZES_TO_RUNTIME_TYPE(...) \
BENCODE_SERIALIZES_TO(runtime_type, __VA_ARGS__)

#define BENCODE_SERIALIZES_TO_INTEGER(...) \
BENCODE_SERIALIZES_TO(integer, __VA_ARGS__)

#define BENCODE_SERIALIZES_TO_STRING(...) \
BENCODE_SERIALIZES_TO(string, __VA_ARGS__)

#define BENCODE_SERIALIZES_TO_LIST(...) \
BENCODE_SERIALIZES_TO(list, __VA_ARGS__)

#define BENCODE_SERIALIZES_TO_DICT(TYPE, ORDER) \
template <> struct bencode::serialization_traits<TYPE> : serializes_to_dict<dict_key_order::ORDER> {}; \

#define BENCODE_SERIALIZES_TO_DICT_SORTED(TYPE) \
BENCODE_SERIALIZES_TO_DICT(TYPE, sorted)

#define BENCODE_SERIALIZES_TO_DICT_UNSORTED(TYPE) \
BENCODE_SERIALIZES_TO_DICT(TYPE, sorted)


