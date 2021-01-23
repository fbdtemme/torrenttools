#pragma once

#include <bencode/bvalue.hpp>
#include <bencode/events/event_connector.hpp>

#include "dottorrent/file_storage.hpp"
#include "dottorrent/file_entry.hpp"
#include "dottorrent/metafile.hpp"

namespace dottorrent::detail {

namespace bc = bencode;

bencode::bvalue make_bvalue_infodict_v1(const metafile& m);

bencode::bvalue make_bvalue_infodict_v2(const metafile& m);

bencode::bvalue make_bvalue_infodict_hybrid(const metafile& m);

bencode::bvalue make_bvalue_common(const metafile& m);

bencode::bvalue make_bvalue_v1(const metafile& m);

bencode::bvalue make_bvalue_v2(const metafile& m);

bencode::bvalue make_bvalue_hybrid(const metafile& m);

}

namespace dottorrent {

template <std::output_iterator<char> OutputIt>
void write_metafile_to(OutputIt out, const metafile& m, protocol protocol_version = protocol::v1)
{
    bencode::bvalue bv;

    if (protocol_version == protocol::v1) {
        bv = detail::make_bvalue_v1(m);
    }
    else if (protocol_version == protocol::v2) {
        bv = detail::make_bvalue_v2(m);
    }
    else if (protocol_version == protocol::hybrid) {
        bv = detail::make_bvalue_hybrid(m);
    } else {
        throw std::invalid_argument("unrecognised protocol version");
    }
    bencode::encode_to(out, bv);
}

}