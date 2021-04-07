#pragma once

#include <CLI/App.hpp>

#include "common.hpp"

void setup_console();

void list_available_checksums();

void print_version();

void configure_main_app(CLI::App* app, main_app_options& options);