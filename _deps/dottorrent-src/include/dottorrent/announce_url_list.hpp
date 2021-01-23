#pragma once
#include "dottorrent/announce_url.hpp"

namespace dottorrent {

/// Container for announce urls.
class announce_url_list
{
public:
    using value_type = announce_url;
    using reference = announce_url&;
    using const_reference = const announce_url&;
    using pointer = announce_url*;
    using const_pointer = const announce_url*;
    using iterator = typename std::vector<announce_url>::iterator;
    using const_iterator = typename std::vector<announce_url>::const_iterator;

    explicit announce_url_list() = default;
    announce_url_list(const announce_url_list&) = default;
    announce_url_list(announce_url_list&&) = default;
    announce_url_list& operator=(const announce_url_list&) = default;
    announce_url_list& operator=(announce_url_list&&) = default;

    //  Iterators

    auto begin() const -> const_iterator
    { return std::begin(announce_list_); }

    auto end() const -> const_iterator
    { return std::end(announce_list_); }

    auto begin() -> iterator
    { return std::begin(announce_list_); }

    auto end() -> iterator
    { return std::end(announce_list_); }

    //  Element access

    auto at(std::size_t pos) const -> const_reference
    { return announce_list_.at(pos); }

    auto at(std::size_t pos) -> reference
    { return announce_list_.at(pos); }

    auto operator[](std::size_t pos) const -> const_reference
    { return announce_list_[pos]; }

    auto operator[](std::size_t pos) -> reference
    { return announce_list_[pos]; }

    auto front() const -> const_reference
    { return announce_list_.front(); }

    auto front() -> reference
    { return announce_list_.front(); }

    auto back() const -> const_reference
    { return announce_list_.back(); }

    auto back() -> reference
    { return announce_list_.back(); }

    auto data() const -> const_pointer
    { return announce_list_.data(); }

    auto data() -> pointer
    { return announce_list_.data(); }

//   Capacity                                                                //

    auto empty() const -> bool
    { return announce_list_.empty(); }

    auto size() const -> std::size_t
    { return announce_list_.size(); }

    auto tier_count() const -> std::size_t
    { return announce_list_.empty() ? 0 : announce_list_.back().tier + 1; }

    auto tier_size(std::size_t tier) const -> std::size_t
    {
        const auto [first, last] = get_tier(tier);
        return std::distance(first, last);
    }

    //   Modifiers                                                               //

    void clear()
    { announce_list_.clear(); }

    auto insert(const announce_url& announce)
    {
        Expects(announce.tier <= tier_count());

        auto it = std::upper_bound(announce_list_.begin(), announce_list_.end(), announce);
        announce_list_.insert(it, announce);
        return it;
    }

    auto insert(announce_url&& announce)
    {
        Expects(announce.tier <= tier_count());

        auto it = std::upper_bound(announce_list_.begin(), announce_list_.end(), announce);
        announce_list_.insert(it, std::move(announce));
        return it;
    }

    auto emplace(std::string_view url, std::size_t tier)
    {
        Expects(tier <= tier_count());

        auto [begin, end] = get_tier(tier);
        auto it = std::upper_bound(begin, end, url, url_comparator);
        announce_list_.emplace(it, url, tier);
        return it;
    }

    auto erase(iterator pos)
    {
        Expects(announce_list_.begin() <= pos);
        Expects(pos < announce_list_.end());

        auto pos_tier = pos->tier;
        auto next_it = announce_list_.erase(pos);

        // update higher announces if tracker was the last of a tier and
        // erasing created a gap in the tier range.
        if (next_it->tier != pos_tier) {
            auto tier_begin = get_tier_begin(pos_tier);
            // check if we deleted the last element of the tier.
            if (tier_begin->tier != pos_tier) {
                std::for_each(next_it, announce_list_.end(),
                        [](announce_url& a) { --a.tier; });
            }
        }
        return next_it;
    }
    auto erase(const announce_url& announce)
    {
        return erase(find(announce));
    }

    //   Lookup                                                                   //

    auto find(std::string_view url) const -> const_iterator
    {
        auto comparator = [=](const announce_url& a) { return a.url == url; };
        return std::find_if(announce_list_.begin(), announce_list_.end(), comparator);
    }

    auto find(std::string_view url) -> iterator
    {
        auto comparator = [=](const announce_url& a) { return a.url == url; };
        return std::find_if(announce_list_.begin(), announce_list_.end(), comparator);
    }

    auto find(const announce_url& value) const -> const_iterator
    {
        const auto [tier_begin, tier_end] = get_tier(value.tier);
        auto it = std::find(tier_begin, tier_end, value);
        return (it == tier_end) ? announce_list_.end() : it;
    }

    auto find(const announce_url& value) -> iterator
    {
        const auto [tier_begin, tier_end] = get_tier(value.tier);
        auto it = std::find(tier_begin, tier_end, value);
        return (it == tier_end) ? announce_list_.end() : it;
    }

    auto contains(std::string_view url) const -> bool
    {
        auto begin_range = announce_list_.begin();
        for (std::size_t tier = 0; tier <= tier_count(); ++tier) {
            auto range = get_tier(tier);
            bool exists = std::binary_search(range.first, range.second, url, url_comparator);
            if (exists) return true;
        }
        return false;
    }

    auto contains(const announce_url& announce) const -> bool
    {
        return std::binary_search(announce_list_.begin(), announce_list_.end(), announce);
    }

    auto get_tier_begin(std::size_t tier) const -> const_iterator
    {
        const auto tmp = announce_url("", tier);
        return std::lower_bound(announce_list_.begin(), announce_list_.end(), tmp, tier_comparator);
    }

    auto get_tier_begin(std::size_t tier) -> iterator
    {
        const auto tmp = announce_url("", tier);
        return std::lower_bound(announce_list_.begin(), announce_list_.end(), tmp, tier_comparator);
    }

    auto get_tier_end(std::size_t tier) const -> const_iterator
    {
        const auto tmp = announce_url("", tier);
        return std::upper_bound(announce_list_.begin(), announce_list_.end(), tmp, tier_comparator);
    }

    auto get_tier_end(std::size_t tier) -> iterator
    {
        const auto tmp = announce_url("", tier);
        return std::upper_bound(announce_list_.begin(), announce_list_.end(), tmp, tier_comparator);
    }

    auto get_tier(std::size_t tier) const -> std::pair<const_iterator, const_iterator>
    {
        const auto tmp = announce_url("", tier);
        return std::equal_range(announce_list_.begin(), announce_list_.end(), tmp, tier_comparator);
    }

    auto get_tier(std::size_t tier) -> std::pair<iterator, iterator>
    {
        const auto tmp = announce_url("", tier);
        return std::equal_range(announce_list_.begin(), announce_list_.end(), tmp, tier_comparator);
    }

    auto operator==(const announce_url_list& that) const -> bool
    { return announce_list_ == that.announce_list_; }

    std::weak_ordering operator<=>(const announce_url_list& that) const
    { return std::compare_weak_order_fallback(announce_list_, that.announce_list_); }

private:
    static constexpr auto tier_comparator =
            [](const value_type& lhs, const value_type& rhs) constexpr
    { return lhs.tier < rhs.tier; };

    static constexpr auto url_comparator  =
            [](std::string_view lhs, std::string_view rhs) constexpr
    { return lhs < rhs; };

    std::vector<announce_url> announce_list_;
};

} // namespace dottorrent