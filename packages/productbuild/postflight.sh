#!/bin/zsh

# Create symlink o torrenttools executable in PATH 

if [ -f /usr/local/bin/torrenttools ]; then
    rm /usr/local/bin/torrenttools
fi

ln -s /Library/torrenttools/bin/torrenttools /usr/local/bin/torrenttools