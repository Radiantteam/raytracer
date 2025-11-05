#include "../doctest.h"
#include "Vec3.hpp"

TEST_CASE("Vec3 default constructor initializes to zero")
{
    Vec3 v;
    CHECK(v.x == 0.0f);
    CHECK(v.y == 0.0f);
    CHECK(v.z == 0.0f);
}

TEST_CASE("Vec3 parameter constructor sets values correctly")
{
    Vec3 v(1.0f, 2.0f, 3.0f);
    CHECK(v.x == 1.0f);
    CHECK(v.y == 2.0f);
    CHECK(v.z == 3.0f);
}

TEST_CASE("Vec3 addition works")
{
    Vec3 a(1, 2, 3), b(4, 5, 6);
    Vec3 r = a + b;
    CHECK(r.x == 5.0f);
    CHECK(r.y == 7.0f);
    CHECK(r.z == 9.0f);
}

TEST_CASE("Vec3 subtraction works")
{
    Vec3 a(4, 6, 8), b(1, 2, 3);
    Vec3 r = a - b;
    CHECK(r.x == 3.0f);
    CHECK(r.y == 4.0f);
    CHECK(r.z == 5.0f);
}

TEST_CASE("Vec3 scalar multiplication works")
{
    Vec3 v(1, 2, 3);
    Vec3 r = v * 2.0f;
    CHECK(r.x == 2.0f);
    CHECK(r.y == 4.0f);
    CHECK(r.z == 6.0f);
}

TEST_CASE("Vec3 scalar division works")
{
    Vec3 v(2, 4, 6);
    Vec3 r = v / 2.0f;
    CHECK(r.x == 1.0f);
    CHECK(r.y == 2.0f);
    CHECK(r.z == 3.0f);
}

TEST_CASE("Vec3 unary minus works")
{
    Vec3 v(1, -2, 3);
    Vec3 r = -v;
    CHECK(r.x == -1.0f);
    CHECK(r.y == 2.0f);
    CHECK(r.z == -3.0f);
}
