#pragma once
#include <filesystem>
#include <unordered_set>
#include <set>

#include <gsl-lite/gsl-lite.hpp>
#include <fmt/format.h>
#include <re2/re2.h>
#include <re2/set.h>


namespace torrenttools {

using namespace fmt::literals;

namespace { namespace fs = std::filesystem; }

/// Recurse over the files contained in a given path and filter the results.
///
/// @param file_include_list: allow only given extensions in the output;
/// @param file_exclude_list: do not allow given extensions in the output;
/// @param exclude_directories: do not recurse in directories matching pattern
/// When combining both include lists and exclude lists the include list will be applied first.
class file_matcher
{
public:
    file_matcher()
        : directory_exclude_list_()
        , file_include_list_(make_default_options(), re2::RE2::Anchor::ANCHOR_START)
        , file_exclude_list_(make_default_options(), re2::RE2::Anchor::ANCHOR_START)
        , include_hidden_files_()
    {};

    void include_hidden_files(bool flag)
    {
        include_hidden_files_ = flag;
    }

    void allow_extension(std::string_view extension)
    {
        Ensures(!is_compiled_);

        if (extension.starts_with(".")) {
            extension = extension.substr(1);
        }

        std::string error;
        std::string pattern = ".*.{}$"_format(extension);
        file_include_list_.Add(pattern, &error);
        file_include_list_empty_ = false;
        Ensures(error.empty());
    }

    void block_extension(std::string_view extension)
    {
        Ensures(!is_compiled_);

        if (extension.starts_with(".")) {
            extension = extension.substr(1);
        }

        std::string error;
        std::string pattern = ".*\\.{}$"_format(extension);
        file_exclude_list_.Add(pattern, &error);
        file_exclude_list_empty_ = false;
        Ensures(error.empty());
    }

    void include_pattern(std::string_view pattern)
    {
        Ensures(!is_compiled_);

        std::string error;
        file_include_list_.Add(pattern, &error);
        file_include_list_empty_ = false;

        if (!error.empty()) {
            throw std::invalid_argument(error);
        }
    }

    void exclude_pattern(std::string_view pattern)
    {
        Ensures(!is_compiled_);

        std::string error;
        file_exclude_list_.Add(pattern, &error);
        file_exclude_list_empty_ = false;

        if (!error.empty()) {
            throw std::invalid_argument(error);
        }
    }

    /// Do not include files contained in dir.
    /// The path is relative to the root directory
    void exclude_directory(const fs::path& dir)
    {
        Ensures(!is_compiled_);
        Ensures(dir.is_relative());

        directory_exclude_list_.insert(dir);
    }

    /// Compile given filters
    void compile()
    {
        if (is_compiled_)
            return;

        bool status = true;
        status &= file_include_list_.Compile();
        status &= file_exclude_list_.Compile();

        if (!status) {
            throw std::runtime_error("re2 compiler out of memory");
        }

        is_compiled_ = true;
    }

    auto run(const fs::path& root) -> std::vector<fs::path>
    {
        Ensures(fs::exists(root));

        std::vector<fs::path> result {};
        run(root, std::back_inserter(result));
        return result;
    }

    template <typename OutputIterator>
    std::size_t run(const fs::path& root, OutputIterator out)
    {
        Ensures(fs::exists(root));

        if (!is_compiled_)
            compile();

        std::size_t count = 0;
        for (auto it = fs::recursive_directory_iterator(root); it != fs::end(it); ++it) {
            if (it->is_directory()) {
                if (directory_exclude_list_.contains(it->path().lexically_relative(root))) {
                    it.disable_recursion_pending();
                }
            }
            else if (it->is_regular_file()) {
                auto s = it->path().string();
                if (file_include_list_empty_) {
                    if (!include_hidden_files_ && is_hidden_file(*it)) {
                        continue;
                    }
                    if (file_exclude_list_empty_ || !file_exclude_list_.Match(s, nullptr)) {
                        *out++ = it->path();
                        ++count;
                    }
                } else if (file_include_list_.Match(s, nullptr)) {
                    if (file_exclude_list_empty_ || !file_exclude_list_.Match(s, nullptr)) {
                        *out++ = it->path();
                        ++count;
                    }
                }
            }
        }
        return count;
    };

private:
    static bool is_hidden_file(const fs::directory_entry& entry)
    {
        return entry.path().filename().string().starts_with(".");
    }

    static re2::RE2::Options make_default_options()
    {
        auto options = re2::RE2::Options{};
        options.set_log_errors(false);
        return options;
    }

    re2::RE2::Set file_include_list_;
    re2::RE2::Set file_exclude_list_;
    std::set<fs::path> directory_exclude_list_;
    bool include_hidden_files_ = true;

    bool file_include_list_empty_ = true;
    bool file_exclude_list_empty_ = true;
    bool is_compiled_ = false;
};


} // namespace torrenttools