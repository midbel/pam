#ifndef __PAM_H__
#define __PAM_H__

#include <vector>
#include <string>
#include <memory>

namespace pam {

  class matcher {
  public:
    virtual ~matcher() {}
    virtual bool match(std::string::iterator &str) =0;
  };

  class list: public matcher {
  public:
    void append(std::unique_ptr<matcher> &m);
    virtual bool match(std::string::iterator &str);
    virtual ~list();
  private:
    std::vector<std::unique_ptr<matcher>> matchers;
  };

  class choice: public matcher {
  public:
    void append(std::unique_ptr<matcher> &m);
    virtual bool match(std::string::iterator &str);
    virtual ~choice();
  private:
    std::vector<std::unique_ptr<matcher>> matchers;
  };

  class simple: public matcher {
  public:
    simple(std::string str);
    virtual bool match(std::string::iterator &str);
    virtual ~simple();
  private:
    std::string pattern;

    bool match_star(char ch, std::string::iterator &it);
    bool match_range(char ch, std::string::iterator &it);
    void skip(char ch, std::string::iterator &it);
  };

  class reverse: public matcher {
  public:
    reverse(std::unique_ptr<matcher> &ptr);
    virtual ~reverse();
    virtual bool match(std::string::iterator &str);
  private:
    std::unique_ptr<matcher> ptr;
  };

  class one_or_more: public matcher {
  public:
    one_or_more(std::unique_ptr<matcher> &ptr);
    virtual ~one_or_more();
    virtual bool match(std::string::iterator &str);
  private:
    std::unique_ptr<matcher> ptr;
  };

  class zero_or_more: public matcher {
  public:
    zero_or_more(std::unique_ptr<matcher> &ptr);
    virtual ~zero_or_more();
    virtual bool match(std::string::iterator &str);
  private:
    std::unique_ptr<matcher> ptr;
  };

  class zero_or_one: public matcher {
  public:
    zero_or_one(std::unique_ptr<matcher> &ptr);
    virtual ~zero_or_one();
    virtual bool match(std::string::iterator &str);
  private:
    std::unique_ptr<matcher> ptr;
  };

  std::unique_ptr<matcher> compile(std::string pattern);
  std::unique_ptr<matcher> compile(std::string::iterator &it);

  bool match(std::string str, std::string pattern);
}

#endif
