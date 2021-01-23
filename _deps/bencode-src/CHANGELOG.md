# Changelog

## v0.5.0

* More consistent naming by changing event_consumer methods from (begin|end)\_(list|dict)
  to (begin|end)\_(list|dict).
* Add more benchmarks.
* Disable contract checks in release build for improved performance.
* Fix parsing from a pair of InputItererators.
* Add string_parsing_mode options to push_parser to allow parsing strings to string_view. 
* Add experimental SSE4.1 and AVX2 integer parsing backends. Enable SWAR integer parsing by default. 
* Add limited std::span serialization/deserialization support.

## v0.4.0

* Reworked parser internals for increased decoding speed.
* SWAR optimizations for fast integer decoding.
* Switch benchmarks to google/benchmark for more consistent results.
* Add benchmark with arvidn/libtorrent.
* Change gsl-lite to the gsl-lite-v1 version.

## v0.3.0

* Add support for std::shared_ptr, std::unique_ptr, std::weak_ptr and raw pointers.
* Add event_connector.
* Add comparison to jimporter/bencode benchmark.

## v0.2.0

*   make `descriptor_table::get_root` not being const qualified.
*   rename `conversion_error` to `bad_conversion`.
*   rename `format_json_to` to `encode_json_to`
*   throw `bencode::out_of_range` instead of `std::out_of_range`
*   add accessor functions to `bview` similar to the accessor functions of `basic_bvalue`:
    *   `at(std::size_t)` 
    *   `at(std::string_view)` 
    *   `operator[](std::size_t)` 
    *   `front()` 
    *   `back()`
*   add `bpointer`: a bencode pointer similar to json pointers.

 
## v0.1.1

*   Fix invalid output of `events::debug_to` for lists and dicts.
*   Replace macro `BENCODE_SERIALIZES_TO_DICT` by `BENCODE_SERIALIZES_TO_DICT_SORTED`
    and `BENCODE SERIALIZED_TO_DICT_UNSORTED`. `serializes_to_dict` 
    now takes an enum non-type template argument `dict_key_order` 
    to indicate if the dict default iteration order is sorted or not.
*   Fix various event producing compilation errors.

## v0.1.0

Initial release.