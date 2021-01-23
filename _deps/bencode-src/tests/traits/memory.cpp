#include <catch2/catch.hpp>

#include <bencode/bencode.hpp>
#include <bencode/traits/all.hpp>
#include <iostream>

#include "../bview/data.hpp"


struct custom_int_deleter {
    void operator()(int* v) {
        std::default_delete<int>{}(v);
    }
};

TEST_CASE("memory: test serializable_to", "[traits-memory]")
{
    using namespace bencode;

    SECTION("unique_ptr") {
        using T1 = std::unique_ptr<int>;
        using T2 = std::unique_ptr<int, custom_int_deleter>;

        CHECK(serializable_to<T1, bencode_type::integer>);
        CHECK(serializable_to<T2, bencode_type::integer>);
        CHECK(serialization_traits<T1>::is_pointer == true);
        CHECK(serialization_traits<T2>::is_pointer == true);
    }
    SECTION("shared_ptr") {
        using T1 = std::shared_ptr<int>;

        CHECK(serializable_to<T1, bencode_type::integer>);
        CHECK(serialization_traits<T1>::is_pointer == true);
    }
    SECTION("weak_ptr") {
        using T1 = std::weak_ptr<int>;

        CHECK(serializable_to<T1, bencode_type::integer>);
        CHECK(serialization_traits<T1>::is_pointer == true);
    }

    SECTION("raw pointer") {
        using T1 = std::add_pointer_t<int>;

        CHECK(serializable_to<T1, bencode_type::integer>);
        CHECK(serialization_traits<T1>::is_pointer == true);
    }


}

TEST_CASE("memory: test connect",  "[traits-memory]")
{
    using namespace bencode;
    auto consumer = bencode::events::to_bvalue{};

    SECTION("unique_ptr") {
        auto v = std::make_unique<int>(10);
        bencode::connect(consumer, v);
        CHECK(consumer.value() == 10);
    }
    SECTION("shared_ptr") {
        auto v = std::make_shared<int>(10);
        bencode::connect(consumer, v);
    }
    SECTION("weak_ptr") {
        auto v = std::make_shared<int>(10);
        bencode::connect(consumer, std::weak_ptr(v));
    }
    SECTION("raw pointer") {
        auto ptr = std::make_unique<int>(10);
        bencode::connect(consumer, ptr.get());
    }
}


TEST_CASE("memory: test bvalue construction", "[construction]")
{
    SECTION("unique_ptr") {
        auto ptr = std::make_unique<int>(10);
        bencode::bvalue b(ptr);
        CHECK(b.type() == bencode::bencode_type::integer);
    }
    SECTION("shared_ptr") {
        auto ptr = std::make_shared<int>(10);
        bencode::bvalue b(ptr);
        CHECK(b.type() == bencode::bencode_type::integer);
    }
    SECTION("weak_ptr") {
        auto ptr = std::make_shared<int>(10);
        auto b = bencode::bvalue(std::weak_ptr(ptr));
        CHECK(b.type() == bencode::bencode_type::integer);
    }
    SECTION("raw pointer")
    {
        auto v = std::make_unique<int>(10);
        auto b = bencode::bvalue(v.get());
        CHECK(b.type() == bencode::bencode_type::integer);
    }
}

TEST_CASE("memory: test bvalue assignment",  "[traits-memory]")
{
    using namespace bencode;
    bencode::bvalue bv {};

    SECTION("unique_ptr") {
        auto v = std::make_unique<int>(10);
        bv = v;
        CHECK(bv.type() == bencode_type::integer);
    }
    SECTION("shared_ptr") {
        auto v = std::make_shared<int>(10);
        bv = v;
        CHECK(bv.type() == bencode_type::integer);
    }
    SECTION("weak_ptr") {
        auto v_shared = std::make_shared<int>(10);
        auto v_weak = std::weak_ptr(v_shared);
        bv = v_weak;
        CHECK(bv.type() == bencode_type::integer);
    }
    SECTION("raw pointer") {
        auto v = std::make_unique<int>(10);
        bv = v.get();
        CHECK(bv.type() == bencode_type::integer);
    }
}


