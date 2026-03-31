#ifndef __CODE__included__
#define __CODE__included__

#include "deal.II/base/logstream.h"
#include "deal.II/lac/sparse_matrix.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <map>
#include <vector>
#include <cstring>
#include "discretisation.hxx"
#include "solution.hxx"
#include "os.hxx"
#include "probe.hxx"
#include "state.hxx"
#include "codeBase.hxx"

template<int dim, int ncomponents>
class Code : public CodeBase {

  public:

    Code(Discretisation<dim> & D, const char *name = "", int rank = -1)
      : CodeBase(name, rank), 
        _discretisation(D), 
        _u_current(D, ncomponents), 
        _u_previous(D, ncomponents)
    {
      _n = _discretisation.dof_handler().n_dofs();
    }

    void run(double Tmin, double Tmax, double dT, int order=-1);

    Solution<dim> & current()  { return _u_current; }
    Solution<dim> & previous() { return _u_previous; }
    const Solution<dim> & current() const { return _u_current; }
    const Solution<dim> & previous() const { return _u_previous; }

    void setInput(const Solution<dim> & u) { 
      _u_previous = u;
    }
    void getOutput(Solution<dim> & u) { u = _u_current; }
    void getPrevious(Solution<dim> & u) { u = _u_previous;}

  protected:

    Discretisation<dim> &_discretisation;
    Solution<dim> _u_current, _u_previous;
  };

template<int dim, int ncomponents>
void Code<dim, ncomponents>::run(double Tmin, double Tmax, double dT, int order)
{
  _log.push("Run  ");
  LOGSTREAM(*this) << " Tmin = " << std::setprecision(18) << Tmin 
       << " Tmax = " << std::setprecision(18) << Tmax
       << " dT   = " << std::setprecision(18) << dT << std::endl;

  double nextOutput = Tmin;
  _u_previous.time() = Tmin;
  startProbes(order);
  recordProbes(Tmin, _u_previous);
  if (_outTime > 0.0)
    nextOutput += _outTime;

  for (_t = Tmin; _t < Tmax; )
  {
    std::cerr <<  " Time step " << _it 
         << " at t=" << std::setprecision(5)<< _t << "     \r";
    LOGSTREAM(*this) << " Time step " << _it 
         << " at t=" << std::setprecision(5) << _t << std::endl;
    if (_verbose)
      LOGSTREAM(*this) << _internalState;

    double t_new = _t + dT;
    if (t_new > Tmax) {
      t_new = Tmax;
      dT = t_new - _t;
    }
    _timeStep(dT);
    _it ++;
    _t = t_new;

    if (_t < Tmax - dT*0.001) recordProbes(_t, _u_current);
    if (_outTime > 0.0 && _t >= nextOutput) {
      print();
      nextOutput += _outTime;
    }
  }
  recordProbes(Tmax, _u_current);
  std::cerr <<  " Time step " << _it 
            << " at t=" << std::setprecision(5) << _t << "     \r";
  _log << " Time step " << _it 
       << " at t=" << std::setprecision(5) << _t << std::endl;
  if (_verbose)
    _log << _internalState;

  if (_outTime > 0.0)
    print();
  stopProbes();

  _log.pop();
  std::cerr << std::endl;
}

inline
dealii::LogStream &  operator<< (dealii::LogStream &f, dealii::SparseMatrix<double>&m)
{
  const double * x = m.data().get();
  size_t n = m.size();
  for (size_t i= 0; i<n; i++)
  {
    f << " " << i << ": " << std::setprecision(18) << x[i] << std::endl;
  }
  return f;
}

#endif
