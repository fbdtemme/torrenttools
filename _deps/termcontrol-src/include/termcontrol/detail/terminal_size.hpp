#pragma once

#include <iostream>
#include <csignal>
#include <atomic>

#ifdef __linux__
    #include <sys/ioctl.h>
    #include <unistd.h>
#elif _WIN32
    #include <windows.h>
#endif

namespace termcontrol {

struct terminal_size
{
    unsigned short int rows;
    unsigned short int cols;
};

static_assert(std::atomic<terminal_size>::is_always_lock_free,
        "terminal_size struct is not lock-free on all platforms!");

inline auto get_terminal_size() -> terminal_size
{
#if defined(__linux__) || defined(__APPLE__)
    static struct winsize w;

    int fileno = -1;
    const auto filenos = std::array{STDOUT_FILENO, STDIN_FILENO, STDERR_FILENO};
    // Check which if the file descriptors is a tty in case of piped input and output
    for (int i = 0; i < 3; ++i) {
        fileno = filenos[i];
        if (isatty(fileno)) {
            break;
        }
    }

    ioctl(fileno, TIOCGWINSZ, &w);
    return terminal_size {
        .rows = w.ws_row,
        .cols = w.ws_col
    };

#elif _WIN32
    static CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return terminal_size {
        .rows = static_cast<unsigned short>(csbi.srWindow.Bottom - csbi.srWindow.Top + 1),
        .cols = static_cast<unsigned short>(csbi.srWindow.Right - csbi.srWindow.Left + 1),
    };
#endif
}

} // namespace cliprogress::detail