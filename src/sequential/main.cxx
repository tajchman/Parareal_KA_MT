#include <iostream>
#include "wave2D.hxx"
#include "parameters.hxx"
#include "os.hxx"

constexpr double dt_default = 0.001;

void usage(const std::string & progname)
{
  std::cerr << "usage : " << progname << " file" << std::endl;
}

int main(int argc, char** argv){

  std::cout << "\nSequential code" << std::endl;
  
  int order = 0;

  Parameters p;
  p.Add("refinment", 5, "mesh refinment");
  p.Add("dt", dt_default, "time step");
  p.Parse(argc, argv);

  double T_start, T_end, dT;
  int refinment;

  p.Get("tmin", T_start);
  p.Get("tmax", T_end);
  p.Get("dt", dT);
  p.Get("refinment", refinment);

  Discretisation<2> D(refinment);
  Wave2D C(D, "wave2d_sequential");
  Solution<2> u0(D, 2);
  C.initialize(u0, initU, initV);
 
  C.outputDir("out/sequential");

  p.Get("output", C.outputTime());
  p.Get("verbose", C.verbose());

  C.addProbe(C.n()/2, 0);

  C.setInput(u0);

  C.run(T_start, T_end, dT);

  return 0;
}
