#pragma once
#include <cstdint>
#include <concepts>
#include <tuple>
#include <array>
#include <optional>
#include <type_traits>

#include "formatted_size.hpp"


namespace termcontrol {

template <typename T>
concept control_sequence_definition =
    requires (T) {
        typename T::parameters;
        { T::final_byte } -> std::convertible_to<char>;
    };

namespace detail {

template <control_sequence_definition T>
struct control_sequence_definition_tag {};

template <typename T> struct default_parameters {};

template <typename T>
concept has_default_parameters =
    requires (T) { { default_parameters<T>::value }; };

// Forward declarations



// Marker type to group functions parameter types
template <typename... Ts> struct argument_pack;

// Marker type to
template <typename... Ts> struct overload_set;

// General purpose traits

template < template <typename...> class Template, typename T>
struct is_template_instantiation_of : std::false_type {};

template <typename... Args, template <typename...> class Template>
struct is_template_instantiation_of<Template, Template<Args...>> : std::true_type {};

template <typename T, template <typename...> class Template>
concept is_instantiation_of = is_template_instantiation_of<Template, T>::value;



template <typename... Args>
concept first_element_is_control_sequence_definition_tag =
    (std::tuple_size_v<std::tuple<Args...>> != 0) &&
    is_instantiation_of<std::tuple_element_t<0, std::tuple<Args...>>, control_sequence_definition_tag>;

//============================================================================//
// Concepts                                                                   //
//============================================================================//


template <typename T>
concept has_intermediate_bytes =
        requires (T) { { T::intermediate_bytes[0] } -> std::same_as<char>; };

template <typename T>
concept has_parameter_byte =
        requires (T) { { T::parameter_byte } -> std::same_as<char>; };




template <typename... Ts>            //general : 0 or more
struct is_overload_set_parameter : std::false_type {};

template <typename T>
struct is_overload_set_parameter<T> :
        std::bool_constant<detail::is_instantiation_of<T, overload_set>> {};


template <typename... Ts>
struct is_valid_overloaded_sequence_def
{
    static constexpr bool value = std::apply([](auto... v) {
        return (detail::is_instantiation_of<decltype(v), argument_pack> && ...);
    }, typename std::tuple_element_t<0, std::tuple<Ts...>>::type{});
};

template <typename... Ts>
constexpr std::size_t count_va_args(Ts... args) { return sizeof...(args); }

template <char... intermediate_bytes_>
struct intermediate_bytes_mixin
{
    using storage_type = std::array<char, sizeof...(intermediate_bytes_)>;
    static constexpr storage_type intermediate_bytes = {intermediate_bytes_...};
};

template <char... intermediate_bytes_>
struct max_formatted_size<intermediate_bytes_mixin<intermediate_bytes_...>>
{
    static constexpr std::size_t value = sizeof...(intermediate_bytes_);
};


template <char parameter_byte_>
struct parameter_byte_mixin
{
    static_assert(parameter_byte_ == '<' ||
                  parameter_byte_ == '=' ||
                  parameter_byte_ == '<' ||
                  parameter_byte_ == '?',
            "invalid parameter byte");

    static constexpr char parameter_byte = parameter_byte_;
};

template <char parameter_byte_>
struct max_formatted_size<intermediate_bytes_mixin<parameter_byte_>>
{
    static constexpr std::size_t value = 1;
};


/// Control sequence starting with CSI
template <char final_byte_,
          typename... Ts>
//    requires
struct basic_control_sequence_definition
{
    // check at instantiation time
    static_assert(
            std::disjunction_v<std::negation<is_overload_set_parameter<Ts...>>,
                                             is_valid_overloaded_sequence_def<Ts...>>,
            "Missing argument_pack in definition of control_sequence definition with overloads");
    static constexpr char final_byte = final_byte_;
    using parameters = std::tuple<Ts...>;

