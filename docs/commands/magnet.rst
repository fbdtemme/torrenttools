.. _magnet_command:

Magnet
======

.. code-block:: none

    Usage: torrenttools magnet [OPTIONS] metafile

    Positionals:
      metafile <path>                  Target bittorrent metafile.

    Options:
      -h,--help                        Print this help message and exit
      -v,--protocol <protocol>         Include only the infohash of the specified protocol for
                                       hybrid metafiles. Options are: 1, 2, hybrid [Default: hybrid].
                                       This option is only used for hybrid metafiles.
                                       When hybrid is specified, hybrid magnet URI's will
                                       include both the v1 and v2 infohash.

