#include <iostream>
#include "parareal.hxx"
#include "parameters.hxx"
#include "os.hxx"
#include <unistd.h>
#include <mpi.h>
#include "wave2D.hxx"

constexpr double dt_fine_default = 0.001;
constexpr double dt_coarse_default = 0.1;

void usage(const std::string & progname)
{
  std::cerr << "usage : " << progname << " file" << std::endl;
}

int main(int argc, char** argv){

  pid_t pid = getpid();
  std::cerr << "\n\tpid = " << pid << std::endl;

  MPI_Init(&argc, &argv);

  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  double tolerance;
  if (size<5)
    tolerance = 0.00031;
  else if (size<10) 
    tolerance = 0.036;
  else if (size<20)
    tolerance = 0.046;
  else if (size<30)
    tolerance = 0.046;
  else if (size<40)
    tolerance = 0.04;

  if (rank == 0)
    std::cout << "\nParallel in time code on " << size << " processes " 
              << " tol = " << tolerance << std::endl << std::endl;
  MPI_Barrier(MPI_COMM_WORLD);

  double error = 1e20;

  Parameters p;
  p.Add("refinment", 5, "mesh refinment");
  p.Add("dt_fine", dt_fine_default, "time fine step");
  p.Add("dt_coarse", dt_coarse_default, "time coarse step");
  p.Parse(argc, argv);

  int refinment;
  p.Get("refinment", refinment);

  Discretisation<2> D(refinment);
  D.constraints().close();

  Wave2D CoarseScheme(D, "wave2d_coarse", rank);
  Wave2D FineScheme  (D, "wave2d_fine", rank);
  
  double output;
  p.Get("output", output);

  CoarseScheme.outputDir("out/parareal/coarse/");
  FineScheme.outputDir("out/parareal/fine/");

  p.Get("output", CoarseScheme.outputTime());
  p.Get("output", FineScheme.outputTime());

  p.Get("verbose", CoarseScheme.verbose());
  p.Get("verbose", FineScheme.verbose());

  Solution<2> u0(D, 2);
  CoarseScheme.initialize(u0, initU, initV);

  CoarseScheme.addProbe(CoarseScheme.n()/2, 0);
  FineScheme.addProbe(CoarseScheme.n()/2, 0);
  
  Parareal process(rank, size, CoarseScheme, FineScheme, D, p);
  if (rank == 0)
    process.setInitialSolution(u0);

  int k=0, kmax = size;
  kmax = 1;

  while ((error > tolerance) && (k < kmax))
  {
    process.CoarseStep(k);
    MPI_Barrier(MPI_COMM_WORLD);
    process.FineStep(k);
    MPI_Barrier(MPI_COMM_WORLD);
    k = k+1;
  }

  MPI_Finalize();
  return 0;
}
