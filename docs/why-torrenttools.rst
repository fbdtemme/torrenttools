Features
========

* **Multiple subcommand**:
    Torrenttools has multiple subcommands to satisfy all kinds of BitTorrent metafile task. Not only creating
    torrent files, but also editing and inspecting metafiles is supported.

* **v2 support**:
    Torrenttools is ready for the new v2 protocol with support for creating hybrid and v2-only metafiles.

* **Multi-threaded hashing**:
    Generating hashes of data is fully asynchronous to fully leverage fast storage.

* **Per-file checksums**:
    Support for multiple checksum algorithms to include a per-file hash inside the BitTorrent metafile.
    This enables clients to deduplicate or detect common files between different torrents.
    For the default OpenSSL backend following checksums are supported:

    * whirlpool
    * ripemd160
    * shake256
    * shake128
    * sha3_512
    * sha3_384
    * sha3_256
    * sha3_224
    * sha512
    * sha384
    * sha256
    * sha224
    * sha1
    * blake2s_256
    * blake2b_512
    * md5
    * md4

* **Named trackers**:
    Named trackers allow uploaders to quickly generate metafiles without having to lookup announce urls for different trackers.


Supported BEPS
----------------

Bittorrent Enhancement proposals that do not have any impact on the metafiles are not shown.

.. csv-table:: BEP support
    :file: bep-support.csv
    :widths: 30, 30, 70
    :header-rows: 1

Notes:

* BEP-30 is deprecated with the introduction of BEP-53
