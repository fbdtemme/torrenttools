Create
======

The create command is used to create new BitTorrent metafiles.

The basic invocation requires only a target directory or file to create a .torrent from.

.. code-block:: bash

    torrenttools create [OPTIONS] <target>

Either specify all options first or give the target first followed by all options.

Options
-------
``-v,--protocol``
+++++++++++++++++

Set the bittorrent protocol to generate a metafile for. Available options are: 1, 2 or hybrid.
The default options is to create v1-only metafiles. 1 and 2 can be prefixed with a v.

.. code-block:: bash

    torrenttools create --protocol v1 test-dir
    torrenttools create -v1 test-dir
    torrenttools create -vhybrid test-dir


``-o,--output``
+++++++++++++++

Set the filename and/or output directory of the created file.
If no options are given the torrent will be written to <name>.torrent in the current directory,
with name the name of target directory or file.

.. code-block:: bash

    torrenttools create test-dir --output "my-name.torrent"


Use a path with trailing slash to only set the output directory and keep the name to the default

.. code-block:: bash

    torrenttools create test-dir --output ~/torrents/

Will create a torrent with full path ~/torrents/test-dir.torrent


``-a,--announce``
++++++++++++++++
Add one or multiple announces urls.

.. code-block:: bash

    torrenttools create test-dir --announce "url1" "url2"

Multiple trackers will be added in seperate tiers by default. Use square brackets to groups urls in a single tier.

.. code-block:: bash

    torrenttools create test-dir --announce "[url1 url2]"

``--stdout``
++++++++++++
Write the torrent to the standard output. Normal output will be redirected to stderr.

``-w,--web-seed``
+++++++++++++++++
Add one or multiple HTTP/FTP urls as seeds.

``-d,--dht-node``
+++++++++++++++++
Add one or multiple DHT nodes. The expected format is <host>:<port>.

.. code-block::

    torrenttools create test-dir ---dht-node "127.0.0.1:8686" "192.168.0.0:9999:

``-c,--comment``
+++++++++++++++
Add a comment.

.. code-block:

    torrenttools create test-dir --comment "Hello there!"

``-p,--private``
++++++++++++++++
Set the private flag to disable DHT and PEX.
When no options are given this will enable the private flag.
Pass "on" or "off" to override the defaults when using supported trackers     .

``-l,--piece-size``
+++++++++++++++++++
Set the piece size.
When no unit is specified block size will be either 2^<n> bytes or <n> bytes if n is larger or equal to 16384.
Piece size must be a power of two in range 16K to 64M.
Leave empty or pass "auto" to determine by total file size.

Piece size as a power of two. (2**20 = 1MiB)

.. code-block::

    torrenttools create test-dir --piece-size 20

Piece size as a size in bytes. (65536 = 64 KiB)

.. code-block::

    torrenttools create test-dir --piece-size 65536

Piece size with unit

.. code-block::

    torrenttools create test-dir --piece-size 2M
    torrenttools create test-dir --piece-size 2MiB
    torrenttools create test-dir --piece-size "2 MiB"


``-s,--source``
+++++++++++++++
Add a source tag to facilitate cross-seeding.

``-n,--name``
+++++++++++++

Set the name of the torrent. The default option to use the basename of the target.

.. warning::

    This options changes the filename inside the torrent for single file torrents
    or the root directory name for multi-file torrents. Use with caution.

``-t,--threads``
++++++++++++++++

Set the number of threads to use for hashing pieces. Default is 2.

.. note::

    The hashing bottleneck is usually the maximum sequential read speed of you storage device
    so only increasing this as long as you notice a difference.
    Increasing this usually makes sense only for very fast SSD or Optane storage.

``--checksum``
+++++++++++++++
Include a per file checksum for given algorithm.
The possible options depend on the cryptographic library used.
All possible options can be listed with:

.. code-block::

    torrenttools --checksum-algorithms

.. note::

    This is only useful for v1 metafiles.
    v2 and hybrid metafiles have per-file merkle roots which makes this options redundant.

``--no-creation-date``
++++++++++++++++++++++
Do not include the current date in the  creation date field.

``--creation-date``
+++++++++++++++++++
Override the value of the creation date field as an ISO-8601 time or POSIX time string.

.. code-block::

    torrentools create test-dir --creation-date "2021-01-22T18:21:46Z+0100"
    torrentools create test-dir --creation-date 1611339706

``--no-created-by``
+++++++++++++++++++
Do not include the name and version of this program.

``--include-hidden``
++++++++++++++++++++
Do not skip hidden files when scanning the target directory for files.

.. code-block::

    torrenttools create test-dir --include-hidden

``--created-by``
++++++++++++++++
Override the value of the created by field.

.. code-block::

    torrenttools test-dir --created-by "Me"

``--include``
+++++++++++++

.. code-block::

    torrenttools create test-dir --include "

.. note::

    When the include pattern matches hidden files these will be included in the torrent
    even if --include-hidden was not specified.

``--exclude``
+++++++++++++
Do not add files matching given regex to the metafile. Multiple patterns can be specified.
When used together with --include, the include patterns will be evaluated first and further filtered by the exclude patterns.


``--io-block-size``
+++++++++++++++++++
The size of blocks read from storage.
Set to a large value for disks used heavy load to reduce the number of IO operations per second.
This value must be larger or equal to the piece-size.


