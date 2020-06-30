#include "pam.h"
#include <iostream>

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

  simple::simple(std::string pattern): pattern(pattern) {}

  simple::~simple() {}

  bool simple::match(std::string::iterator &str) {
    auto it = pattern.begin();
    while(*it) {
      switch(*it) {
        case lsquare:
        if (!match_range(*str, it)) {
          return false;
        }
        break;
        case star:
        skip(star, it);
        if (!*it) {
          while(*str) {
            str = next(str);
          }
          return true;
        }
        if (!match_star(*it, str)) {
          return false;
        }
        break;
        case mark:
        break;
        default:
        skip(backslash, it);
        if (*it != *str) {
          return false;
        }
        break;
      }
      it = std::next(it);
      str = std::next(str);
    }
    return !*it;
  }

  bool simple::match_star(char ch, std::string::iterator &it) {
    while (*it && *it != ch) {
      it = std::next(it);
    }
    return *it == ch;
  }

  bool simple::match_range(char ch, std::string::iterator &it) {
    it = std::next(it);
    bool negate = *it == bang || *it == bang;
    if (negate) {
      it = std::next(it);
    }
    bool found = *it == rsquare;
    while(!found && *it && *it != rsquare) {
      found = *it == ch;
      if (found) {
        break;
      }
      if (*it == dash && *(it+1) != rsquare) {
        found = *(it-1) <= ch && *(it+1) >= ch;
        if (found) {
          break;
        }
        it = std::next(it);
      }
      it = std::next(it);
    }
    while(*it && *it != rsquare) {
      it = std::next(it);
    }
    return (negate ? !found : found) && *it;
  }

  void simple::skip(char ch, std::string::iterator &it) {
    while (*it == ch) {
      it = next(it);
    }
  }

  void list::append(std::unique_ptr<matcher> &m) {
    matchers.push_back(std::move(m));
  }

  bool list::match(std::string::iterator &str) {
    for (auto& ptr: matchers) {
      bool ok = ptr->match(str);
      if (!ok) {
        return ok;
      }
    }
    return !*str; //true;
  }

  list::~list() {
    matchers.clear();
  }

  void choice::append(std::unique_ptr<matcher> &m) {
    matchers.push_back(std::move(m));
  }

  bool choice::match(std::string::iterator &str) {
    auto it = str;
    int length = 0;
    for (auto& ptr: matchers) {
      bool ok = ptr->match(it);
      if (ok) {
        int z = std::distance(str, it);
        length = z > length ? z : length;
      }
      it = str;
    }
    if (length) {
      str = next(str, length);
    }
    return length;
  }

  choice::~choice() {
    matchers.clear();
  }

  reverse::reverse(std::unique_ptr<matcher> &ptr): ptr(std::move(ptr)) {}

  reverse::~reverse() {}

  bool reverse::match(std::string::iterator &it) {
    auto str = it;
    bool ok = !ptr->match(str);
    if (ok) {
      it = str;
    }
    return ok;
  }

  one_or_more::one_or_more(std::unique_ptr<matcher> &ptr): ptr(std::move(ptr)) {}

  one_or_more::~one_or_more() {}

  bool one_or_more::match(std::string::iterator &it) {
    int pass = 0;
    while(*it) {
      if (!ptr->match(it)){
        break;
      }
      pass++;
    }
    return pass >= 1;
  }

  zero_or_more::zero_or_more(std::unique_ptr<matcher> &ptr): ptr(std::move(ptr)) {}

  zero_or_more::~zero_or_more() {}

  bool zero_or_more::match(std::string::iterator &it) {
    while(ptr->match(it)) {}
    return true;
  }

  zero_or_one::zero_or_one(std::unique_ptr<matcher> &ptr): ptr(std::move(ptr)) {}

  zero_or_one::~zero_or_one() {}

  bool zero_or_one::match(std::string::iterator &it) {
    int pass = 0;
    while(ptr->match(it)) {
      pass++;
    }
    return pass <= 1;
  }

  std::unique_ptr<matcher> compile(std::string pattern) {
    auto it = pattern.begin();
    return compile(it);
  }

  std::unique_ptr<matcher> compile_choice(std::string::iterator& it) {
    choice *chc = new choice;
    while(*it != rparen) {
      auto m = compile(it);
      if (*it == pipe) {
        it = std::next(it);
      }
      chc->append(m);
    }
    return std::unique_ptr<matcher>{chc};
  }

  std::unique_ptr<matcher> compile(std::string::iterator &it) {
    auto begin = it;

    list *lst = new list;
    while (*it) {
      switch (*it) {
        case pipe:
        return std::unique_ptr<matcher>{new simple{std::string(begin, it)}};
        case rparen:
        return std::unique_ptr<matcher>{new simple{std::string(begin, it)}};
        case arobase:
        if (*(it+1) == lparen) {
          auto m = std::unique_ptr<matcher>{new simple{std::string(begin, it)}};
          lst->append(m);
          it = next(it, 2);

          auto ptr = compile_choice(it);
          lst->append(ptr);

          begin = std::next(it);
          break;
        }
        case plus:
        if (*(it+1) == lparen) {
          auto m = std::unique_ptr<matcher>{new simple{std::string(begin, it)}};
          lst->append(m);
          it = std::next(it, 2);

          auto ptr = compile_choice(it);
          one_or_more *one = new one_or_more{ptr};
          std::unique_ptr<matcher> mt{one};
          lst->append(mt);

          begin = std::next(it);
          break;
        }
        case star:
        if (*(it+1) == lparen) {
          auto m = std::unique_ptr<matcher>{new simple{std::string(begin, it)}};
          lst->append(m);
          it = std::next(it, 2);

          auto ptr = compile_choice(it);
          zero_or_more *zero = new zero_or_more{ptr};
          std::unique_ptr<matcher> mt{zero};
          lst->append(mt);

          begin = std::next(it);
          break;
        }
        case mark:
        if (*(it+1) == lparen) {
          auto m = std::unique_ptr<matcher>{new simple{std::string(begin, it)}};
          lst->append(m);
          it = std::next(it, 2);

          auto ptr = compile_choice(it);
          zero_or_one *zero = new zero_or_one{ptr};
          std::unique_ptr<matcher> mt{zero};
          lst->append(mt);

          begin = std::next(it);
          break;
        }
        case bang:
        if (*(it+1) == lparen) {
          auto m = std::unique_ptr<matcher>{new simple{std::string(begin, it)}};
          lst->append(m);
          it = next(it, 2);

          auto ptr = compile_choice(it);
          reverse *rev = new reverse{ptr};
          std::unique_ptr<matcher> mt{rev};
          lst->append(mt);

          begin = std::next(it);
          break;
        }
        default:
        if (*it == backslash) {
          it = std::next(it);
        }
        break;
      }
      it = std::next(it);
    }
    if (begin < it) {
      auto m = std::unique_ptr<matcher>{new simple{std::string(begin, it)}};
      lst->append(m);
    }
    std::unique_ptr<matcher> ptr{lst};
    return ptr;
  }

  bool match(std::string str, std::string pattern) {
    std::unique_ptr<matcher> m = compile(pattern);
    auto it = str.begin();
    return m->match(it);
  }
}
