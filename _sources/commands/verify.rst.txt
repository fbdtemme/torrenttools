.. _verify_command:

Verify
=======

Overview
---------

.. code-block:: none

    Verify local data against bittorrent metafiles.
    Usage: torrenttools verify [OPTIONS] metafile target

    Positionals:
      metafile <path>                  Metafile path.
      target <path>                    Target filename or directory to verify pieces for.

    Options:
      -h,--help                        Print this help message and exit
      -v,--protocol <protocol>         Set the bittorrent protocol to use. Options are 1, 2 or hybrid. [default: 1]
      -t,--threads <n>                 Set the number of threads to use for hashing. [default: 2]


