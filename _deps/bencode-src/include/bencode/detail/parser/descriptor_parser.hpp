#pragma once
#include <cassert>
#include <cstdint>
#include <vector>
#include <stack>
#include <algorithm>
#include <ranges>
#include <nonstd/expected.hpp>

#include "bencode/detail/bencode_type.hpp"
#include "bencode/detail/symbol.hpp"
#include "bencode/detail/bitmask_operators.hpp"
#include "bencode/detail/utils.hpp"

#include "bencode/detail/parser/from_chars.hpp"
#include "bencode/detail/parser/parser_options.hpp"
#include "bencode/detail/parser/parser_state.hpp"
#include "bencode/detail/parser/parsing_error.hpp"

#include "bencode/detail/descriptor.hpp"
#include "bencode/detail/descriptor_table.hpp"


namespace bencode {

namespace rng = std::ranges;

namespace detail {

struct descriptor_parser_stack_frame
{
    detail::parser_state state;
    // start position in the input stream of the structured data
    std::uint32_t position;
    // number of elements
    std::uint32_t size;
};

constexpr descriptor_type descriptor_type_modifier(parser_state s) noexcept
{
    switch (s) {
    case parser_state::expect_list_value:
        return descriptor_type::list_value;
    case parser_state::expect_dict_key:
        return descriptor_type::dict_key;
    case parser_state::expect_dict_value:
        return descriptor_type::dict_value;
    default:
        return {};
    }
};

}

/// Parse bencoded data into a descriptor_table.
class descriptor_parser
{
    using state = detail::parser_state;
    using options = parser_options;
    using stack_frame = detail::descriptor_parser_stack_frame;

    using iterator_t = const char*;
    using sentinel_t = const char*;

public:
    explicit descriptor_parser(const parser_options& options = {})
            :  options_(options)
    {}

    std::optional<descriptor_table> parse(std::string_view s) noexcept
    { return parse<std::string_view>(s); }

    template <typename R>
    requires rng::contiguous_range<R> && rng::sized_range<R> &&
            (std::convertible_to<rng::range_value_t<R>, char> ||
             std::same_as<rng::range_value_t<R>, std::byte>)
    std::optional<descriptor_table> parse(const R& range) noexcept
    {
        if constexpr ( std::same_as<rng::range_value_t<R>, std::byte>) {
            begin_ = reinterpret_cast<const char*>(rng::data(range));
            it_ = reinterpret_cast<const char*>(rng::data(range));
            end_ = reinterpret_cast<const char*>(std::next(rng::data(range), rng::size(range)));
        }
        else {
            begin_ = rng::data(range);
            it_ = rng::data(range);
            end_ = std::next(rng::data(range), rng::size(range));
        }
        descriptors_.clear();
        error_ = std::nullopt;

        auto success = parse_loop();

        if (!success) {
            Expects(error_);
            return std::nullopt;
        }
        return descriptor_table(std::move(descriptors_), rng::data(range));
    }

    bool has_error() noexcept
    { return error_.has_value(); }

