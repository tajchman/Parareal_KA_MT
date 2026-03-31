#ifndef __DISCRETISATION__included__
#define __DISCRETISATION__included__

#include "deal.II/grid/tria.h"
#include "deal.II/fe/fe_q.h"
#include "deal.II/dofs/dof_handler.h"
#include "deal.II/grid/grid_generator.h"

#include <iostream>
#include <cmath>

template<int dim>
class Discretisation {

  public:
    Discretisation(int refinment);

    dealii::Triangulation<dim> & triangulation() {
      return _triangulation;
    }
    
    const dealii::FE_Q<dim> & fe() {
      return _fe;
    }
    
    dealii::DoFHandler<dim> & dof_handler() {
      return _dof_handler;
    }
    
    dealii::AffineConstraints<double> & constraints() {
      return _constraints;
    }
    
  private:

    dealii::Triangulation<dim> _triangulation;
    const dealii::FE_Q<dim>    _fe;
    dealii::DoFHandler<dim>    _dof_handler;
    dealii::AffineConstraints<double> _constraints;
};

template<int dim>
Discretisation<dim>::Discretisation(int refinment) 
  : _fe(1), _dof_handler(_triangulation)
{
  dealii::GridGenerator::hyper_cube(_triangulation, -1, 1);
  _triangulation.refine_global(refinment);

  _dof_handler.distribute_dofs(_fe);

}

#endif