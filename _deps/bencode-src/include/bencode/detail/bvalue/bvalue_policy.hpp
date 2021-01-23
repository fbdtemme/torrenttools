#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <variant>
#include <map>
#include <concepts>



namespace bencode {

// forward decl
template <typename Policy> class basic_bvalue;

/// Policy class defining the behavior of the basic_bvalue container and default serialisation traits_old.
template<
        typename IntegralType,
        typename StringType,
        template <typename T> typename ListType,
        template <typename K, typename V> typename DictType
>
struct bvalue_policy
{
    /// Types used to store the different possible bencode types.
    ///
    /// According to bencode specification these types should meet following requirements.
    ///
    /// integer_type      : signed integer type
    ///                     The bencode specification does not mention a required width but to
    ///                     support metafiles containing files larger then 4 GiB it must be at
    ///                     least 64 bits wide.
    /// string_view_type  : non-owning string container for utf8 strings.
    /// string_type       : owning string container for utf8 strings.
    /// list_type         : list container
    /// dict_type         : dictionary container with sorted keys


    /// Requirements:
    /// string_type and string_view_type must conform to the std interface.
    template<typename BV>
    struct storage
    {
        using uninitialized_type = std::monostate;
        using integer_type       = IntegralType;                // bencoded integer
        using string_type        = StringType;                  // bencoded string
        using list_type          = ListType<BV>;                // bencoded list
        using dict_type          = DictType<string_type, BV>;   // bencoded dict

        static_assert(std::numeric_limits<integer_type>::is_integer,    "integer storage type must be integral");
        static_assert(std::numeric_limits<integer_type>::is_signed,     "integer storage type must be signed");

    };

    /// Enable overflow safe narrowing conversion to unsigned integers.
    ///
    /// Construction or assignment will throw std::overflow_error when the integer has a negative bvalue.
    /// Only conversions to unsigned integers of the same bit width as the integer storage type are considered.
    static constexpr bool enable_unsigned_narrowing_conversion = true;
};


namespace detail {

/// type aliases for easier access to the policy class

template <typename Policy>
using policy_uninitialized_t = typename Policy::template storage<basic_bvalue<Policy>>::uninitialized_type;

template <typename Policy>
using policy_integer_t = typename Policy::template storage<basic_bvalue<Policy>>::integer_type;

template <typename Policy>
using policy_string_t = typename Policy::template storage<basic_bvalue<Policy>>::string_type;

template <typename Policy>
using policy_list_t = typename Policy::template storage<basic_bvalue<Policy>>::list_type;

template <typename Policy>
using policy_dict_t = typename Policy::template storage<basic_bvalue<Policy>>::dict_type;

template <typename Policy>
using policy_dict_value_t = typename Policy::template storage<basic_bvalue<Policy>>::dict_type::value_type;

template <typename Policy>
using policy_string_value_t = typename Policy::template storage<basic_bvalue<Policy>>::string_type::value_type;

template <typename Policy>
using policy_string_init_list_t = std::initializer_list<policy_string_value_t<Policy>>;

template <typename Policy>
using policy_dict_init_list_t = std::initializer_list<policy_dict_value_t<Policy>>;

template <typename Policy>
using policy_list_init_list_t = std::initializer_list<basic_bvalue<Policy>>;

template <typename Policy>
using policy_dict_key_compare = typename Policy::template storage<basic_bvalue<Policy>>::dict_type::key_compare;


// TODO: test custom policy support

struct default_policy_helper
{
    template<typename V>
    using list_type = std::vector<V>;

    template<typename K, typename V>
    using dict_type = std::map<K, V, std::less<>>;
};

//template <template <typename> typename Alloc>
//struct scoped_allocation_policy_helper
//{
//    template<typename V>
//    using list_type = std::vector<V, std::scoped_allocator_adaptor<Alloc<V>>>;
//
//    template<typename K, typename V>
//    using dict_type = std::map<K, V, std::less<K>, std::scoped_allocator_adaptor<Alloc<std::pair<K, V>>>>;
//};
//
//
//struct transparent_comparator_policy_helper
//{
//    template<typename V>
//    using list_type = std::vector<V>;
//
//    template<typename K, typename V>
//    using dict_type = std::map<K, V, std::less<>>;
//};


} // namespace detail


// use default std::allocator for all allocations
struct default_bvalue_policy
        : bvalue_policy<
            std::int64_t,
            std::string,
            detail::default_policy_helper::template list_type,
            detail::default_policy_helper::template dict_type
        > {};

//// use transparent comparator
//struct transparent_comparator_policy : policy<
//        std::int64_t,
//        std::string_view,
//        std::string,
//        detail::transparent_comparator_policy_helper::template list_type,
//        detail::transparent_comparator_policy_helper::template dict_type,
//        traits_old
//    > {};
//
//
//// use a single allocator for all dynamic allocations
//template <template <typename> typename Alloc>
//struct scoped_allocation_policy : policy <
//        std::int64_t,
//        std::string_view,
//        std::basic_string<char, std::char_traits<char>, Alloc<char>>,
//        detail::scoped_allocation_policy_helper<Alloc>::template list_type,
//        detail::scoped_allocation_policy_helper<Alloc>::template dict_type,
//        traits_old
//        > {};

} // namespace bencode