    parsing_error error() {
        Expects(error_.has_value());
        return *error_;
    }

private:
    bool parse_loop() noexcept
    {
        // aliases for brevity
        // TODO [c++20] : change to using enum
        constexpr auto list_t = state::expect_list_value;
        constexpr auto dict_t = state::expect_dict_value;
        constexpr auto value_t = state::expect_value;

        while (it_ != end_ && !error_.has_value()) {

            // verify bvalue limits
            if (descriptors_.size() > options_.value_limit) [[unlikely]] {
                set_error(parsing_errc::value_limit_exceeded);
                return false;
            }

            // read a character
            const char c = *it_;

            // check current parsing context
            if (!stack_.empty()) {
                auto [context, position, size] = stack_.top();

                switch (context) {
                case state::expect_dict_key: {
                    if (c == symbol::digit) [[likely]] {
                        handle_dict_key();
                        continue;
                    }
                    else if (c == symbol::end) {
                        handle_dict_end();
                        continue;
                    }
                    else [[unlikely]] {
                        set_error(parsing_errc::expected_dict_key_or_end, btype::dict);
                        continue;
                    }
                }
                case state::expect_dict_value: {
                    handle_value<dict_t>();
                    continue;
                }
                case state::expect_list_value: {
                    if (c == symbol::end) [[unlikely]] {
                        handle_list_end();
                        continue;
                    }
                    else {
                        handle_value<list_t>();
                        continue;
                    }
                }
                default:
                    set_error(parsing_errc::internal_error);
                }
                break;
            }
            // No current parsing context. This means we are parsing the first element
            // from the data or successive elements from a stream.
            handle_value<value_t>();
        }

        if (error_.has_value())
            return false;

        if (!stack_.empty()) {
            if (stack_.top().state == detail::parser_state::expect_dict_key) {
                set_error(parsing_errc::expected_dict_key_or_end);
            }
            if (stack_.top().state == detail::parser_state::expect_list_value) {
                set_error(parsing_errc::expected_list_value_or_end);
            }
            return false;
        }

        // set stop flag on last token
        if (!descriptors_.empty())
            descriptors_.back().set_stop_flag();

        Ensures(stack_.empty());
        Ensures(it_ == end_);
        return true;
    }

    inline bool handle_integer(descriptor_type modifier) noexcept
    {
        Expects(*it_ == symbol::integer_begin);

        const auto type = (descriptor_type::integer | modifier);
        auto& t = descriptors_.emplace_back(type, current_position());

        std::int64_t value;
        auto result = detail::binteger_from_chars(it_, end_, value,
                detail::implementation::BENCODE_FROM_CHARS_INTEGER_IMPL);
        it_ = result.ptr;

        if (result.ec != parsing_errc{}) [[unlikely]] {
            set_error(result.ec, btype::integer);
            return false;
        }
        t.set_value(value);
        return true;
    }

    inline bool handle_string(descriptor_type modifier) noexcept
    {
        Expects(*it_ == symbol::digit);

        const auto type = (descriptor_type::string | modifier);
        const auto position = static_cast<std::size_t>(std::distance(begin_, it_));

        std::size_t offset;
        std::size_t size;
        auto result = detail::bstring_from_chars(
                it_, end_, offset, size,
                detail::implementation::BENCODE_FROM_CHARS_STRING_IMPL);

        it_ = result.ptr;

        if (result.ec != parsing_errc{}) [[unlikely]] {
            set_error(result.ec, btype::string);
            return false;
        }
        auto& t = descriptors_.emplace_back(type, position);
        t.set_offset(offset);
        t.set_size(size);
        return true;
    }

    template <state ParserState>
    inline bool handle_value() noexcept
    {
        Expects(ParserState != state::expect_dict_key);
        Expects(stack_.empty() || stack_.top().state == ParserState);

        constexpr auto type_modifier = detail::descriptor_type_modifier(ParserState);

        auto dispatch = [&](bool success = true) {
            if (!success) [[unlikely]] return false;

            if constexpr (ParserState == state::expect_dict_value) {
                auto& [st, start, size] = stack_.top();
                st = state::expect_dict_key;
                ++size;
            }
            else if constexpr (ParserState == state::expect_list_value) {
                auto& [st, start, size] = stack_.top();
                ++size;
            }
            return true;
        };

        const char c = *it_;

        switch (c) {
        case symbol::integer_begin:
            return dispatch(handle_integer(type_modifier));
        case symbol::list_begin:
            return handle_list_begin(type_modifier);
        case symbol::dict_begin:
            return handle_dict_begin(type_modifier);
        default: {
            if (c == symbol::digit) [[likely]] {
                return dispatch(handle_string(type_modifier));
            }
            if constexpr (ParserState == state::expect_list_value) {
                set_error(parsing_errc::expected_list_value_or_end, btype::list);
            }
            else if constexpr (ParserState == state::expect_dict_value) {
                set_error(parsing_errc::expected_dict_value, btype::dict);
            } else {
                set_error(parsing_errc::expected_value);
            }
            return false;
        }
        }
    }

