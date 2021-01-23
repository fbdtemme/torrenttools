.. cpp:namespace:: bencode


Events system
================

The events interface allows different parts of the library to communicate and to efficiently
convert between data representations.
It consists of two concepts :cpp:concept:`event_consumer` and :cpp:concept:`event_producer`.

Event consumers
---------------

The :cpp:concept:`event_consumer` concepts is modeled after the SAX-interface for XML.
The most simple consumer which ignores all events would look as follows:

.. code-block:: cpp

    struct discard_consumer
    {
        void integer(std::int64_t value) { }
        void string(std::string_view value) { }
        void list_begin() {}
        void list_begin(std::size_t size) {}
        void list_end() {}
        void list_end(std::size_t size) {}
        void list_item() {}
        void dict_begin() {}
        void dict_begin(std::size_t size) {}
        void dict_end() {}
        void dict_end(std::size_t size) {}
        void dict_key() {}
        void dict_value() {}
    };

Every bencode data structure can be described by successive calls to this interface.
Following bencode data structure (formatted as JSON) would generate these calls.

.. code-block::

    {
      "foo": ["a", "b"],
      "bar": {"one": 1, "two": 2}
    }

    dict_begin(2)
        string("foo")
        dict_key()
        list_begin(2)
            begin_string("a")
            list_item()
            string("b")
            list_item()
        list_end()
        dict_value()
        string("bar")
        dict_key()
        dict_begin(2)
            string("one")
            dict_key()
            integer(1)
            dict_value()
            string("two")
            dict_key()
            integer(2)
            dict_value()
        dict_end()
        dict_value()
    dict_end()

Note that list_item(), dict_key() and dict_value() are called after describing the value.

Event producers
---------------

The :cpp:concept:`event_producer` concept is defined as any object that can generate calls to a
class satisfying the :cpp:concept:`event_consumer` concept through the
:cpp:func:`connect` method.


Connecting consumer and producers
---------------------------------

:cpp:concept:`event_producers` are connected to :cpp:concept:`event_consumers`
with the connect function.
Connecting a producer and consumer will generate calls from the producer to the consumers interface.

:code:`connect(EC& consumer, U&& producer)`

Available consumers
-------------------

:cpp:class:`bencode::events::debug_to` will print events to an output stream or output iterator.
It is defined in ``<bencode/events/debug_to.hpp>``.

:cpp:class:`bencode::events::encode_to` will convert events into bencode.
This consumer is used by the ``encode`` and ``encode_to`` convenience functions.
It is defined in ``<bencode/events/encode_to.hpp>``.

:cpp:class:`bencode::events::format_json_to` will convert events into json;
It is defined in ``<bencode/events/format_json_to.hpp>``.
