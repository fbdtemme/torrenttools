.. cpp:namespace:: bencode

.. _customization-points:

User-defined type support
=========================

There are various customization points to integrate user-defined types in this library.

Serialization traits
--------------------

The first step to integrating a custom type is by providing which bencode data type it encodes
to by specializing :cpp:class:`template <typename T> serialization_traits` for your type.

Serialization traits has a single member :cpp:member:`type` of type :cpp:enum:`bencode_type`
that defines what bencode data type this type serializes to.

To make specialization of :cpp:class:`serialization_traits` easy a few helpers are provided.

Helper classes:
    * :cpp:class:`serializes_to_runtime_type`
    * :cpp:class:`serializes_to_integer`
    * :cpp:class:`serializes_to_integer`
    * :cpp:class:`serializes_to_string`
    * :cpp:class:`serializes_to_list`
    * :cpp:class:`template <dict_key_order Order = dict_key_order::sorted> struct serializes_to_dict`

Helper macros:
    * :c:macro:`BENCODE_SERIALIZES_TO_RUNTIME_TYPE`
    * :c:macro:`BENCODE_SERIALIZES_TO_INTEGER`
    * :c:macro:`BENCODE_SERIALIZES_TO_STRING`
    * :c:macro:`BENCODE_SERIALIZES_TO_LIST`
    * :c:macro:`BENCODE_SERIALIZES_TO_DICT_SORTED`
    * :c:macro:`BENCODE_SERIALIZES_TO_DICT_UNSORTED`


When the user-defined type can be converted to different bencode data types depending on the value
:cpp:class:`serializes_to_runtime_type` or :c:macro:`BENCODE_SERIALIZES_TO_RUNTIME_TYPE` should be used.

When a type serializes to a dict we make a differentiation between sorted and
unsorted dicts. Since a bencode dict requires keys to be in sorted order we must mark
map-like types with unsorted keys as such.

To define a type that behaves like a pointer (eg. smart pointers), the static member variable
:cpp:var:`is_pointer` must be set to true.
This will make sure that when serializing/deserializing that type the value is dereferenced when needed.

For types that behave like standard library types, specializing
:cpp:class:`template <typename T> serialization_traits` can be enough to enable full support.
The bencode library will try to find an implementation that works for given type.
If no suitable build-in methods exist, additional customization points must be implemented.

After specializing :cpp:class:`serialization_traits` the user-defined type satisfies
the :cpp:concept:`serializable` concept.

Example:

.. code-block:: cpp

    struct rgb_color
    {
        std::uint8_t r, g, g;
    };

    // Specialization with a macro.
    namespace bencode {
    BENCODE_SERIALIZES_TO_LIST(rgb_color)
    }

    // Equivalent specialization without macro use.
    namespace bencode {
    template <> struct serialization_traits<rgb_color> : serializes_to_list {};
    }


.. code-block:: cpp

    template <typename T>
    class my_smart_pointer : {...}

    namespace bencode {
    template <typename T>
    struct serialization_traits<my_smart_pointer<T>>
            : serializes_to<serialization_traits<T>::type>
    {
        static constexpr bool is_pointer = true;
    };
    }

Event producer
--------------

The second required customization point to enable support for a user-defined type is
the :cpp:func:`bencode_connect`.

.. code-block:: cpp

    template <event_consumer EC>
    constexpr void bencode_connect(
            customization_point_type<rgb_color>, EC& consumer, const rgb_color& value)
    {
        consumer.list_begin();
        consumer.integer(value.r);
        consumer.list_item();
        consumer.integer(value.g);
        consumer.list_item();
        consumer.integer(value.b);
        consumer.list_item();
        consumer.list_end()
    }

After overriding this function the type satisfies the :cpp:concept:`event_producer` concept.

After satisfying :cpp:concept:`serializable` and cpp:concept:`event_producer` the user defined type
can be serialized with :cpp:class:`encoder` and assigned to :cpp:class:`bvalue`.

.. important::

    All customization points prefixed with :code:`bencode_` must be defined in the
    namespace of the type for which you want to enable a library feature.
    These functions use Argument-dependent lookup (ADL) to identify the correct overload.

Assignment to bvalue
--------------------

