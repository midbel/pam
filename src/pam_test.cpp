#include "pam.h"
#include "catch.hpp"

TEST_CASE("simple pattern matching", "[simple]") {
  CHECK(pam::match("foobar", "foobar"));
  CHECK(pam::match("foobar", "fo\\obar"));
  CHECK(pam::match("foobar", "f??bar"));
  CHECK_FALSE(pam::match("foobar", "barfoo"));
  CHECK_FALSE(pam::match("foobar", "foobaz"));
  CHECK_FALSE(pam::match("foo", "foobar"));
}

TEST_CASE("star pattern matching", "[simple]") {
  CHECK(pam::match("foo", "foo*"));
  CHECK(pam::match("foobar", "foo*"));
  CHECK(pam::match("foobar", "**bar"));
  CHECK(pam::match("foo*bar", "foo\\*bar"));
}

TEST_CASE("range pattern matching", "[simple]") {
  CHECK(pam::match("foobar", "f[oO][a-z]bar"));
}

TEST_CASE("alternative pattern matching", "[extended]") {
  CHECK(pam::match("foo@bar", "foo@bar"));
  CHECK(pam::match("foo@bar", "foo\\@bar"));
  CHECK(pam::match("foobar", "foo@(bar|baz)"));
  CHECK(pam::match("foobar", "foo@(baz|bar)"));
}

TEST_CASE("one or more pattern matching", "[extended]") {
  CHECK(pam::match("foo+bar", "foo+bar"));
  CHECK(pam::match("foo+bar", "foo\\+bar"));
  CHECK(pam::match("foobar", "foo+(bar|baz)"));
  CHECK(pam::match("foobarbazbar", "foo+(bar|baz)"));
  CHECK_FALSE(pam::match("foobar", "foo+(tar|baz)"));
}

TEST_CASE("zero or more pattern matching", "[extended]") {
  CHECK(pam::match("foobar", "foo*(bar|baz)"));
  CHECK(pam::match("foobarbarbaz", "foo*(bar|baz)"));
  CHECK(pam::match("foo", "foo*(bar|baz)"));
  CHECK_FALSE(pam::match("foobar", "foo*(tar|baz)"));
  CHECK_FALSE(pam::match("foo", "foo*(bar|baz)!"));
}

TEST_CASE("zero or one pattern matching", "[extended]") {
  CHECK(pam::match("foo", "foo?(bar|baz)"));
  CHECK(pam::match("foobar", "foo?(bar|baz)"));
  CHECK_FALSE(pam::match("foobarbaz", "foo?(bar|baz)"));
}
