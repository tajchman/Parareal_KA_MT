#ifndef __SOLUTION_included
#define __SOLUTION_included

#include <cstddef>
#include <iostream>
#include <cstring>
#include <fstream>
#include <cmath>
#include <cassert>

#include "deal.II/lac/vector.h"
#include "deal.II/base/function.h"
#include "deal.II/numerics/vector_tools.h"

#include "discretisation.hxx"
#include "evaluate.hxx"

typedef std::vector<dealii::Vector<double>> sComponents;

template<int dim>
class Solution 
{
  public:

    Solution(Discretisation<dim> & D, int nComponents) : _t(0), _D(D), _u(nComponents) {
      _n = _D.dof_handler().n_dofs();
      for (auto & u:_u)
        u.reinit(_n);
    }
    virtual ~Solution() {}

    virtual void operator=(const Solution& other) {
      assert(&_D == &(other._D));
      _n = other._n;
      _u = other._u;
      _t = other._t;
    }
    virtual void operator() (const Solution &other) {
      *this = other;
    }

    virtual int size() const { return _n; }

    double & time() { return _t; }
    Discretisation<dim> & D() { return _D;}

    int nComponents() { return _u.size(); }
    dealii::Vector<double> & operator[](int i) { return _u[i]; }
    const dealii::Vector<double> & operator[](int i) const { return _u[i]; }
    sComponents & components() { return _u; }
    const sComponents & components() const { return _u; }

    protected:

    size_t _n;
    double _t;
    Discretisation<dim> & _D;
    sComponents _u;
};

template <int dim>
std::ostream & operator<< (std::ostream &f, const Solution<dim> &s)
{
  int iC, nC = s.nComponents();
  size_t n = s.n();

  f << std::endl;
  for (iC=0; iC<nC; iC++)
    f << "              " << 'u' + iC;
  f << std::endl;
  for (size_t i=0; i<n; i++) {
    f << std::right << std::setw(7) << i;
    for (iC=0; iC<nC; iC++)
      f << std::setw(15) << std::setprecision(8) << s.component(iC)[i]; 
    f << std::endl;
  }
  return f;
}

template <int dim>
double difference(const Solution<dim> &s1, const Solution<dim> &s2)
{
  return 0;
}



#endif