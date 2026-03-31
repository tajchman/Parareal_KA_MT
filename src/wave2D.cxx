#include "wave2D.hxx"
#define _USE_MATH_DEFINES
#include <cmath>

#include "deal.II/dofs/dof_tools.h"
#include "deal.II/numerics/data_out.h"
#include "deal.II/lac/solver_cg.h"
#include "deal.II/numerics/matrix_creator.h"
#include "deal.II/numerics/matrix_tools.h"
#include "deal.II/lac/precondition.h"

double initU(const dealii::Point<2> & /*p*/)
{
  return 0.0;
}

double initV(const dealii::Point<2> & /*p*/)
{
  return 0.0;
}

double boundU(const dealii::Point<2> & p, double t)
{
  if ((t <= 0.5) && (p[0] < 0) && (p[1] < 1. / 3) && (p[1] > -1. / 3))
    return std::sin(t * 4 * M_PI);
  else
    return 0.0;
}

double boundV(const dealii::Point<2> & p, double t)
{
  if ((t <= 0.5) && (p[0] < 0) && (p[1] < 1. / 3) && (p[1] > -1. / 3))
    return std::cos(t * 4 * M_PI) * 4 * M_PI;
  else
    return 0.0;
}

double force(const dealii::Point<2> & /*p*/, double /*t*/)
{
  return 0.0;
}

Wave2D::Wave2D(Discretisation<2> & D, const char *name, int rank) 
    : Code<2,2>(D, name, rank), _theta(0.5)
{
  dealii::DynamicSparsityPattern dsp(_n, _n);
  dealii::DoFTools::make_sparsity_pattern(_discretisation.dof_handler(), dsp);
  _sparsity_pattern.copy_from(dsp);

  _mass_matrix.reinit(_sparsity_pattern);
  _laplace_matrix.reinit(_sparsity_pattern);
  _matrix_u.reinit(_sparsity_pattern);
  _matrix_v.reinit(_sparsity_pattern);

  dealii::MatrixCreator::create_mass_matrix(
    _discretisation.dof_handler(),
    dealii::QGauss<2>(_discretisation.fe().degree + 1),
    _mass_matrix);
  dealii::MatrixCreator::create_laplace_matrix(
    _discretisation.dof_handler(),
    dealii::QGauss<2>(_discretisation.fe().degree + 1),
    _laplace_matrix);

  _system_rhs.reinit(_n);

  _discretisation.constraints().close();

}

void Wave2D::initialize(Solution<2> u, 
                 double (& U)(const dealii::Point<2, double>&), 
                 double (& V)(const dealii::Point<2, double>&))
{
  
  dealii::VectorTools::project
    ( _discretisation.dof_handler(),
      _discretisation.constraints(),
      dealii::QGauss<2>(_discretisation.fe().degree + 1),
      EvaluateX<2>(U),
      u[0]);
  dealii::VectorTools::project
    ( _discretisation.dof_handler(),
      _discretisation.constraints(),
      dealii::QGauss<2>(_discretisation.fe().degree + 1),
      EvaluateX<2>(V),
      u[1]);
}

void Wave2D::_solve_u()
{
  dealii::SolverControl  solver_control(1000, 1e-8 * _system_rhs.l2_norm());
  dealii::SolverCG<dealii::Vector<double>> cg(solver_control);

  cg.solve(_matrix_u, _u_current[0], _system_rhs, 
     dealii::PreconditionIdentity());

  if (_verbose)
    LOGSTREAM(*this) << " u-equation: " << solver_control.last_step()
                    << " CG iterations." << std::endl;
}

void Wave2D::_solve_v()
{
  dealii::SolverControl            solver_control(1000, 1e-8 * _system_rhs.l2_norm());
  dealii::SolverCG<dealii::Vector<double>> cg(solver_control);

  cg.solve(_matrix_v, _u_current[0], _system_rhs, 
     dealii::PreconditionIdentity());

  if (_verbose)
    LOGSTREAM(*this) << " u-equation: " << solver_control.last_step()
                    << " CG iterations." << std::endl;
}

