//
// Created by fbdtemme on 17/07/19.
//

#include <catch2/catch.hpp>

#include "bencode/traits/all.hpp"
#include "bencode/bvalue.hpp"

using namespace bencode;



TEST_CASE("conversion from integer (bvalue)", "[bvalue][conversion][accessors]")
{
    bencode::bvalue b = 1;

    SECTION("bool") {
        bool t = true;
        bencode::bvalue b_t(t);
        auto as_t = get_as<bool>(b_t);
        CHECK(t == as_t);

        bool f = false;
        bencode::bvalue b_f(f);
        auto as_f = get_as<bool>(b_f);
        CHECK(f == as_f);
    }

    SECTION("int8_t") {
        int8_t n = 42;
        bencode::bvalue b(n);
        auto as = get_as<int8_t>(b);
        CHECK(n == as);
    }

    SECTION("int16_t") {
        int16_t n = 42;
        bencode::bvalue b(n);
        auto as = get_as<int16_t>(b);
        CHECK(n == as);
    }

    SECTION("int32_t") {
        int32_t n = 42;
        bencode::bvalue b(n);
        auto as = get_as<int32_t>(b);
        CHECK(n == as);
    }

    SECTION("int64_t") {
        int64_t n = 42;
        bencode::bvalue b(n);
        auto as = get_as<int64_t>(b);
        CHECK(n == as);
    }

    SECTION("uint8_t") {
        uint8_t n = 42;
        bencode::bvalue b(n);
        auto as = get_as<uint8_t>(b);
        CHECK(n == as);
    }

    SECTION("uint16_t") {
        uint16_t n = 42;
        bencode::bvalue b(n);
        auto as = get_as<uint16_t>(b);
        CHECK(n == as);
    }

    SECTION("uint32_t") {
        uint32_t n = 42;
        bencode::bvalue b(n);
        auto as = get_as<uint32_t>(b);
        CHECK(n == as);
    }

    SECTION("uint64_t") {
        uint64_t n = 42;
        bencode::bvalue b(n);
        auto as = get_as<uint64_t>(b);
        CHECK(n == as);
    }

    SECTION("integer literal") {
        bencode::bvalue b(42);
        auto as = get_as<int>(b);
        CHECK(42 == as);
    }

    SECTION("error - not integer type") {
        bencode::bvalue b{};
        CHECK_THROWS_WITH(
                get_as<unsigned long long>(b), Catch::Contains("integer"));
    }
}


TEST_CASE("conversion from string (bvalue)", "[bvalue][conversion][accessors]")
{
    std::string test1 = "long test string 1 to make sure there is no SSO and check if move is forwarded correctly";
    bencode::bvalue t_string = test1;

    SECTION("std::vector<char>") {
        using T = std::vector<char>;

        auto v_string = get_as<T>(t_string);
        bool cmp = std::equal(begin(v_string), end(v_string),
                              begin(test1), end(test1));
        CHECK(cmp);
    }

    SECTION("fs::path") {
        using T = std::filesystem::path;

        SECTION("success") {
            auto v_string = get_as<T>(t_string);
            CHECK(v_string.string()==t_string);
        }
        SECTION("error - not string type") {
            bvalue b = 4;
            CHECK_THROWS_WITH(get_as<T>(b), Catch::Contains("string"));
        }
    }

    SECTION("std::vector<std::byte>") {
        using T = std::vector<std::byte>;

        SECTION("success") {
            auto v_string = get_as<T>(t_string);
            SUCCEED();
        }
        SECTION("error - not string type") {
            bvalue b = 4;
            CHECK_THROWS_WITH(get_as<T>(b), Catch::Contains("string"));
        }
    }

    SECTION("error - not string type") {
        using T = std::string_view;
        bvalue b = 4;
        CHECK_THROWS_WITH(get_as<T>(b), Catch::Contains("string"));
    }
}

