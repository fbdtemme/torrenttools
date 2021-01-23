#pragma once
#include <stack>
#include <optional>
#include <string_view>
#include <type_traits>
#include <gsl-lite/gsl-lite.hpp>

#include <iostream>

#include "bencode/detail/event_connector.hpp"

namespace bencode {

template <std::output_iterator<char> OutputIt>
class encoder : public event_connector<events::encode_to<OutputIt>>
{
public:
    using base = event_connector<events::encode_to<OutputIt>>;

    explicit encoder(std::basic_ostream<char>& os)
            : consumer_(os), base(consumer_)
    {}

    explicit encoder(std::basic_ostringstream<char>& os)
            : consumer_(os), base(consumer_)
    {}

    explicit encoder(OutputIt it)
            : consumer_(it), base(consumer_)
    {}

private:
    events::encode_to<OutputIt> consumer_;
};

encoder(std::basic_ostream<char>& os) -> encoder<std::ostreambuf_iterator<char>>;

encoder(std::basic_ostringstream<char>& os) -> encoder<std::ostreambuf_iterator<char>>;

template <std::output_iterator<char> OIter>
encoder(OIter it) -> encoder<OIter>;

} // namespace bencode