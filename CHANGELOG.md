# Changelog
All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).

## [v0.6.0] - 2021-08-02
### Added
* Enable new metafiles for cross-seeding by default.

### Changed
* Read user-config location from XDG_CONFIG_HOME on linux.
* Fix macOS installer bundling system library leading to linking errors.
* Add support for more trackers in the default trackers.json file (thanks @Audionut)
* Fix freeze when LS_COLORS contains 8-bit or 24-bit colors.
* Increase re2 compiler memory to support long regexes.

## [v0.5.1] - 2021-07-26
### Changed
* Fix misconfiguration of global config directory on linux.

## [v0.5.0] - 2021-07-25
### Added
* Add support for windows and macOS.
* Add support for create and edit profiles.
* Add missing show subcommands and info entries for dht-nodes and web-seeds.
* Add support for BEP 38: Finding Local Data Via Torrent File Hints (#9).
* Add support for BEP-17: http-seeding (#8).
* Add support for printing per-file checksums in sha1sum format (#7)
* Add progress reporting while scanning for files. (#10)

### Changed
* Improve formatting of info command output.
* Fix invalid v1 infohash reporting for hybrid torrents.
* Fix rare race condition in progress bar causing deadlocks.
* Report progress when scanning filesystem and preparing metafile.
* Optionally accelerate sorting when linked to Intel TBB.
* Skip file tree for metafiles with more than 1000 files.
* Fix v2 and hybrid issues with empty files.

## [v0.4.1] - 2021-04-15
### Changed
* Fix undefined behaviour in verify command when output is piped.
* Fix missing progress lines for create command when output is piped.

## [v0.4.0] - 2021-04-10
### Added
* Add named tracker groups.
* Add `--config` and `--trackers-config` to pass custom configuration files locations.
* Add colored file tree using LS_COLORS environment variable.
* Add docker container.
* Add limited wolfssl support.

### Changed
* Fix progressbar for verify command.
* Fix hybrid torrent issues for both v1 and v2 verification.
* Fix infinite loop on unrecoverable IO error during hashing.
* Fix threads options ignored when verifying.
* Fix progress reporting when there are no files inside the torrent.
* Fix issue with worker threads exiting due to uninitialized stop state.

## [v0.3.2] - 2021-02-20
### Changed
* Fix announce url substitution overriding announce url

## [v0.3.1] - 2021-02-19
### Changed
* Fix regression in create command --output option.

## [v0.3.0] - 2021-02-19
### Added
* Faster hashing backend using Intel ISA-L Crypto multi-buffer hashing.
* Read target name from input stream.

### Changed
* Fix announce url not properly set for single tracker metafiles.
* Progress bar now shows total progress instead of per-file progress and ETA. 
* Reduced resource consumption by removing busy waiting.

## [v0.2.2] - 2021-01-30
### Changed
* Fix source tag not being set in create command.

## [v0.2.1] - 2021-01-28
### Added 
* magnet command to generate magnet URI's from a bittorrent metafile.
* Simpler progress reporting when output is piped (eg. ruTorrent task output).
* Add "show size" command.

### Changed
* Fix race triggered when hashing multiple small files in v2 hasher.

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