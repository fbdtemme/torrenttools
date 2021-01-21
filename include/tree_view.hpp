#pragma once

#include <string>
#include <string_view>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <concepts>

#include <fmt/format.h>
#include <fmt/color.h>

#include <cliprogressbar/functional/progress_bar.hpp>
#include <termcontrol/detail/format.hpp>
#include <termcontrol/detail/color.hpp>

#include <dottorrent/metafile.hpp>
#include <dottorrent/storage_verifier.hpp>

#include "natural_sort.hpp"
#include "utils.hpp"


namespace fs = std::filesystem;
namespace tc = termcontrol;
namespace dt = dottorrent;
namespace rng = std::ranges;
namespace clp = cliprogress;


using namespace std::string_view_literals;


// TODO: Support user defined color in file tree via LS_COLORS on linux.

struct tree_options
{
    bool show_file_size = true;
    bool show_directory_size = true;
    bool use_color = true;
    bool use_native_style = true;
    bool list_padding_files = false;
    std::size_t max_entry_size = 100;
};


inline bool ellipsize(std::string& line, std::size_t max_size = 100)
{
    static const std::string_view ellipses = "...";
    bool needs_ellipsis = line.size() > max_size;
    if (needs_ellipsis) {
        line.resize(line.size()-ellipses.size());
        line.append(ellipses);
    }
    return needs_ellipsis;
}


struct tree_style
{
    tc::text_style file = {};
    tc::text_style directory = fg(tc::terminal_color::red) | tc::emphasis::bold;
};


struct filetree_index
{
    struct node {
        std::size_t first;                      // index of first file in dir
        std::size_t last;                       // index of one past last file in dir
        std::size_t total_file_size;            // total file_size of files between first and last
        bool padding_dir = false;
    };

    struct directory_content_entry
    {
        fs::path path;
        const dottorrent::file_entry* entry;
    };


    explicit filetree_index(const dottorrent::file_storage& storage, bool list_padding_files = false)
            : indices_()
            , directories_()
            , storage_(storage)
            , list_padding_files_(list_padding_files)
    {
        create_sorted_file_indices(storage);
        create_directory_map(storage);

        if (!list_padding_files_) {
            remove_padding_file_only_directories();
        }
    }

    auto get_directory_size(const fs::path& dir) -> std::size_t
    {
        Ensures(directories_.contains(dir));
        return directories_.at(dir).total_file_size;
    }

    std::vector<directory_content_entry> list_directory_content(const fs::path& root)
    {
        std::vector<directory_content_entry> out {};

        const auto& storage = storage_.get();
        const auto& node = directories_.at(root);

        for (auto i = node.first; i < node.last; ) {
            Ensures(node.last != std::size_t(-1));
            const auto &f = storage[indices_[i]];
            fs::path relative_path = f.path().lexically_relative(root);

            // path has subdirectories: list the directory but do not list the files inside the subdirectory
            if (relative_path.has_parent_path()) {
                auto dir_path = *relative_path.begin();

                const auto& node_ref = directories_.at(root / dir_path);

                // do not add an entry for paddinf ifle only directories
                if (!node_ref.padding_dir) {
                    out.push_back({.path = dir_path, .entry = nullptr});
                }
                i = node_ref.last;
            }
            else {
                if (!list_padding_files_ && f.is_padding_file()) {
                    ++i;
                }
                else {
                    out.push_back({.path = relative_path.filename(), .entry = &f});
                    ++ i;
                }
            }
        }
        return out;
    }

private:
    void create_sorted_file_indices(const dottorrent::file_storage& storage)
    {
        indices_.resize(storage.file_count());
        std::iota(indices_.begin(), indices_.end(), 0);

        static auto cmp =  [&](std::size_t lhs, std::size_t rhs) {
            return storage[lhs] < storage[rhs];
        };
        std::sort(indices_.begin(), indices_.end(), cmp);
    }

