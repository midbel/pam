#ifndef __PAM_H__
#define __PAM_H__

#include <string>

namespace pam {
  class cursor {
  public:
    cursor(std::string str);
    cursor(std::string::iterator b, std::string::iterator e);
    cursor(const cursor& c);

    bool match(std::string str);

    void reset();

  private:
    std::string::iterator begin;
    std::string::iterator end;
    std::string::iterator it;

    bool match(cursor& str);
    bool match_star(char ch);
    bool match_range(char ch);
    bool match_group(cursor& str);

    bool done() const;
    bool has() const;
    char current() const;
    char peek() const;

    void advance(int n = 0);
    void advance(char ch);
    void skip_chars(char ch, int n = 0);

    std::string input() const;
    std::string str() const;
    std::string rstr() const;
    int length() const;
    int rlength() const;
    int size() const;
  };

  bool match(std::string str, std::string pat);
}

#endif
