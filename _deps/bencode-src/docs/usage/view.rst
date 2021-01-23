.. cpp:namespace:: bencode

.. _bview:

View interface
==============

Introduction
------------

:cpp:class:`bview` is a sum type that provides access to the values stored in a bencoded buffer.
It holds two pointers, one to a :cpp:class:`descriptor`, and one to the buffer contained the
bencoded data.

The :cpp:class:`descriptor` describes the type and content of the bencoded token the
:cpp:class:`bview` points to and where the data can be found in the bencoded buffer.
This information allows :cpp:class:`bview` to navigate through the bencoded buffer and
provide access to the values using an interface similar to standard C++ containers.

:cpp:class:`bview` is used together with four subclasses:

*   :cpp:class:`integer_bview`
*   :cpp:class:`string_bview`
*   :cpp:class:`list_bview`
*   :cpp:class:`dict_bview`


Each subclass provides an extra interface over :cpp:class:`bview`
for the corresponding bencode data type.

These four classes satisfy the concept :cpp:concept:`bview_alternative_type`.

:cpp:class:`integer_bview` is implicitly convertible to :cpp:class:`std::int64_t`.
The value stored can also be retrieved using :cpp:func:`integer_bview::value`

:cpp:class:`string_bview` provides an interface equal to that of :cpp:class:`std::string_view`.
It has an implicit conversion operator to :cpp:class:`std::string_view`.

:cpp:class:`list_bview` provides and interface similar to
:cpp:class:`std::vector<bc::bview>`. Its iterators are only
:cpp:concept:`bidirectional_iterator` instead of :cpp:concept:`contiguous_iterator`.
Access to the elements is linear in the size of the :cpp:class:`list_bview`.


:cpp:class:`dict_bview` provides the interface similar to
:cpp:class:`std::map\<bc::string_bview, bc::bview>`.
Access to the elements is linear in the size of the :cpp:class:`dict_bview`.

The :ref:`API reference <bview_reference>`  provides more information on how to use these types

.. important::

    The reference returned by the dereference operator for
    :cpp:class:`list_bview::iterator` and :cpp:class:`dict_bview::iterator`
    is only valid until the next dereference.

Construction
-------------

:cpp:class:`bview` should rarely be constructed directly. :cpp:class:`bview` is the result of calling
:cpp:func:`get_root()` on a :cpp:class:`descriptor_table` instance which is the result of parsing
a bencoded string with :cpp:func:`decode_view`.

.. code-block::

    const std::string data = "d3:cow3:moo4:spam4:eggse";
    bc::descriptor_table desc_table = bencode::decode_view(data);
    bc::bview root_element = desc_table.get_root();


Type checking
-------------

Checking the alternative type of a :cpp:class:`bview` can be done using the following functions:

* :cpp:func:`bool holds_integer(const bview&)`
* :cpp:func:`bool holds_string(const bview&)`
* :cpp:func:`bool holds_list(const bview&)`
* :cpp:func:`bool holds_dict(const bview&)`
* :cpp:func:`template \<enum bencode_type E> bool holds_alternative(const bview&)`
* :cpp:expr:`template \<bview_alternative_type T> bool holds_alternative(const bview&)`

.. code-block:: cpp

    holds_integer(root_element)    // returns false
    holds_dict(root_element)       // returns true

    // type tag based check
    bc::holds_alternative<bc::type::dict>(root_element); // returns true

    // bview type based check
    bc::holds_alternative<bc::dict_bview>(root_element); // returns true


Accessors
---------

Retrieving the alternative type from the :cpp:class:`bview` instance is done using accessor functions.

Throwing accessor function will throw :cpp:class:`bad_bview_access` when trying to
convert a :cpp:class:`bview` to an alternative type that does not match the bencode data type.

* :cpp:func:`const integer_bview& get_integer(const bview&)`
* :cpp:func:`const string_bview& get_string(const bview&)`
* :cpp:func:`const list_bview& get_list(const bview&)`
* :cpp:func:`const dict_bview& get_dict(const bview&)`
* :cpp:func:`template \<enum bencode_type E> const bview_alternative_t<E>& get(const bview&)`
* :cpp:expr:`template \<bview_alternative_type T> const T& get(const bview&)`

Non throwing accessor function will return a :cpp:expr:`nullptr` when trying to convert
a bview to a bview alternative type that does not match the bencode data type.

* :cpp:func:`bool get_if_integer(const bview*)`
* :cpp:func:`bool get_if_string(const bview*)`
* :cpp:func:`bool get_if_list(const bview*)`
* :cpp:func:`bool get_if_dict(const bview*)`
* :cpp:func:`template \<enum bencode_type E> const bview_alternative_t<E>* get_if(const bview*)`
* :cpp:expr:`template \<bview_alternative_type T> const T* get_if(const bview&)`


.. code-block:: cpp

    auto dict_view = get_dict(root_element);    // return dict_bview instance
    auto list_view = get_list(root_element)     // throws bad_bview_access

    // type tag based check
    auto get<bc::btype::dict>(root_element);    // return dict_bview instance

    // bview type based check
    auto get<bc::dict_bview>(root_element);     // return dict_bview instance


Conversion
----------

To copy the content of a :cpp:class:`bview` value to a specific type, generic converters are used.
The throwing converter will throw :cpp:class:`bad_conversion` when an error occurs.

* :cpp:func:`template \<typename T> T get_as(const bview&)`

The non throwing converter will return a instance of :cpp:class:`nonstd::expected`
with the converted value or a :cpp:enum:`conversion_errc`.

* :cpp:func:`template \<typename T> nonstd::expected\<T, conversion_errc> try_get_as(const bview&)`

:cpp:class:`bview` values can be converted to any type that satisfies :cpp:concept:`retrievable_from_bview`.
Conversion to standard library types can be enabled by including the corresponding trait header.
Conversion to user-defined types can be enabled by implementing
the necessary :ref:`customization points <customization-convert-from-bview>`.


.. code-block::

    #include <bencode/traits/map.hpp>
    #include <bencode/traits/string.hpp>

    // copy a view to a std::map
    auto d = get_as<std::map<std::string, bc::bvalue>>(root_element); //

  // copy a view to a std::map
    auto d2 = try_get_as<std::map<std::string, int>>(root_element);
    if (!d2) {
        //  returns conversion_errc::dict_mapped_type_construction_error
        //  and assign it to a generic std::error_code
        std::error_code ec = d2.error()
    }


Comparison
----------

Most types can be compared with :cpp:class:`bview` instances.
Comparison is deep and will compare the content of the bencode data type.
When the bencode type of :cpp:class:`bview` is not
the same as the bencode type of the the type you compare with when serialized,
the fallback order is `integer < string < list < dict`

Conversion to standard library types can be enabled by including the corresponding trait header.
Comparison to user-defined types can be enabled by implementing
the necessary :ref:`customization point <customization-compare-to-bview>`.

.. code-block:: cpp

    bview b;        // b points to a bencoded string with text "foo";
    b == "foo";     // return true
    b > "aa";       // returns true
    b > 3;          // return true (integer < string)
    b > std::map<std::string, int> {{"foo", 1}}; // return false (string < dict)


