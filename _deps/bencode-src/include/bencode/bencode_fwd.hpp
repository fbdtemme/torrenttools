

///@file forward declarations

namespace bencode {

template<
        typename IntegralType,
        typename StringType,
        template <typename T> typename ListType,
        template <typename K, typename V> typename DictType
>
struct bvalue_policy;

struct default_bvalue_policy;
template <typename Policy> class basic_bvalue;

class bview;
class integer_bview;
class string_bview;
class list_bview;
class dict_bview;

class bpointer;

class exception;
class bad_access;
class bad_bview_access;
class bad_bvalue_access;
class out_of_range;
class parsing_error;
class bad_conversion;
class bpointer_error;
class encoding_error;

class descriptor;
class descriptor_table;

class descriptor_parser;
enum class string_parsing_mode : bool;

}