TEST_CASE("conversion to tuple like list types (bvalue)", "[bvalue][conversion][accessors]")
{
    std::string test1 = "long test string 1 to make sure there is no SSO and check if move is forwarded correctly";
    std::string test2 = "long test string 2 to make sure there is no SSO and check if move is forwarded corre ctly";

    auto t_list_of_string = bencode::bvalue(bencode::btype::list, {test1, test2});
    auto t_list_mixed = bencode::bvalue(bencode::btype::list, {1, test2});

    SECTION("std::tuple") {
        using T = std::pair<std::string, std::string>;

        SECTION("copy") {
            auto v_string_copy = get_as<T>(t_list_of_string);
            CHECK(std::get<0>(v_string_copy)==test1);
            CHECK(std::get<1>(v_string_copy)==test2);
            CHECK_FALSE(get_string(t_list_of_string[0]).empty());
            CHECK_FALSE(get_string(t_list_of_string[1]).empty());
        }

        SECTION("move") {
            auto v_string_move = get_as<T>(std::move(t_list_of_string));
            CHECK(std::get<0>(v_string_move)==test1);
            CHECK(std::get<1>(v_string_move)==test2);

            CHECK(get_string(t_list_of_string[0]).empty());
            CHECK(get_string(t_list_of_string[1]).empty());
        }
    }

    SECTION("std::pair") {
        using T = std::pair<std::string, std::string>;
        SECTION("copy") {
            auto v_string_copy = get_as<T>(t_list_of_string);
            CHECK(std::get<0>(v_string_copy)==test1);
            CHECK(std::get<1>(v_string_copy)==test2);
            CHECK_FALSE(get_string(t_list_of_string[0]).empty());
            CHECK_FALSE(get_string(t_list_of_string[1]).empty());
        }
        SECTION("move") {
            auto v_string_move = get_as<T>(std::move(t_list_of_string));
            CHECK(std::get<0>(v_string_move)==test1);
            CHECK(std::get<1>(v_string_move)==test2);
            CHECK(get_string(t_list_of_string[0]).empty());
            CHECK(get_string(t_list_of_string[1]).empty());
        }
    }

    SECTION("std::array") {
        using T = std::array<std::string, 2>;

        SECTION("copy") {
            using T = std::array<std::string, 2>;
            auto v_string_copy = get_as<T>(t_list_of_string);
            CHECK(v_string_copy[0]==test1);
            CHECK(v_string_copy[1]==test2);
            CHECK_FALSE(get_string(t_list_of_string[0]).empty());
            CHECK_FALSE(get_string(t_list_of_string[1]).empty());

        }
        SECTION("move") {
            auto v_string_move = get_as<T>(std::move(t_list_of_string));
            CHECK(v_string_move[0]==test1);
            CHECK(v_string_move[1]==test2);
            CHECK(get_string(t_list_of_string[0]).empty());
            CHECK(get_string(t_list_of_string[1]).empty());
        }
    }

    SECTION("error - not list type") {
        using T = std::array<int, 2>;
        bencode::bvalue b{};
        CHECK_THROWS_WITH(
                get_as<T>(b), Catch::Contains("list"));
    }

    SECTION("error - size mismatch") {
        using T = std::array<int, 2>;
        bencode::bvalue b(btype::list, 1);
        CHECK_THROWS_WITH(
                get_as<T>(b), Catch::Contains("mismatch"));
    }

    SECTION("error - list value type construction error") {
        using T = std::array<int, 2>;
        CHECK_THROWS_WITH(
                get_as<T>(t_list_of_string), Catch::Contains("construction"));
    }
}


