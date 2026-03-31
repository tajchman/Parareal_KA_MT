#ifndef __CODEBASE__included__
#define __CODEBASE__included__

#include "deal.II/base/logstream.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <map>
#include <vector>
#include <cstring>
#include "os.hxx"
#include "probe.hxx"
#include "state.hxx"
#include "global.hxx"
#include "solution.hxx"

class CodeBase {

  public:

    CodeBase(const char *name, int rank)
      : _t(0.0), _dt(0.0), _it(0L), _codeName(name)
    {
      std::string s = _codeName;
      if (rank >= 0)
        s += "_"+ std::to_string(rank);
      s += ".log";
      _f.open(s.c_str(), std::ios_base::out);
      _log.depth_file(10);
      _log.attach(_f);
      _log << " Log file: " << s << std::endl;
    }

    virtual ~CodeBase()
    {
      _log.detach();
      _f.close();
  
      for (auto & p: _probes)
        p.stop();
    }

    const char *name() const { return _codeName.c_str(); }

    size_t n()
    {
      return _n;
    }

    double t_current() const { return _t; }
    double dt_current() const { return _dt; }
    double &t_current() { return _t; }
    double &dt_current() { return _dt; }

    virtual void print() const {}

    void outputDir(const char *out, bool create=true) {
       _outDir = out; if (create) mkdir_p(out); 
      }
    void outputDir(const std::string & out, bool create=true) {
       _outDir = out; if (create) mkdir_p(out.c_str()); 
      }
    const std::string & outputDir() const { return _outDir; }
    double & outputTime() { return _outTime; }
    double outputTime() const { return _outTime; }
    unsigned long & iteration() { return _it; }
    unsigned long iteration() const { return _it; }

    void addProbe(size_t index, int component)
    {
      _probes.push_back(Probe(name(), index, component));
    }

    template<class T>
    void recordProbes(double t, const T &v)
    {
      for (auto &p : _probes)
        p.record(t, v.components());
    }
    void startProbes(int order=-1)
    {
      for (auto &p : _probes)
        p.start(_outDir, order);
    }

    void stopProbes()
    {
      for (auto &p : _probes)
        p.stop();
    }
    bool & verbose() { return _verbose; }

    virtual void initState() {}
    virtual void allocateState(State &s) {}

    State & getInternalState() { return _internalState; }

    dealii::LogStream & logStream() 
    {
      return _log;
    }
  
    void importState(const State & other)
    {
      auto & c1 = _internalState.components;
      auto & c2 = other.components;
      size_t i, n = c1.size();

      for (i=0; i<n; i++)
        std::memcpy(c1[i].data, c2[i].data, c1[i].n * sizeof(double));
    }

    void exportState(State &S) {
      size_t i, n = _internalState.components.size();
      S.clear();
      S.components.resize(n);
      for (i=0; i<n; i++) {
        auto & c1 = S.components[i];
        auto & c2 = _internalState.components[i];
        c1.data = new double[c2.n];
        c1.n = c2.n;
        c1.name = c2.name;
        std::memcpy(c1.data, c2.data, c1.n * sizeof(double));
        }
    }

  protected:

    size_t _n;
    std::ofstream _f;
    dealii::LogStream _log;
    bool _verbose;
    std::string _codeName;

    virtual void _timeStep(double dT) = 0;
    
    double _tmin, _t, _dt, _tmax;
    unsigned long _it;
    std::string _outDir;
    double _outTime;
    std::vector<Probe> _probes;

    State _internalState;
  };


#endif