    inline bool handle_list_begin(descriptor_type modifier) noexcept
    {
        Expects(*it_ == symbol::list_begin);

        const auto type = (descriptor_type::list | modifier);
        const auto position = current_position();

        if (stack_.size() >= options_.recursion_limit) {
            set_error(parsing_errc::recursion_depth_exceeded);
            return false;
        }

        descriptors_.emplace_back(type, position);
        stack_.push({
                .state = state::expect_list_value,
                .position = static_cast<std::uint32_t>(descriptors_.size()-1),
                .size = 0
        });

        ++it_;
        return true;
    }

    inline bool handle_dict_begin(descriptor_type modifier)
    {
        Expects(*it_ == symbol::dict_begin);

        const auto type = (descriptor_type::dict | modifier);
        const auto position = current_position();

        if (stack_.size() >= options_.recursion_limit) {
            set_error(parsing_errc::recursion_depth_exceeded);
            return false;
        }

        descriptors_.emplace_back(type, position);
        stack_.push({
                .state = state::expect_dict_key,
                .position = static_cast<std::uint32_t>(descriptors_.size()-1),
                .size = 0
        });

        ++it_;
        return true;
    }

    inline bool handle_list_end()
    {
        Expects(*it_ == symbol::end);
        Expects(stack_.top().state == state::expect_list_value);

        auto type = (descriptor_type::list | descriptor_type::end);
        const auto [state, start_pos, size] = stack_.top();
        const auto offset = descriptors_.size() - start_pos;
        const auto position = current_position();

        stack_.pop();
        ++it_;
        handle_nested_structures();
        type |= detail::descriptor_type_modifier(state);

//        if (auto s = handle_nested_structures(); s)
//            type |= detail::descriptor_type_modifier(*s);

        auto& t = descriptors_.emplace_back(type, position);
        descriptors_[start_pos].set_offset(offset);
        descriptors_[start_pos].set_size(size);
        t.set_offset(offset);
        t.set_size(size);
        return true;
    }

    inline bool handle_dict_end() noexcept
    {
        Expects(*it_ == symbol::end);
        Expects(stack_.top().state == state::expect_dict_key);

        auto type = (descriptor_type::dict | descriptor_type::end);
        const auto [state, start_pos, size] = stack_.top();
        const auto offset = descriptors_.size() - start_pos;
        const auto position = current_position();

        stack_.pop();
        ++it_;
        handle_nested_structures();
        type |= detail::descriptor_type_modifier(state);

        auto& t = descriptors_.emplace_back(type, position);
        descriptors_[start_pos].set_offset(offset);
        descriptors_[start_pos].set_size(size);
        t.set_offset(offset);
        t.set_size(size);
        return true;
    }

    inline bool handle_dict_key() noexcept
    {
        Expects(stack_.top().state == state::expect_dict_key);
        Expects(*it_ == symbol::digit);

        constexpr auto type = (
                descriptor_type::string | descriptor_type::dict_key );
        auto position = current_position();

        std::size_t offset;
        std::size_t size;
        auto result = detail::bstring_from_chars(it_, end_, offset, size,
                detail::implementation::BENCODE_FROM_CHARS_STRING_IMPL);

        if (result.ec != parsing_errc{}) [[unlikely]] {
            set_error(result.ec, btype::string);
            return false;
        }

        stack_.top().state = state::expect_dict_value;

        auto& t = descriptors_.emplace_back(type, position);
        t.set_offset(offset);
        t.set_size(size);
        it_ = result.ptr;
        return true;
    }

    inline void handle_nested_structures() noexcept
    {
        if (stack_.empty()) return;

        auto&[state, position, size] = stack_.top();
        ++size;

        if (state == state::expect_dict_value) {
            state = state::expect_dict_key;
        }
    }

    inline std::size_t current_position() noexcept
    { return (it_ - begin_); }

    inline void set_error(parsing_errc ec,
                          std::optional<bencode_type> context = std::nullopt) noexcept
    {
        error_.emplace(ec, current_position(), context);
    }

    iterator_t begin_{};
    iterator_t it_{};
    sentinel_t end_{};
    std::vector<descriptor> descriptors_ {};
    std::stack<stack_frame> stack_{};
    std::optional<parsing_error> error_ = std::nullopt;
    options options_;
};

}