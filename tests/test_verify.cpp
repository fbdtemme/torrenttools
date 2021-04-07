
#include <experimental/source_location>

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <CLI/CLI.hpp>

#include <dottorrent/dht_node.hpp>
#include "verify.hpp"
#include "tracker_database.hpp"
#include "test_resources.hpp"

namespace dt = dottorrent;
namespace tt = torrenttools;

#define PARSE_ARGS(cmd) \
try { app.parse(cmd); } catch (CLI::ParseError& e) { FAIL(e.what()); } \

#define PARSE_ARGS_THROWING(cmd) \
app.parse(cmd)

TEST_CASE("Test verification")
{

}