//
// Created by fbdtemme on 07/04/19.
//
#include <catch2/catch.hpp>

#include <sstream>
#include <numeric>
#include <filesystem>

#include "bencode/traits/all.hpp"
#include "bencode/bvalue.hpp"

using namespace bencode;
using namespace std::string_literals;
using namespace std::string_view_literals;

TEST_CASE("test default construction", "[construction]")
{
    SECTION("no argument default construction") {
        auto b = bencode::bvalue();
        CHECK(b.type() == bencode::bencode_type::uninitialized);
    }

    SECTION("uniform initialization") {
        auto b = bencode::bvalue{};
        CHECK(b.type() == bencode::bencode_type::uninitialized);
    }
}


TEST_CASE("test construction with a type tag", "[construction]")
{
    SECTION("uninitialized") {
        constexpr auto t = bencode::btype::uninitialized;
        bencode::bvalue b(t);
        CHECK(b.type() == t);
    }

    SECTION("dict") {
        constexpr auto t = bencode::btype::dict;
        bencode::bvalue b(t);
        CHECK(b.type() == t);
    }

    SECTION("list") {
        constexpr auto t = bencode::btype::list;
        bencode::bvalue b(t);
        CHECK(b.type() == t);
    }

    SECTION("string") {
        constexpr auto t = bencode::btype::string;
        bencode::bvalue b(t);
        CHECK(b.type() == t);
        CHECK(b == "");
    }

    SECTION("integer") {
        constexpr auto t = bencode::btype::integer;
        bencode::bvalue b(t);
        CHECK(b.type() == t);
        CHECK(b == 0);
    }
}


