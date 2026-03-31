#ifndef __STATE_HXX
#define __STATE_HXX

#include <ostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdlib>

struct componentState
{
  size_t n;
  double * data;
  std::string name;
};

struct State {
  State();
  ~State();
  
  void clear();
  State & operator=(const State &other);

  bool owner;
  std::vector<componentState> components;
};

std::ostream & operator<< (std::ostream & f, const State & S);

#endif
