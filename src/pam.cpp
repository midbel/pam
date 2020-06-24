#include "pam.h"

namespace pam {
  const char arobase = '@';
  const char star = '*';
  const char mark = '?';
  const char plus = '+';
  const char lsquare = '[';
  const char rsquare = ']';
  const char bang = '!';
  const char caret = '^';
  const char backslash = '\\';
  const char lparen = '(';
  const char rparen = ')';
  const char pipe = '|';
  const char dash = '-';


  bool match(std::string str, std::string pat) {
      cursor c{pat};
      return c.match(str);
  }

  cursor::cursor(std::string str): begin(str.begin()), end(str.end()), it(str.begin()) {}

  cursor::cursor(std::string::iterator b, std::string::iterator e): begin(b), end(e), it(b) {}

  cursor::cursor(const cursor& c): begin(c.it), end(c.end), it(c.it) {}

  bool cursor::match(std::string str) {
    if (!str.size()) {
      return false;
    }
    return match(cursor{str});
  }

  bool cursor::match(cursor str) {
    while (has() && str.has()) {
      switch (current()) {
        case pipe:
        return true;
        case rparen:
        return true;
        case bang: // !(abc|xyz)
        if (peek() == lparen) {
          advance(2);
          break;
        }
        case arobase: // @(abc|xyz)
        if (peek() == lparen) {
          advance(2);
          int length = match_group(str);
          if (!length) {
            return false;
          }
          str.advance(length);
          break;
        }
        case plus: // +(abc|xyz)
        if (peek() == lparen) {
          advance(2);
          break;
        }
        case star: // *(abc|xyz) || *
        if (peek() == lparen) {
          advance(2);
        } else {
          skip_chars(star);
          if (done()) {
            return true;
          }
          if (!str.match_star(current())) {
            return false;
          }
        }
        break;
        case mark: // ?(abc|xyz) || ?
        if (peek() == lparen) {
          advance(2);
          break;
        }
        break;
        case lsquare:
        if (!match_range(str.current())) {
          return false;
        }
        break;
        default:
        skip_chars(backslash, 1);
        if (current() != str.current()) {
          return false;
        }
        break;
      }
      advance();
      str.advance();
    }
    char last = current();
    return str.done() && (done() || last == rparen || last == pipe);
  }

  void cursor::reset() {
    it = begin;
  }

  bool cursor::match_star(char ch) {
    while(*it) {
      if (*it == ch) {
        return true;
      }
      advance();
    }
    return false;
  }

  bool cursor::match_range(char ch) {
    if (*it == lsquare) {
      advance();
    }
    bool rev = current() == caret || current() == bang;
    if (rev) {
      advance();
    }
    bool found = current() == rsquare;
    while (!found && has() && current() != rsquare) {
      found = current() == ch;
      if (found) {
        break;
      }
      if (current() == dash && peek() != rsquare) {
        found = *(it-1) <= ch && *(it+1) >= ch;
        if (found) {
          break;
        }
        advance();
      }
      advance();
    }
    found = (rev ? !found : found) && has();
    advance(rsquare);
    return found;
  }

  int cursor::match_group(cursor str) {
    cursor s = str;
    int length = 0;
    while(current() != rparen) {
      if (current() == pipe) {
        advance();
      }
      bool ok = match(s);
      if (int z = s.length(); ok) {
        length = z > length ? z : length;
      }
      while (true) {
        advance();
        if (current() == pipe || current() == rparen) {
          break;
        }
      }
      s.reset();
    }
    return length;
  }

  bool cursor::done() const {
    return !has();
  }

  bool cursor::has() const {
    return it < end;
  }

  std::string cursor::input() const {
    return std::string(begin, end);
  }

  std::string cursor::str() const {
    return std::string(begin, it);
  }

  std::string cursor::rstr() const {
    return std::string(it, end);
  }

  int cursor::rlength() const {
    return std::distance(it, end);
  }

  int cursor::length() const {
    return std::distance(begin, it);
  }

  char cursor::current() const {
    return *it;
  }

  char cursor::peek() const {
    return *std::next(it);
  }

  void cursor::advance(int n) {
    if (n <= 0) {
      n = 1;
    }
    it = std::next(it, n);
  }

  void cursor::advance(char ch) {
    while (*it != ch) {
      advance();
    }
  }

  void cursor::skip_chars(char ch, int n) {
    int skip = 0;
    while (*it == ch) {
      if (n > 0 && skip >= n) {
        break;
      }
      skip++;
      it = next(it);
    }
  }
}
