#!/usr/bin/env python3.8
import urllib.request
import json
import re
from typing import *


SPINNERS_JSON_URL = "https://raw.githubusercontent.com/sindresorhus/cli-spinners/master/spinners.json"


def to_snake_case(name):
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).lower()


def download_data():
    print("Downloading throbbers.json...")
    urllib.request.urlretrieve(SPINNERS_JSON_URL, 'data/throbbers.json')


def print_src_headers(f):
    f.write('#include "throbbers_data.hpp"\n')
    f.write("\n\n")
    open_namespace(f, "throbbers::detail")
    f.write("using namespace std::string_view_literals;\n\n")


def open_namespace(f, name: str):
    f.write(f"namespace {name} {{\n")


def close_namespace(f, name: str = None):
    if name:
        f.write(f"}} // namespace {name}\n\n\n")


def generate_switch(f, names):
    f.write(
"""constexpr auto get_preset(preset name) -> const throbber_style&
{
    switch (name) {
""")
    for n in names:
        f.write(
f"""
    case preset::{n}:
        return detail::{n};""")

    f.write("""
    default:
        throw std::invalid_argument("invalid preset name");
    }
    }
""")

def sanitize(f: str):
   return f.replace('\\', "\\\\")


if __name__ == "__main__":
    download_data()

    with open("data/throbbers.json", "r") as cpp:
        throbbers_data = json.load(cpp)

    names_list = []

    with open("include/cliprogressold/throbber_style.hpp", "w") as hpp:
        hpp.write("""
#pragma once
#include <array>
#include <string_view>
#include <cstdint>
#include "detail/span.hpp"
    
namespace cliprogress {

struct throbber_style 
{
    span<const std::string_view> frames;
    std::uint16_t interval;
};

namespace throbber_presets {

using namespace std::string_view_literals;

""")
        for name, data in throbbers_data.items():
            name = to_snake_case(name)
            names_list.append(name)
            frames = ",\n        ".join(
                '"{}"sv'.format(sanitize(i)) for i in data["frames"]
            )
            hpp.write(
                f"inline constexpr std::array {name}_frames = {{\n"
                f"        {frames}\n"
                f"}};\n\n"
            )
            hpp.write(
                f"inline constexpr throbber_style {name} = {{\n"
                f"    .frames = {name}_frames,\n"
                f"    .interval = {data['interval']},\n"
                # f"    .name = \"{name}\",\n"
                f"}};\n\n"
            )

        hpp.write(
            "} // namespace throbber_presets \n"
            "} // namespace cliprogress \n"
        )




