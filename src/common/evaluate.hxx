#ifndef __EVALUATE_HXX
#define __EVALUATE_HXX

#include "deal.II/base/function.h"

template <int dim>
class EvaluateX : public dealii::Function<dim>
{
  public:
    EvaluateX(double f(const dealii::Point<dim> & p)) 
      : _f(f) {}

    virtual double
    value(const dealii::Point<dim> &p, const unsigned int component = 0) const
    {
      return _f(p);
    }
  private:
    double (*_f)(const dealii::Point<dim> & p);

};

template <int dim>
class EvaluateXT : public dealii::Function<dim>
{
  public:
    EvaluateXT(double f(const dealii::Point<dim> & p, double t)) 
      : _f(f) {}

    virtual double
    value(const dealii::Point<dim> &p, const unsigned int component = 0) const
    {
      double t = this->get_time();
      return _f(p, t);
    }
  private:
    double (*_f)(const dealii::Point<dim> & p, double t);

};

#endif
