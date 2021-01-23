# Changelog
All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).

## [UNRELEASED] - 2021-01-23
### Added
* Add experimental windows support and installer.
* Add "--creation-date" option to override the creation date.
* Add "--created-by" option to override the default created by string (program and version)
* Add "--stdout" options enable writing torrent file to the standard output.
### Changed
* Fix behavior of options accepting multiple values: "--announce", "--web-seeds", "--dht-nodes".
* Fix loading config files from user-local directory.
* Allow to include hidden files when they match an include regex without specifing the --include-hidden flag.

[comment]: <> (### Removed)