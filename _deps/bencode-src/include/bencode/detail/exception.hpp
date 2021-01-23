#pragma once
#include <exception>
#include <string>

namespace bencode {

/// Base class for all exceptions.
class exception : public std::exception
{
public:
    exception() noexcept = default;

    explicit exception(const std::string& msg)
        : msg_(msg)
    {}

    explicit exception(std::string&& msg)
        : msg_(std::move(msg))
    {}

    /// Returns the explanatory string.
    const char* what() const noexcept override
    {
        return msg_.c_str();
    }

private:
    std::string msg_;
};

}