#include <string_view>

#include "profile.hpp"
#include "exceptions.hpp"
#include "utils.hpp"

namespace torrenttools {

static const std::set<std::string_view> create_config_keys {
        "announce",
        "announce-group",
        "checksum",
        "collection",
        "comment",
        "created-by",
        "creation-date",
        "dht-node",
        "exclude",
        "http-seed",
        "include",
        "include-hidden",
        "io-block-size",
        "name",
        "output",
        "piece-size",
        "private",
        "protocol",
        "set-created-by",
        "set-creation-date",
        "similar",
        "source",
        "threads",
        "web-seed",
};


profile parse_create_profile(const YAML::Node& profile_data)
{
    profile result { .command = "create", .options = create_app_options{} };
    auto& options = std::get<create_app_options>(result.options);

    if (!profile_data.IsMap()) {
        throw tt::profile_error("profile options must be a map");
    }

    // Verify keys
    for (const auto& p : profile_data) {
        auto key = p.first.as<std::string>();
        if (!create_config_keys.contains(std::string_view(key))) {
            throw profile_error(
                    fmt::format("invalid option {} for command \"create\" in options section", key));
        }
    }

    // announces
    if (auto n = profile_data["announce"]; n) {
        if (!n.IsSequence()) {
            throw profile_error("value type for key announce must be a list");
        }
        options.announce_list = announce_transformer(n);
    }

    // announce group
    if (auto n = profile_data["announce-group"]; n) {
        try {
            options.announce_group_list = n.as<std::vector<std::string>>();
        } catch (const YAML::BadConversion& err) {
            throw profile_error("value type for key announce-group must be a list");
        }
    }

    // checksum
    if (auto n = profile_data["checksum"]; n) {
        try {
            options.checksums = checksum_transformer(n.as<std::vector<std::string>>());
        } catch (const YAML::BadConversion& err) {
            throw profile_error("value type for key checksum must be a list of strings");
        }
     }
    // collection
    if (auto n = profile_data["collection"]; n) {
        try {
            options.collections = n.as<std::vector<std::string>>();
        } catch (const YAML::BadConversion& err) {
            throw profile_error("value type for key checksum must be a list of strings");
        }
    }

    // comment
    if (auto n = profile_data["comment"]; n) {
        try {
            options.comment = n.as<std::string>();
        } catch (const YAML::BadConversion& err) {
            throw profile_error("value type for key checksum must be a string");
        }
    }

    // created-by
    if (auto n = profile_data["created-by"]; n) {
        try  { options.created_by = n.as<std::string>(); }
        catch (const YAML::BadConversion& err) {
            throw profile_error("value type for key created-by must be a string");
        }
    }

    // creation-date
    if (auto n = profile_data["creation-date"]; n) {
        try { options.creation_date = creation_date_transformer("creation-date", {n.as<std::string>()}); }
        catch (const YAML::BadConversion& err) {
            throw profile_error("value type for key creation-date must be a string");
        }
    }

    // dht-node
    if (auto n = profile_data["dht-node"]; n) {
        try { options.dht_nodes = dht_node_transformer(n.as<std::vector<std::string>>()); }
        catch (const YAML::BadConversion& err) {
            throw profile_error("value type for key exclude must be a list of strings");
        }
    }

    // exclude
    if (auto n = profile_data["exclude"]; n) {
        try { options.exclude_patterns =  n.as<std::vector<std::string>>(); }
        catch (const YAML::BadConversion& err) {
            throw profile_error("value type for key exclude must be a list of strings");
        }
    }

    // http-seed
    if (auto n = profile_data["http-seed"]; n) {
        try {
            options.http_seeds = seed_transformer(
                    "http-seeds", n.as<std::vector<std::string>>(), /*allow_ftp=*/false);
        } catch (const YAML::BadConversion& err) {
            throw profile_error("value type for key http-seed must be a list");
        }
    }

    // include
    if (auto n = profile_data["include"]; n) {
        try { options.include_patterns = n.as<std::vector<std::string>>(); }
        catch (const YAML::BadConversion& err) {
            throw profile_error("value type for key include must be a list of strings");
        }
    }

    // include-hidden
    if (auto n = profile_data["include-hidden"]; n) {
       try { options.include_hidden_files = n.as<bool>(); }
       catch (const YAML::BadConversion& err) {
           throw profile_error("value type for key include-hidden must be a boolean");
       }
    }

    // io-block-size
    if (auto n = profile_data["io-block-size"]; n) {
        try {
            options.io_block_size = io_block_size_transformer({n.as<std::string>()});
        } catch (const YAML::BadConversion& err) {
            throw profile_error("value type for key io-block-size must be a string or integer");
        }
    }

    // name
    if (auto n = profile_data["name"]; n) {
        try { options.name = n.as<std::string>(); }
        catch (const YAML::BadConversion& err) {
            throw profile_error("value type for key name must be a string");
        }
    }

    // output
    if (auto n = profile_data["output"]; n) {
        try {
            auto destination = path_transformer({n.as<std::string>()}, /*check_exists=*/false);
            if (destination == "-") {
                options.write_to_stdout = true;
            }
            else {
                options.destination = destination;
            }
        } catch (const YAML::BadConversion& err) {
            throw profile_error("value type for key output must be a string");
        }
    }

    // piece-size
    if (auto n = profile_data["piece-size"]; n) {
        try { options.piece_size = piece_size_transformer({n.as<std::string>()}); }
        catch (const YAML::BadConversion& err) {
            throw profile_error("value type for key piece-size must be a string or integer");
        }
    }

    // private
    if (auto n = profile_data["private"]; n) {
        try {
            options.is_private = n.as<bool>();
        } catch (const YAML::BadConversion& err) {
            throw profile_error("value type for key private must be a boolean");
        }
    }

    // protocol
    if (auto n = profile_data["protocol"]; n) {
        try {
            options.protocol_version = protocol_transformer({n.as<std::string>()});
        } catch ( const YAML::BadConversion& err) {
            throw profile_error("value type for key protocol must be a string");
        }
    }

    // set-created-by
    if (auto n = profile_data["set-created-by"]; n) {
        try {
            options.set_created_by = n.as<bool>();
        } catch (const YAML::BadConversion& err) {
            throw profile_error("value type for key set-created-by must be a boolean");
        }
    }

    // set-creation-date
    if (auto n = profile_data["set-creation-date"]; n) {
        try {
            options.set_creation_date = n.as<bool>();
        } catch (const YAML::BadConversion& err) {
            throw profile_error("value type for key set-creation-date must be a boolean");
        }
    }

    // similar
    if (auto n = profile_data["similar"]; n) {
        try {
            options.similar_torrents = similar_transformer("similar", n.as<std::vector<std::string>>());
        } catch (const YAML::BadConversion& err) {
            throw profile_error("value type for key similar must be a list of strings");
        }
    }
    // source
    if (auto n = profile_data["source"]; n) {
        try {
            options.source = n.as<std::string>();
        } catch (const YAML::BadConversion& err) {
            throw profile_error("value type for key source must be a string");
        }
    }

    // threads
    if (auto n = profile_data["threads"]; n) {
        try {
            options.threads = n.as<std::size_t>();
        } catch (const YAML::BadConversion& err) {
            throw profile_error("value type for key threads must be an integer");
        }
    }

    // web-seed
    if (auto n = profile_data["web-seed"]; n) {
        try {
            options.web_seeds = seed_transformer(
                    "web-seeds", n.as<std::vector<std::string>>(), /*allow_ftp=*/true);
        } catch (const YAML::BadConversion& err) {
            throw profile_error("value type for key web-seed must be a list of strings");
        }
    }

    return result;
}

}