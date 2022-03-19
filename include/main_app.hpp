#pragma once

#include <CLI/App.hpp>

#include "common.hpp"

#ifdef _WIN32
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <termcontrol/detail/windows.hpp>
#endif


class console_handler {
public:
    console_handler();
    ~console_handler();
private:
    std::locale original_locale_;
#ifdef _WIN32
    termcontrol::win32_terminal terminal_;
#endif
};

void list_available_checksums();

void print_version();

void configure_main_app(CLI::App* app, main_app_options& options);