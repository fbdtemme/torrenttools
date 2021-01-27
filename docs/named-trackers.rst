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




