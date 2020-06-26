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
      matcher c{pat};
      return c.match(str);
  }

  matcher::matcher(std::string str): begin(str.begin()), end(str.end()), it(str.begin()) {}

  matcher::matcher(std::string::iterator b, std::string::iterator e): begin(b), end(e), it(b) {}

  matcher::matcher(const matcher& c): begin(c.it), end(c.end), it(c.it) {}

  matcher::operator bool() const {
    return has();
  }

  bool matcher::match(std::string str) {
    if (!str.size()) {
      return false;
    }
    matcher s{str};
    return match(s);
  }

  bool matcher::match(matcher& str) {
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
          if (!match_group(str)) {
            return false;
          }
          break;
        }
        case plus: // +(abc|xyz)
        if (peek() == lparen) {
          advance(2);
          matcher pat = *this;
          int length = 0;
          while (str.has() && pat.has() && pat.match_group(str)) {
            length = pat.length();
            pat.reset();
          }
          if (!length) {
            return false;
          }
          advance(length);
          break;
        }
        case star: // *(abc|xyz) || *
        if (peek() == lparen) {
          advance(2);
          matcher pat = *this;
          int length = 0;
          while(str.has() && pat.has() && pat.match_group(str)) {
            length = pat.length();
            pat.reset();
          }
          advance(length);
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
          matcher pat = *this;
          int pass = 0;
          int length = 0;
          while (pass <= 1 && str.has() && pat.has()) {
            if (!pat.match_group(str)) {
              break;
            }
            pass++;
            length = pat.length();
            pat.reset();
          }
          if (pass > 1) {
            return false;
          }
          advance(length);
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

  void matcher::reset() {
    it = begin;
  }

  bool matcher::match_star(char ch) {
    while(*it) {
      if (*it == ch) {
        return true;
      }
      advance();
    }
    return false;
  }

  bool matcher::match_range(char ch) {
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

  bool matcher::match_group(matcher& in) {
    matcher s = in;
    int length = 0;
    while(current() != rparen) {
      if (current() == pipe) {
        advance();
      }
      bool ok = match(s);
      if (int z = s.length(); ok && z) {
        length = z > length ? z : length;
      }
      while (!done() && (current() != pipe && current() != rparen)) {
        advance();
      }
      s.reset();
    }
    in.advance(length);
    return length > 0;
  }

  bool matcher::done() const {
    return !has();
  }

  bool matcher::has() const {
    return it < end;
  }

  std::string matcher::input() const {
    return std::string(begin, end);
  }

  std::string matcher::str() const {
    return std::string(begin, it);
  }

  std::string matcher::rstr() const {
    return std::string(it, end);
  }

  int matcher::rlength() const {
    return std::distance(it, end);
  }

  int matcher::length() const {
    return std::distance(begin, it);
  }

  int matcher::size() const {
    return std::distance(begin, end);
  }

  char matcher::current() const {
    return *it;
  }

  char matcher::peek() const {
    return *std::next(it);
  }

  void matcher::advance(int n) {
    if (n <= 0) {
      n = 1;
    }
    it = std::next(it, n);
  }

  void matcher::advance(char ch) {
    while (*it != ch) {
      advance();
    }
  }

  void matcher::skip_chars(char ch, int n) {
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
