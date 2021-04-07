#include <CLI/App.hpp>
#include <dottorrent/metafile.hpp>

#include "pad.hpp"
#include "argument_parsers.hpp"
#include "common.hpp"

namespace dt = dottorrent;

void configure_pad_app(CLI::App* app, pad_app_options& options)
{
    CLI::callback_t target_parser = [&](const CLI::results_t v) -> bool {
        options.target = target_transformer(v);
        return true;
    };
    CLI::callback_t metafile_parser = [&](const CLI::results_t v) -> bool {
        options.metafile = metafile_transformer(v);
        return true;
    };

    app->add_option("target", target_parser, "Target directory.")
       ->type_name("<path>")
       ->required();

    app->add_option("metafile", metafile_parser, "Target bittorrent metafile.")
       ->type_name("<metafile>")
       ->required();
}

void run_pad_app(const main_app_options& main_options, const pad_app_options& options)
{
    auto m = dt::load_metafile(options.metafile);

    for (const auto& f : m.storage()) {
        if (!f.is_padding_file())
            continue;

        auto path = fs::weakly_canonical(options.target / f.path());

        if (!fs::exists(path.parent_path())) {
            fs::create_directories(path.parent_path());
        }

        // create sparse file with the correct size
        std::ofstream(path, std::ios::binary);
        fs::resize_file(path, f.file_size());
    }
}