TEST_CASE("conversions from list", "[conversion][accessors]")
{
    std::string test1 = "long test string 1 to make sure there is no SSO and check if move is forwarded correctly";
    std::string test2 = "long test string 2 to make sure there is no SSO and check if move is forwarded correctly";

    auto t_list_of_string = bencode::bvalue(bencode::btype::list, {test1, test2});
    auto t_list_mixed = bencode::bvalue(bencode::btype::list, {1, test2});

    SECTION("error - not list type") {
        using T = std::vector<std::string>;

        bencode::bvalue b{};
        CHECK_THROWS_WITH(
                get_as<T>(b), Catch::Contains("list"));
    }
    SECTION("error - value conversion error") {
        using U = std::vector<int>;
        CHECK_THROWS_WITH(
                get_as<U>(t_list_of_string), Catch::Contains("integer"));
    }
    SECTION("std::vector") {
        using T = std::vector<std::string>;

        SECTION("copy") {
            auto v_string_copy = get_as<T>(t_list_of_string);
            CHECK(v_string_copy[0]==test1);
            CHECK(v_string_copy[1]==test2);
            CHECK_FALSE(get_string(t_list_of_string[0]).empty());
            CHECK_FALSE(get_string(t_list_of_string[1]).empty());

        }
        SECTION("move") {
            auto v_string_move = get_as<T>(std::move(t_list_of_string));
            CHECK(v_string_move[0]==test1);
            CHECK(v_string_move[1]==test2);
            CHECK(get_string(t_list_of_string[0]).empty());
            CHECK(get_string(t_list_of_string[1]).empty());
        }
    }
    SECTION("std::list") {
        using T = std::list<std::string>;

        SECTION("copy") {
            auto v_string_copy = get_as<T>(t_list_of_string);
            CHECK(v_string_copy.front() == test1);
            CHECK(v_string_copy.back() == test2);
            CHECK_FALSE(get_string(t_list_of_string[0]).empty());
            CHECK_FALSE(get_string(t_list_of_string[1]).empty());

        }
        SECTION("move") {
            auto v_string_move = get_as<T>(std::move(t_list_of_string));
            CHECK(v_string_move.front() == test1);
            CHECK(v_string_move.back() == test2);
            CHECK(get_string(t_list_of_string[0]).empty());
            CHECK(get_string(t_list_of_string[1]).empty());
        }
    }
    SECTION("std::deque") {
        using T = std::deque<std::string>;

        SECTION("copy") {
            auto v_string_copy = get_as<T>(t_list_of_string);
            CHECK(v_string_copy[0]==test1);
            CHECK(v_string_copy[1]==test2);
            CHECK_FALSE(get_string(t_list_of_string[0]).empty());
            CHECK_FALSE(get_string(t_list_of_string[1]).empty());

        }
        SECTION("move") {
            auto v_string_move = get_as<T>(std::move(t_list_of_string));
            CHECK(v_string_move[0]==test1);
            CHECK(v_string_move[1]==test2);
            CHECK(get_string(t_list_of_string[0]).empty());
            CHECK(get_string(t_list_of_string[1]).empty());
        }
    }
    SECTION("std::forward_list") {
        using T = std::forward_list<std::string>;

        SECTION("copy") {
            auto v_string_copy = get_as<T>(t_list_of_string);
            CHECK(*v_string_copy.begin() == test1);
            CHECK(*std::next(v_string_copy.begin()) == test2);
            CHECK_FALSE(get_string(t_list_of_string[0]).empty());
            CHECK_FALSE(get_string(t_list_of_string[1]).empty());

        }
        SECTION("move") {
            auto v_string_move = get_as<T>(std::move(t_list_of_string));
            CHECK(*v_string_move.begin() == test1);
            CHECK(*std::next(v_string_move.begin()) == test2);
            CHECK(get_string(t_list_of_string[0]).empty());
            CHECK(get_string(t_list_of_string[1]).empty());
        }
    }
    SECTION("std::set") {
        using T = std::set<std::string>;

        SECTION("copy") {
            auto v_string_copy = get_as<T>(t_list_of_string);
            CHECK(*v_string_copy.find(test1) == test1);
            CHECK(*v_string_copy.find(test2) == test2);
            CHECK_FALSE(get_string(t_list_of_string[0]).empty());
            CHECK_FALSE(get_string(t_list_of_string[1]).empty());

        }
        SECTION("move") {
            auto v_string_move = get_as<T>(std::move(t_list_of_string));
            CHECK(*v_string_move.find(test1) == test1);
            CHECK(*v_string_move.find(test2) == test2);
            CHECK(get_string(t_list_of_string[0]).empty());
            CHECK(get_string(t_list_of_string[1]).empty());
        }
    }
    SECTION("std::unordered_set") {
        using T = std::unordered_set<std::string>;

        SECTION("copy") {
            auto v_string_copy = get_as<T>(t_list_of_string);
            CHECK(*v_string_copy.find(test1) == test1);
            CHECK(*v_string_copy.find(test2) == test2);
            CHECK_FALSE(get_string(t_list_of_string[0]).empty());
            CHECK_FALSE(get_string(t_list_of_string[1]).empty());

        }
        SECTION("move") {
            auto v_string_move = get_as<T>(std::move(t_list_of_string));
            CHECK(*v_string_move.find(test1) == test1);
            CHECK(*v_string_move.find(test2) == test2);
            CHECK(get_string(t_list_of_string[0]).empty());
            CHECK(get_string(t_list_of_string[1]).empty());
        }
    }
    SECTION("error - size_mismatch") {
        using T = std::array<std::string, 0>;
        CHECK_THROWS_WITH(get_as<T>(t_list_of_string), Catch::Contains("size mismatch"));
    }
    SECTION("error - not list type") {
        using T = std::map<std::string, int>;
        bvalue b = 4;
        CHECK_THROWS_WITH(get_as<T>(b), Catch::Contains("not"));
    }
}