void Wave2D::_timeStep(double dT)
{
  dealii::Vector<double> tmp(_n);
  dealii::Vector<double> forcing_terms(_n);

  {
    _mass_matrix.vmult(_system_rhs, _u_previous[0]);

    _mass_matrix.vmult(tmp, _u_previous[1]);
    _system_rhs.add(dT, tmp);

    _laplace_matrix.vmult(tmp, _u_previous[0]);
    _system_rhs.add(-_theta * (1 - _theta) * dT * dT, tmp);

    EvaluateXT<2> rhs_function(force);
    rhs_function.set_time(_t);
    dealii::VectorTools::create_right_hand_side(_discretisation.dof_handler(),
                                            dealii::QGauss<2>(_discretisation.fe().degree + 1),
                                            rhs_function,
                                            tmp);
    forcing_terms = tmp;
    forcing_terms *= _theta * dT;

    rhs_function.set_time(_t - dT);
    dealii::VectorTools::create_right_hand_side(_discretisation.dof_handler(),
                                        dealii::QGauss<2>(_discretisation.fe().degree + 1),
                                        rhs_function,
                                        tmp);

    forcing_terms.add((1 - _theta) * dT, tmp);

    _system_rhs.add(_theta * dT, forcing_terms);
    {
      EvaluateXT<2> boundary_values_u_function(boundU);
      boundary_values_u_function.set_time(_t);

      std::map<dealii::types::global_dof_index, double> boundary_values;
      dealii::VectorTools::interpolate_boundary_values(_discretisation.dof_handler(),
                                               0,
                                               boundary_values_u_function,
                                               boundary_values);

      _matrix_u.copy_from(_mass_matrix);
      _matrix_u.add(_theta * _theta * dT * dT, _laplace_matrix);
 
      dealii::MatrixTools::apply_boundary_values(boundary_values,
                                         _matrix_u,
                                         _u_current[0],
                                         _system_rhs);
     }
    _solve_u();


    _laplace_matrix.vmult(_system_rhs, _u_current[0]);
    _system_rhs *= -_theta * dT;

    _mass_matrix.vmult(tmp, _u_previous[1]);
    _system_rhs += tmp;

    _laplace_matrix.vmult(tmp, _u_previous[0]);
    _system_rhs.add(-dT * (1 - _theta), tmp);

    _system_rhs += forcing_terms;

    {
      EvaluateXT<2> boundary_values_v_function(boundV);
      boundary_values_v_function.set_time(_t);

      std::map<dealii::types::global_dof_index, double> boundary_values;
      dealii::VectorTools::interpolate_boundary_values(_discretisation.dof_handler(),
                                               0,
                                               boundary_values_v_function,
                                               boundary_values);
      _matrix_v.copy_from(_mass_matrix);
      dealii::MatrixTools::apply_boundary_values(boundary_values,
                                         _matrix_v,
                                         _u_current[1],
                                         _system_rhs);
    }
    _solve_v();
    _t += dT;

    if (_verbose) {
      _log << " Total energy: " << std::setprecision(18)
           << (_mass_matrix.matrix_norm_square(_u_current[1]) +
               _laplace_matrix.matrix_norm_square(_u_current[0])) / 2
            <<std::endl;
    }
    _u_previous = _u_current;
  }
}

void Wave2D::print() const
{
  dealii::DataOut<2> data_out;

  data_out.attach_dof_handler(_discretisation.dof_handler());
  data_out.add_data_vector(_u_current[0], "U");
  data_out.add_data_vector(_u_current[1], "V");

  data_out.build_patches();
  std::string filename = _outDir + "/solution-"
     + dealii::Utilities::int_to_string(_it, 6) + ".vtu";
  dealii::DataOutBase::VtkFlags vtk_flags;
  vtk_flags.compression_level = dealii::DataOutBase::CompressionLevel::best_speed;
  data_out.set_flags(vtk_flags);
  std::ofstream output(filename);
  data_out.write_vtu(output);
}

