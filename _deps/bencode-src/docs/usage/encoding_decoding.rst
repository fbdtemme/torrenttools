.. cpp:namespace:: bencode


Encoding and decoding
=====================

Decoding
--------

Decoding bencoded data can be done with :cpp:func:`decode_value` and :cpp:func:`decode_view`.
:cpp:func:`decode` will return a :cpp:class:`bvalue` instance and :cpp:func:`decode_view`
will return a descriptor table from which a :cpp:class:`bview` to the root element can be obtained.

.. code-block:: cpp

    const std::string data = "d3:cow3:moo4:spam4:eggse";

    bc::bvalue decoded= bc::decode_value(data);

    // pass custom policy template argument for the basic_bvalue
    bc::basic_bvalue<MyPolicy> decoded2 = bc::decode_value<MyPolicy>(data);

    bc::descriptor_table desc_table = bc::decode_view(data);
    bc::bview root_view = desc_table.get_root();


Encoding
--------

.. code-block:: cpp

    // encode a bvalue
    auto bv = bc::bvalue(bc::type::dict, {{"cow", "moo"}, {"spam", "eggs"}});
    auto bv_enc = bc::encode(bv);

    // true
    (bv_enc == "d3:cow3:moo4:spam4:eggse")

    // encode an event_producer
    auto s = bencode::encode("string");

    // true
    (s == "6:string");

    // encode to a std::stringstream

    std::ostringstream ss {}
    bc::encode_to(ss, 20);

    auto stream_str = ss.str()
    // true
    (stream_str == "i20e")

    // encode to an output_iterator
    std::string out {}
    out.reserve(100);

    bencode::encode_to(out, 20);

    // true
    (out == "i20e")


Encoder
-------

Encoding :cpp:class:`bvalue` instances or object satisfying :cpp:concept:`event_producer`
can be encoded with :cpp:func:`encode` which will return a :cpp:class:`std::string`
or :cpp:func:`encode_to` which accept a cpp:class:`std::ostream`
or :cpp:concept:`output_iterator` as its first argument.

:cpp:func:`decode` will return a :cpp:class:`bvalue` instance and :cpp:func:`decode_view`
will return a descriptor table from which a :cpp:class:`bview` to the root element can be obtained.


:cpp:class:`encoder` provides an output stream interface that serializes objects to bencode
and writes the encoded form to the wrapped storage which can be passed as an
:cpp:concept:`OutputIterator` or as a :cpp:class:`std::ostream`.

This allows to create complex bencoded objects without the need to create an intermediary
:cpp:class:`bvalue`. Bencode container types can be created with the use of the following tags:

* list_begin
* list_end
* dict_begin
* dict_end

Example:

.. code-block::

    std::ostringstream os {};
    auto eos = bencode::encoder(os);

    eos << bencode::dict_begin
        << "key1" << 1
        << "key2" << "two"
        << "key3"
        << bencode::list_begin
            << 1
            << 2
            << 3
        << bencode::list_end
      << bencode::dict_end;

    eos.str() // "d4:key1i1e4:key23:two4:key3li1ei2ei3eee"

Any object that satisfies the :cpp:concept:`event_producer` concept can be serialized using encoding_ostream.





