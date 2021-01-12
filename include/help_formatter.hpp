#pragma once

#include <CLI/Formatter.hpp>


class help_formatter : public CLI::Formatter {
public:
    std::string make_option_opts(const CLI::Option* opt) const override {
        std::string s;
        auto out = std::back_inserter(s);

        if (opt->get_type_size() != 0) {
            if (!opt->get_type_name().empty())
                fmt::format_to(out, " {}", get_label(opt->get_type_name()));
            if (opt->get_expected_max() == CLI::detail::expected_max_vector_size)
                fmt::format_to(out, " ...");
        }
        return s;
    }
};