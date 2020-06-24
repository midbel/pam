#include <iostream>
#include "pam.h"

int main(int argc, char** argv) {
  using pam::match;
  using std::cout;
  using std::endl;
  using std::boolalpha;

  cout << "foobar (f??bar): " << boolalpha << match("foobar", "f??bar") << endl;
  cout << "foobar (foobar): " << boolalpha << match("foobar", "foobar") << endl;
  cout << "foo*bar (foo\\*bar): " << boolalpha << match("foo*bar", "foo\\*bar") << endl;
  cout << "foobar (foo**): " << boolalpha << match("foobar", "foo**") << endl;
  cout << "foobar (**bar): " << boolalpha << match("foobar", "**bar") << endl;
  cout << "foobar (f[a-z][oO]bar): " << boolalpha << match("foobar", "f[a-z][oO]bar") << endl;
  cout << "foobar (foobar): " << boolalpha << match("foobar", "foobar") << endl;
  // cout << "foobar (foo@(bar|gun)): " << boolalpha << match("foobar", "foo@(bar|gun)") << endl;
  // cout << "foobar (foo@(gun|bar)): " << boolalpha << match("foobar", "foo@(gun|bar)") << endl;
  // cout << "foobar (foo@(baz|bar)): " << boolalpha << match("foobar", "foo@(baz|bar)") << endl;
  // cout << "foobar (foo+(baz|bar)): " << boolalpha << match("foobar", "foo+(baz|bar)") << endl;
}
