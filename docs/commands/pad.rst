.. _pad_command:

Pad
======

The pad command is used to generate padding files for BitTorrent metafiles that contain those.
BitTorrent clients that do not support padding files will download these padding files the same way as any other file.
Since these files do not contain any useful content they are often removed after downloading.
When a user later decides to start seeding a torrent again the padding files that were removed are now missing
and the torrent will not check 100% in the bittorrent client. The pad command can regenerates these missing files.

.. code-block:: none

    Generate padding files for a BitTorrent metafile.
    Usage: torrenttools pad [OPTIONS] target metafile

    Positionals:
      target <path>                    Target directory.
      metafile <metafile>              Target bittorrent metafile.

    Options:
      -h,--help                        Print this help message and exit



