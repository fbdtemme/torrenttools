Descriptor
==========

The :cpp:class:`descriptor` describes the type and content of the bencoded token the
:cpp:class:`bview` points to and where the data can be found in the bencoded buffer.
This information allows :cpp:class:`bview` to navigate through the bencoded buffer and
provide access to the values.

A :cpp:class:`descriptor_table` holds a contiguous sequence of descriptors and a pointer to a bencoded string.
it is the result of parsing data with :cpp:class:`descriptor_parser`.

Details
-------

A descriptor is a 16 byte structure that describes the structural elements of a bencoded value.
Integers and string have one :cpp:class:`descriptor`.
Lists and dicts have one :cpp:class:`descriptor` at the start and one :cpp:class:`descriptor`
descriptor at the end of the datastructure.

A :cpp:class:`descriptor` stores the type of the bencode data type and the position relative to the start of the buffer in a :cpp:enum:`descriptor_type`
value. The data stored depends on the type of the bencode value:

| :code:`type`: the bencode data type
| :code:`position`: the position in the bencoded data buffer relative to the start.

*  **integer**:
    | :code:`value`: the integer value.
*  **string**:
    | :code:`offset`: the position in the string where the actual string data starts.
    | :code:`size`: the size of the string data.
*  **list**:
    | :code:`offset`: the number of descriptors until the list end descriptor.
    | :code:`size`: the size of the list
*  **dict**:
    | :code:`offset`: the number of descriptors until the dict end descriptor.
    | :code:`size`: the size of the dict


Following examples shows how descriptors describe the bencoded data:

.. code-block:: text

    bencoded data:

    0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17
    | | | | | | | | | |  |  |  |  |  |  |  |
    l i e 1 l 3 : f o o  3  :  b  a  r  e  e

    json formatted : [ 1, ["foo", "bar"] ]

    +-----+--------------------------------------------------------------------+
    | idx |  descriptor                                                        |
    +-----+--------------------------------------------------------------------+
    | 0   |   (type: list,                 position: 0,  offset: 3, size: 2)   |
    | 1   |   (type: integer | list_value  position: 1,  value:1           )   |
    | 2   |   (type: list    | list_value  position: 4,  offset: 3, size: 2)   |
    | 3   |   (type: string  | list_value  position: 5,  offset: 2, size: 3)   |
    | 4   |   (type: string  | list_value  position: 10, offset: 2, size: 3)   |
    | 5   |   (type: list    | end         position: 15, offset: 3, size: 2)   |
    | 6   |   (type: list    | end         position: 16, offset: 6, size: 2)   |
    | 7   |   (type: stop,                 position: 17, offset: 0, size: 0)   |
    +-----+--------------------------------------------------------------------+

.. code-block::

    bencoded data:

    0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
    | | | | | | | | | |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    d 4 : s p a m i 1 e  3  :  b  a  r  l  i  1  e  i  2  e  e  e

    json formatted : { "spam": 1, "bar": [1, 2] }

    +-----+--------------------------------------------------------------------+
    | idx |  descriptor                                                        |
    +-----+--------------------------------------------------------------------+
    | 0   |   (type: dict,                 position: 0,  offset: 8, size: 2)   |
    | 1   |   (type: string  | dict_key,   position: 1,  offset: 2, size: 4)   |
    | 2   |   (type: integer | dict_value, position: 7,  value: 1          )   |
    | 3   |   (type: string  | dict_key,   position: 10, offset: 2, size: 3)   |
    | 4   |   (type: list    | dict_value, position: 15, offset: 3, size: 2)   |
    | 5   |   (type: integer | list_value, position: 16, value: 1          )   |
    | 6   |   (type: integer | list_value, position: 19, value: 2          )   |
    | 7   |   (type: list    | end,        position: 22, offset: 3, size: 2)   |
    | 8   |   (type: dict    | end,        position: 23, offset: 8, size: 2)   |
    | 9   |   (type: stop,                 position: 24)                       |
    +-----+--------------------------------------------------------------------+



