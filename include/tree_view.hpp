#pragma once

#include <string>
#include <string_view>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <numeric>

#include <fmt/format.h>
#include <fmt/color.h>

#include <termcontrol/detail/format.hpp>
#include <termcontrol/detail/color.hpp>

#include <dottorrent/metafile.hpp>
#include "natural_sort.hpp"
#include "utils.hpp"


namespace fs = std::filesystem;
namespace tc = termcontrol;
namespace dt = dottorrent;
namespace rng = std::ranges;


using namespace std::string_view_literals;


// TODO: Support user defined color in file tree via LS_COLORS on linux.

struct tree_options
{
    bool show_file_size = true;
    bool show_directory_size = true;
    bool use_color = true;
    bool use_native_style = true;
    bool list_padding_files = false;
};


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

    auto list_directory_content(const fs::path& root)
           -> std::vector<std::pair<fs::path, const dottorrent::file_entry*>>
    {
        using value_type = std::pair<fs::path, const dottorrent::file_entry*>;

        std::vector<value_type> out {};
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
                    out.emplace_back(dir_path, nullptr);
                }
                i = node_ref.last;
            }
            else {
                if (!list_padding_files_ && f.is_padding_file()) {
                    ++i;
                }
                else {
                    out.emplace_back(relative_path.filename(), &f);
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
            bool contains_all_padding_files = true;
            // loop over files
            auto i = node.first;
            for (; i < node.last; ++i) {
                Ensures(node.last != std::size_t(- 1));
                const auto& f = storage[indices_[i]];
                contains_all_padding_files &= f.is_padding_file();
            }
            if (contains_all_padding_files && i != node.first) {
                node.padding_dir = true;
            }
        }
    }

    std::reference_wrapper<const dottorrent::file_storage> storage_;
    std::vector<std::size_t> indices_;
    std::map<fs::path, node> directories_;
    bool list_padding_files_;
};

template <typename OutputIterator>
class tree_printer_impl
{
    static constexpr auto directory_color = fg(tc::terminal_color::blue) /*| fmt::emphasis::bold*/;
    static constexpr auto file_color = tc::text_style{};
    static constexpr auto node = "├── "sv;
    static constexpr auto end_node = "└── "sv;

    using directory_size_map = std::unordered_map<std::string, std::size_t>;
    using entry = dottorrent::file_entry;

public:
    tree_printer_impl(OutputIterator out,
                      const dottorrent::file_storage& s,
                      std::string_view prefix = ""sv,
                      tree_options options = {})
            : storage_(s)
            , out_(out)
            , prefix_(prefix)
            , index_(s, options.list_padding_files)
            , options_(options)
    { };

    void walk(const fs::path& root = "", std::size_t recursion_depth = 0)
    {
        static const std::string_view sub = "│   "sv;
        static const std::string_view sub_last = "    "sv;

        auto elements = index_.list_directory_content(root);

        if (elements.empty()) return;

        auto it = std::begin(elements);

        auto print_entry = [&](auto it, std::string_view node) {
            const auto& [path, entry_ptr] = *it;

            // file
            if (entry_ptr != nullptr) {
                std::string formatted_name;

//                if (!options_.show_padding_files) {
//                    if (entry_ptr->is_padding_file()) {
//                        return false;
//                    }
//                }
                if (options_.use_color) {
                    formatted_name = tc::format(file_color, "{}\n", path.string());
                } else {
                    formatted_name = fmt::format("{}\n", path.string());
                }
                if (options_.show_file_size) {
                    fmt::format_to(out_, "{}{}[{}] {}",
                            prefix_, node, format_tree_size(entry_ptr->file_size()), formatted_name);
                } else {
                    fmt::format_to(out_, "{}{}{}",
                            prefix_, node, format_tree_size(entry_ptr->file_size()), formatted_name);
                }
                return false;
            }
            // directory
            else {
                auto dir_size = index_.get_directory_size(root / path);

                std::string formatted_name;

                if (options_.use_color) {
                    formatted_name = tc::format(file_color, "{}\n", path.string());
                } else {
                    formatted_name = fmt::format("{}\n", path.string());
                }

                if (options_.show_directory_size) {
                    fmt::format_to(out_, "{}{}[{}] {}", prefix_, node,
                                  format_tree_size(dir_size), formatted_name);
                } else {
                    fmt::format_to(out_, "{}{}{}", prefix_, node, formatted_name);
                }
                return true;
            }
        };


        for (std::size_t i = 0; i < elements.size()-1; ++i) {
            bool is_directory_entry = print_entry(it, node);
            if (is_directory_entry) {
                prefix_ += sub;
                walk(root / it->first, recursion_depth+1);
                prefix_.resize(prefix_.size()-sub.size());
            }
            ++it;
        }

        bool is_directory_entry = print_entry(it, end_node);
        if (is_directory_entry) {
            prefix_ += sub_last;
            walk(root / it->first, recursion_depth+1);
            prefix_.resize(prefix_.size()-sub_last.size());
        }
    }

private:
    OutputIterator out_;
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
        auto printer = tree_printer_impl(out, m.storage(), prefix, options);
        printer.walk();
    }

    else  {
        fmt::format_to(out, "  {}\n", m.name());
    }
    return out;
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