TEST_CASE("conversions from dict", "[conversion][accessors]")
{
    std::string test1 = "long test string 1 to make sure there is no SSO and check if move is forwarded correctly";
    std::string test2 = "long test string 2 to make sure there is no SSO and check if move is forwarded correctly";

    auto t_dict = bencode::bvalue(btype::dict, {{"test1", test1}, {"test2", test2}});
    auto t_multidict = bencode::bvalue(btype::dict, {
        {"test1", bvalue(btype::list, {test1, test2})},
        {"test2", test2}
    });
    auto t_dict_fail = bencode::bvalue(btype::dict, {
            {"test1", test1},
            {"test2", 3}
    });
    auto t_multidict_fail = bencode::bvalue(btype::dict, {
            {"test1", bvalue(btype::list, {test1, 2})},
            {"test2", test2}
    });

    SECTION("std::map") {
        using T = std::map<std::string, std::string>;

        SECTION("copy") {
            auto v_dict_copy = get_as<T>(t_dict);
            CHECK(v_dict_copy.contains("test1"));
            CHECK(v_dict_copy.contains("test2"));
            CHECK(get_string(t_dict["test1"])==test1);
            CHECK(get_string(t_dict["test2"])==test2);
        }

        SECTION("move") {
            auto v_dict_move = get_as<T>(std::move(t_dict));
            CHECK(v_dict_move.contains("test1"));
            CHECK(v_dict_move.contains("test2"));
            CHECK(get_string(t_dict["test1"]).empty());
            CHECK(get_string(t_dict["test2"]).empty());
        }
        SECTION("error - construction error") {
            CHECK_THROWS_AS(get_as<T>(t_dict_fail), bad_conversion);
        }
    }

    SECTION("std::unordered_map") {
        using T = std::unordered_map<std::string, std::string>;

        SECTION("copy") {
            auto v_dict_copy = get_as<T>(t_dict);
            CHECK(v_dict_copy.contains("test1"));
            CHECK(v_dict_copy.contains("test2"));
            CHECK(get_string(t_dict["test1"])==test1);
            CHECK(get_string(t_dict["test2"])==test2);
        }

        SECTION("move") {
            auto v_dict_move = get_as<T>(std::move(t_dict));
            CHECK(v_dict_move.contains("test1"));
            CHECK(v_dict_move.contains("test2"));
            CHECK(get_string(t_dict["test1"]).empty());
            CHECK(get_string(t_dict["test2"]).empty());
        }
        SECTION("error - mapped type construction error") {
            CHECK_THROWS_AS(get_as<T>(t_dict_fail), bad_conversion);
        }
    }

    SECTION("std::multi_map") {
        using T = std::multimap<std::string, std::string>;

        SECTION("copy") {
            T v_dict_copy = get_as<T>(t_multidict);
            CHECK(v_dict_copy.contains("test1"));
            CHECK(v_dict_copy.contains("test2"));
            auto& [k1, v1] = *v_dict_copy.find("test1");
            CHECK((v1 == test1 || v1 == test2));
            auto& [k2, v2] = *v_dict_copy.find("test2");
            CHECK(v2 == test2);
        }

        SECTION("move") {
            auto v_dict_move = get_as<T>(std::move(t_multidict));
            CHECK(v_dict_move.contains("test1"));
            CHECK(v_dict_move.contains("test2"));
            auto& [k1, v1] = *v_dict_move.find("test1");
            CHECK((v1 == test1 || v1 == test2));
            auto& [k2, v2] = *v_dict_move.find("test2");
            CHECK(v2 == test2);

            CHECK(get_string(get_list(t_multidict["test1"])[0]).empty());
            CHECK(get_string(t_multidict["test2"]).empty());
        }

        SECTION("error - not dict type") {
            bvalue b = 1;
            CHECK_THROWS_AS(get_as<T>(b), bad_conversion);
        }

        SECTION("error - mapped type construction error") {
            CHECK_THROWS_AS(get_as<T>(t_multidict_fail), bad_conversion);
        }
    }

    SECTION("std::unordered_multimap") {
        using T = std::unordered_multimap<std::string, std::string>;

        SECTION("copy") {
            T v_dict_copy = get_as<T>(t_multidict);
            CHECK(v_dict_copy.contains("test1"));
            CHECK(v_dict_copy.contains("test2"));
            auto& [k1, v1] = *v_dict_copy.find("test1");
            CHECK((v1 == test1 || v1 == test2));
            auto& [k2, v2] = *v_dict_copy.find("test2");
            CHECK((v2 == test1 || v2 == test2));
        }

        SECTION("move") {
            auto v_dict_move = get_as<T>(std::move(t_multidict));
            CHECK(v_dict_move.contains("test1"));
            CHECK(v_dict_move.contains("test2"));
            auto& [k1, v1] = *v_dict_move.find("test1");
            CHECK((v1 == test1 || v1 == test2));
            auto& [k2, v2] = *v_dict_move.find("test2");
            CHECK(v2 == test2);

            CHECK(get_string(get_list(t_multidict["test1"])[0]).empty());
            CHECK(get_string(t_multidict["test2"]).empty());
        }
        SECTION("error - mapped type construction error") {
            CHECK_THROWS_AS(get_as<T>(t_multidict_fail), bad_conversion);
        }
    }
    SECTION("error - not dict type") {
        using T = std::map<std::string, int>;
        bvalue b = 4;
        CHECK_THROWS_WITH(get_as<T>(b), Catch::Contains("dict"));
    }
}
