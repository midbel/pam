#define CATCH_CONFIG_MAIN
#include "pam.h"
#include "catch.hpp"

TEST_CASE("simple pattern matching", "[simple]") {
  REQUIRE(pam::match("foobar", "foobar"));
  REQUIRE(pam::match("foobar", "fo\\obar"));
  REQUIRE(pam::match("foobar", "f??bar"));
  REQUIRE(!pam::match("foobar", "barfoo"));
  REQUIRE(!pam::match("foobar", "foobaz"));
}

TEST_CASE("star pattern matching", "[simple]") {
  REQUIRE(pam::match("foobar", "foo*"));
  REQUIRE(pam::match("foobar", "**bar"));
  REQUIRE(pam::match("foo*bar", "foo\\*bar"));
}

TEST_CASE("range pattern matching", "[simple]") {
  REQUIRE(pam::match("foobar", "f[oO][a-z]bar"));
}
