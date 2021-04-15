.. _edit_command:

Edit
=====

The edit commands is used to modify metadata fields in an existing bittorrent file.
Many of the options are the same as those for the `create <create_command>`_ command

Overview
---------

.. code-block:: none

    Edit bittorrent metafiles.
    Usage: torrenttools edit [OPTIONS] target

    Positionals:
      target <path>                    Target bittorrent metafile.

    Options:
      -h,--help                        Print this help message and exit
      -o,--output <path>               Set the filename and/or output directory of the edited file.
                                       Default [<name>.torrent].
                                       This will overwrite the existing file if the name is the same.
                                       Use a path with trailing slash to only set the output directory.
      -m,--list-mode <list-mode>       How to modify options that accept multiple arguments.
                                       Options are: append, prepend, replace. [Default: replace]
                                       The first character of these options is valid as well.
      -a,--announce <url>...           Add one or multiple announces urls.
                                       Multiple trackers will be added in seperate tiers by default.
                                       Use square brackets to groups urls in a single tier:
                                       eg. "url1 [url1 url2]"
      -g,--announce-group <name>...    Add the announce-urls defined from an announce group specified in the configuration file.
                                       Multiple groups can be passed. eg. "--announce-group group1 group2"
      -w,--web-seed <url>...           Add one or multiple HTTP/FTP urls as seeds.
      -d,--dht-node <host:port>...     Add one or multiple DHT nodes.
      -c,--comment <string>            Replace the comment.
                                        Set to an empty string to remove the field.
      -p,--private <[on|off]>          Set the private flag to disable DHT and PEX.
                                       Pass off to disable the flag.
      -s,--source <source>             Replace the source tag.
                                       Set to an empty string to remove the field.
      -n,--name <name>                 Replace the name of the torrent.
                                       This changes the filename for single file torrents
                                       or the root directory name for multi-file torrents.

      --no-creation-date               Do not include the creation date.
      --creation-date <ISO-8601|POSIX time>
                                       Replace the creation date field.
                                       Input is expected as an ISO-8601 or POSIX timestamp.
                                       Example: "2021-01-22T18:21:46+0100"
                                       Set to an empty string to remove the field.
      --no-created-by                  Do not include the name and version of this program.
      --created-by <string>            Replace the created-by field.
                                       Set to an empty string to remove the field.
      --stdout                         Write the edited metafile to the standard output

Options
--------
``-o,--output``
++++++++++++++++++++++
Set the filename and/or output directory of the edited file.
Use a trailing slash to specify the output directory.

``-m,--list-mode``
+++++++++++++++++++
Control how the options ``--announce``, ``--web-seed`` and ``dht-node`` behave.
The possible options are: replace, append, prepend or the equivalent r, a, p.
The default mode for multi-argument options is replace.

The follow command will append the new tracker at the end of the announce list.

.. code-block:: bash

    torrenttools edit --list-mode append test.torrent --announce "https://new-announce-server.org/announce"


``a,--announce``
++++++++++++++++
Edit the announces of a bittorrent metafile.

.. code-block:: bash

    torrenttools edit test-dir --announce "url1" "url2"

Multiple trackers will be added in seperate tiers by default. Use square brackets to groups urls in a single tier.

.. code-block:: bash

    torrenttools create test-dir --announce "[url1 url2]"


`` -g,--announce-group``
+++++++++++++++++++++++++
Add all announces inside a tracker group to the metafile. Thes eopt

.. code-block:: bash

    torrenttools edit test-dir --announce-group "public-trackers"


``-w,--web-seed``
+++++++++++++++++
Edit the web-seeds of a bittorrent metafile.


``-d,--dht-node <host:port>``
+++++++++++++++++++++++++++++


``-c,--comment``
++++++++++++++++
Replace or remove a comment from a bittorrent metafile.

Pass an empty string to remove an existing comment:

.. code-block::

    torrenttools edit target.torrent --comment ""

``-p,--private``
++++++++++++++++++

Edit the private flag of a bittorrent metafile.
Passing the flag without any arguments or with "on" or 1 will enable the flag.
Passing the flag with "off" or 0 will disable the flag.

.. code-block::

    torrenttools edit public.torrent --private --output private.torrent


``-s,--source``
+++++++++++++++
Edit the source tag in a bittorrent metafile.
This field is used by private trackers to avoid the risk of torrents with the same infohash but different announce-urls.
It is set to the name of the tracker for supported trackers.

.. code-block::

    torrenttools edit test.torrent --source "my tracker"

``-n,--name``
++++++++++++++
Replace the name of the torrent.
This changes the filename for single file torrents or the root directory name for multi-file torrents.

``--no-creation-date``
+++++++++++++++++++++++

Do not update the creation date when editing other fields.

``--creation-date``
+++++++++++++++++++
Override the value of the creation date field as an ISO-8601 time or POSIX time string.

.. code-block::

    torrentools edit test-dir --creation-date "2021-01-22T18:21:46+0100"
    torrentools edit test-dir --creation-date 1611339706


``--no-created-by``
+++++++++++++++++++
Do not include the name and version of this program.


``--created-by``
++++++++++++++++
Override the value of the created by field.

.. code-block::

    torrenttools test-dir --created-by "Me"





