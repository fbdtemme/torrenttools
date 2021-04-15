.. _info_command:

Info
======

The info command is used to print an overview of BitTorrent metafiles features.

The basic invocation requires only the target .torrent file.

.. code-block:: bash

    torrenttools info <target>

Either specify all options first or give the target first followed by all options.

Overview
---------

.. code-block:: none

    General information about bittorrent metafiles.
    Usage: torrenttools info [OPTIONS] target

    Positionals:
      target <path>                    Target bittorrent metafile.

    Options:
      -h,--help                        Print this help message and exit
      --raw                            Print the metafile data formatted as JSON. Binary data is filtered out.
      --show-pieces                    Print the metafile data formatted as JSON.
                                       Binary data is included as hexadecimal strings.
      --show-padding-files             Show padding files in the file tree.

Output looks as follows:

.. code-block::

    Metafile:         bittorrent-v2-hybrid-test.torrent
    Protocol version: v1 + v2 (hybrid)
    Infohash:         v1: 8c9a2f583949c757c32e085413b581067eed47d0
                      v2: d8dd32ac93357c368556af3ac1d95c9d76bd0dff6fa9833ecdac3d53134efabb
    Piece size:       512 KiB (524288 bytes)
    Created by:       libtorrent
    Created on:       2020-06-03 08:45:06 UTC
    Private:          false
    Name:             bittorrent-v1-v2-hybrid-test
    Source:
    Comment:

    Announces:

    Files:
      bittorrent-v1-v2-hybrid-test
      ├── [6.23 MiB] Darkroom (Stellar, 1994, Amiga ECS) HQ.mp4
      ├── [19.6 MiB] Spaceballs-StateOfTheArt.avi
      ├── [ 326 MiB] cncd_fairlight-ceasefire_(all_falls_down)-1080p.mp4
      ├── [58.8 MiB] eld-dust.mkv
      ├── [ 265 MiB] fairlight_cncd-agenda_circling_forth-1080p30lq.mp4
      ├── [42.5 MiB] meet the deadline - Still _ Evoke 2014.mp4
      ├── [61.0   B] readme.txt
      ├── [25.1 MiB] tbl-goa.avi
      └── [ 111 MiB] tbl-tint.mpg

      854.06 MiB in 0 directories, 9 files

Options
-------
``--raw``
++++++++++
This options will print a JSON representation of the torrent with binary fields replaced by a string descibing the content.
The torrent from the previous example will output as follows

