#ifndef __WAVE2D__included__
#define __WAVE2D__included__

#include "deal.II/base/point.h"
#include <deal.II/lac/dynamic_sparsity_pattern.h>
#include "deal.II/lac/sparsity_pattern.h"
#include "deal.II/lac/vector.h"

double initU(const dealii::Point<2> & p);
double initV(const dealii::Point<2> & p);
double boundU(const dealii::Point<2> & p, double t);
double boundV(const dealii::Point<2> & p, double t);
double force(const dealii::Point<2> & p, double t);


#include "code.hxx"

class Wave2D : public Code<2,2> {

  public:
    Wave2D(Discretisation<2> & D, const char *name = "", int rank = -1);
    ~Wave2D() {}

    void print() const;

    void initialize(Solution<2> u,
                 double (& U)(const dealii::Point<2, double>&), 
                 double (& V)(const dealii::Point<2, double>&));

  private:

    void _solve_u();
    void _solve_v();
    void _timeStep(double dT);

    dealii::SparsityPattern      _sparsity_pattern;
    dealii::SparseMatrix<double> _mass_matrix;
    dealii::SparseMatrix<double> _laplace_matrix;
    dealii::SparseMatrix<double> _matrix_u;
    dealii::SparseMatrix<double> _matrix_v;

    dealii::Vector<double> _system_rhs;

    double       _theta;
};


#endif