Types that satisfy :cpp:concept:`event_producer` have a default implementation
that allows the type to be assigned to :cpp:class:`bvalue`, but is not always the most efficient.
The default can be overriden by overriding :cpp:func:`bencode_assign_to_bvalue`

.. code-block:: cpp

    template <typename Policy>
    constexpr auto bencode_assign_to_bvalue(
            customization_point_type<rgb_color>, basic_bvalue<Policy>& bv, const rgb_color& value)
    {
        auto& l = bv.emplace_list();
        l.push_back(value.r);
        l.push_back(value.g);
        l.push_back(value.b);
    }

Direct comparison to bvalue
---------------------------

The content of a :cpp:class:`bvalue` can be compared with that of a custom type without
creating a temporary :cpp:class:`bvalue` object.
This is done be overriding :cpp:func:`bencode_compare_equality_with_bvalue`

.. code-block:: cpp

    template <typename Policy>
    bencode_compare_equality_with_bvalue(
            customization_point_type<rgb_color>, basic_bvalue<Policy>& bv, const rgb_color& value)
    {
        if (!is_list(bv)) return false;
        if (bv.size() != 3) return false;
        return (bv[0] == value.r) && (bv[1] == value.g) && (b[2] == value.b);
    }

For types that can be ordered :cpp:func:`bencode_compare_three_way_with_bvalue` can be overridden.

.. code-block:: cpp

    template <typename Policy>
    std::partial_ordering bencode_compare_three_way_with_bvalue(
            customization_point_type<rgb_color>, basic_bvalue<Policy>& bv, const rgb_color& value)
    {
        if (!is_list(bv))  return std::partial_ordering::unordered;
        if (bv.size() < 3) return std::partial_ordering::greater;
        if (bv.size() > 3) return std::partial_ordering::less;

        auto first_ordering = (bv[0] <=> value.r);
        if (first_ordering == std::partial_ordering::equivalent) {
            auto second_ordering = (bv[1] <=> value.g);
            if (second_ordering == std::partial_ordering::equivalent) {
                return b[2] <=> value.b;
            } else {
                return second_ordering;
            }
        }
        return first_ordering
    }

Conversion from bvalue to custom type
-------------------------------------

You can retrieve your custom type directly from a :cpp:clas::`bvalue`
by implementing the :cpp:func:`bencode_convert_from_bvalue` customization point.
This will allow the use of :cpp:func:`get_as<T>` with your type.
Errors are reported with :cpp:class:`nonstd::expected<T, conversion_ercc>`.

.. code-block:: cpp

    template <typename Policy>
    nonstd::expected<rgb_color, conversion_errc>
    bencode_convert_from_bvalue(customization_point_type<rgb_color>, const basic_bvalue<Policy>& bv)
    {
        if (!is_list(bv))
            return nonstd::make_unexpected(conversion_errc::not_list_type);

        const auto& l = get_list(bv)

        if (l.size() != 3)
            return nonstd::make_unexpected(conversion_errc::size_mismatch);

        return rgb_color {.r = l[0], .g = l[1], .b = l[2]};
    }

.. _customization-compare-to-bview:

Direct comparison to bview
--------------------------

Analogue with comparison with :cpp:class:`bvalue` there are two comparison customization points
for :cpp:class:`bview`:

* :cpp:func:`bencode_compare_equality_with_bview`
* :cpp:func:`bencode_compare_three_way_with_bview`

The implementation for our example user-defined class is exactly the same as for
the implementation for :cpp:class:`bvalue`, except the function signature.

.. code-block::

    constexpr bool bencode_compare_equality_with_bview(
        customization_point_type<rgb_color>, const bview& bv, rgb_color value);

    constexpr bool bencode_compare_three_way_with_bview(
        customization_point_type<rgb_color>, const bview& bv, rgb_color value);


.. _customization-convert-from-bview:

Conversion from bview to custom type
-------------------------------------

Similar to conversion from :cpp:class:`bvalue` there is a conversion from :cpp:class:`bview`
by implementing the :cpp:func:`bencode_convert_from_bview` customization point.

The implementation for our example user-defined class is exactly the same as for
the implementation for :cpp:class:`bvalue`, except the function signature.

.. code-block:: cpp

    nonstd::expected<rgb_color, conversion_errc>
    bencode_convert_from_bview(customization_for<rgb_color>, const bview& bv);

