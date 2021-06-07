#include <ranges>

#include <gsl-lite/gsl-lite.hpp>
#include "common.hpp"
#include "config_parser.hpp"

namespace rng = std::ranges;



/// Load configuration and tracker database.
/// Search first in custom locations given in main_options.
std::pair<const torrenttools::config*, const torrenttools::tracker_database*>
load_config_and_tracker_db(const main_app_options& main_options)
{
    const tt::config* config = nullptr;
    const tt::tracker_database* tracker_db = nullptr;

    if (!main_options.config.empty()) {
        config = torrenttools::load_config(main_options.config);
    } else {
        config = torrenttools::load_config();
    }
    if (!main_options.tracker_db.empty()) {
        tracker_db = torrenttools::load_tracker_database(main_options.tracker_db);
    } else {
        tracker_db = torrenttools::load_tracker_database();
    }

    bool use_config = true;

    if (use_config && config == nullptr) {
        fmt::print("Warning: could not find config.yml file: not all features will be available.");
        use_config = false;
    }

    if (use_config && tracker_db == nullptr) {
        fmt::print("Warning: could not find trackers.json file: not all features will be available.");
        use_config = false;
    }

    return {config, tracker_db};
}


void set_tracker_group(dottorrent::metafile& m, const std::vector<std::string>& announce_group_list,
        const torrenttools::tracker_database* tracker_db,
        const torrenttools::config* config)
{
    Expects(tracker_db);
    Expects(config);

    std::vector<bool> private_flags{};

    // Process the group names
    for (const auto& group_name : announce_group_list) {
        auto& announces = config->get_announce_group(group_name);

        /// Add each tracker in a seperate tier
        for (const auto& announce : announces) {
            // Check for tracker name or abbreviation
            if (tracker_db->contains(announce)) {
                const auto& tracker_entry = tracker_db->at(announce);
                m.add_tracker(tracker_entry.substitute_parameters(*config));
                private_flags.push_back(tracker_entry.is_private);
            }
            else {
                m.add_tracker(announce);
            }
        }
    }
    // If one of the tracker in the group is private the torrent wil have to be private!
    if (rng::any_of(private_flags, std::identity{})) {
        m.set_private(true);
    }
}

void set_trackers(dottorrent::metafile& m, const std::vector<std::vector<std::string>>& announce_list,
        const torrenttools::tracker_database* tracker_db,
        const torrenttools::config* config)
{
    // Use simple version when config and/or tracker_db are not available
    if (tracker_db == nullptr || config == nullptr) {
        return set_trackers(m, announce_list);
    }

    // a single tracker
    if (announce_list.size() == 1 && announce_list.at(0).size() == 1) {
        const auto& tracker = announce_list.at(0).at(0);

        // Check for tracker name or abbreviation
        if (tracker_db->contains(tracker)) {
            const auto& tracker_entry = tracker_db->at(tracker);
            m.add_tracker(tracker_entry.substitute_parameters(*config));
            m.set_private(tracker_entry.is_private);

            // set source tag to facilitate cross-seeding
            if (tracker_entry.is_private) {
                m.set_source(tracker_entry.name);
            }
        }
            // Check for tracker url -> do not substitute but do set private flag and source tag
        else if (auto it = tracker_db->find_by_url(tracker); it != tracker_db->end()) {
            const auto& tracker_entry = *it;
            m.add_tracker(tracker);
            m.set_private(tracker_entry.is_private);

            // set source tag to facilitate cross-seeding
            if (tracker_entry.is_private) {
                m.set_source(tracker_entry.name);
            }
        }
        else {
            m.add_tracker(tracker, 0);
        }
    }
    // Multiple trackers
    else {
        std::size_t tier_idx = 0;
        std::vector<bool> private_flags{};

        for (const auto& tier : announce_list) {
            for (const auto& tracker : tier) {

                // load data from tracker database
                if (bool in_db = tracker_db->contains(tracker); in_db) {
                    const auto& tracker_entry = tracker_db->at(tracker);
                    m.add_tracker(tracker_entry.substitute_parameters(*config), tier_idx);
                    private_flags.push_back(tracker_entry.is_private);
                }
                else {
                    m.add_tracker(tracker, tier_idx);
                }
            }
            ++tier_idx;
        }

        if (rng::any_of(private_flags, std::identity{})) {
            m.set_private(true);
        }
    }
}


void set_trackers(dottorrent::metafile& m, const std::vector<std::vector<std::string>>& announce_list)
{
    // a single tracker
    if (announce_list.size() == 1 && announce_list.at(0).size() == 1) {
        const auto& tracker = announce_list.at(0).at(0);
        m.add_tracker(tracker, 0);
    }
    else {
        std::size_t tier_idx = 0;
        std::vector<bool> private_flags{};

        for (const auto& tier : announce_list) {
            for (const auto& tracker : tier) {
                m.add_tracker(tracker, tier_idx);
            }
            ++tier_idx;
        }
    }
}

std::filesystem::path get_destination_path(dottorrent::metafile& m, std::optional<fs::path> destination_path)
{
    const auto* tracker_db = torrenttools::load_tracker_database();

    // complete destination: directory / filename
    fs::path destination {};
    // destination directory
    fs::path destination_directory {};
    // destination filename
    std::string destination_name {};

    if (destination_path.has_value()) {
        // options is a complete path + filename
        if (destination_path->has_filename() &&
                !std::filesystem::is_directory(*destination_path))
        {
            destination = *destination_path;
            return destination;
        }
            // option is only a destination directory and not a filename
        else if (destination_path->filename().empty()) {
            destination_directory = destination_path->parent_path();
        }
        else {
            destination_directory = *destination_path;
        }
    }
    else {
        destination_directory = fs::current_path();
    }

    destination_name = fmt::format("{}.torrent", m.name());

    if (m.trackers().size() == 1) {
        if (tracker_db != nullptr) {
            auto it = tracker_db->find_by_url(m.trackers().front());
            if (it != tracker_db->end()) {
                destination_name = fmt::format(
                        "[{}]{}.torrent",
                        it->abbreviation,
                        m.name());
            }
        }
    }
    return destination_directory / destination_name;
}