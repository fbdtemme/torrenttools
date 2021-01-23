Trait headers
=============

Trait headers contain specialisations for :cpp:class:`template <typename T> serialization_traits`.
Including a trait header enables library support for the types specialized in that header.

Following headers can be included to enable support for standard library types.
To include all trait headers, include :file:`bencode/traits/all.hpp`.

================================================= ====================================
  Implementation for                                Header
================================================= ====================================
  fundamental C++ types                             bencode/traits/fundamentals.hpp
  std::array	                                    bencode/traits/array.hpp
  std::deque                                        bencode/traits/deque.hpp
  enum types                                        bencode/traits/enum.hpp
  std::filesystem::path                             bencode/traits/filesystem.hpp
  std::forward_list                                 bencode/traits/forward_list.hpp
  std::list                                         bencode/traits/list.hpp
  std::map        	                                bencode/traits/map.hpp
  std::multimap                                     bencode/traits/map.hpp
  std::pair        	                                bencode/traits/pair.hpp
  std::set                                          bencode/traits/set_traits.hpp
  std::multiset                                     bencode/traits/multiset.hpp
  std::string                                       bencode/traits/string.hpp
  std::string_view                                  bencode/traits/string_view.hpp
  std::stringstream                                 bencode/traits/stringstream.hpp
  std::tuple                                        bencode/traits/tuple.hpp
  std::unordered_map                                bencode/traits/unordered_map.hpp
  std::unordered_multimap                           bencode/traits/unordered_map.hpp
  std::unordered_set                                bencode/traits/unordered_set.hpp
  std::unordered_multiset                           bencode/traits/unordered_set.hpp
  std::valarray                                     bencode/traits/valarray.hpp
  std::vector	                                    bencode/traits/vector.hpp
  std::unique_ptr, std::shared_ptr, std::weak_ptr   bencode/traits/memory.hpp
================================================= ====================================
