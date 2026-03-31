#ifndef __PROBE_included__
#define __PROBE_included__

#include <cstddef>
#include <fstream>

#include "global.hxx"

class Probe {

  public :

  Probe() : _n(0), _name(""), _rank(-1) {}
  
  Probe(const std::string &name, int n, int c, int rank=-1) 
    : _n(n), _c(c), _name(name), _rank(rank) {}
  
  Probe(const Probe &other) {
    _n = other._n;
    _c = other._c;
    _name = other._name;
    _rank = other._rank;
  }

  Probe & operator=(const Probe &other) {
    _n = other._n;
    _c = other._c;
    _name = other._name;
    _rank = other._rank;
    return *this;
  }
  
  void start(const std::string &prefix, int order=-1);
  void record(double t, const sComponents &v);
  void stop();

  private:
    size_t _n;
    int _c;
    int _rank;
    std::string _name;
    std::ofstream _f;
};

#endif
