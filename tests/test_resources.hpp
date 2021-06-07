#include <filesystem>
#include <exception>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>

namespace fs = std::filesystem;

inline auto fedora_torrent =     fs::path(TEST_RESOURCES_DIR) / "Fedora-Workstation-Live-x86_64-30.torrent";
inline auto bittorrent_hybrid =  fs::path(TEST_RESOURCES_DIR) / "bittorrent-v2-hybrid-test.torrent";
inline auto bittorrent_v2 =      fs::path(TEST_RESOURCES_DIR) / "bittorrent-v2-test.torrent";
inline auto ubuntu_torrent =     fs::path(TEST_RESOURCES_DIR) / "ubuntu-20.04.1-live-server-amd64.iso.torrent";
inline auto camelyon_torrent =   fs::path(TEST_RESOURCES_DIR) / "CAMELYON17.torrent";
inline auto private_torrent =    fs::path(TEST_RESOURCES_DIR) / "private.torrent";
inline auto tree_index_test =    fs::path(TEST_RESOURCES_DIR) / "tree_index_test.torrent";
inline auto web_seeds_torrent =  fs::path(TEST_RESOURCES_DIR) / "web-seed.torrent";
inline auto dht_nodes_torrent =  fs::path(TEST_RESOURCES_DIR) / "dht-node.torrent";
inline auto http_seeds_torrent = fs::path(TEST_RESOURCES_DIR) / "http-seeds.torrent";
inline auto similar_v1_torrent = fs::path(TEST_RESOURCES_DIR) / "similar-v1.torrent";
inline auto similar_v2_torrent = fs::path(TEST_RESOURCES_DIR) / "similar-v2.torrent";
inline auto collection_torrent = fs::path(TEST_RESOURCES_DIR) / "collection.torrent";

struct cout_redirect {
    cout_redirect( std::streambuf * new_buffer )
            : old( std::cout.rdbuf( new_buffer ) )
    { }

    ~cout_redirect( ) {
        std::cout.rdbuf( old );
    }

private:
    std::streambuf * old;
};


struct temporary_directory {
    temporary_directory()
        : path_(create_temporary_directory())
    {}

    const fs::path& path() const {
        return path_;
    }
    explicit operator fs::path() const
    {
        return path_;
    }

    ~temporary_directory( ) {
        fs::remove_all(path_);
    }

private:
    // source: https://stackoverflow.com/a/58454949
    static fs::path create_temporary_directory(unsigned long long max_tries = 1000) {
        auto tmp_dir = std::filesystem::temp_directory_path();
        unsigned long long i = 0;
        std::random_device dev;
        std::mt19937 prng(dev());
        std::uniform_int_distribution<uint64_t> rand(0);
        std::filesystem::path path;

        while (true) {
            std::stringstream ss;
            ss << std::hex << rand(prng);
            path = tmp_dir / ss.str();
            // true if the directory was created.
            if (std::filesystem::create_directory(path)) {
                break;
            }
            if (i == max_tries) {
                throw std::runtime_error("could not find non-existing directory");
            }
            i++;
        }
        return path;
    }

    std::filesystem::path path_;
};