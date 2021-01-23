View interface
==============

.. contents::

.. _bview_reference:

bview
-----

.. doxygenclass:: bencode::bview
   :members:

integer_bview
-------------

.. doxygenclass:: bencode::integer_bview
   :members:

string_bview
------------

.. doxygenclass:: bencode::string_bview
   :members:

list_bview
----------

.. doxygenclass:: bencode::list_bview
   :members:

dict_bview
----------

.. doxygenclass:: bencode::dict_bview
   :members:

Accessor functions
------------------

.. doxygenfile:: detail/bview/accessors.hpp


Exceptions
----------

.. doxygenfile:: detail/bview/bad_bview_access.hpp

.. doxygenfile:: detail/bad_conversion.hpp

Concepts
--------

.. doxygenfile:: detail/bview/concepts.hpp
