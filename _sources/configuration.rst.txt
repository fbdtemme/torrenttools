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

When passing the full name or abbreviation to the ``-a|--announce`` parameter,
the private flag will be set according to the value specified in the tracker database.
The user can always override this value by explicitly passing a different value to the ``-p|-private`` option.

Source field
-------------

When passing the full name or abbreviation to the ``-a|--announce`` parameter,
the source field will be set to the full name of the tracker if the tracker is marked as a private tracker.
The source tag can be overridden on the commandline or removed by passing the ``-s|--source|-s``
option with an empty string.

BitTorrent metafile default filename
-------------------------------------

Named trackers will prefix the abbreviation between square brackets to the
default filename of the generated BitTorrent metafile.
For a tracker named PrivateTracker and abbreviation PT the default filename for a target directory "content"
would be: ``[PT]example.torrent``.


Tracker groups
===============

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

Profiles
========

Profiles are a way to store a set of commandline options in the configuration file
and apply them using a single commandline argument.

Profiles are defined under the profiles key.

Following options can be used for create profiles:

.. hlist::
   :columns: 3

   * announce
   * announce-group
   * checksum
   * collection
   * comment
   * created-by
   * creation-date
   * dht-node
   * exclude
   * http-seed
   * include
   * include-hidden
   * io-block-size
   * name
   * output
   * piece-size
   * private
   * protocol
   * set-created-by
   * set-creation-date
   * similar
   * source
   * threads
   * web-sees

Following options can be used for edit profiles:

.. hlist::
    :columns: 3

    * announce
    * announce-group
    * collection
    * comment
    * created-by
    * creation-date
    * dht-node
    * http-seed
    * list-mode
    * name
    * output
    * private
    * set-created-by
    * set-creation-date
    * similar
    * source
    * web-seed


For the behaviour of each flag we refer to the documentation on the commandline arguments for the respective subcommand.


.. code-block:: yaml
    :caption: Schema for a profile.

    profiles:
        <profile_name>:
            command: [create|edit]
            options:
                <option> : <value>
                ...


.. code-block:: yaml
    :caption: Example of a profile for the create command.

    profiles:
      public:
        command: "create"
        options:
          announce-group: [ public-trackers ]
          private: false
          protocol: 1