    void create_directory_map(const dottorrent::file_storage& storage)
    {
        fs::path current_root_dir {};

        // root node is the default constructed path.
        directories_.try_emplace(fs::path{},
                                 node{0, indices_.size(), storage.total_file_size()});

        for (std::size_t meta_index = 0; meta_index < indices_.size(); ++meta_index) {
            auto index = indices_[meta_index];
            const auto& f = storage[index];
            const auto& path = f.path();

            auto part_count = std::distance(path.begin(), path.end());
            auto it = path.begin();
            fs::path partial_path {};

            for (std::size_t i = 0; i < part_count - 1; ++i, ++it) {
                partial_path /= *it;
                auto node_it = directories_.find(partial_path);

                // directory is not already present
                if (node_it == directories_.end()) {
                    directories_.try_emplace(
                            partial_path,
                            node{meta_index, meta_index+1, f.file_size()});
                    current_root_dir = *it;
                }
                else {
                    node_it->second.total_file_size += f.file_size();
                    node_it->second.last += 1;
                }
            }
        }
    }

    void remove_padding_file_only_directories()
    {
        const dottorrent::file_storage& storage = storage_;

        for (auto& [path, node] : directories_) {
            bool contains_all_padding_files = dt::is_padding_directory(storage, path);
            if (contains_all_padding_files) {
                node.padding_dir = true;
            }
        }
    }

    std::reference_wrapper<const dottorrent::file_storage> storage_;
    std::vector<std::size_t> indices_;
    std::map<fs::path, node> directories_;
    bool list_padding_files_;
};


class tree_printer
{
    static constexpr auto node = "├── "sv;
    static constexpr auto end_node = "└── "sv;

    static constexpr auto directory_color = fg(tc::terminal_color::blue) /*| fmt::emphasis::bold*/;
    static constexpr auto file_color = tc::text_style{};

    using directory_size_map = std::unordered_map<std::string, std::size_t>;
    using entry = dottorrent::file_entry;

    struct stack_frame
    {
        fs::path path;
        std::vector<filetree_index::directory_content_entry> content;
        std::size_t index;
    };

public:
    tree_printer(const dottorrent::file_storage& s, std::string_view prefix = ""sv, tree_options options = {})
            : storage_(s)
            , prefix_(prefix)
            , index_(s, options.list_padding_files)
            , options_(options)
    { };

    const std::vector<std::pair<std::string, const dt::file_entry*>>
    entries() const noexcept
    {
        return output_;
    }

    [[nodiscard]]
    std::string result() const
    {
        std::string out;
        for (const auto& [line, entry]: output_) {
            out.append(line);
            out.push_back('\n');
        }
        return out;
    }

