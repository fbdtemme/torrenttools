Bencode format
==============

Introduction
------------

Bencode is a simple encoding developed for BitTorrent.
It support 4 basic data types:

*  integer
*  string
*  lists
*  dictionary

The main advantages of bencode are its simplicity and that is unaffected by endianness,
which is important for cross-platform applications.
Another advantage is that their exists a bijection between values and their encoded form.
As a consequence values can be compared in encoded form.

Encoding
--------
Integers
++++++++

Integers are encoded as: :code:`i<base ten ASCII representation>e`.
Leading zeros are not allowed except for the number zero.
Negative values are encoded by prefixing the number with a hyphen-minus.
Negative zero is not permitted.
The number 42 would thus be encoded as :code:`i42e`, 0 as :code:`i0e`, and -42 as :code:`i-42e`.

Strings
+++++++

Strings are length-prefixed base ten followed by a colon and the string: :code:`<length>:<contents>`.
The length is encoded in base 10, like integers and must be positive (zero is allowed).
The contents are the bytes that make up the string.
If the content represents text the encoding must be UTF-8,
but the string datatype can also be used for a sequence of raw bytes.
For example 'spam' is encoded as :code:`4:spam`.

Lists
+++++

A list of values is encoded as an 'l' followed by their encoded elements followed by an 'e': :code:`l<contents>e`.
Elements are in order and concatenated.
A list consisting of the string "spam" and the number 42 would be encoded as: :code:`l4:spami42ee`.

Dictonaries
+++++++++++

Dictionaries are encoded as a 'd' followed by a list of alternating keys and their
corresponding values followed by an 'e': :code:`d<bencoded key><bencoded value>>e`.
Keys must be strings and appear in lexicographicly sorted order.
For example :code:`{'cow': 'moo', 'spam': 'eggs'}` is encoded as :code:`d3:cow3:moo4:spam4:eggse`
and :code:`{'spam': ['a', 'b']}`. as :code:`d4:spaml1:a1:bee`


Sources:
    * `wikipedia - Bencode <https://en.wikipedia.org/wiki/Bencode>`_
    * `BitTorrent protocol specification v1 <http://www.bittorrent.org/beps/bep_0003.html>`_
