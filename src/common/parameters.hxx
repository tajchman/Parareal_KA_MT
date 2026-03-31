#ifndef _PARAMETERS_HXX
#define _PARAMETERS_HXX

#include <string>

#include "arguments.hxx"

class Parameters
{
public:
  Parameters();

  template<typename T>
  void Get(const char *name, T &v) {
    _args.get(name, v);
  }
  
  template<typename T>
  void Add(const char *name, const T &v, const char *help) {
    _args.add(name, v, help);
  }
  
  void Parse(int argc, char **argv) {
    _args.parse(argc, argv);
  }

private:
  Arguments _args;
};


#endif
