#!/usr/bin/env python3
import urllib.request
from dataclasses import dataclass
from typing import List
import re

data: bytes = urllib.request.urlopen("https://gitlab.freedesktop.org/xorg/app/rgb/raw/master/rgb.txt").read()


@dataclass(frozen=True)
class X11Color:
    name: str
    r: int
    g: int
    b: int


colors: List[X11Color] = []

for line in data.splitlines():
    s = line.decode("utf-8", "ignore")
    if s.startswith('!'):
        continue

    if match := re.match(r"(\d+)\s+(\d+)\s+(\d+)[\s\t]*([a-zA-Z].*)", s):
        r, g, b, name = match.groups()
        # Skip capitalized variants
        if name[0].isupper():
            continue

        name = name.replace(' ', '_')
        colors.append(X11Color(name, int(r), int(g), int(b)))


for c in colors:
    name = f"{c.name} = 0x{c.r:02X}{c.g:02X}{c.b:02X},"
    print(f"{name:<35}           //rgb({c.r},{c.g},{c.b})")
