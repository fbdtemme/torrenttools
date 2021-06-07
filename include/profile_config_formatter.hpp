#pragma once

#include <CLI/Config.hpp>

class profile_config_formatter : public CLI::Config
{
    std::string to_config(const CLI::App *app, bool default_also, bool, std::string) const override;

    std::vector<CLI::ConfigItem> from_config(std::istream &input) const override;
};

std::vector<CLI::ConfigItem> profile_config_formatter::from_config(std::istream& input) const
{
    return std::vector<CLI::ConfigItem>();
}
