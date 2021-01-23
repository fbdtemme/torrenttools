#pragma once

#include <mutex>
#include <ostream>

namespace cliprogress {

class terminal_writer_transaction;

/// Provide centralized terminal output for multithreaded contexts.
class terminal_writer
{
public:
    explicit terminal_writer(std::ostream& output = std::cout, std::istream& input = std::cin)
        : output_(output)
        , input_(input)
    {}

    terminal_writer(const terminal_writer& other) noexcept = default;
    terminal_writer& operator=(const terminal_writer& other) noexcept = default;

    terminal_writer& write(char c)
    {
        std::ostream& os = output_;
        frame_buffer_.push_back(c);
        return *this;
    }

    terminal_writer& write(std::string_view s)
    {
        std::ostream& os = output_;
        frame_buffer_.append(s);
        return *this;
    }

    terminal_writer& write(std::size_t n, char c)
    {
        std::ostream& os = output_;
        frame_buffer_.append(n, c);
        return *this;
    }

    // Flush a frame to the terminal
    terminal_writer& flush()
    {
        std::ostream& os = output_;
        os.write(frame_buffer_.data(), frame_buffer_.size());
        frame_buffer_.clear();
        std::flush(os);
        return *this;
    }

    template <typename T>
    friend terminal_writer& operator<<(terminal_writer& writer, T&& value);

private:
    std::reference_wrapper<std::ostream> output_ = std::cout;
    std::reference_wrapper<std::istream> input_  = std::cin;

    std::string frame_buffer_ {};
};



template <typename T>
inline terminal_writer& operator<<(terminal_writer& writer, T&& value)
{
    std::stringstream ss {};
    ss << std::forward<T>(value);
    writer.write(ss.str());
    return writer;
}



}