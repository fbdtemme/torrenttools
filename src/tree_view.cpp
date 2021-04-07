#include "tree_view.hpp"

tree_printer::tree_printer(const dottorrent::metafile& m, std::string_view prefix, tree_options options)
        : metafile_(m)
        , prefix_(prefix)
        , index_(m.storage(), options.list_padding_files)
        , options_(options)
        , ls_colors_()
{
}

const std::vector<std::pair<std::string, const dt::file_entry*>> tree_printer::entries() const noexcept {
    return output_;
}

std::string tree_printer::result() const {
    std::string out;
    for (const auto& [line, entry]: output_) {
        out.append(line);
        out.push_back('\n');
    }
    return out;
}

void tree_printer::walk(const fs::path& root)
{
    // Print the name of the torrent as root directory
    if (options_.use_color) {
        auto style = ls_colors_.directory_style();
        output_.emplace_back(tc::format(style, "{}", metafile_.name()), nullptr);
    } else {
        output_.emplace_back(fmt::format("{}", metafile_.name()), nullptr);
    }

    auto elements = index_.list_directory_content(root);

    std::vector<stack_frame> stack{};
    stack.push_back({.path = root, .content = elements, .index = 0});

    while (!stack.empty()) {
        auto& [root, content, index] = stack.back();

        if (index == content.size()) {
            stack.pop_back();
            if (!stack.empty()) {
                auto&[root, content, index] = stack.back();
                const auto& remove_prefix = (index == content.size()) ? sub_last : sub;
                prefix_.resize(prefix_.size()-remove_prefix.size());
            }
            continue;
        }
        auto current_element = content.at(index);

        bool is_directory_entry;

        if (index == content.size()-1) {
            is_directory_entry = print_entry(current_element.path, current_element.entry, end_node, root);
        } else {
            is_directory_entry = print_entry(current_element.path, current_element.entry, node, root);
        }
        ++index;

        if (is_directory_entry) {
            // only remove the prefix if the current stack frame is not the last
            // index is already increment so compare with size instead of size-1
            const auto& add_prefix = (index == content.size()) ? sub_last : sub;
            prefix_ += add_prefix;
            auto new_root = root / current_element.path;
            auto new_contents = index_.list_directory_content(new_root);
            stack.push_back({.path = new_root, .content = std::move(new_contents), .index = 0});
        }
    }
}

bool tree_printer::print_entry(
        const fs::path& path, const dt::file_entry* entry_ptr, std::string_view node,
        const fs::path& root) {
    std::string formatted_name {};
    std::string line {};
    bool is_directory = false;

    if (entry_ptr != nullptr) {
        is_directory =  false;

        if (options_.use_color) {
            auto style = ls_colors_.file_style(*entry_ptr);
            formatted_name = tc::format(style, "{}", path.string());
        } else {
            formatted_name = fmt::format("{}", path.string());
        }

        if (options_.show_file_size) {
            line = fmt::format(
                    "{}{}[{}] {}", prefix_, node, tt::format_tree_size(entry_ptr->file_size()), formatted_name);
        } else {
            line = fmt::format("{}{}{}", prefix_, node, formatted_name);
        }
    }
        // directory
    else {
        is_directory =  true;
        auto dir_size = index_.get_directory_size(root / path);

        if (options_.use_color) {
            auto style = ls_colors_.directory_style();
            formatted_name = tc::format(style, "{}", path.string());
        } else {
            formatted_name = fmt::format("{}", path.string());
        }

        if (options_.show_directory_size) {
            line = fmt::format(
                    "{}{}[{}] {}", prefix_, node, tt::format_tree_size(dir_size), formatted_name);
        } else {
            line = fmt::format("{}{}{}", prefix_, node, formatted_name);
        }
    }

    output_.emplace_back(std::move(line), entry_ptr);
    return is_directory;
}

std::string format_file_tree(const dottorrent::metafile& m, std::string_view prefix, const tree_options& options)
{
    std::string result;
    auto out = std::back_inserter(result);
    const auto& storage = m.storage();

    if (storage.file_mode() == dottorrent::file_mode::multi) {
        auto printer = tree_printer(m, prefix, options);
        printer.walk();
        fmt::format_to(out, printer.result());
    }
    else  {
        // Print the name of the torrent as root directory
        if (options.use_color) {
            ls_colors ls_colors{};
            auto style = ls_colors.directory_style();
            tc::format_to(out, style, "  {}\n", m.name());
        } else {
            fmt::format_to(out, "  {}\n", m.name());
        }
    }
    return result;
}