TEST_CASE("test integer creation", "[construction]")
{
    SECTION("create an integer number (implicit)") {
        // reference dicts
        bencode::bvalue::integer_type n_reference = 42;
        bencode::bvalue b_reference(n_reference);

        SECTION("bool") {
            bool t = true;
            bencode::bvalue b_t(t);
            CHECK(b_t.type() == bencode::bencode_type::integer);
            CHECK(b_t == 1);

            bool f = false;
            bencode::bvalue b_f(f);
            CHECK(b_f.type() == bencode::bencode_type::integer);
            CHECK(b_f == 0);
        }

        SECTION("short") {
            short n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("unsigned short") {
            unsigned short n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("int") {
            int n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("unsigned int") {
            unsigned int n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("long") {
            long n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("unsigned long") {
            unsigned long n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("long long") {
            long long n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("unsigned long long") {
            unsigned long long n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("int8_t") {
            int8_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("int16_t") {
            int16_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("int32_t") {
            int32_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("int64_t") {
            int64_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("int_fast8_t") {
            int_fast8_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("int_fast16_t") {
            int_fast16_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("int_fast32_t") {
            int_fast32_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("int_fast64_t") {
            int_fast64_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("int_least8_t") {
            int_least8_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("int_least16_t") {
            int_least16_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("int_least32_t") {
            int_least32_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("int_least64_t") {
            int_least64_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("uint8_t") {
            uint8_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("uint16_t") {
            uint16_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("uint32_t") {
            uint32_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("uint64_t") {
            uint64_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("uint_fast8_t") {
            uint_fast8_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("uint_fast16_t") {
            uint_fast16_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("uint_fast32_t") {
            uint_fast32_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("uint_fast64_t") {
            uint_fast64_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("uint_least8_t") {
            uint_least8_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("uint_least16_t") {
            uint_least16_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("uint_least32_t") {
            uint_least32_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("uint_least64_t") {
            uint_least64_t n = 42;
            bencode::bvalue b(n);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("integer literal without suffix") {
            bencode::bvalue b(42);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("integer literal with u suffix") {
            bencode::bvalue b(42u);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("integer literal with l suffix") {
            bencode::bvalue b(42l);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("integer literal with ul suffix") {
            bencode::bvalue b(42ul);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("integer literal with ll suffix") {
            bencode::bvalue b(42ll);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }

        SECTION("integer literal with ull suffix") {
            bencode::bvalue b(42ull);
            CHECK(b.type() == bencode::bencode_type::integer);
            CHECK(b == b_reference);
        }
    }

    SECTION("create an integer number from unsigned - overflow") {
#ifndef _WIN32
        SECTION("unsigned long ") {
            unsigned long n = std::numeric_limits<unsigned long>::max();
            CHECK_THROWS_AS(bencode::bvalue(n), std::overflow_error);
        }
#endif
        SECTION("unsigned long long") {
            unsigned long long n = std::numeric_limits<unsigned long long>::max();
            CHECK_THROWS_AS(bencode::bvalue(n), std::overflow_error);
        }

        SECTION("uint64_t") {
            uint64_t n = std::numeric_limits<uint64_t>::max();
            CHECK_THROWS_AS(bencode::bvalue(n), std::overflow_error);
        }

        SECTION("uint_fast64_t") {
            uint_fast64_t n = std::numeric_limits<uint_fast64_t>::max();
            CHECK_THROWS_AS(bencode::bvalue(n), std::overflow_error);
        }

        SECTION("uint_least64_t") {
            uint_least64_t n = std::numeric_limits<uint_least64_t>::max();
            CHECK_THROWS_AS(bencode::bvalue(n), std::overflow_error);
        }
    }
}

TEST_CASE("test string creation", "[construction]")
{
    // reference string
    bencode::bvalue::string_type s_reference{"Hello world"};
    bencode::bvalue b_reference(s_reference);

    SECTION("std::string") {
        std::string s{"Hello world"};
        bencode::bvalue b(s);
        CHECK(b.type() == bencode::bencode_type::string);
        CHECK(b == b_reference);
    }

    SECTION("char[]") {
        char s[]{"Hello world"};
        bencode::bvalue b(s);
        CHECK(b.type() == bencode::bencode_type::string);
        CHECK(b == b_reference);
    }

    SECTION("const char*") {
        const char* s{"Hello world"};
        bencode::bvalue b(s);
        CHECK(b.type() == bencode::bencode_type::string);
        CHECK(b == b_reference);
    }

    SECTION("std::vector<std::byte>") {
        std::vector<std::byte> s{};
        for (char c: s_reference) { s.push_back(static_cast<std::byte>(c)); }
        bencode::bvalue b(s);
        CHECK(b.type() == bencode::bencode_type::string);
        CHECK(b == b_reference);
    }

    SECTION("string literal") {
        bencode::bvalue b("Hello world"s);
        CHECK(b.type() == bencode::bencode_type::string);
        CHECK(b == b_reference);
    }

    SECTION("stringstream") {
        std::stringstream ss {};
        ss << "Hello world";
        bencode::bvalue b(ss);

        CHECK(b.type() == bencode::bencode_type::string);
        CHECK(b == "Hello world");
    }

    SECTION("std::filesystem::path") {
        auto p = std::filesystem::current_path();
        bencode::bvalue b(p);

        CHECK(b.type() == bencode::bencode_type::string);
        CHECK(b == p);
    }

    SECTION("std::array<char, N>") {
        auto p = std::array{'a', 'b', 'c'};
        bencode::bvalue b(p);

        CHECK(b.type() == bencode::bencode_type::string);
        CHECK(b == "abc");
    }
}

TEST_CASE("test list creation", "[construction]")
{
    SECTION("create a list (explicit)") {
        SECTION("copy") {
            bencode::bvalue::list_type a{1, 1u, false, "string", bencode::bvalue()};
            bencode::bvalue b(a);
            CHECK(b.type() == bencode::bencode_type::list);
        }

        SECTION("move") {
            bencode::bvalue::list_type a{1, 1u, false, "string", bencode::bvalue()};
            bencode::bvalue b(std::move(a));
            CHECK(b.type() == bencode::bencode_type::list);
            CHECK(a.size() == 0);
        }
    }

    SECTION("create a list (implicit)") {
        // reference array
        bencode::bvalue::list_type a_reference{1, 1u, false, "string", bencode::bvalue()};
        bencode::bvalue b_reference(a_reference);

        SECTION("std::list<bencode::bvalue>") {
            std::list<bencode::bvalue> a{1, 1u, false, "string", bencode::bvalue()};
            bencode::bvalue b(a);
            CHECK(b.type() == bencode::bencode_type::list);
            CHECK(b == b_reference);
            const auto& b_list = get_list(b);

            CHECK(b_list[0] == 1);
            CHECK(b_list[1] == 1u);
            CHECK(b_list[2] == 0);
            CHECK(b_list[3] == "string");
            CHECK(b_list[4] == bencode::bvalue());
        }

        SECTION("std::pair") {
            std::pair<int, std::string> p{2, "string"};
            bencode::bvalue b(p);
            const auto& b_list = get_list(b);

            CHECK(b.type() == bencode::bencode_type::list);
            CHECK(b_list[0] == std::get<0>(p));
            CHECK(b_list[1] == std::get<1>(p));
        }

        SECTION("std::tuple") {
            const auto t = std::make_tuple(1, "string", 42, std::vector<int>{0, 1});
            bencode::bvalue b(t);
            const auto& b_list = get_list(b);

            CHECK(b.type() == bencode::bencode_type::list);
            CHECK(b_list[0] == std::get<0>(t));
            CHECK(b_list[1] == std::get<1>(t));
            CHECK(b_list[2] == std::get<2>(t));
            CHECK(b_list[3].at(0) == 0);
            CHECK(b_list[3].at(1) == 1);
        }

        SECTION("std::forward_list") {
            std::forward_list<bencode::bvalue> a{
                    bencode::bvalue(1), bencode::bvalue(1u), bencode::bvalue(false),
                    bencode::bvalue("string"), bencode::bvalue()
            };
            bencode::bvalue b(a);
            CHECK(b.type() == bencode::bencode_type::list);
            CHECK(b == b_reference);
        }

        SECTION("std::array<bencode::bvalue, 5>") {
            std::array<bencode::bvalue, 5> a{
                    1, 1u, false, "string", bencode::bvalue()
            };
            bencode::bvalue b(a);
            CHECK(b.type() == bencode::bencode_type::list);
            CHECK(b == b_reference);
        }

        SECTION("std::valarray<int>") {
            std::valarray<int> va = {1, 2, 3, 4, 5};
            bencode::bvalue b(va);
            CHECK(b.type() == bencode::bencode_type::list);
            CHECK(b == bencode::bvalue(btype::list, {1, 2, 3, 4, 5}));
        }

        SECTION("std::vector<bencode::bvalue>") {
            std::vector<bencode::bvalue> a{1, 1u, false, "string", bencode::bvalue()};
            bencode::bvalue b(a);
            CHECK(b.type() == bencode::bencode_type::list);
            CHECK(b == b_reference);
        }

        SECTION("std::deque<bencode::bvalue>") {
            std::deque<bencode::bvalue> a{1, 1u, false, "string", bencode::bvalue()};
            bencode::bvalue b(a);
            CHECK(b.type() == bencode::bencode_type::list);
            CHECK(b == b_reference);
        }

        SECTION("std::set<bencode::bvalue>") {
            std::set<bencode::bvalue> a{1, 1u, false, "string", bencode::bvalue()};
            bencode::bvalue b(a);
            CHECK(b.type() == bencode::bencode_type::list);
            // we cannot really check for equality here
        }

        SECTION("std::unordered_set<std::string>") {
            std::unordered_set<std::string> a{"string1", "string2", "string3"};
            bencode::bvalue b(a);
            CHECK(b.type() == bencode::bencode_type::list);
            // we cannot really check for equality here
        }
    }
}

TEST_CASE("test dict creation", "[construction]")
{
    SECTION("create a dict (explicit)") {
        SECTION("copy") {
            auto d = bencode::bvalue(btype::dict, {
                    {"a", 1},
                    {"b", 1u},
                    {"d", false},
                    {"e", "string"},
                    {"f", bencode::bvalue()}
            });
            bencode::bvalue b(d);
            CHECK(b.type() == bencode::bencode_type::dict);
        }

        SECTION("move") {
            auto d = bencode::bvalue(btype::dict, {
                    {"a", 1},
                    {"b", 1u},
                    {"d", false},
                    {"e", "string"},
                    {"f", bencode::bvalue()}
            });
            bencode::bvalue b(std::move(d));
            CHECK(b.type() == bencode::bencode_type::dict);
            CHECK(get_dict(d).empty());
        }
    }

    SECTION("create an dict (implicit)") {
        // reference dict
        bencode::bvalue::dict_type d_reference{
                {"a", 1},
                {"b", 1u},
                {"d", false},
                {"e", "string"},
                {"f", bencode::bvalue()}
        };

        bencode::bvalue::dict_type d_multi_reference{
                {"a", bencode::bvalue(bencode::btype::list, 1, 2)},
                {"b", 1u},
                {"d", false},
                {"e", "string"},
                {"f", bencode::bvalue()}
        };
        bencode::bvalue b_reference(d_reference);
        bencode::bvalue b_multi_reference(d_multi_reference);


        SECTION("std::map<bencode::bvalue::string_type, bencode::bvalue>") {
            std::map<bencode::bvalue::string_type, bencode::bvalue> d{
                    {"a", 1},
                    {"b", 1u},
                    {"d", false},
                    {"e", "string"},
                    {"f", bencode::bvalue()}
            };
            bencode::bvalue b(d);
            CHECK(b.type() == bencode::bencode_type::dict);
        }

        SECTION("std::map<std::string, std::string>") {
            std::map<std::string, std::string> m;
            m["a"] = "b";
            m["c"] = "d";
            m["e"] = "f";

            bencode::bvalue b(m);
            CHECK(b.type() == bencode::bencode_type::dict);
        }

        SECTION("std::map<const char*, bencode::bvalue>") {
            std::map<const char*, bencode::bvalue> d{
                    {"a", 1},
                    {"b", 1u},
                    {"d", false},
                    {"e", "string"},
                    {"f", bencode::bvalue()}
            };
            bencode::bvalue b(d);
            CHECK(b.type() == bencode::bencode_type::dict);
        }

        SECTION("std::multimap<bencode::bvalue::string_type, bencode::bvalue>") {
            std::multimap<bencode::bvalue::string_type, bencode::bvalue> d{
                    {"a", 1},
                    {"a", 2},
                    {"b", 1u},
                    {"d", false},
                    {"e", "string"},
                    {"f", bencode::bvalue()}
            };
            bencode::bvalue b(d);
            CHECK(b.type() == bencode::bencode_type::dict);
            // Every bvalue has been transformed in list of values.
            CHECK_FALSE(b == b_multi_reference);
        }

        SECTION("std::unordered_map<bencode::bvalue::string_type, bencode::bvalue>") {
            std::unordered_map<bencode::bvalue::string_type, bencode::bvalue> d{
                    {"a", 1},
                    {"b", 1u},
                    {"d", false},
                    {"e", "string"},
                    {"f", bencode::bvalue()}
            };
            bencode::bvalue b(d);
            CHECK(b.type() == bencode::bencode_type::dict);
            CHECK(b == b_reference);
        }

        SECTION("std::unordered_multimap<bencode::bvalue::policy_string_t, bencode::bvalue>") {
            std::unordered_multimap<bencode::bvalue::string_type, bencode::bvalue> d{
                    {"a", 1},
                    {"a", 2},
                    {"b", 1u},
                    {"d", false},
                    {"e", "string"},
                    {"f", bencode::bvalue()}
            };
            bencode::bvalue b(d);
            CHECK(b.type() == bencode::bencode_type::dict);
            CHECK_FALSE(b == b_multi_reference);
        }

        SECTION("initializer list") {
            bencode::bvalue b{
                    {"a", 1},
                    {"b", 1u},
                    {"d", false},
                    {"e", "string"},
                    {"f", bencode::bvalue()}
            };
            CHECK(b.type() == bencode::bencode_type::dict);
            CHECK(b == b_reference);
        }
    }
}


TEST_CASE("test initializer list constructor", "[constructor]")
{
    SECTION("empty") {
        bencode::bvalue b({});
        CHECK(b.type() == bencode::bencode_type::dict);
    }

    SECTION("not empty") {
        bencode::bvalue b{
                {"one",  1},
                {"two",  1u},
                {"four", false}
        };
        CHECK(b.type() == bencode::bencode_type::dict);
    }
}