.. code-block::

    {
        "created by": "libtorrent",
        "creation date": 1591173906,
        "info": {
            "file tree": {
                "Darkroom (Stellar, 1994, Amiga ECS) HQ.mp4": {
                    "": {
                        "length": 6535405,
                        "pieces root": "<merkle root 8e31d30f9e25c6336768e978261219c8a9ee8ba81f1bf8a07d8b84664428ccc6>"
                    }
                },
                "Spaceballs-StateOfTheArt.avi": {
                    "": {
                        "attr": "x",
                        "length": 20506624,
                        "pieces root": "<merkle root ef988424c9c4eb263d55dd84ed2cfc366f60a994e9c80b0dd6979c822ed451ae>"
                    }
                },
                "cncd_fairlight-ceasefire_(all_falls_down)-1080p.mp4": {
                    "": {
                        "length": 342230630,
                        "pieces root": "<merkle root 697d9b53c31b6185867e5df15dba2a7e81fdda6c9aedeaaa83a1d6441989919a>"
                    }
                },
                "eld-dust.mkv": {
                    "": {
                        "length": 61638604,
                        "pieces root": "<merkle root c7a9116ac109bc3283f28f1561c417e758ff803ab3a51bc1141f9ad24015b59a>"
                    }
                },
                "fairlight_cncd-agenda_circling_forth-1080p30lq.mp4": {
                    "": {
                        "length": 277889766,
                        "pieces root": "<merkle root 07e3096be336c1383533bba2d01f0a7ca5fbc5c127fb671d0fbe8bfc38ec9972>"
                    }
                },
                "meet the deadline - Still _ Evoke 2014.mp4": {
                    "": {
                        "length": 44577773,
                        "pieces root": "<merkle root cfa94f471a79086ae919ceb8c36e9748435043920692ba4b22f937d72d55f5fc>"
                    }
                },
                "readme.txt": {
                    "": {
                        "attr": "x",
                        "length": 61,
                        "pieces root": "<merkle root 3b3c50a12e27a6b3421b817afd49dfa0d54f69a086cf9914164a9f516e7416e4>"
                    }
                },
                "tbl-goa.avi": {
                    "": {
                        "attr": "x",
                        "length": 26296320,
                        "pieces root": "<merkle root cd4403e73c8f92fa9ed322f946a5de509f8e774fa3e4f713a3c3b785a085510f>"
                    }
                },
                "tbl-tint.mpg": {
                    "": {
                        "length": 115869700,
                        "pieces root": "<merkle root 77e9b83c9428ccfaf2423de0e8e4f7ae5ad52fdbc65a29af3d5dc3abb2fc420a>"
                    }
                }
            },
            "files": [
                {
                    "length": 6535405,
                    "path": [
                        "Darkroom (Stellar, 1994, Amiga ECS) HQ.mp4"
                    ]
                },
                {
                    "attr": "p",
                    "length": 280339,
                    "path": [
                        ".pad",
                        "280339"
                    ]
                },
                {
                    "attr": "x",
                    "length": 20506624,
                    "path": [
                        "Spaceballs-StateOfTheArt.avi"
                    ]
                },
                {
                    "attr": "p",
                    "length": 464896,
                    "path": [
                        ".pad",
                        "464896"
                    ]
                },
                {
                    "length": 342230630,
                    "path": [
                        "cncd_fairlight-ceasefire_(all_falls_down)-1080p.mp4"
                    ]
                },
                {
                    "attr": "p",
                    "length": 129434,
                    "path": [
                        ".pad",
                        "129434"
                    ]
                },
                {
                    "length": 61638604,
                    "path": [
                        "eld-dust.mkv"
                    ]
                },
                {
                    "attr": "p",
                    "length": 227380,
                    "path": [
                        ".pad",
                        "227380"
                    ]
                },
                {
                    "length": 277889766,
                    "path": [
                        "fairlight_cncd-agenda_circling_forth-1080p30lq.mp4"
                    ]
                },
                {
                    "attr": "p",
                    "length": 507162,
                    "path": [
                        ".pad",
                        "507162"
                    ]
                },
                {
                    "length": 44577773,
                    "path": [
                        "meet the deadline - Still _ Evoke 2014.mp4"
                    ]
                },
                {
                    "attr": "p",
                    "length": 510995,
                    "path": [
                        ".pad",
                        "510995"
                    ]
                },
                {
                    "attr": "x",
                    "length": 61,
                    "path": [
                        "readme.txt"
                    ]
                },
                {
                    "attr": "p",
                    "length": 524227,
                    "path": [
                        ".pad",
                        "524227"
                    ]
                },
                {
                    "attr": "x",
                    "length": 26296320,
                    "path": [
                        "tbl-goa.avi"
                    ]
                },
                {
                    "attr": "p",
                    "length": 442368,
                    "path": [
                        ".pad",
                        "442368"
                    ]
                },
                {
                    "length": 115869700,
                    "path": [
                        "tbl-tint.mpg"
                    ]
                }
            ],
            "meta version": 2,
            "name": "bittorrent-v1-v2-hybrid-test",
            "piece length": 524288,
            "pieces": "<1715 piece hashes>"
        },
        "piece layers": {
            "<merkle root 07e3096be336c1383533bba2d01f0a7ca5fbc5c127fb671d0fbe8bfc38ec9972>": "<531 piece hashes>",
            "<merkle root 697d9b53c31b6185867e5df15dba2a7e81fdda6c9aedeaaa83a1d6441989919a>": "<653 piece hashes>",
            "<merkle root 77e9b83c9428ccfaf2423de0e8e4f7ae5ad52fdbc65a29af3d5dc3abb2fc420a>": "<222 piece hashes>",
            "<merkle root 8e31d30f9e25c6336768e978261219c8a9ee8ba81f1bf8a07d8b84664428ccc6>": "<13 piece hashes>",
            "<merkle root c7a9116ac109bc3283f28f1561c417e758ff803ab3a51bc1141f9ad24015b59a>": "<118 piece hashes>",
            "<merkle root cd4403e73c8f92fa9ed322f946a5de509f8e774fa3e4f713a3c3b785a085510f>": "<51 piece hashes>",
            "<merkle root cfa94f471a79086ae919ceb8c36e9748435043920692ba4b22f937d72d55f5fc>": "<86 piece hashes>",
            "<merkle root ef988424c9c4eb263d55dd84ed2cfc366f60a994e9c80b0dd6979c822ed451ae>": "<40 piece hashes>"
        }
    }


``--show-pieces``
+++++++++++++++++
This options must be combined with ``--raw``. Instead of a string like <20 piece hashes> a full list with all pieces in
hexadecimal representation will be printed.

.. code-block::

    {
       "pieces": [
            "<piece: 0, SHA1: 23fb3eeb2fb05ab3359f6c27ca2d6e6630f31469>",
            "<piece: 1, SHA1: f29c9d5fe198834cc67a95c86a14526a7776e916>",
            "<piece: 2, SHA1: 6fc9a5304d0206edce9243ac4ab86c442f38d4e4>",
            "<piece: 3, SHA1: 00c48068e56dcd8871e2d1056365688dbe67c23e>",
            ...
            ...
            "<piece: 1713, SHA1: 46b4d380397c28b8f0f588b3a3069adb4abb4ff9>",
            "<piece: 1714, SHA1: 81a78f2ca2bebdfc662722b943736863f342ca3c>"
       ]
    },
    "piece layers": {
        "<merkle root 07e3096be336c1383533bba2d01f0a7ca5fbc5c127fb671d0fbe8bfc38ec9972>": [
            "<piece: 0, SHA256: 0b28e608a4ef66e5da29939d6b39d82a95e68f8a>",
            "<piece: 1, SHA256: c917113c2c577995f153eae664166501e973d43a>",
            "<piece: 2, SHA256: d72af73e0a219cee3ceea72f7b6630dbf5401642>",
            ...
        ],
        ...
    }

``--show-padding-files``
+++++++++++++++++++++++++

This option will include padding files for hybrid torrent in the file tree.
By default padding files are not listed.