std::string format_verify_file_tree(
        const dottorrent::metafile& m,
        const dottorrent::storage_verifier& verifier,
        std::string_view prefix,
        std::size_t max_line_length,
        const tree_options& options)
{
    std::string result {};
    auto out = std::back_inserter(result);

    using entry_type = std::pair<std::string, const dt::file_entry*>;
    using table_line_type = std::tuple<std::string, std::string, std::string, std::string>;
    const std::size_t max_file_line_size = std::max(std::size_t(50), max_line_length) - 24;
    std::vector<table_line_type> table {};

    const auto& storage = m.storage();

    std::vector<entry_type> entries {};
//    entries.emplace_back(fmt::format("{}{}", prefix, m.name()), nullptr);

    if (storage.file_mode() == dottorrent::file_mode::multi) {
        auto printer = tree_printer(m, prefix, options);
        printer.walk();
        rng::copy(printer.entries(), std::back_inserter(entries));
    }

    std::size_t largest_file_line_size = std::transform_reduce(
            entries.begin(), entries.end(), 0ul,
            [](const std::size_t& lhs, const std::size_t& rhs) { return std::max(lhs, rhs); },
            [](const entry_type& e) { return e.first.size(); }
    );

    std::size_t file_line_size = std::min(largest_file_line_size, max_file_line_size);

    std::string file_size {};
    std::string percentage_bar {};
    std::string percentage {};

    for (auto [line, file_ptr] : entries) {
        ellipsize(line, file_line_size);
        line.resize(max_file_line_size, ' ');

        if (file_ptr != nullptr) {
            file_size = tt::format_size(file_ptr->file_size());
            double pct = verifier.percentage(*file_ptr);
            percentage_bar = clp::draw_progress_bar(pct,
                    { .complete_frames = std::span(clp::bar_frames::horizontal_blocks) }, {}, 10);
            percentage = tt::format_percentage(pct);
        }
        else {
            file_size.clear();
            percentage_bar.clear();
            percentage.clear();
        }

        table.emplace_back(std::move(line), std::move(file_size), std::move(percentage_bar), std::move(percentage));
    }
    for (const auto& [file_tree_line, size, bar, perc]: table) {
        fmt::format_to(out, "{}    {} {} {}\n", file_tree_line, size, bar, perc);
    }

    return result;
}


std::string format_file_stats(const dottorrent::metafile& m, std::string_view prefix, bool include_pad_files)
{
    const auto& storage = m.storage();
    fmt::memory_buffer out {};

    constexpr auto size_template =
            "{prefix} {total_file_size} in {directory_count} directories, {file_count} files\n";

    std::string total_file_size {};
    std::size_t total_file_count = 0;

    if (include_pad_files) {
        total_file_size = tt::format_size(m.total_file_size());
        total_file_count = storage.file_count();
    }
    else {
       std::size_t file_size_counter = 0;
        for (const auto& entry: storage) {
            if (!entry.is_padding_file()) {
                file_size_counter += entry.file_size();
                total_file_count += 1;
            }
        }
        total_file_size = tt::format_size(file_size_counter);
    }

    fmt::format_to(out, size_template,
            fmt::arg("prefix", prefix),
            fmt::arg("total_file_size", total_file_size),
            fmt::arg("directory_count", directory_count(storage, "", include_pad_files)),
            fmt::arg("file_count", total_file_count));

    return fmt::to_string(out);
}



std::string format_announce_tree(const dottorrent::announce_url_list& e, std::string_view line_format)
{
    if (line_format.empty())
        line_format = "{}";

    const auto node = fmt::format(line_format, "{}├── {}\n"sv);
    const auto end_node = fmt::format(line_format, "{}└── {}\n"sv);
    const auto tier_format = fmt::format(line_format, "tier {}\n"sv);

    std::string out {};
    auto inserter = std::back_inserter(out);

    for (std::size_t tier = 0; tier < e.tier_count(); tier++) {
        fmt::format_to(inserter, tier_format, tier+1);

        auto it = e.get_tier_begin(tier);
        while (it != (e.get_tier_end(tier)-1)) {
            fmt::format_to(inserter, node , "  ", it->url);
            ++it;
        }
        fmt::format_to(inserter, end_node, "  ", it->url);
    }
    return out;
}


