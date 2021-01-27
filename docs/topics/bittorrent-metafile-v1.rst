BitTorrent metafile v1
=======================

Introduction
-------------

Metainfo files (also known as .torrent files) are bencoded dictionaries with the following keys:

* announce: The URL of the tracker.
* info : This maps to a dictionary, with keys described below.

All strings in a .torrent file that contains text must be UTF-8 encoded.

info dictionary
---------------
name
++++
The **name** key maps to a UTF-8 encoded string which is the suggested name to save the file (or directory) as.
It is purely advisory.

piece length
++++++++++++
The **piece length** maps to the number of bytes in each piece the file is split into.
For the purposes of transfer, files are split into fixed-size pieces which are all the same length,
except for possibly the last one which may be truncated. piece length is almost always a power of two.

pieces
++++++
**pieces** maps to a string whose length is a multiple of 20.
It is to be subdivided into strings of length 20, each of which is the SHA1 hash of the piece at the corresponding index.

files
++++++
This key is only present if no length key is present. One of the two must be present.
If **files** is present the metafile represent a set of files which go in a directory structure.
**files** maps to a list representing all files in to metafile.
Each file maps to dictionaries containing two keys :

* **length**: The length of the file, in bytes.
* **path**: A list of UTF-8 encoded strings corresponding to subdirectory names, the last of which is the actual file name (a zero length list is an error case).


.. code-block::
   :caption: **files** list structure in JSON.

     "files": [
        {
            "length": 6535405,
            "path": [
                "Darkroom (Stellar, 1994, Amiga ECS) HQ.mp4"
            ]
        },
        {
            "length": 20506624,
            "path": [
                "subdirectory",
                "Spaceballs-StateOfTheArt.avi"
            ]
        }
     ]



length
++++++
This key is only present if no files key is present. One of the two must be present.
If "length" is present then the download represents a single file.
Length maps to the length of the file in bytes.

For the purposes of the other keys, the multi-file case is treated as only having a single file by concatenating the files in the order they appear in the files list.
The files list is the value files maps to, and is a list of dictionaries containing the following keys:

Extensions
----------

Over the years various extensions were proposed to the original bittorrent metainfo file format.
The extensions are published as BEP (BitTorrent Enhancement Proposal) modeled after PEP's (Python Enhancement Proposal).

Multitracker Metadata Extension
++++++++++++++++++++++++++++++++
This extensions adds support for multiple trackers. A new key **announce-list** is added.

The tiers of announces will be processed sequentially; all URLs in each tier must be checked before the client goes on to the next tier.
URLs within each tier will be processed in a randomly chosen order;
in other words, the list will be shuffled when first read, and then parsed in order.
In addition, if a connection with a tracker is successful, it will be moved to the front of the tier.

.. code-block::
    :caption: The **announce-list** key in JSON for a single tier per tracker.

    "announce-list" : [
        [ "tracker1" ],
        [ "backup1" ],
        [ "backup2" ]
    ]

On each announce, first try tracker1, then if that cannot be reached, try backup1 and backup2 respectively.
On the next announce, repeat in the same order.
This is meant for when the trackers are standard and can not share information.


.. code-block::
    :caption: The **announce-list** key in JSON with multiple trackers in a tier.

    "announce-list": [
        [ "tracker1", "tracker2"],
        [ "backup1" ]
    ]

This form is meant for trackers which can trade peer information and will cause the clients to help balance the load between the trackers.
The first tier, consisting of tracker1 and tracker2, is shuffled.
Both trackers 1 and 2 will be tried on each announce (though perhaps in varying order) before the client tries to reach backup1.

Private torrents
+++++++++++++++++

Private torrents are indicated by the key-value pair "private: 1" in the "info" dict of the torrent's metainfo file.
This is used to disable peer sharing mechanism such as DHT and PEX.

DHT protocol
++++++++++++
DHT is a way to share peers ina swarm without a centralized tracker.
A trackerless torrent dictionary does not have an **announce** key.
Instead, a trackerless torrent has a "nodes" key.
This key should be set to the K closest nodes in the torrent generating client's routing table.
Alternatively, the key could be set to a known good node such as one operated by the person generating the torrent.

.. code-block::
    :caption: **nodes** list in JSON

    "nodes" : [
        ["127.0.0.1", 6881],
        ["your.router.node", 4804],
        ["2001:db8:100:0:d5c8:db3f:995e:c0f7", 1941]
    ]


Extended file attributes
++++++++++++++++++++++++

This extensions adds the following fields to the **files** list:

* **attr** : A variable-length string.
    When present the characters each represent a file attribute.
    l = symlink, x = executable, h = hidden, p = padding file.
    Characters appear in no particular order and unknown characters should be ignored.
* **sha1** : 20 bytes.
    The SHA1 digest calculated over the contents of the file itself, without any additional padding.
    Can be used to aid file deduplication.
    The hash should only be considered as a hint, pieces hashes are the canonical reference for integrity checking.
* **symlink path** : An array of strings.
    Path of the symlink target relative to the torrent root directory.


.. code-block::
    :caption: Extended attributes in JSON for multi-file format

    {
      "info":
      {
        "files":
        {[
          {
            "attr": "phxl",
            "sha1": <20 bytes>,
            "symlink path": ["dir1", "dir2", "target.ext"],
            ...
          },
          {
            ...
          }
        ]},
        ...
      },
      ...
    }


.. code-block::
    :caption: Extended attributes in JSON for single-file format

    {
      "info":
      {
        "attr": "hx",
        "sha1": <20 bytes>,
        ...
      },
      ...
    }



Sources:
    * `wikipedia - Bencode <https://en.wikipedia.org/wiki/Bencode>`_
    * `BitTorrent protocol specification v1 <http://www.bittorrent.org/beps/bep_0003.html>`_
    * `BEP 5 <http://www.bittorrent.org/beps/bep_0005.html>`_
    * `BEP 12 <http://www.bittorrent.org/beps/bep_0012.html>`_
    * `BEP 27 <http://www.bittorrent.org/beps/bep_0027.html>`_
