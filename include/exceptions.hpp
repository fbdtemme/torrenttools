#pragma once

namespace torrenttools
{

class config_error : public std::invalid_argument
{
public:
    using invalid_argument::invalid_argument;
};

class profile_error : public config_error
{
public:
    using config_error::config_error;
};

}