#ifndef __PARAREAL_included__
#define __PARAREAL_included__

#include "os.hxx"
#include "code.hxx"
#include "parameters.hxx"
#include "parareal.hxx"
#include "probe.hxx"
#include <iostream>
#include <sstream>
#include <mpi.h>


template<int dim, int ncomponents>
class Parareal
{
public:
  Parareal(int rProcess, int nProcess,
           Code<dim, ncomponents> & C_coarse, Code<dim, ncomponents> & C_fine, 
           Discretisation<dim> &D, Parameters &p)
      : rank(rProcess), rankMin(0), rankMax(nProcess-1), 
        _C_coarse(C_coarse), _C_fine(C_fine),
        _initialSolution(nullptr),
        _currentCoarseSolution(D, ncomponents),
        _previousCoarseSolution(D, ncomponents),
        _previousFineSolution(D, ncomponents),
        _currentFineSolution(D, ncomponents)

  { 
    if (rank == 0) {
      p.Get("dt_fine", _dt_fine);
      p.Get("dt_coarse", _dt_coarse);
    }
    else {
      p.Get("dt_fine", _dt_fine);
      _dt_coarse = 0;
    }

    double tmin, tmax;
    p.Get("tmin", tmin);
    p.Get("tmax", tmax);
    double interval = (tmax - tmin)/nProcess;

    _tmin = tmin + interval * rank;
    _tmax = _tmin + interval;

    MPI_Barrier(MPI_COMM_WORLD);
  }

  void setInitialSolution(Solution<dim> & initialSolution) {
     _initialSolution = &initialSolution; 
    }

  void CoarseStep(int k)
  { 
    CoarseInitialization(k);

    _C_coarse.logStream() << "start CoarseStep " << k 
                          << ", process " << rank << std::endl;

    if (_C_coarse.verbose())
      LOGSTREAM(_C_coarse) << _C_coarse.getInternalState();

    _C_coarse.run(_tmin, _tmax, _dt_coarse, 0);

    _C_coarse.logStream() << "\nend CoarseStep"
                          << ", process " << rank << std::endl;

    CoarseFinalization(k);
  }

  void FineStep(int k)
  {
    _C_fine.logStream() << "\nstart FineStep " << k 
                        << ", process " << rank 
                        << " _tmin " << _tmin << " _tmax " << _tmax << std::endl;

    _C_fine.setInput(_currentCoarseSolution);
    if (_C_fine.verbose())
      LOGSTREAM(_C_fine) << _C_fine.getInternalState();
  
    _C_fine.run(_tmin, _tmax, _dt_fine, k);

    if (rank < rankMax) {
      _send(_C_fine);
    }
//    if (rank > rankMin) {
//      _recieveState(_previousFineState);
//    }

    _C_fine.logStream() << "\nend NextFineStep " << k 
                        << ", process " << rank << std::endl;
  }

  void CoarseInitialization(int k)
  {
    if (rank == rankMin) {
      _C_coarse.setInput(*_initialSolution);
      _currentCoarseState = _C_coarse.getInternalState();
    }
    else {
      MPI_Status status;
      if (k>0)
        _previousCoarseState = _currentCoarseState;
      _recieve(_C_coarse);
      double buffer[2];
      MPI_Recv(buffer, 2, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &status);
      _tmin = buffer[0];
      _dt_coarse = buffer[1];
    }

    if (k>0) {

      _C_fine.logStream()  << "\nstart CoarseCorrection " << k 
                           << ", process " << rank << std::endl;

      size_t i, n = _currentCoarseSolution.components().size();
      for (i=0; i<n; i++) {
        size_t m = _currentCoarseSolution.components()[i].size();
        auto s = _previousCoarseSolution.components()[i];
        double * u_coarse_old = _previousCoarseSolution.components()[i].data();
        double * u_coarse_new = _currentCoarseSolution.components()[i].data();
        double * u_fine_old = _previousFineSolution.components()[i].data();
        double * u_fine_new = _currentFineSolution.components()[i].data();

        for (size_t j=0; j<m; j++)
          u_coarse_new[j] = u_coarse_old[j] + (u_fine_new[j] - u_fine_old[j]); 
      }

      _C_fine.logStream() << "\nend CoarseCorrection " << k 
                          << ", process " << rank << std::endl;
    }
  }

  void CoarseFinalization(int k)
  {
    if (rank < rankMax) {
      _send(_C_coarse);
      double buffer[2];
      buffer[0] = _tmax;
      buffer[1] = _dt_coarse;
      MPI_Send(buffer, 2, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD);
    }
  }
  
  int rankMin, rankMax, rank;
  
private:
  double _dt_coarse, _dt_fine;
  double _tmin, _tmax;
  Code<dim, ncomponents> &_C_coarse, &_C_fine;
  Solution<2> _currentCoarseSolution,
              _previousCoarseSolution,
              _previousFineSolution,
              _currentFineSolution;
  State _currentCoarseState,
              _previousCoarseState,
              _previousFineState,
              _currentFineState;

  void _recieve(Code<dim, ncomponents> & C)
  {
    MPI_Status status;

    auto & solution = C.current();
    for (auto c:solution.components()) {
      MPI_Recv(c.data(), c.size(), MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &status);
    }
  }

  void _send(Code<dim, ncomponents> & C)
  {
    Solution<dim> & solution = C.current();
    for (auto c:solution.components()) {
      MPI_Send(c.data(), c.size(), MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD);
    }
  }

  Solution<2> * _initialSolution;
};


#endif
