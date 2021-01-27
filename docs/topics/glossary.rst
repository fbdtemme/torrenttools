Glossary
========

* announce:
    The act of telling a tracker or the DHT network about the existence of oneself and how other peers can connect, by specifying port one is listening on.
* block:
    A subset of a piece. Almost always 16 kiB of payload, unless the piece size is smaller. This is the granularity file payload is requested from peers on the network.
* DHT:
    The distributed hash table is a cross-swarm, world-wide network of bittorrent peers. It's loosely connected, implementing the Kademlia protocol. Its purpose is to act as a tracker. Peers can announce their presence to nodes on the DHT and other peers can discover them to join the swarm.
* HTTP tracker:
    A tracker that uses the HTTP protocol for announces.
* info dictionary:
    The subset of a torrent file that describes piece hashes and file names. This is the only mandatory part necessary to join the swarm (network of peers) for the torrent.
* info hash:
    The hash of the info dictionary. This uniquely identifies a torrent and is used by the protocol to ensure peers talking to each other agree on which swarm they are participating in. Sometimes spelled info-hash.
* leecher:
    A peer that is still interested in downloading more pieces for the torrent. It is not a seed.
* magnet link:
    A URI containing the info hash for a torrent, allowing peers to join its swarm. May optionally contain a display name, trackers and web seeds. Typically magnet links rely on peers joining the swarm via the DHT.
* metadata:
    Synonymous to a torrent file
* peer:
    A computer running bittorrent client software that participates in the network for a particular torrent/set of files.
* piece:
    The smallest number of bytes that can be validated when downloading (no longer the case in bittorrent V2). The smallest part of the files that can be advertised to other peers. The size of a piece is determined by the info dictionary inside the torrent file.
* seed:
    A computer running bittorrent client software that has the complete files for a specific torrent, able to share any piece for that file with other peers in the network
* swarm:
    The network of peers participating in sharing and downloading of a specific torrent.
* torrent:
    May refer to a torrent file or the swarm (network of peers) created around the torrent file.
* torrent file:
    A file ending in .torrent describing the content of a set of files (but not containing the content). Importantly, it contains hashes of all files, split up into pieces. It may optionally contain references to trackers and nodes on the DHT network to aid peers in joining the network of peers sharing these files.
* tracker:
    A server peers can announce to and receive other peers back belonging to the same swarm. Trackers are used to introduce peers to each other, within a swarm. When announcing, the info hash of the torrent is included. Trackers can introduce peers to any info-hash that's specified, given other peers also use the same tracker. Some trackers restrict which info hashes they support based on a white list.
* UDP tracker:
    A tracker that uses a UDP based protocol for announces.
* web seed:
    A web server that is acting a seed, providing access to all pieces of all files over HTTP. This is an extension that client software may or may not support.

Sources:
    * `libtorrent documentation <https://www.libtorrent.org/manual-ref.html#glossary>`_
