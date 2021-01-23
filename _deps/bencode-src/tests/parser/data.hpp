#include <string_view>

using namespace std::string_view_literals;

inline constexpr auto recursion_limit_list = "lllllllllllllllllllleeeeeeeeeeeeeeeeeeee";
inline constexpr auto recursion_limit_dict = "d1:ad1:bd1:cd1:dd1:ed1:fd1:gd1:hd1:id1:jd1:ki1eeeeeeeeeeee";
inline constexpr auto error_integer = "i002e";
inline constexpr auto error_string = "5:foo";
inline constexpr auto error_dict_key = "d41333i1ee";

inline constexpr auto error_missing_end_list = "llllleeee";
inline constexpr auto error_missing_end_dict = "d1:ad1:bd1:ci1eee";
inline constexpr auto error_missing_value = "bar";
inline constexpr auto error_missing_dict_value = "d1:ae";
inline constexpr auto error_missing_list_value_or_end = "li1e!e"sv;
inline constexpr auto error_missing_dict_key_or_end = "d3:key5:valueli1ee"sv;


inline constexpr auto example = (
        "d"
        "3:one" "i1e"
        "5:three" "l" "d" "3:bar" "i0e" "3:foo" "i0e" "e" "e"
        "3:two" "l" "i3e" "3:foo" "i4e" "e"
        "e"sv
);

inline constexpr auto example_json_result = (
        R"({
    "one": 1,
    "three": [
        {
            "bar": 0,
            "foo": 0
        }
    ],
    "two": [
        3,
        "foo",
        4
    ]
})"
);

inline constexpr auto sintel_torrent = (
        "d8:announce40:udp://tracker.leechers-paradise.org:696913:announce-listll40:udp://tracker.leechers-paradise.org:6969el34:udp://tracker.coppersurfer.tk:6969el33:udp://tracker.opentrackr.org:1337el23:udp://explodie.org:6969el31:udp://tracker.empire-js.us:1337el26:wss://tracker.btorrent.xyzel32:wss://tracker.openwebtorrent.comel25:wss://tracker.fastcast.nzee"
        "7:comment34:WebTorrent <https://webtorrent.io>10:created by34:WebTorrent <https://webtorrent.io>13:creation datei1490916637e8:encoding5:UTF-8"
        "4:infod5:filesld6:lengthi1652e4:pathl13:Sintel.de.srteed6:lengthi1514e4:pathl13:Sintel.en.srteed6:lengthi1554e4:pathl13:Sintel.es.srteed6:lengthi1618e4:pathl13:Sintel.fr.srteed6:lengthi1546e4:pathl13:Sintel.it.srteed6:lengthi129241752e4:pathl10:Sintel.mp4eed6:lengthi1537e4:pathl13:Sintel.nl.srteed6:lengthi1536e4:pathl13:Sintel.pl.srteed6:lengthi1551e4:pathl13:Sintel.pt.srteed6:lengthi2016e4:pathl13:Sintel.ru.srteed6:lengthi46115e4:pathl10:poster.jpgeee4:name6:Sintel12:piece lengthi131072e6:pieces0:e"
        "8:url-listl31:https://webtorrent.io/torrents/ee"sv
);


inline constexpr auto sintel_json_result = (
        R"({
    "announce": "udp://tracker.leechers-paradise.org:6969",
    "announce-list": [
        [
            "udp://tracker.leechers-paradise.org:6969"
        ],
        [
            "udp://tracker.coppersurfer.tk:6969"
        ],
        [
            "udp://tracker.opentrackr.org:1337"
        ],
        [
            "udp://explodie.org:6969"
        ],
        [
            "udp://tracker.empire-js.us:1337"
        ],
        [
            "wss://tracker.btorrent.xyz"
        ],
        [
            "wss://tracker.openwebtorrent.com"
        ],
        [
            "wss://tracker.fastcast.nz"
        ]
    ],
    "comment": "WebTorrent <https://webtorrent.io>",
    "created by": "WebTorrent <https://webtorrent.io>",
    "creation date": 1490916637,
    "encoding": "UTF-8",
    "info": {
        "files": [
            {
                "length": 1652,
                "path": [
                    "Sintel.de.srt"
                ]
            },
            {
                "length": 1514,
                "path": [
                    "Sintel.en.srt"
                ]
            },
            {
                "length": 1554,
                "path": [
                    "Sintel.es.srt"
                ]
            },
            {
                "length": 1618,
                "path": [
                    "Sintel.fr.srt"
                ]
            },
            {
                "length": 1546,
                "path": [
                    "Sintel.it.srt"
                ]
            },
            {
                "length": 129241752,
                "path": [
                    "Sintel.mp4"
                ]
            },
            {
                "length": 1537,
                "path": [
                    "Sintel.nl.srt"
                ]
            },
            {
                "length": 1536,
                "path": [
                    "Sintel.pl.srt"
                ]
            },
            {
                "length": 1551,
                "path": [
                    "Sintel.pt.srt"
                ]
            },
            {
                "length": 2016,
                "path": [
                    "Sintel.ru.srt"
                ]
            },
            {
                "length": 46115,
                "path": [
                    "poster.jpg"
                ]
            }
        ],
        "name": "Sintel",
        "piece length": 131072,
        "pieces": ""
    },
    "url-list": [
        "https://webtorrent.io/torrents/"
    ]
})"
);

inline constexpr auto expected_dict_value = "d3:keye"sv;
inline constexpr auto string_decode_error = "4!:spam"sv;