# Changelog
All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).

## [UNRELEASED] 
### Added 
* magnet command to generate magnet URI's from a bittorrent metafile.

## [v0.2.0] - 2021-01-25
### Added
* Add experimental windows support and installer.
* Add show command to query specific fields of a metafile.
* Add edit command to edit an existing metafile.
* Add "--creation-date" option to override the creation date.
* Add "--created-by" option to override the default created by string (program and version)
* Add "--stdout" options enable writing torrent file to the standard output.
* Add "--version" option to show program version.

### Changed
* Fix behavior of options accepting multiple values: "--announce", "--web-seeds", "--dht-nodes".
* Fix loading config files from user-local directory.
* Allow to include hidden files when they match an include regex without specifying the --include-hidden flag.
* Move query options of info to the new show command.
* Fix --name option being ignored.
* Fix multiple announce urls being invalidly serialized.
* Allow serialization of metafiles with zero-length files.
* Fix dht-node serialization and deserialization.

[comment]: <> (### Removed)