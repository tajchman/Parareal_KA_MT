#ifndef _ARGUMENTS_HXX
#define _ARGUMENTS_HXX

#include <map>
#include <string>
#include <iostream>
#include <any>
#include <stdexcept>

class Arguments {

public:
  
  void parse(int argc, char **argv);

  template <typename T>
  void add(const char *s, const T defaultValue, const char *help)
  {
    _arguments[s] = std::pair<std::any, std::string>(std::any(defaultValue), help);
  }

  template <typename T>
  void get(const char *s, T & value)
  {
    auto t = _arguments.find(s);
    if (t == _arguments.end())
      throw std::invalid_argument(s);

    value =  std::any_cast<T>(t->second.first);
  }

  void get(const char *s, std::string & value)
  {
    auto t = _arguments.find(s);
    if (t == _arguments.end())
      throw std::invalid_argument(s);

    value =  std::any_cast<const char *>(t->second.first);
  }

  void help(const char *);

private:

  std::map<std::string, std::pair<std::any, std::string> > _arguments;
  friend std::ostream & operator<<(std::ostream & f, const Arguments & args);

};


#endif
