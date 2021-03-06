#!/usr/bin/env bash
#
SOURCE_DIR=$(realpath ${1:-$PWD})
APPDIR=$SOURCE_DIR/packages/appimage/AppDir
BUILD_DIR="$SOURCE_DIR"/cmake-build-release

# Copy icons file
#mkdir -p "$APPDIR/usr/share/icons/scalable/apps"
mkdir -p "$APPDIR/usr/share/icons/256x256/apps"
#cp "$SOURCE_DIR/resources/icons/scalable/torrenttools.svg" "$APPDIR/usr/share/icons/scalable/apps/"
cp "$SOURCE_DIR/resources/icons/256x256/torrenttools.png" "$APPDIR/usr/share/icons/256x256/apps/"

# Copy desktop file
mkdir -p "$APPDIR/usr/share/applications/"
cp "$SOURCE_DIR/packages/appimage/torrenttools.desktop" "$APPDIR/usr/share/applications/torrenttools.desktop"
