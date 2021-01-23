#pragma once

#include <cstdio>
#include <cwchar>
#include <windows.h>

namespace termcontrol {

/// Enable virtual terminal processing on windows
/// @returns True on success, false otherwise
inline bool enable_virtual_terminal_processing() noexcept
{
    // Set output mode to handle virtual terminal sequences
    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    if (handle_out == INVALID_HANDLE_VALUE) {
        return false;
    }
    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    if (handle_in == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD original_out_mode = 0;
    DWORD original_in_mode = 0;
    if (! GetConsoleMode(handle_out, &original_out_mode)) {
        return false;
    }
    if (! GetConsoleMode(handle_in, &original_in_mode)) {
        return false;
    }

    DWORD requested_out_modes = ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
    DWORD requested_in_modes = ENABLE_VIRTUAL_TERMINAL_INPUT;

    DWORD out_mode = original_out_mode | requested_out_modes;
    if (! SetConsoleMode(handle_out, out_mode)) {
        // we failed to set both modes, try to step down mode gracefully.
        requested_out_modes = ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        out_mode = original_out_mode | requested_out_modes;
        if (!SetConsoleMode(handle_out, out_mode)) {
            // Failed to set any VT mode, can't do anything here.
            return false;
        }
    }

    DWORD in_mode = original_in_mode | ENABLE_VIRTUAL_TERMINAL_INPUT;
    if (!SetConsoleMode(handle_in, in_mode)) {
        // Failed to set VT input mode, can't do anything here.
        return false;
    }
    return true;
}

} // namespace termcontrol