    static constexpr bool has_overloads() {
        return is_overload_set_parameter<Ts...>::value;
    };
};


template <char final_byte_>
struct max_formatted_size<basic_control_sequence_definition<final_byte_>>
{
    static constexpr std::size_t value = 3;
};

template <char final_byte_, typename... Ts>
struct max_formatted_size<basic_control_sequence_definition<final_byte_, Ts...>>
{
    static constexpr std::size_t value =
            max_formatted_size_v<basic_control_sequence_definition<final_byte_>>
            + (max_formatted_size<Ts>::value + ...);
};


/// Control sequence starting with CSI and with one intermediary byte
template <char final_byte_,
          char intermediate_byte_,
          typename... Ts>
struct extended_control_sequence_definition
        : public basic_control_sequence_definition<final_byte_, Ts...>
        , public intermediate_bytes_mixin<intermediate_byte_> {};


template <char final_byte_, char intermediate_byte_, typename... Ts>
struct max_formatted_size<extended_control_sequence_definition<final_byte_, intermediate_byte_, Ts...>>
{
    static constexpr std::size_t value = (
              max_formatted_size<basic_control_sequence_definition<final_byte_, Ts...>>::value
            + max_formatted_size<intermediate_bytes_mixin<intermediate_byte_>>::value
    );
};

template <typename... Ts>
struct argument_pack { using type = std::tuple<Ts...>; };


template <typename... Ts>
struct max_formatted_size<argument_pack<Ts...>>
{
    static constexpr std::size_t value = (max_formatted_size<Ts>::value + ...);
};

template <typename... Ts>
struct overload_set { using type = std::tuple<Ts...>; };


template <typename T>
concept is_parameter_type_placeholder =
    is_instantiation_of<T, argument_pack> ||
    is_instantiation_of<T, overload_set>;


template <typename... Ts>
struct max_formatted_size<overload_set<Ts...>>
{
    static constexpr std::size_t value = std::max({max_formatted_size<Ts>::value...});
};


// Placeholder to define functions as taking a variable number of arguments
template <typename T>
struct variadic { using type = T; };

template <typename T>
struct max_formatted_size<variadic<T>>
{
    static constexpr std::size_t value = std::numeric_limits<std::size_t>::max();
};

template <std::size_t idx, typename T>
struct overload_set_element {
    // point to the std::tuple and not the argumeoverload_setnt_pack;
    using type = typename std::tuple_element_t<idx,
                // remove_overload_set
                typename std::tuple_element_t<0, typename T::parameters>::type>;
};

/// T must be a tuple with a single element of type overload_set
template <typename T>
struct overload_set_size
        : std::integral_constant<std::size_t,
            std::tuple_size_v<typename std::tuple_element_t<0, T>::type>> {};


template <typename T>
    requires detail::is_template_instantiation_of<argument_pack, T>::value
struct argument_pack_size
    : std::integral_constant<std::size_t,
        std::tuple_size_v<typename T::type>> {};


template <std::size_t I, typename T>
    requires detail::is_template_instantiation_of<argument_pack, T>::value
struct argument_pack_element {
    using type = std::tuple_element_t<I, typename T::type>;
};


template <typename D, std::size_t Idx, typename... Ts>
consteval bool call_signature_matches_overloads_impl(std::tuple<Ts...>)
{
    // TypeList is a detail::argument_pack type
    using TypeList = typename overload_set_element<Idx, D>::type;

    // No argument overload
    if constexpr (sizeof...(Ts) == 0) {
        return argument_pack_size<TypeList>::value == 0;
    }

    // variadic<T> : Overload of variable number of parameters of a single type
    using T0 = argument_pack_element<0, TypeList>;
    if constexpr (is_instantiation_of<T0, variadic>) {
        return (std::is_convertible_v<T0, Ts> && ... );
    }
    if constexpr (std::tuple_size_v<typename TypeList::type> == sizeof...(Ts)) {
        // Normal overload: check if arguments are convertible to expected types
        return std::apply([](auto... v) {
            return (std::is_convertible_v<decltype(v), Ts> && ... );
        }, typename TypeList::type{});
    }
    return false;
}


template <typename D, typename... Ts, std::size_t... Is>
consteval bool call_signature_matches_overloads(std::index_sequence<Is...>)
{
    // return true if one of the defined overloads can be called with given
    // parameters types Ts...
    return (call_signature_matches_overloads_impl<D, Is>(std::tuple<Ts...>{}) || ...);
}

template <typename D, typename... Ts>
consteval bool call_signature_matches_impl()
{
    // Control sequence definition has multiple call signatures
    if constexpr (D::has_overloads()) {
        using IS = std::make_index_sequence<overload_set_size<typename D::parameters>::value>;
        return call_signature_matches_overloads<D, Ts...>(IS{});
    }

    // Control sequence definition has a single call signature
    if constexpr (sizeof...(Ts) == std::tuple_size_v<typename D::parameters>) {
        return std::apply([](auto... v) {
            return (std::is_convertible_v<decltype(v), Ts> && ... );
        }, typename D::parameters{});
    }
    // Zero-arg call: single call signature has default values for ALL parameters
    else if constexpr (sizeof...(Ts) == 0) {
        return has_default_parameters<D>;
    }
    return false;
}



} // namespace detail


template <typename D, typename... Ts>
concept call_signature_matches = detail::call_signature_matches_impl<D, Ts...>();

#define TERMCONTROL_DEFINE_DEFAULT_ARGUMENTS(DEFINITION, ...)       \
template <>                                               \
struct termcontrol::detail::default_parameters<DEFINITION> { \
    static_assert(                                                                             \
        detail::count_va_args(__VA_ARGS__) == std::tuple_size_v<typename DEFINITION::parameters>, \
        "invalid number of default arguments given for \""#DEFINITION"\""); \
    static constexpr typename DEFINITION::parameters value {__VA_ARGS__}; \
}


} // namespace termcontrol