    void walk(const fs::path& root = "", std::size_t recursion_depth = 0)
    {
        static const std::string_view sub = "│   "sv;
        static const std::string_view sub_last = "    "sv;

        auto elements = index_.list_directory_content(root);

        std::vector<stack_frame> stack{};
        stack.push_back({.path = root, .content = elements, .index = 0});

        while (!stack.empty()) {
            auto& [root, content, index] = stack.back();

            if (index == content.size()) {
                stack.pop_back();
                if (!stack.empty()) {
                    auto&[root, content, index] = stack.back();
                    const auto& remove_prefix = (index == content.size()-1) ? sub_last : sub;
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
                const auto& add_prefix = (index == content.size()-1) ? sub_last : sub;
                prefix_ += add_prefix;
                auto new_root = root / current_element.path;
                auto new_contents = index_.list_directory_content(new_root);
                stack.push_back({.path = new_root, .content = std::move(new_contents), .index = 0});
            }
        }
    }

    bool print_entry(const fs::path& path, const dt::file_entry* entry_ptr, std::string_view node, const fs::path& root)
    {
        std::string formatted_name {};
        std::string line {};
        bool is_directory = false;

        if (entry_ptr != nullptr) {
            is_directory =  false;

            if (options_.use_color) {
                formatted_name = tc::format(file_color, "{}", path.string());
            } else {
                formatted_name = fmt::format("{}", path.string());
            }

            if (options_.show_file_size) {
                line = fmt::format(
                        "{}{}[{}] {}", prefix_, node, format_tree_size(entry_ptr->file_size()), formatted_name);
            } else {
                line = fmt::format("{}{}{}", prefix_, node, formatted_name);
            }
        }
        // directory
        else {
            auto dir_size = index_.get_directory_size(root / path);

            if (options_.use_color) {
                formatted_name = tc::format(file_color, "{}", path.string());
            } else {
                formatted_name = fmt::format("{}", path.string());
            }

            if (options_.show_directory_size) {
                line = fmt::format(
                        "{}{}[{}] {}", prefix_, node, format_tree_size(dir_size), formatted_name);
            } else {
                line = fmt::format("{}{}{}", prefix_, node, formatted_name);
            }
        }

        output_.emplace_back(std::move(line), entry_ptr);
        return is_directory;
    }

private:
    std::vector<std::pair<std::string, const dt::file_entry*>> output_;
    const dottorrent::file_storage& storage_;
    std::string prefix_;
    filetree_index index_;
    tree_options options_;
};





template <typename OutputIterator>
inline auto format_file_tree(OutputIterator out, const dottorrent::metafile& m,
                             std::string_view prefix = ""sv,
                             const tree_options& options = {})
{
    const auto& storage = m.storage();

    if (storage.file_mode() == dottorrent::file_mode::multi) {
        fmt::format_to(out, "  {}\n", m.name());
        auto printer = tree_printer(m.storage(), prefix, options);
        printer.walk();
        fmt::format_to(out, printer.result());
    }
    else  {
        fmt::format_to(out, "  {}\n", m.name());
    }
    return out;
}




template <std::output_iterator<char> OutputIterator>
inline auto format_verify_file_tree(
        OutputIterator out,
        const dottorrent::metafile& m,
        const dottorrent::storage_verifier& verifier,
        std::string_view prefix = ""sv,
        std::size_t max_line_length = 100,
        const tree_options& options = {})
{
    using entry_type = std::pair<std::string, const dt::file_entry*>;
    using table_line_type = std::tuple<std::string, std::string, std::string, std::string>;
    const std::size_t max_file_line_size = std::max(std::size_t(50), max_line_length) - 24;
    std::vector<table_line_type> table {};

    const auto& storage = m.storage();

    std::vector<entry_type> entries {};
    entries.emplace_back(fmt::format("{}{}", prefix, m.name()), nullptr);

    if (storage.file_mode() == dottorrent::file_mode::multi) {
        auto printer = tree_printer(m.storage(), prefix, options);
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
            file_size = format_size(file_ptr->file_size());
            double pct = verifier.percentage(*file_ptr);
            percentage_bar = clp::draw_progress_bar(pct,
                    { .complete_frames = std::span(clp::bar_frames::horizontal_blocks) }, {}, 10);
            percentage = format_percentage(pct);
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
}


inline auto format_file_stats(const dottorrent::metafile& m,
                              std::string_view prefix = "",
                              bool include_pad_files = false) -> std::string
{
    const auto& storage = m.storage();
    fmt::memory_buffer out {};

    constexpr auto size_template =
            "{prefix} {total_file_size} in {directory_count} directories, {file_count} files\n";

    std::string total_file_size {};
    std::size_t total_file_count = 0;

    if (include_pad_files) {
        total_file_size = format_size(m.total_file_size());
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
        total_file_size = format_size(file_size_counter);
    }

    fmt::format_to(out, size_template,
            fmt::arg("prefix", prefix),
            fmt::arg("total_file_size", total_file_size),
            fmt::arg("directory_count", directory_count(storage, "", include_pad_files)),
            fmt::arg("file_count", total_file_count));

    return fmt::to_string(out);
}


inline auto format_announce_tree(const dottorrent::announce_url_list& e, std::string_view line_format = "")
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


