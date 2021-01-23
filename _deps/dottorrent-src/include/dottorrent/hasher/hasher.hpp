#pragma once
#include <span>
#include <cstddef>
#include <unordered_set>

namespace dottorrent {

class hasher
{
public:
    virtual void update(std::span<const std::byte> data) = 0;

    virtual void update(std::string_view data)
    {
        auto s = std::span{reinterpret_cast<const std::byte*>(data.data()), data.size()};
        update(s);
    }

    virtual void finalize_to(std::span<std::byte> out) = 0;

    static const std::unordered_set<hash_function>& supported_algorithms() noexcept
    {
        static std::unordered_set<hash_function> empty_set {};
        return empty_set;
    }

    virtual ~hasher() = default;
};

}