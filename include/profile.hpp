#pragma once

#include <string>
#include <vector>
#include <variant>
#include <map>

#include <yaml-cpp/yaml.h>

#include "create.hpp"
#include "edit.hpp"
#include "argument_parsers.hpp"

namespace torrenttools {


struct profile
{
    std::string command;
    std::variant<std::monostate, create_app_options, edit_app_options> options;
};

profile parse_create_profile(const YAML::Node& profile_data);

profile parse_edit_profile(const YAML::Node& profile_data);

}