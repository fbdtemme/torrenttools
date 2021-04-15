Configuration
+++++++++++++

Named trackers
===============

"Named trackers" are trackers which are declared in the trackers.json configuration file.

Announce url substitution
--------------------------

When a tracker is defined in the trackers.json file you can specify the full name or abbreviation to
the ``-a|--announce`` option in the create or edit command.
The tracker name will be replaced by the announce url in the BitTorrent metafile.
Some private trackers have unique identifiers per user in the tracker announce url.
These are supported as well by defining announce-url parameters between curly brackets in the announce-url.
The announce-url parameter will be replaced by the value defined in the config.yml configuration file.

.. code-block:: json
    :caption: Declaration of a private tracker with parameter *pid* in trackers.json.

    {
        "name": "PrivateTracker",
        "abbreviation": "PT",
        "announce_url": "http://private-tracker.org:8600/{pid}/announce",
        "private": true
      }


.. code-block:: yaml
    :caption: Definition of the *pid* parameter in config.yml

    tracker-parameters:
      PrivateTracker:
        pid: 01qwqwdlc922_sample_pid_d93fqd6fji9


Private flag selection
----------------------

When passing the full name or abbreviation to the ``-a|--announce`` parameter
the private flag will be set according to the value specified in the tracker database.
The user can always override this value by explicitly passing a different value to the ``-p|-private`` option.

BitTorrent metafile default filename
-------------------------------------

Named trackers will prefix the abbreviation between square brackets to the
default filename of the generated BitTorrent metafile.
For a tracker named PrivateTracker and abbreviation PT the default filename for a target directory "content"
would be: ``[PT]example.torrent``.


Tracker groups
=============

A tracker group is an alias that refers to a group of trackers.
Instead of having to pass each tracker on the commandline, the name of the alias can be used to add all trackers in
the group to a BitTorrent metafile.

.. code-block:: yaml
    :caption: Definition of "public-trackers" tracker group containing three trackers.

    tracker-groups:
      public-trackers:
        - http://tracker.opentrackr.org:1337/announce
        - udp://tracker.openbittorrent.com:6969/announce
        - udp://exodus.desync.com:6969/announce

Named trackers can be used inside the definition of the tracker group.
A list with 20 public trackers named "public-trackers" is included in the default configuration file.