TEST_CASE("memory: test compare with bvalue",  "[traits-memory]")
{
    using namespace bencode;
    bencode::bvalue bv {};

    SECTION("unique_ptr") {
        auto v = bvalue(10);
        auto c_nullptr = std::unique_ptr<int>(nullptr);
        auto c_equal = std::make_unique<int>(10);
        auto c_less = std::make_unique<int>(9);
        auto c_greater = std::make_unique<int>(11);

        CHECK(v < c_greater);       CHECK(c_greater > v);
        CHECK(v <= c_greater);      CHECK(c_greater >= v);
        CHECK(v <= c_equal);        CHECK(c_equal >= v);
        CHECK(v == c_equal);        CHECK(c_equal == v);
        CHECK(v >= c_equal);        CHECK(c_equal <= v);
        CHECK(v >= c_less);         CHECK(c_less <= v);
        CHECK(v > c_less);          CHECK(c_less < v);

        CHECK_FALSE(v == c_nullptr);
        CHECK(v != c_nullptr);

        CHECK(v <=> c_nullptr == std::weak_ordering::equivalent);
    }
    SECTION("shared_ptr") {
        auto v = bvalue(10);

        auto c_nullptr = std::shared_ptr<int>(nullptr);
        auto c_equal = std::make_shared<int>(10);
        auto c_less = std::make_shared<int>(9);
        auto c_greater = std::make_shared<int>(11);

        CHECK(v < c_greater);       CHECK(c_greater > v);
        CHECK(v <= c_greater);      CHECK(c_greater >= v);
        CHECK(v <= c_equal);        CHECK(c_equal >= v);
        CHECK(v == c_equal);        CHECK(c_equal == v);
        CHECK(v >= c_equal);        CHECK(c_equal <= v);
        CHECK(v >= c_less);         CHECK(c_less <= v);
        CHECK(v > c_less);          CHECK(c_less < v);


        CHECK_FALSE(v == c_nullptr);
        CHECK(v != c_nullptr);

        CHECK(v <=> c_nullptr == std::weak_ordering::equivalent);
    }
    SECTION("weak_ptr") {
        auto v = bvalue(10);
        auto c_equal_shared = std::make_shared<int>(10);
        auto c_less_shared = std::make_shared<int>(9);
        auto c_greater_shared = std::make_shared<int>(11);

        auto c_equal = std::weak_ptr(c_equal_shared);
        auto c_less = std::weak_ptr(c_less_shared);
        auto c_greater= std::weak_ptr(c_greater_shared);

        CHECK(v < c_greater);       CHECK(c_greater > v);
        CHECK(v <= c_greater);      CHECK(c_greater >= v);
        CHECK(v <= c_equal);        CHECK(c_equal >= v);
        CHECK(v == c_equal);        CHECK(c_equal == v);
        CHECK(v >= c_equal);        CHECK(c_equal <= v);
        CHECK(v >= c_less);         CHECK(c_less <= v);
        CHECK(v > c_less);          CHECK(c_less < v);

        auto tmp = std::make_shared<int>(10);
        auto c_expired = std::weak_ptr(tmp);
        tmp.reset();

        CHECK_FALSE(v == c_expired);
        CHECK(v != c_expired);

        CHECK(v <=> c_expired == std::weak_ordering::equivalent);
    }
}

TEST_CASE("memory: retrieve from bvalue",  "[traits-memory]")
{
    using namespace bencode;

    SECTION("unique_ptr") {
        auto bv = bvalue("long test string to allocated on the heap");
        auto t = get_as<std::unique_ptr<std::string>>(bv);
        CHECK(*t == bv);
    }
    SECTION("shared_ptr") {
        auto bv = bvalue("long test string to allocated on the heap");
        auto t = get_as<std::shared_ptr<std::string>>(bv);
        CHECK(*t == bv);
    }
    SECTION("raw_ptr") {
        auto bv = bvalue("long test string to allocated on the heap");
        auto t = get_as<std::string*>(bv);
        CHECK(t == bv);
        delete t;
    }
}


TEST_CASE("memory: retrieve from bview",  "[traits-memory]")
{
    using namespace bencode;

    SECTION("unique_ptr") {
        auto t = get_as<std::unique_ptr<std::string>>(s_view);
        CHECK(*t == s_view);
    }
    SECTION("shared_ptr") {
        auto bv = bvalue("long test string to allocated on the heap");
        auto t = get_as<std::shared_ptr<std::string>>(s_view);
        CHECK(*t == s_view);
    }
    SECTION("raw_ptr") {
        auto bv = bvalue("long test string to allocated on the heap");
        auto t = get_as<std::string*>(s_view);
        CHECK(*t == s_view);
        delete t;
    }
}